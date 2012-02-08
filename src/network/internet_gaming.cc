/*
 * Copyright (C) 2004-2006, 2008-2009, 2012 by the Widelands Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "internet_gaming.h"

#include "i18n.h"
#include "internet_gaming_messages.h"
#include "log.h"
#include "warning.h"

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>



/// Private constructor by purpose: NEVER call directly. Always call InternetGaming::ref(), this will ensure
/// that only one instance is running at time.
InternetGaming::InternetGaming() :
	m_sock                   (0),
	m_sockset                (0),
	m_state                  (OFFLINE),
	m_clientrights           (INTERNET_CLIENT_UNREGISTERED),
	m_maxclients             (1),
	m_gameip                 (""),
	clientupdateonmetaserver (false),
	gameupdateonmetaserver   (false),
	clientupdate             (false),
	gameupdate               (false),
	time_offset              (0)
{
	// Fill the list of possible messages from the server
	InternetGamingMessages::fill_map();
}

/// resets all stored variables without the chat messages for a clean new login (not relogin)
void InternetGaming::reset() {
	m_sock                   = 0;
	m_sockset                = 0;
	m_state                  = OFFLINE;
	m_pwd                    = "";
	m_reg                    = false;
	m_meta                   = INTERNET_GAMING_METASERVER;
	m_port                   = INTERNET_GAMING_PORT;
	m_clientname             = "";
	m_clientrights           = INTERNET_CLIENT_UNREGISTERED;
	m_maxclients             = 1;
	m_gamename               = "";
	m_gameip                 = "";
	clientupdateonmetaserver = false;
	gameupdateonmetaserver   = false;
	clientupdate             = false;
	gameupdate               = false;
	time_offset              = 0;
	waitcmd                  = "";
	waittimeout              = std::numeric_limits<int32_t>::max();

	clientlist.clear();
	gamelist.clear();
}


/// the one and only InternetGaming instance.
static InternetGaming * ig = 0;


/// \returns the one and only InternetGaming instance.
InternetGaming & InternetGaming::ref() {
	if (not ig)
		ig = new InternetGaming();
	return * ig;
}


void InternetGaming::initialiseConnection() {
	// First of all try to connect to the metaserver
	dedicatedlog("InternetGaming: Connecting to the metaserver.\n");
	IPaddress peer;
	if (hostent * const he = gethostbyname(m_meta.c_str())) {
		peer.host = (reinterpret_cast<in_addr *>(he->h_addr_list[0]))->s_addr;
		peer.port = htons(m_port);
	} else
		throw warning
			(_("Connection problem"), "%s", _("Widelands has not been able to connect to the metaserver."));

	SDLNet_ResolveHost (&peer, m_meta.c_str(), m_port);
	m_sock = SDLNet_TCP_Open(&peer);
	if (m_sock == 0)
		throw warning
			(_("Could not establish connection to host"),
			 _
			 	("Widelands could not establish a connection to the given address.\n"
			 	 "Either there was no metaserver running at the supposed port or\n"
			 	 "your network setup is broken."));

	m_sockset = SDLNet_AllocSocketSet(1);
	SDLNet_TCP_AddSocket (m_sockset, m_sock);
}



/// Login to metaserver
bool InternetGaming::login
	(std::string const & nick, std::string const & pwd, bool reg, std::string const & meta, uint32_t port)
{
	assert(m_state == OFFLINE);

	m_pwd  = pwd;
	m_reg  = reg;
	m_meta = meta;
	m_port = port;

	initialiseConnection();

	// If we are here, a connection was established and we can send our login package through the socket.
	dedicatedlog("InternetGaming: Sending login request.\n");
	SendPacket s;
	s.String(IGPCMD_LOGIN);
	s.String(boost::lexical_cast<std::string>(INTERNET_GAMING_PROTOCOL_VERSION));
	s.String(nick);
	s.String(build_id());
	s.String(bool2str(reg));
	if (reg)
		s.String(pwd);
	s.send(m_sock);

	// Now let's see, whether the metaserver is answering
	uint32_t const secs = time(0);
	m_state = CONNECTING;
	while (INTERNET_GAMING_TIMEOUT > time(0) - secs) {
		handle_metaserver_communication();
		// Check if we are a step further... if yes handle_packet has taken care about all the
		// paperwork, so we put our feet up and just return. ;)
		if (m_state != CONNECTING) {
			if (m_state == LOBBY) {
				formatAndAddChat("", "", true, _("For hosting a game, please take a look at the notes at:"));
				formatAndAddChat("", "", true, "http://wl.widelands.org/wiki/InternetGaming");
				return true;
			} else if (m_state == ERROR)
				return false;
		}
	}
	dedicatedlog("InternetGaming: No answer from metaserver!\n");
	logout("NO_ANSWER");
	return false;
}



/// Relogin to metaserver after loosing connection
bool InternetGaming::relogin()
{
	assert(m_state == ERROR);

	initialiseConnection();

	// If we are here, a connection was established and we can send our login package through the socket.
	dedicatedlog("InternetGaming: Sending relogin request.\n");
	SendPacket s;
	s.String(IGPCMD_RELOGIN);
	s.String(boost::lexical_cast<std::string>(INTERNET_GAMING_PROTOCOL_VERSION));
	s.String(m_clientname);
	s.String(build_id());
	s.String(bool2str(m_reg));
	if (m_reg)
		s.String(m_pwd);
	s.send(m_sock);

	// Now let's see, whether the metaserver is answering
	uint32_t const secs = time(0);
	m_state = CONNECTING;
	while (INTERNET_GAMING_TIMEOUT > time(0) - secs) {
		handle_metaserver_communication();
		// Check if we are a step further... if yes handle_packet has taken care about all the
		// paperwork, so we put our feet up and just return. ;)
		if (m_state != CONNECTING) {
			if (m_state == LOBBY) {
				break;
			} else if (m_state == ERROR)
				return false;
		}
	}

	if (INTERNET_GAMING_TIMEOUT <= time(0) - secs) {
		dedicatedlog("InternetGaming: No answer from metaserver!\n");
		return false;
	}

	// Client is reconnected, so let's try resend the timeouted command.
	if (waitcmd == IGPCMD_GAME_CONNECT)
		join_game(m_gamename);
	else if (waitcmd == IGPCMD_GAME_OPEN) {
		m_state = IN_GAME;
		open_game();
	} else if (waitcmd == IGPCMD_GAME_START) {
		m_state = IN_GAME;
		set_game_playing();
	} else
		assert(false); // should never be here

	return true;
}



/// logout of the metaserver
/// \note \arg msgcode should be a message from the list of InternetGamingMessages
void InternetGaming::logout(std::string const & msgcode) {

	// Just in case the metaserver is listening on the socket - tell him we break up with him ;)
	SendPacket s;
	s.String(IGPCMD_DISCONNECT);
	s.String(msgcode);
	s.send(m_sock);

	const std::string & msg = InternetGamingMessages::get_message(msgcode);
	dedicatedlog("InternetGaming: %s\n", msg.c_str());
	formatAndAddChat("", "", true, msg);

	reset();
}



/// handles all communication between the metaserver and the client
void InternetGaming::handle_metaserver_communication() {
	try {
		while (m_sock != 0 && SDLNet_CheckSockets(m_sockset, 0) > 0) {
			// Perform only one read operation, then process all packets
			// from this read. This ensures that we process DISCONNECT
			// packets that are followed immediately by connection close.
			if (!m_deserializer.read(m_sock)) {
				logout("CONNECTION_LOST");
				return;
			}

			// Process all the packets from the last read
			while (m_sock && m_deserializer.avail()) {
				RecvPacket packet(m_deserializer);
				handle_packet(packet);
			}
		}
	} catch (std::exception const & e) {
		std::string reason = _("Something went wrong: ");
		reason += e.what();
		logout(reason);
	}

	if (m_state == LOBBY) {
		// client is in the lobby and therefore we want realtime information updates
		if (clientupdateonmetaserver) {
			dedicatedlog("send client\n");
			SendPacket s;
			s.String(IGPCMD_CLIENTS);
			s.send(m_sock);

			clientupdateonmetaserver = false;
		}

		if (gameupdateonmetaserver) {
			dedicatedlog("send game\n");
			SendPacket s;
			s.String(IGPCMD_GAMES);
			s.send(m_sock);

			gameupdateonmetaserver = false;
		}
	}

	if (waitcmd.size() > 0) {
		// Check if timeout is reached
		time_t now = time(0);
		if (now > waittimeout) {
			m_state = ERROR;
			waittimeout = std::numeric_limits<int32_t>::max();
			if (!relogin()) {
				// Do not try to relogin again automatically.
				reset();
				m_state = ERROR;
			}
		}
	}
}



/// Handle one packet received from the metaserver.
void InternetGaming::handle_packet(RecvPacket & packet)
{
	std::string cmd = packet.String();

	// First check if everything is fine or whether the metaserver broke up with the client.
	if (cmd == IGPCMD_DISCONNECT) {
		std::string reason = packet.String();
		logout(reason);
		return;
	}

	// Are we already online?
	if (m_state == CONNECTING) {
		if (cmd == IGPCMD_LOGIN) {
			// Clients request to login was granted
			m_clientname   = packet.String();
			m_clientrights = packet.String();
			m_state        = LOBBY;
			dedicatedlog("InternetGaming: Client %s logged in.\n", m_clientname.c_str());
			return;

		} else if (cmd == IGPCMD_RELOGIN) {
			// Clients request to relogin was granted
			m_state = LOBBY;
			dedicatedlog("InternetGaming: Client %s relogged in.\n", m_clientname.c_str());
			return;

		} else if (cmd == IGPCMD_ERROR) {
			if (packet.String() != "LOGIN")
				throw warning(_("Mixed up"), _("The metaserver sent a strange ERROR during connection"));
			// Clients login request got rejected
			logout(packet.String());
			m_state = ERROR;
			return;

		} else {
			logout();
			m_state = ERROR;
			throw warning
				(_
				 	("Expected a LOGIN, RELOGIN or REJECTED packet from server, but received command "
				 	 "%s. Maybe the metaserver is using a different protocol version ?"),
				 cmd.c_str());
		}
	}

	try {
		if (cmd == IGPCMD_LOGIN || cmd == IGPCMD_RELOGIN) {
			// Login specific commands but not in CONNECTING state...
			dedicatedlog
				("InternetGaming: Received %s cmd although client is not in CONNECTING state.\n", cmd.c_str());
			std::string temp =
				(boost::format
					(_("WARNING: Received a %s command although we are not in CONNECTING state.")) % cmd)
				.str();
			formatAndAddChat("", "", true, temp);
		}

		else if (cmd == IGPCMD_TIME) {
			// Client received the server time
			time_offset = boost::lexical_cast<int>(packet.String()) - time(0);
			dedicatedlog("InternetGaming: Server time offset is %i seconds.\n", time_offset);
			std::string temp = (boost::format(_("Server time offset is %i seconds.")) % time_offset).str();
			formatAndAddChat("", "", true, temp);
		}

		else if (cmd == IGPCMD_PING) {
			// Client received a PING and should immediately PONG as requested
			SendPacket s;
			s.String(IGPCMD_PONG);
			s.send(m_sock);
		}

		else if (cmd == IGPCMD_CHAT) {
			// Client received a chat message
			std::string sender   = packet.String();
			std::string message  = packet.String();
			bool        personal = str2bool(packet.String());
			bool        system   = str2bool(packet.String());

			formatAndAddChat(sender, personal ? m_clientname : "", system, message);
		}

		else if (cmd == IGPCMD_GAMES_UPDATE) {
			// Client received a note, that the list of games was changed
			dedicatedlog("InternetGaming: Game update on metaserver.\n");
			gameupdateonmetaserver = true;
		}

		else if (cmd == IGPCMD_GAMES) {
			// Client received the new list of games
			uint8_t number = boost::lexical_cast<int>(packet.String()) & 0xff;
			gamelist.clear();
			dedicatedlog("InternetGaming: Received a game list update with %u items.\n", number);
			for (uint8_t i = 0; i < number; ++i) {
				INet_Game * ing  = new INet_Game();
				ing->name        = packet.String();
				ing->build_id    = packet.String();
				ing->connectable = str2bool(packet.String());
				gamelist.push_back(*ing);
			}
			gameupdate = true;
		}

		else if (cmd == IGPCMD_CLIENTS_UPDATE) {
			// Client received a note, that the list of clients was changed
			dedicatedlog("InternetGaming: Client update on metaserver.\n");
			clientupdateonmetaserver = true;
		}

		else if (cmd == IGPCMD_CLIENTS) {
			// Client received the new list of clients
			uint8_t number = boost::lexical_cast<int>(packet.String()) & 0xff;
			clientlist.clear();
			dedicatedlog("InternetGaming: Received a client list update with %u items.\n", number);
			for (uint8_t i = 0; i < number; ++i) {
				INet_Client * inc  = new INet_Client();
				inc->name        = packet.String();
				inc->build_id    = packet.String();
				inc->game        = packet.String();
				inc->type        = packet.String();
				inc->points      = packet.String();
				clientlist.push_back(*inc);
			}
			clientupdate = true;
		}

		else if (cmd == IGPCMD_GAME_OPEN) {
			// Client received the acknowledgment, that the game was opened
			assert (waitcmd == IGPCMD_GAME_OPEN);
			waitcmd = "";
		}

		else if (cmd == IGPCMD_GAME_CONNECT) {
			// Client received the ip for the game it wants to join
			assert (waitcmd == IGPCMD_GAME_CONNECT);
			waitcmd = "";
			// save the received ip, so the client cann connect to the game
			m_gameip = packet.String();
		}

		else if (cmd == IGPCMD_GAME_START) {
			// Client received the acknowledgment, that the game was started
			assert (waitcmd == IGPCMD_GAME_START);
			waitcmd = "";
		}

		else if (cmd == IGPCMD_ERROR) {
			// Client received an ERROR message - seems something went wrong
			std::string cmd    (packet.String());
			std::string reason (packet.String());
			std::string message(_("ERROR: "));

			if (cmd == IGPCMD_CHAT) {
				// Something went wrong with the chat message the user sent.
				message += _("Chat message could not be sent. ");
				if (reason == "NO_SUCH_USER")
					message +=
						(boost::format
							(InternetGamingMessages::get_message(reason)) % packet.String().c_str())
						.str();
			}

			else if (cmd == IGPCMD_GAME_OPEN) {
				// Something went wrong with the newly opened game
				message += InternetGamingMessages::get_message(reason);
			}

			// Finally send the error message as system chat to the client.
			formatAndAddChat("", "", true, message);
		}

		else
			// Inform the client about the unknown command
			formatAndAddChat("", "", true, _("Received an unknown command from the metaserver: ") + cmd);

	} catch (warning & e) {
		formatAndAddChat("", "", true, e.what());
	}

}



/// \returns the ip of the game the client is on or wants to join (or the client is hosting)
///          or 0, if no ip available.
const std::string & InternetGaming::ip() {
	return m_gameip;
}



/// called by a client to join the game \arg gamename
void InternetGaming::join_game(const std::string & gamename) {
	if (!logged_in())
		return;

	SendPacket s;
	s.String(IGPCMD_GAME_CONNECT);
	s.String(gamename);
	s.send(m_sock);
	m_gamename = gamename;
	dedicatedlog("InternetGaming: Client tries to join a game with the name %s\n", m_gamename.c_str());
	m_state = IN_GAME;


	// From now on we wait for a reply from the metaserver
	waitcmd     = IGPCMD_GAME_CONNECT;
	waittimeout = time(0) + INTERNET_GAMING_TIMEOUT;
}



/// called by a client to open a new game with name m_gamename
void InternetGaming::open_game() {
	if (!logged_in())
		return;

	SendPacket s;
	s.String(IGPCMD_GAME_OPEN);
	s.String(m_gamename);
	s.String(boost::lexical_cast<std::string>(m_maxclients));
	s.send(m_sock);
	dedicatedlog("InternetGaming: Client opened a game with the name %s.\n", m_gamename.c_str());
	m_state = IN_GAME;

	// From now on we wait for a reply from the metaserver
	waitcmd     = IGPCMD_GAME_OPEN;
	waittimeout = time(0) + INTERNET_GAMING_TIMEOUT;
}



/// called by a client that is host of a game to inform the metaserver, that the game started
void InternetGaming::set_game_playing() {
	if (!logged_in())
		return;

	SendPacket s;
	s.String(IGPCMD_GAME_START);
	s.send(m_sock);
	dedicatedlog("InternetGaming: Client announced the start of the game %s.\n", m_gamename.c_str());

	// From now on we wait for a reply from the metaserver
	waitcmd     = IGPCMD_GAME_START;
	waittimeout = time(0) + INTERNET_GAMING_TIMEOUT;
}



/// called by a client that is host of a game to inform the metaserver, that the game was ended.
void InternetGaming::set_game_done() {
	if (!logged_in())
		return;

	SendPacket s;
	s.String(IGPCMD_GAME_DISCONNECT);
	s.send(m_sock);
	m_gameip  = "";
	dedicatedlog("InternetGaming: Client announced the end of the game %s.\n", m_gamename.c_str());
}



/// \returns whether the local gamelist was updated
/// \note this function resets gameupdate. So if you call it, please really handle the output.
bool InternetGaming::updateForGames() {
	bool temp = gameupdate;
	gameupdate = false;
	return temp;
}



/// \returns the tables in the room
std::vector<INet_Game> const & InternetGaming::games() {
	return gamelist;
}



/// \returns whether the local clientlist was updated
/// \note this function resets clientupdate. So if you call it, please really handle the output.
bool InternetGaming::updateForClients() {
	bool temp = clientupdate;
	clientupdate = false;
	return temp;
}



/// \returns the players in the room
std::vector<INet_Client> const & InternetGaming::clients() {
	return clientlist;
}



/// ChatProvider: sends a message via the metaserver.
void InternetGaming::send(std::string const & msg) {
	if (!logged_in()) {
		formatAndAddChat
			("", "", true, _("Message could not be sent: You are not connected to the metaserver!"));
		return;
	}

	SendPacket s;
	s.String(IGPCMD_CHAT);

	if (msg.size() && *msg.begin() == '@') {
		// Format a personal message
		std::string::size_type const space = msg.find(' ');
		if (space >= msg.size() - 1) {
			formatAndAddChat
				("", "", true, _("Message could not be sent: Was this supposed to be a private message?"));
			return;
		}
		s.String(msg.substr(space + 1));    // message
		s.String(msg.substr(1, space - 1)); // recipient

		formatAndAddChat(m_clientname, msg.substr(1, space - 1), false, msg.substr(space + 1));
	} else {
		s.String(msg);
		s.String("");
	}

	s.send(m_sock);
}



/**
 * \returns the boolean value of a string received from the metaserver.
 * If conversion fails, it throws a \ref warning
 */
bool InternetGaming::str2bool(std::string str) {
	if ((str != "true") && (str != "false"))
		throw warning("Conversion from std::string to bool failed. String was \"%s\"", str.c_str());
	return str == "true";
}

/// \returns a string containing the boolean value \arg b to be send to metaserver
std::string InternetGaming::bool2str(bool b) {
	return b ? "true" : "false";
}


/// formates a chat message and adds it to the list of chat messages
void InternetGaming::formatAndAddChat(std::string from, std::string to, bool system, std::string msg) {
	ChatMessage c;
	c.time      = time(0);
	c.sender    = !system && from.empty() ? _("<unknown>") : from;
	c.playern   = system ? -1 : to.size() ? 3 : 7;
	c.msg       = msg;
	c.recipient = to;

	receive(c);
	if (system && (m_state == IN_GAME)) {
		// Save system chat messages seperately as well, so the nethost can import and show them in game;
		c.msg = "METASERVER: " + msg;
		ingame_system_chat.push_back(c);
	}
}

