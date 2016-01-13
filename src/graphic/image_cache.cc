/*
 * Copyright (C) 2006-2012 by the Widelands Development Team
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "graphic/image_cache.h"

#include <cassert>
#include <memory>
#include <set>
#include <string>

#include <SDL.h>
#include <boost/format.hpp>

#include "graphic/image.h"
#include "graphic/image_io.h"
#include "graphic/texture.h"

ImageCache::ProxyImage::ProxyImage(std::unique_ptr<const Image> original_image)
   : image_(std::move(original_image)) {
}

const Image& ImageCache::ProxyImage::image() {
	return *image_;
}

void ImageCache::ProxyImage::set_image(std::unique_ptr<const Image> original_image) {
	image_ = std::move(original_image);
}

int ImageCache::ProxyImage::width() const {
	return image_->width();
}

int ImageCache::ProxyImage::height() const {
	return image_->height();
}

const BlitData& ImageCache::ProxyImage::blit_data() const {
	return image_->blit_data();
}

ImageCache::ImageCache() {
}

ImageCache::~ImageCache() {
}

bool ImageCache::has(const std::string& hash) const {
	return images_.count(hash);
}

const Image* ImageCache::insert(const std::string& hash, std::unique_ptr<const Image> image) {
	assert(!has(hash));
	const Image* return_value = image.get();
	images_.insert(make_pair(hash, std::unique_ptr<ProxyImage>(new ProxyImage(std::move(image)))));
	return return_value;
}

const Image* ImageCache::get(const std::string& hash) {
	ImageMap::const_iterator it = images_.find(hash);
	if (it == images_.end()) {
		images_.insert(
		   make_pair(hash, std::unique_ptr<ProxyImage>(new ProxyImage(load_image(hash)))));
		return get(hash);
	}
	return it->second.get();
}
