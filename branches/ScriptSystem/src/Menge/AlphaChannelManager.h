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
#	include <map>

namespace Menge
{
	class AlphaChannelManager
		: public Holder<AlphaChannelManager>
	{
	public:
		AlphaChannelManager();
		~AlphaChannelManager();
	
	public:
		unsigned char* createAlphaBuffer( const String& _name, size_t _width, size_t _height );
		unsigned char* getAlphaBuffer( const String& _name );	// increases ref counter
		void releaseAlphaBuffer( const String& _name );			// decrease ref counter
		void deleteAlphaBuffer( const String& _name );			// explicitly delete buffer

	protected:
		struct AlphaBuffer
		{
			unsigned char* buffer;
			size_t ref_count;
		};
		typedef std::map<String, AlphaBuffer> TBufferMap;
		TBufferMap m_bufferMap;
	};
}	// namespace Menge
