/*
 * Copyright (C) 2017 by the Widelands Development Team
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

#include "graphic/text_layout.h"
#include "graphic/text/rendered_text.h"

namespace UI {


RenderedRect::RenderedRect(Recti init_point, const Image* init_image)
	: rect_(init_point), image_(init_image), locked_(false) {
}
/// RenderedRects are considered identical if they have the same position and dimensions.
bool RenderedRect::operator==(const RenderedRect& other) const {
	return get_x() == other.get_x() && get_y() == other.get_y() && width() == other.width() && height() == other.height();
}
const Image* RenderedRect::image() const {
	return image_.get();
}

int RenderedRect::get_x() const {
	return rect_.x;
}

int RenderedRect::get_y() const {
	return rect_.y;
}

int RenderedRect::width() const {
	return rect_.w;
}
int RenderedRect::height() const {
	return rect_.h;
}


int RenderedText::width() const {
	int result = 0;
	for (const auto& rect : texts) {
		result = std::max(result, rect->get_x() + rect->width());
	}
	return result;
}
int RenderedText::height() const {
	int result = 0;
	for (const auto& rect : texts) {
		result = std::max(result, rect->get_y() + rect->height());
	}
	return result;
}

void RenderedText::draw(RenderTarget& dst,
					const Vector2i& position,
					Recti region, Align align) const {

	Vector2i aligned_pos(position.x - region.x, position.y - region.y); // un-const the position and adjust for region
	UI::correct_for_align(align, region.w, &aligned_pos);
	for (const auto& rect : texts) {
		Vector2i blit_point(aligned_pos.x + rect->get_x(), aligned_pos.y + rect->get_y());
		dst.blit(blit_point, rect->image());
		// TODO(GunChleoc): Remove this line when testing is done.
		//dst.draw_rect(Rectf(blit_point.x, blit_point.y, rect->width(), rect->height()), RGBColor(100, 100, 100));
	}
}

void RenderedText::draw(RenderTarget& dst, const Vector2i& position, UI::Align align) const {
	draw(dst, position, Recti(0, 0, width(), height()), align);
}

std::unique_ptr<Texture> RenderedText::as_texture() const {
	std::unique_ptr<Texture> texture(new Texture(width(), height()));
	for (const auto& rect : texts) {
		const Rectf dest(rect->get_x(), rect->get_y(), rect->width(), rect->height());
		texture->blit(dest, *rect->image(), dest, 1., BlendMode::Copy);
	}
	return texture;
}

}  // namespace UI
