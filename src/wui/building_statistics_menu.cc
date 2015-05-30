/*
 * Copyright (C) 2002-2004, 2006-2011 by the Widelands Development Team
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

#include "wui/building_statistics_menu.h"

#include <cmath>

#include <boost/bind.hpp>
#include <boost/format.hpp>

#include "base/i18n.h"
#include "graphic/font_handler1.h"
#include "logic/player.h"
#include "logic/militarysite.h"
#include "logic/productionsite.h"
#include "logic/tribe.h"

constexpr int kBuildGridCellSize = 50;
constexpr int kMargin = 5;
constexpr int kColumns = 5;
constexpr int kButtonHeight = 20;
constexpr int kButtonRowHeight = kButtonHeight + kMargin;
constexpr int kLabelHeight = 18;
constexpr int kLabelFontSize = 12;
constexpr int kTabHeight = 35 + 5 * (kBuildGridCellSize + kLabelHeight + kLabelHeight);
constexpr int32_t kWindowWidth = kColumns * kBuildGridCellSize;
constexpr int32_t kWindowHeight = kTabHeight + kMargin + 4 * kButtonRowHeight;

constexpr int32_t kUpdateTime = 1000;  //  1 second, gametime

namespace {
void set_label_font(UI::Textarea* label) {
	label->set_font(UI::g_fh1->fontset().serif(), kLabelFontSize, UI_FONT_CLR_FG);
}
void set_editbox_font(UI::EditBox* editbox) {
	editbox->set_font(UI::g_fh1->fontset().serif(), kLabelFontSize, UI_FONT_CLR_FG);
}

}  // namespace

inline InteractivePlayer& BuildingStatisticsMenu::iplayer() const {
	return dynamic_cast<InteractivePlayer&>(*get_parent());
}

BuildingStatisticsMenu::BuildingStatisticsMenu(InteractivePlayer& parent,
                                               UI::UniqueWindow::Registry& registry)
   : UI::UniqueWindow(&parent,
                      "building_statistics",
                      &registry,
                      kWindowWidth,
                      kWindowHeight,
                      _("Building Statistics")),
     tabs_(this, 0, 0, g_gr->images().get("pics/but1.png")),
     small_tab_(&tabs_, 0, 0, UI::Box::Vertical),
     medium_tab_(&tabs_, 0, 0, UI::Box::Vertical),
     big_tab_(&tabs_, 0, 0, UI::Box::Vertical),
     mines_tab_(&tabs_, 0, 0, UI::Box::Vertical),
     ports_tab_(&tabs_, 0, 0, UI::Box::Vertical),
     building_name_(this,
                    get_inner_w() / 2,
                    get_inner_h() - 4 * kButtonRowHeight,
                    0,
                    kButtonHeight,
                    "",
                    UI::Align_Center),
     owned_label_(this,
                  kMargin,
                  get_inner_h() - 3 * kButtonRowHeight,
                  0,
                  kButtonHeight,
                  _("Owned:"),
                  UI::Align_CenterLeft),
     construction_label_(this,
                         kMargin,
                         get_inner_h() - 2 * kButtonRowHeight,
                         0,
                         kButtonHeight,
                         _("Under Construction:"),
                         UI::Align_CenterLeft),
     unproductive_box_(this, kMargin, get_inner_h() - kButtonRowHeight + 3, UI::Box::Horizontal),
     unproductive_label_(
        &unproductive_box_,
        /** TRANSLATORS: This is the first part of productivity with input field */
        /** TRANSLATORS: Building statistics window -  'Low Production: <input>%' */
        _("Low Production: "),
        UI::Align_BottomLeft),
     unproductive_percent_(
        &unproductive_box_, 0, 0, 35, kLabelHeight, g_gr->images().get("pics/but1.png")),
     unproductive_label2_(
        &unproductive_box_,
        /** TRANSLATORS: This is the second part of productivity with input field */
        /** TRANSLATORS: Building statistics window -  'Low Production: <input>%' */
        _("%"),
        UI::Align_BottomLeft),
     no_owned_label_(this,
                     get_inner_w() - 2 * kButtonRowHeight - kMargin,
                     get_inner_h() - 3 * kButtonRowHeight,
                     0,
                     kButtonHeight,
                     "",
                     UI::Align_CenterRight),
     no_construction_label_(this,
                            get_inner_w() - 2 * kButtonRowHeight - kMargin,
                            get_inner_h() - 2 * kButtonRowHeight,
                            0,
                            kButtonHeight,
                            "",
                            UI::Align_CenterRight),
     no_unproductive_label_(this,
                            get_inner_w() - 2 * kButtonRowHeight - kMargin,
                            get_inner_h() - kButtonRowHeight,
                            0,
                            kButtonHeight,
                            "",
                            UI::Align_CenterRight),
     low_production_(33),
     has_selection_(false) {
	tabs_.add("building_stats_small",
	          g_gr->images().get("pics/menu_tab_buildsmall.png"),
	          &small_tab_,
	          _("Small Buildings"));
	tabs_.add("building_stats_medium",
	          g_gr->images().get("pics/menu_tab_buildmedium.png"),
	          &medium_tab_,
	          _("Medium Buildings"));
	tabs_.add("building_stats_big",
	          g_gr->images().get("pics/menu_tab_buildbig.png"),
	          &big_tab_,
	          _("Big Buildings"));
	tabs_.add("building_stats_mines",
	          g_gr->images().get("pics/menu_tab_buildmine.png"),
	          &mines_tab_,
	          _("Mines"));
	tabs_.add("building_stats_ports",
	          g_gr->images().get("pics/menu_tab_buildport.png"),
	          &ports_tab_,
	          _("Ports"));
	tabs_.set_size(kWindowWidth, kTabHeight);

	const TribeDescr& tribe = iplayer().player().tribe();

	const BuildingIndex nr_buildings = tribe.get_nrbuildings();
	building_buttons_ = std::vector<UI::Button*>(nr_buildings);
	owned_labels_ = std::vector<UI::MultilineTextarea*>(nr_buildings);
	productivity_labels_ = std::vector<UI::MultilineTextarea*>(nr_buildings);

	// Column counters
	int small_column = 0;
	int medium_column = 0;
	int big_column = 0;
	int mines_column = 0;
	int ports_column = 0;

	UI::Box* mines_row = new UI::Box(&mines_tab_, 0, 0, UI::Box::Horizontal);
	UI::Box* ports_row = new UI::Box(&ports_tab_, 0, 0, UI::Box::Horizontal);
	UI::Box* big_row = new UI::Box(&big_tab_, 0, 0, UI::Box::Horizontal);
	UI::Box* medium_row = new UI::Box(&medium_tab_, 0, 0, UI::Box::Horizontal);
	UI::Box* small_row = new UI::Box(&small_tab_, 0, 0, UI::Box::Horizontal);

	for (BuildingIndex id = 0; id < nr_buildings; ++id) {
		const BuildingDescr& descr = *tribe.get_building_descr(id);

		if (descr.type() != MapObjectType::CONSTRUCTIONSITE &&
		    descr.type() != MapObjectType::DISMANTLESITE) {
			if (descr.get_ismine()) {
				if (add_button(id, descr, mines_tab_, *mines_row, &mines_column)) {
					mines_row = new UI::Box(&mines_tab_, 0, 0, UI::Box::Horizontal);
				}
			} else if (descr.get_isport()) {
				if (add_button(id, descr, ports_tab_, *ports_row, &ports_column)) {
					ports_row = new UI::Box(&ports_tab_, 0, 0, UI::Box::Horizontal);
				}
			} else {
				switch (descr.get_size()) {
				case BaseImmovable::SMALL:
					if (add_button(id, descr, small_tab_, *small_row, &small_column)) {
						small_row = new UI::Box(&small_tab_, 0, 0, UI::Box::Horizontal);
					}
					break;
				case BaseImmovable::MEDIUM:
					if (add_button(id, descr, medium_tab_, *medium_row, &medium_column)) {
						medium_row = new UI::Box(&medium_tab_, 0, 0, UI::Box::Horizontal);
					}
					break;
				case BaseImmovable::BIG:
					if (add_button(id, descr, big_tab_, *big_row, &big_column)) {
						big_row = new UI::Box(&big_tab_, 0, 0, UI::Box::Horizontal);
					}
					break;
				default:
					throw wexception(
					   "Building statictics: Found building without a size: %s", descr.name().c_str());
				}
			}
		}
	}
	mines_tab_.add(mines_row, UI::Align_Left);
	ports_tab_.add(ports_row, UI::Align_Left);
	small_tab_.add(small_row, UI::Align_Left);
	medium_tab_.add(medium_row, UI::Align_Left);
	big_tab_.add(big_row, UI::Align_Left);

	set_label_font(&owned_label_);
	set_label_font(&construction_label_);
	set_label_font(&unproductive_label_);
	set_editbox_font(&unproductive_percent_);
	set_label_font(&unproductive_label2_);
	set_label_font(&no_owned_label_);
	set_label_font(&no_construction_label_);
	set_label_font(&no_unproductive_label_);

	unproductive_label_.set_size(unproductive_label_.get_w(), kButtonRowHeight);
	unproductive_percent_.set_text(std::to_string(low_production_));
	unproductive_percent_.set_max_length(4);
	unproductive_label2_.set_size(unproductive_label2_.get_w(), kButtonRowHeight);
	unproductive_box_.add(&unproductive_label_, UI::Align_Left);
	unproductive_box_.add(&unproductive_percent_, UI::Align_Left);
	unproductive_box_.add(&unproductive_label2_, UI::Align_Left);
	unproductive_box_.set_size(
	   unproductive_label_.get_w() + unproductive_percent_.get_w() + unproductive_label2_.get_w(),
	   kButtonRowHeight);

	navigation_buttons_[NavigationButton::PrevOwned] =
	   new UI::Button(this,
	                  "previous_owned",
	                  get_inner_w() - 2 * kButtonRowHeight,
	                  get_inner_h() - 3 * kButtonRowHeight,
	                  kButtonHeight,
	                  kButtonHeight,
	                  g_gr->images().get("pics/but4.png"),
	                  g_gr->images().get("pics/scrollbar_left.png"),
	                  _("Show previous"),
	                  false);

	navigation_buttons_[NavigationButton::NextOwned] =
	   new UI::Button(this,
	                  "next_owned",
	                  get_inner_w() - kButtonRowHeight,
	                  get_inner_h() - 3 * kButtonRowHeight,
	                  kButtonHeight,
	                  kButtonHeight,
	                  g_gr->images().get("pics/but4.png"),
	                  g_gr->images().get("pics/scrollbar_right.png"),
	                  _("Show next"),
	                  false);

	navigation_buttons_[NavigationButton::PrevConstruction] =
	   new UI::Button(this,
	                  "previous_constructed",
	                  get_inner_w() - 2 * kButtonRowHeight,
	                  get_inner_h() - 2 * kButtonRowHeight,
	                  kButtonHeight,
	                  kButtonHeight,
	                  g_gr->images().get("pics/but4.png"),
	                  g_gr->images().get("pics/scrollbar_left.png"),
	                  _("Show previous"),
	                  false);

	navigation_buttons_[NavigationButton::NextConstruction] =
	   new UI::Button(this,
	                  "next_constructed",
	                  get_inner_w() - kButtonRowHeight,
	                  get_inner_h() - 2 * kButtonRowHeight,
	                  kButtonHeight,
	                  kButtonHeight,
	                  g_gr->images().get("pics/but4.png"),
	                  g_gr->images().get("pics/scrollbar_right.png"),
	                  _("Show next"),
	                  false);

	navigation_buttons_[NavigationButton::PrevUnproductive] =
	   new UI::Button(this,
	                  "previous_unproductive",
	                  get_inner_w() - 2 * kButtonRowHeight,
	                  get_inner_h() - kButtonRowHeight,
	                  kButtonHeight,
	                  kButtonHeight,
	                  g_gr->images().get("pics/but4.png"),
	                  g_gr->images().get("pics/scrollbar_left.png"),
	                  _("Show previous"),
	                  false);

	navigation_buttons_[NavigationButton::NextUnproductive] =
	   new UI::Button(this,
	                  "next_unproductive",
	                  get_inner_w() - kButtonRowHeight,
	                  get_inner_h() - kButtonRowHeight,
	                  kButtonHeight,
	                  kButtonHeight,
	                  g_gr->images().get("pics/but4.png"),
	                  g_gr->images().get("pics/scrollbar_right.png"),
	                  _("Show next"),
	                  false);

	navigation_buttons_[NavigationButton::PrevOwned]->sigclicked.connect(boost::bind(
	   &BuildingStatisticsMenu::jump_building, boost::ref(*this), JumpTarget::kOwned, true));
	navigation_buttons_[NavigationButton::NextOwned]->sigclicked.connect(boost::bind(
	   &BuildingStatisticsMenu::jump_building, boost::ref(*this), JumpTarget::kOwned, false));
	navigation_buttons_[NavigationButton::PrevConstruction]->sigclicked.connect(boost::bind(
	   &BuildingStatisticsMenu::jump_building, boost::ref(*this), JumpTarget::kConstruction, true));
	navigation_buttons_[NavigationButton::NextConstruction]->sigclicked.connect(boost::bind(
	   &BuildingStatisticsMenu::jump_building, boost::ref(*this), JumpTarget::kConstruction, false));
	navigation_buttons_[NavigationButton::PrevUnproductive]->sigclicked.connect(boost::bind(
	   &BuildingStatisticsMenu::jump_building, boost::ref(*this), JumpTarget::kUnproductive, true));
	navigation_buttons_[NavigationButton::NextUnproductive]->sigclicked.connect(boost::bind(
	   &BuildingStatisticsMenu::jump_building, boost::ref(*this), JumpTarget::kUnproductive, false));

	unproductive_percent_.changed.connect(
	   boost::bind(&BuildingStatisticsMenu::low_production_changed, boost::ref(*this)));
	unproductive_percent_.ok.connect(
	   boost::bind(&BuildingStatisticsMenu::low_production_reset_focus, boost::ref(*this)));
	unproductive_percent_.cancel.connect(
	   boost::bind(&BuildingStatisticsMenu::low_production_reset_focus, boost::ref(*this)));

	update();
}

