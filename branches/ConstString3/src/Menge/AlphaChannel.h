/*
 *	AlphaChannelManager.h
 *
 *	Created by _Berserk_ on 17.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Config/Typedef.h"
#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include <map>

namespace Menge
{
	class AlphaChannel
	{
	public:
		AlphaChannel( size_t _width, size_t _height );

	public:
		unsigned char * createAlphaBuffer();
		bool releaseAlphaBuffer();

		void destroyAlphaBuffer();

		void makeMipMapLevel();

	public:
		unsigned char * getAlphaBuffer( size_t _level, size_t & _width, size_t & _height );

	protected:
		typedef std::vector<unsigned char*> TMipMapBuffers;
		TMipMapBuffers m_mipmap;

		size_t m_width;
		size_t m_height;

		size_t m_refcount;
	};
}	// namespace Menge
