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
	class AlphaChannel;

	class AlphaChannelManager
		: public Holder<AlphaChannelManager>
	{
	public:
		AlphaChannelManager();
		~AlphaChannelManager();
	
	public:		
		AlphaChannel * getAlphaChannel( const ConstString& _name, ResourceImage * _resourceImage );
		void releaseAlphaBuffer( const ConstString& _name );

	protected:
		AlphaChannel * makeAlphaChannel_( const ConstString& _name, ResourceImage * _resourceImage );

	protected:
		typedef std::map<ConstString, AlphaChannel *> TBufferMap;
		TBufferMap m_bufferMap;
	};
}	// namespace Menge