// Adds 3 buttons per building type:
// - Building image, steps through all buildings of the type
// - Buildings owned, steps through constructionsites
// - Productivity, steps though buildings with low productivity and stopped buildings
bool BuildingStatisticsMenu::add_button(
   BuildingIndex id, const BuildingDescr& descr, UI::Box& tab, UI::Box& row, int* column) {
	// Only add headquarter types that are owned by player.
	if (!(descr.is_buildable() || descr.is_enhanced() || descr.global()) &&
	    iplayer().get_player()->get_building_statistics(id).empty()) {
		return false;
	}

	UI::Box* button_box = new UI::Box(&row, 0, 0, UI::Box::Vertical);
	building_buttons_[id] = new UI::Button(button_box,
	                                       (boost::format("building_button%s") % id).str(),
	                                       0,
	                                       0,
	                                       kBuildGridCellSize,
	                                       kBuildGridCellSize,
	                                       g_gr->images().get("pics/but1.png"),
	                                       &g_gr->animations()
	                                           .get_animation(descr.get_animation("idle"))
	                                           .representative_image_from_disk(),
	                                       "",
	                                       false,
	                                       true);
	button_box->add(building_buttons_[id], UI::Align_Left);

	owned_labels_[id] =
	   new UI::MultilineTextarea(button_box, 0, 0, kBuildGridCellSize, kLabelHeight);
	button_box->add(owned_labels_[id], UI::Align_Left);

	productivity_labels_[id] =
	   new UI::MultilineTextarea(button_box, 0, 0, kBuildGridCellSize, kLabelHeight);
	button_box->add(productivity_labels_[id], UI::Align_Left);

	row.add(button_box, UI::Align_Left);

	building_buttons_[id]->sigclicked.connect(
	   boost::bind(&BuildingStatisticsMenu::set_current_building_type, boost::ref(*this), id));

	// Check if the row is full
	++*column;
	if (*column == kColumns) {
		tab.add(&row, UI::Align_Left);
		*column = 0;
		return true;
	}
	return false;
}

