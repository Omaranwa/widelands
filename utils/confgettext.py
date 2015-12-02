#!/usr/bin/python -tt

"""This proggy parses given files for occurences of translatable strings
if some are found they are output in a xgettext style file"""

import sys
import os
from time import strftime,gmtime

from collections import defaultdict

time_now = gmtime()
### Set the header to something sensible, a much as is possible here.
head =  "# Widelands PATH/TO/FILE.PO\n"
head += "# Copyright (C) 2005-" + strftime("%Y", time_now) + " Widelands Development Team\n"
head += "# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.\n"
head += "#\n"
head += "msgid \"\"\n"
head += "msgstr \"\"\n"
head += "\"Project-Id-Version: Widelands svnVERSION\\n\"\n"
head += "\"Report-Msgid-Bugs-To: https://bugs.launchpad.net/widelands\\n\"\n"
head += "\"POT-Creation-Date: " + strftime("%Y-%m-%d %H:%M+0000", time_now) + "\\n\"\n"
head += "\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n"
head += "\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n\"\n"
head += "\"Language-Team: LANGUAGE <widelands-public@lists.sourceforge.net>\\n\"\n"
head += "\"MIME-Version: 1.0\\n\"\n"
head += "\"Content-Type: text/plain; charset=UTF-8\\n\"\n"
head += "\"Content-Transfer-Encoding: 8bit\\n\"\n"
head += "\n"

class occurences:
    def __init__( self, file, line ):
        self.line = line
        self.file = file

class trans_string:
    def __init__( self ):
        self.occurences = []
        self.str = ""
        
def is_multiline( str ):
    l = str.find('""')
    if( l == -1 ):
        return False
    return True

def firstl( str, what ):
    for c in what:
        index = str.find( c )
        if( index != -1 ):
            return index
    return -1

def firstr( str, what ):
    for c in what:
        index = str.rfind( c )
        if( index != -1 ):
            return index
    return -1

def append_string( known_strings, array, string ):
    if known_strings.has_key( string.str ):
        i = known_strings[string.str]
        array[i].occurences += string.occurences
    else:
        array.append( string )
        known_strings[string.str] = len( array ) - 1   
        
def _escape_pot_string(string):
    return string.replace('\\', '\\\\').replace('"', '\\"')

def _format_msgid(tag, string, output):
    # there was a bug in this code that would never output single line
    # msg_ids. I decided not to fix it, since that wuold change a ton
    # of pot files and maybe msg_ids.
    # if not string.count('\n'): <== this was s.count
        # s += 'msgid "%s"\n' % string
    # else:
    string = _escape_pot_string(string)
    output.append('%s ""' % tag)
    lines = string.split('\n')
    for line in lines[:-1]:
        output.append('"%s\\n"' % line)
    output.append('"%s"' % lines[-1])
    return output                

class Conf_GetText(object):
    def __init__(self):
        self.translatable_strings = []
        
    @property
    def found_something_to_translate(self):
        return len(self.translatable_strings) > 0
    
    def parse(self, files):
        known_strings = {}
        curstr = 0
        multiline = 0
        for file in files:
            lines = open( file, 'r' ).readlines();
            for i in range(0,len(lines)):
                line = lines[i].rstrip('\n')
                linenr = i+1
                curstr = 0

                if multiline and  len(line) and line[0]=='_':
                    line = line[1:]
                    rindex = line.rfind('""')
                    if rindex == -1 or line[:2] == '""':
                        line = line.strip()
                        line = line.strip('"')
                        curstr = trans_string()
                        curstr.str = line
                        curstr.occurences.append( occurences( file, linenr ))
                        append_string( known_strings, self.translatable_strings, curstr )
                        continue
                    else:
                        line = line[:(rindex+1)]
                        line = line.strip()
                        line = line.strip('"')
                        curstr = trans_string()
                        curstr.str = line
                        curstr.occurences.append( occurences( file, linenr ))
                        append_string( known_strings, self.translatable_strings, curstr )
                        multiline = False
                        continue

                index = line.find( "=_" )
                if( index > 0 ):
                    curstr = trans_string()
                    curstr.occurences.append( occurences( file, linenr ))
                    restline = line[index+2:]
                    multiline = is_multiline( restline );
                    if multiline: # means exactly one "
                        index = firstl( restline, '"\'')
                        restline = restline[index+1:]
                        restline = restline.strip()
                        restline = restline.strip('"')
                        curstr.str += '"' + restline + '"\n'
                        continue
                    # Is not multiline
                    # If there are ' or " its easy
                    l = firstl( restline, '\"')
                    r = firstr( restline[l+1:], '\"')
                    if( l != -1 and r != -1 ):
                        restline = restline[l+1:]
                        restline = restline[:r]
                    else:
                        # Remove comments
                        rindex = max( restline.rfind('#'), restline.rfind('//'))
                        if rindex != -1:
                            restline = restline[:rindex]
                        # And strip
                        restline = restline.strip()
                    curstr.str = restline
                    append_string( known_strings, self.translatable_strings, curstr )

        self.translatable_strings.sort( lambda str1,str2:
                    cmp(str1.occurences[0].file,str2.occurences[0].file) )
    
    def toString(self, header=True):
        lines = []
        for i in self.translatable_strings:
            for occ in i.occurences:
                lines.append('#: %s:%i' % (os.path.normpath(occ.file), occ.line))
                
            _format_msgid('msgid', i.str, lines)
            lines.extend(['msgstr ""', ''])
        if (header):
            return (head + "\n".join(lines))
        else:
            return ("\n".join(lines))
