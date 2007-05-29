#	include "ResourceImageMNGManager.h"

#	include "ResourceImageMNG.h"

#	include "Holder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceImageMNGManager::ResourceImageMNGManager()
	{
		Holder<ResourceImageMNGManager>::keep( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageMNGManager::registerResource( ResourceImageMNG * _resource )
	{
		if( _resource == 0 )
		{
			return;
		}

		const std::string & name = _resource->getName();

		TMapResourceSprite::iterator it_find = m_mapResourceSprite.find( name );

		if( it_find == m_mapResourceSprite.end() )
		{
			m_mapResourceSprite.insert( std::make_pair( name, _resource ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImageMNG * ResourceImageMNGManager::getResource( const std::string & _name )
	{
		TMapResourceSprite::iterator it_find = m_mapResourceSprite.find( _name );

		if( it_find == m_mapResourceSprite.end() )
		{
			return 0;
		}

		ResourceImageMNG * sprite = it_find->second;
		
		if( sprite->refCount() == 0 )
		{
			sprite->compile();
		}

		sprite->incRef();

		return sprite;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageMNGManager::releaseResource( ResourceImageMNG * _resource )
	{
		if( _resource == 0 )
		{
			return;
		}

		const std::string & name = _resource->getName();

		TMapResourceSprite::iterator it_find = m_mapResourceSprite.find( name );

		if( it_find != m_mapResourceSprite.end() )
		{
			ResourceImageMNG * sprite = it_find->second;
			size_t refcounter = sprite->decRef();

			if( refcounter == 0 )
			{
				sprite->release();
			}
		}
	}
}