void BuildingStatisticsMenu::jump_building(JumpTarget target, bool reverse) {
	bool found = true;
	if (last_building_type_ != current_building_type_) {
		last_building_index_ = 0;
	}
	last_building_type_ = current_building_type_;

	const std::vector<Player::BuildingStats>& stats_vector =
	   iplayer().get_player()->get_building_statistics(current_building_type_);

	switch (target) {
	case JumpTarget::kOwned: {
		int32_t const curindex = last_building_index_;
		if (reverse) {
			while (validate_pointer(&(--last_building_index_), stats_vector.size()) != curindex) {
				if (!stats_vector[last_building_index_].is_constructionsite) {
					break;
				}
			}
		} else {
			while (validate_pointer(&(++last_building_index_), stats_vector.size()) != curindex) {
				if (!stats_vector[last_building_index_].is_constructionsite) {
					break;
				}
			}
		}
		break;
	}
	case JumpTarget::kConstruction: {
		int32_t const curindex = last_building_index_;
		if (reverse) {
			while (validate_pointer(&(--last_building_index_), stats_vector.size()) != curindex) {
				if (stats_vector[last_building_index_].is_constructionsite) {
					break;
				}
			}
		} else {
			while (validate_pointer(&(++last_building_index_), stats_vector.size()) != curindex) {
				if (stats_vector[last_building_index_].is_constructionsite) {
					break;
				}
			}
		}
		break;
	}
	case JumpTarget::kUnproductive: {
		const Map& map = iplayer().egbase().map();
		int32_t const curindex = last_building_index_;
		found = false;
		if (reverse) {
			while (validate_pointer(&(--last_building_index_), stats_vector.size()) != curindex) {
				if (!stats_vector[last_building_index_].is_constructionsite) {
					if (upcast(MilitarySite,
					           militarysite,
					           map[stats_vector[last_building_index_].pos].get_immovable())) {
						if (militarysite->stationed_soldiers().size() <
						    militarysite->soldier_capacity()) {
							found = true;
							break;
						}
					} else if (upcast(ProductionSite,
					                  productionsite,
					                  map[stats_vector[last_building_index_].pos].get_immovable())) {
						if (productionsite->is_stopped() ||
						    productionsite->get_statistics_percent() < low_production_) {
							found = true;
							break;
						}
					}
				}
			}
		} else {
			while (validate_pointer(&(++last_building_index_), stats_vector.size()) != curindex) {
				if (!stats_vector[last_building_index_].is_constructionsite) {
					if (upcast(MilitarySite,
					           militarysite,
					           map[stats_vector[last_building_index_].pos].get_immovable())) {
						if (militarysite->stationed_soldiers().size() <
						    militarysite->soldier_capacity()) {
							found = true;
							break;
						}
					} else if (upcast(ProductionSite,
					                  productionsite,
					                  map[stats_vector[last_building_index_].pos].get_immovable())) {
						if (productionsite->is_stopped() ||
						    productionsite->get_statistics_percent() < low_production_) {
							found = true;
							break;
						}
					}
				}
			}
		}
		if (!found) {  // Now look at the old
			if (upcast(MilitarySite,
			           militarysite,
			           map[stats_vector[last_building_index_].pos].get_immovable())) {
				if (militarysite->stationed_soldiers().size() < militarysite->soldier_capacity()) {
					found = true;
				}
			} else if (upcast(ProductionSite,
			                  productionsite,
			                  map[stats_vector[last_building_index_].pos].get_immovable())) {
				if (productionsite->is_stopped() ||
				    productionsite->get_statistics_percent() < low_production_) {
					found = true;
				}
			}
		}
		break;
	}
	default:
		assert(false);
		break;
	}

	if (found) {
		validate_pointer(&last_building_index_, stats_vector.size());
		iplayer().move_view_to(stats_vector[last_building_index_].pos);
	}
	low_production_reset_focus();
	update();
}

