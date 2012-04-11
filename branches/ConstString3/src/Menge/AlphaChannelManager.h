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
	class ResourceImage;

	class AlphaChannelManager
		: public Holder<AlphaChannelManager>
	{
	public:
		AlphaChannelManager();
		~AlphaChannelManager();
	
	public:		
		unsigned char* getAlphaBuffer( const ConstString& _name, ResourceImage * _resourceImage, size_t _level, size_t & _width, size_t & _height );	// increases ref counter
		void releaseAlphaBuffer( const ConstString& _name );			// decrease ref counter

	protected:
		bool makeAlphaBuffer_( const ConstString& _name, ResourceImage * _resourceImage );

	protected:
		struct AlphaBuffer
		{
			typedef std::vector<unsigned char*> TMipMapBuffers;
			TMipMapBuffers mipmap;

			size_t width;
			size_t height;

			size_t ref_count;
		};

		typedef std::map<ConstString, AlphaBuffer> TBufferMap;
		TBufferMap m_bufferMap;

	private:
		AlphaBuffer & createAlphaBuffer_( const ConstString& _name, size_t _width, size_t _height );
		void destroyAlphaBuffer_( AlphaBuffer & _buffer ) const;
	};
}	// namespace Menge
