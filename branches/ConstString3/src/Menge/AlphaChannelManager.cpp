/*
 *	AlphaChannelManager.cpp
 *
 *	Created by _Berserk_ on 17.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "AlphaChannelManager.h"
#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AlphaChannelManager::AlphaChannelManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AlphaChannelManager::~AlphaChannelManager()
	{
		for( TBufferMap::iterator it = m_bufferMap.begin(), it_end = m_bufferMap.end();
			it != it_end;
			++it )
		{
			delete [] it->second.buffer;
		}
		m_bufferMap.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* AlphaChannelManager::createAlphaBuffer( const ConstString& _name, size_t _width, size_t _height )
	{
		TBufferMap::iterator it_find = m_bufferMap.find( _name );
		if( it_find != m_bufferMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (AlphaChannelManager::createAlphaBuffer) buffer with name '%s' already exists"
				, _name.c_str() 
				);

			return NULL;
		}
		AlphaBuffer ab;
		ab.buffer = new unsigned char[_width*_height];
		ab.ref_count = 1;
		m_bufferMap.insert( std::make_pair( _name, ab ) );
		return ab.buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* AlphaChannelManager::getAlphaBuffer( const ConstString& _name )
	{
		TBufferMap::iterator it_find = m_bufferMap.find( _name );
		if( it_find != m_bufferMap.end() )
		{
			it_find->second.ref_count += 1;
			return it_find->second.buffer;
		}
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void AlphaChannelManager::releaseAlphaBuffer( const ConstString& _name )
	{
		TBufferMap::iterator it_find = m_bufferMap.find( _name );
		if( it_find == m_bufferMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (AlphaChannelManager::deleteAlphaBuffer) buffer with name '%s' does not exist"
				, _name.c_str() 
				);

			return;
		}
		it_find->second.ref_count -= 1;
		if( it_find->second.ref_count == 0 )
		{
			delete it_find->second.buffer;
			m_bufferMap.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AlphaChannelManager::deleteAlphaBuffer( const ConstString& _name )
	{
		TBufferMap::iterator it_find = m_bufferMap.find( _name );
		if( it_find == m_bufferMap.end() )
		{
			MENGE_LOG_ERROR( "Error: (AlphaChannelManager::deleteAlphaBuffer) buffer with name '%s' does not exist"
				, _name.c_str()
				);

			return;
		}
		delete it_find->second.buffer;
		m_bufferMap.erase( it_find );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