/*
 * Update this statistic
 */
void BuildingStatisticsMenu::think() {
	const Game& game = iplayer().game();
	int32_t const gametime = game.get_gametime();

	if ((gametime - lastupdate_) > kUpdateTime) {
		update();
		lastupdate_ = gametime;
	}
}

/*
 * validate if this pointer is ok
 */
int32_t BuildingStatisticsMenu::validate_pointer(int32_t* const id, int32_t const size) {
	if (*id < 0) {
		*id = size - 1;
	}
	if (size <= *id) {
		*id = 0;
	}

	return *id;
}

/*
 * Update Buttons
 */
void BuildingStatisticsMenu::update() {
	const Player& player = iplayer().player();
	const TribeDescr& tribe = player.tribe();
	const Map& map = iplayer().game().map();
	const BuildingIndex nr_buildings = tribe.get_nrbuildings();

	owned_label_.set_visible(false);
	no_owned_label_.set_visible(false);
	navigation_buttons_[NavigationButton::NextOwned]->set_visible(false);
	navigation_buttons_[NavigationButton::PrevOwned]->set_visible(false);
	construction_label_.set_visible(false);
	no_construction_label_.set_visible(false);
	navigation_buttons_[NavigationButton::NextConstruction]->set_visible(false);
	navigation_buttons_[NavigationButton::PrevConstruction]->set_visible(false);
	unproductive_box_.set_visible(false);
	unproductive_label_.set_visible(false);
	unproductive_percent_.set_visible(false);
	unproductive_label2_.set_visible(false);
	navigation_buttons_[NavigationButton::NextUnproductive]->set_visible(false);
	navigation_buttons_[NavigationButton::PrevUnproductive]->set_visible(false);

	for (BuildingIndex id = 0; id < nr_buildings; ++id) {
		const BuildingDescr& building = *tribe.get_building_descr(id);
		if (building_buttons_[id] == nullptr) {
			continue;
		}
		assert(productivity_labels_[id] != nullptr);
		assert(owned_labels_[id] != nullptr);

		const std::vector<Player::BuildingStats>& stats_vector = player.get_building_statistics(id);

		uint32_t nr_owned = 0;
		uint32_t nr_build = 0;
		uint32_t total_prod = 0;
		uint32_t total_soldier_capacity = 0;
		uint32_t total_stationed_soldiers = 0;
		uint32_t nr_unproductive = 0;

		for (uint32_t l = 0; l < stats_vector.size(); ++l) {
			if (stats_vector[l].is_constructionsite)
				++nr_build;
			else {
				++nr_owned;
				BaseImmovable& immovable = *map[stats_vector[l].pos].get_immovable();
				if (building.type() == MapObjectType::PRODUCTIONSITE ||
				    building.type() == MapObjectType::TRAININGSITE) {
					ProductionSite& productionsite = dynamic_cast<ProductionSite&>(immovable);
					int percent = productionsite.get_statistics_percent();
					total_prod += percent;

					if (percent < low_production_ || productionsite.is_stopped()) {
						++nr_unproductive;
					}
				} else if (building.type() == MapObjectType::MILITARYSITE) {
					MilitarySite& militarysite = dynamic_cast<MilitarySite&>(immovable);
					total_soldier_capacity += militarysite.soldier_capacity();
					total_stationed_soldiers += militarysite.stationed_soldiers().size();
					if (total_stationed_soldiers < total_soldier_capacity) {
						++nr_unproductive;
					}
				}
			}
		}

		if (building.type() == MapObjectType::PRODUCTIONSITE ||
		    building.type() == MapObjectType::TRAININGSITE) {
			if (nr_owned) {
				int const percent =
				   static_cast<int>(static_cast<float>(total_prod) / static_cast<float>(nr_owned));

				RGBColor color;
				if (percent < low_production_) {
					color = UI_FONT_CLR_BAD;
				} else if (percent < ((low_production_ < 50) ?
				                         2 * low_production_ :
				                         low_production_ + ((100 - low_production_) / 2))) {
					color = UI_FONT_CLR_OK;
				} else {
					color = UI_FONT_CLR_GOOD;
				}
				const std::string perc_str = (boost::format("%i%%") % percent).str();
				set_labeltext_autosize(productivity_labels_[id], perc_str, color);
			}
			if (has_selection_ && id == current_building_type_) {
				no_unproductive_label_.set_text(nr_unproductive > 0 ? std::to_string(nr_unproductive) :
				                                                      "");
				navigation_buttons_[NavigationButton::NextUnproductive]->set_enabled(nr_unproductive >
				                                                                     0);
				navigation_buttons_[NavigationButton::PrevUnproductive]->set_enabled(nr_unproductive >
				                                                                     0);
				navigation_buttons_[NavigationButton::NextUnproductive]->set_visible(true);
				navigation_buttons_[NavigationButton::PrevUnproductive]->set_visible(true);
				unproductive_label_.set_text(_("Low Production: "));
				unproductive_box_.set_visible(true);
				unproductive_label_.set_visible(true);
				unproductive_percent_.set_visible(true);
				unproductive_label2_.set_visible(true);
			}
		} else if (building.type() == MapObjectType::MILITARYSITE) {
			if (nr_owned) {
				RGBColor color;
				if (total_stationed_soldiers < total_soldier_capacity / 2) {
					color = UI_FONT_CLR_BAD;
				} else if (total_stationed_soldiers < total_soldier_capacity) {
					color = UI_FONT_CLR_OK;
				} else {
					color = UI_FONT_CLR_GOOD;
				}
				const std::string perc_str = (boost::format(_("%1%/%2%")) % total_stationed_soldiers %
				                              total_soldier_capacity).str();
				set_labeltext_autosize(productivity_labels_[id], perc_str, color);
			}
			if (has_selection_ && id == current_building_type_) {
				no_unproductive_label_.set_text(nr_unproductive > 0 ? std::to_string(nr_unproductive) :
				                                                      "");
				navigation_buttons_[NavigationButton::NextUnproductive]->set_enabled(
				   total_soldier_capacity > total_stationed_soldiers);
				navigation_buttons_[NavigationButton::PrevUnproductive]->set_enabled(
				   total_soldier_capacity > total_stationed_soldiers);
				navigation_buttons_[NavigationButton::NextUnproductive]->set_visible(true);
				navigation_buttons_[NavigationButton::PrevUnproductive]->set_visible(true);
				unproductive_label_.set_text(_("Missing soldiers:"));
				unproductive_box_.set_visible(true);
				unproductive_label_.set_visible(true);
			}
		} else {
			productivity_labels_[id]->set_text(" ");
			productivity_labels_[id]->set_visible(false);
		}

		std::string owned_text = "";
		if (!building.global() && (building.is_buildable() || building.is_enhanced())) {
			/** TRANSLATORS Buildings: owned / under construction */
			owned_text = (boost::format(_("%1%/%2%")) % nr_owned % nr_build).str();
		} else {
			owned_text = (boost::format(_("%1%/%2%")) % nr_owned % "–").str();
		}
		set_labeltext_autosize(owned_labels_[id], owned_text, UI_FONT_CLR_FG);
		owned_labels_[id]->set_visible((nr_owned + nr_build) > 0);

		building_buttons_[id]->set_enabled((nr_owned + nr_build) > 0);
		if (has_selection_ && id == current_building_type_) {
			no_owned_label_.set_text(nr_owned > 0 ? std::to_string(nr_owned) : "");
			navigation_buttons_[NavigationButton::NextOwned]->set_enabled(nr_owned > 0);
			navigation_buttons_[NavigationButton::PrevOwned]->set_enabled(nr_owned > 0);
			owned_label_.set_visible(true);
			no_owned_label_.set_visible(true);
			navigation_buttons_[NavigationButton::NextOwned]->set_visible(true);
			navigation_buttons_[NavigationButton::PrevOwned]->set_visible(true);
			if (!building.global() && building.is_buildable()) {
				no_construction_label_.set_text(nr_build > 0 ? std::to_string(nr_build) : "");
				navigation_buttons_[NavigationButton::NextConstruction]->set_enabled(nr_build > 0);
				navigation_buttons_[NavigationButton::PrevConstruction]->set_enabled(nr_build > 0);
				construction_label_.set_visible(true);
				no_construction_label_.set_visible(true);
				navigation_buttons_[NavigationButton::NextConstruction]->set_visible(true);
				navigation_buttons_[NavigationButton::PrevConstruction]->set_visible(true);
			}
		}
		building_buttons_[id]->set_tooltip(building.descname());
	}
}

