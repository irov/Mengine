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
		unsigned char* createAlphaBuffer( const ConstString& _name, size_t _width, size_t _height );
		unsigned char* getAlphaBuffer( const ConstString& _name, ResourceImage * _resourceImage, size_t _frame, size_t & _width, size_t & _height );	// increases ref counter
		void releaseAlphaBuffer( const ConstString& _name );			// decrease ref counter

	protected:
		bool makeAlphaBuffer_( const ConstString& _name, ResourceImage * _resourceImage, size_t _frame );

	protected:
		struct AlphaBuffer
		{
			unsigned char* buffer;
			size_t width;
			size_t height;

			size_t ref_count;
		};

		typedef std::map<ConstString, AlphaBuffer> TBufferMap;
		TBufferMap m_bufferMap;
	};
}	// namespace Menge
