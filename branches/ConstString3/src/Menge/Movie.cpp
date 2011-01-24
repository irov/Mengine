#	include "Movie.h"

#	include "ResourceManager.h"
#	include "ResourceMovie.h"

#	include "ResourceImageDefault.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "BinParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Movie::Movie()
		: m_timming(0.f)
		, m_play(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::setResourceMovie( const ConstString & _resourceName )
	{
		m_resourceMovieName = _resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Movie::getResourceMovie() const
	{
		return m_resourceMovieName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::run()
	{
		m_play = true;
		m_timming = 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::stop()
	{
		m_play = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Movie_Name, &Movie::setResourceMovie );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		m_play = false;
		m_timming = 0.f;

		m_resourceMovie = ResourceManager::get()
			->getResourceT<ResourceMovie>( m_resourceMovieName );

		if( m_resourceMovie == 0 )
		{
			MENGE_LOG_ERROR("Movie can't setup resource '%s'"
				, m_resourceMovieName.c_str()
				);

			return false;
		}

		std::size_t layerSize = m_resourceMovie->getLayerSize();

		for( std::size_t i = 0; i != layerSize; ++i )
		{
			const MovieLayer & layer = m_resourceMovie->getLayer( i );

			String movieImageResource = "MovieLayerImage";
			movieImageResource += layer.name;

			ResourceImageDefault * layer_resource = ResourceManager::get()
				->createResourceT<ResourceImageDefault>( movieImageResource, Consts::get()->c_ResourceImageDefault );

			layer_resource->addImagePath( layer.path );

			Sprite * layer_sprite = NodeManager::get()
				->createNodeT<Sprite>( layer.name, Consts::get()->c_Sprite, "Image" );

			layer_sprite->setImageResource( movieImageResource );
			
			layer_sprite->disable();

			if( layer_sprite->compile() == false )
			{
				return false;
			}

			Affector

			this->addChildren( layer_sprite );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_release()
	{
		ResourceManager::get()
			->releaseResource( m_resourceMovie );

		m_resourceMovie = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie::_update( float _timing )
	{
		if( m_play == false )
		{
			return;
		}

		m_timming += _timing;

		std::size_t layerSize = m_resourceMovie->getLayerSize();

		for( std::size_t i = 0; i != layerSize; ++i )
		{
			const MovieLayer & layer = m_resourceMovie->getLayer(i);
			Sprite * sprite = m_sprites[i];

			if( layer.in > m_timming && layer.out < m_timming )
			{
				sprite->disable();
				continue;
			}

			sprite->enable();

			MovieLayer::TVectorKeyAnchorPoints::size_type size_anchorPoints = layer.anchorPoints.size();

			if( size_anchorPoints == 1 )
			{
				const MovieLayer::KeyAnchorPoint & key = layer.anchorPoints[0];

				sprite->setOrigin( key.value );
			}
			else
			{
				for( MovieLayer::TVectorKeyAnchorPoints::size_type 
					it = 0,
					it_end = layer.anchorPoints.size();
				it != it_end;
				++it )
				{
					it->time
				}
			}
		}
	}
}