void BuildingStatisticsMenu::set_labeltext_autosize(UI::MultilineTextarea* textarea,
                                                    const std::string& text,
                                                    const RGBColor& color) {
	const std::string formatted_str =
	   (boost::format("<rt><p font-face=condensed font-weight=bold "
	                  "font-size=%i font-color=%s>%s</p></rt>") %
	    (text.length() > 5 ? kLabelFontSize - floor(text.length() / 2) : kLabelFontSize) %
	    color.hex_value() % text).str();
	textarea->set_text(formatted_str);
	textarea->set_visible(true);
}

void BuildingStatisticsMenu::set_current_building_type(BuildingIndex id) {
	assert(building_buttons_[id] != nullptr);
	current_building_type_ = id;
	for (BuildingIndex i = 0; i < iplayer().player().tribe().get_nrbuildings(); ++i) {
		if (building_buttons_[i] != nullptr) {
			building_buttons_[i]->set_flat(true);
		}
	}
	building_buttons_[current_building_type_]->set_flat(false);
	building_buttons_[current_building_type_]->set_perm_pressed(true);
	building_name_.set_text(iplayer().player().tribe().get_building_descr(id)->descname());
	low_production_reset_focus();
	has_selection_ = true;
	update();
}

void BuildingStatisticsMenu::low_production_changed() {
	const std::string cutoff = unproductive_percent_.text();
	int number = std::atoi(cutoff.c_str());

	// Make sure that the user specified a correct number
	if (std::to_string(number) == cutoff && 0 <= number && number <= 100) {
		low_production_ = number;
		update();
	}
}

void BuildingStatisticsMenu::low_production_reset_focus() {
	unproductive_percent_.set_can_focus(false);
	unproductive_percent_.set_can_focus(true);
}
