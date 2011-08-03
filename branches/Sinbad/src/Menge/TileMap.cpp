#	include "TileMap.h" 

#	include "ObjectImplement.h"
#	include "XmlEngine.h"
#	include "ResourceManager.h"
#	include "LogEngine.h"
#	include "ResourceTileMap.h"
#	include "ResourceTileSet.h"
#	include "RenderEngine.h"
#	include "RigidBody2D.h"
#	include "SceneManager.h"
#	include "Player.h"
#	include "Camera2D.h"
namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(TileMap);
	//////////////////////////////////////////////////////////////////////////
	TileMap::TileMap()
		: m_resourceMap( NULL )
		, m_resourcename( "" )
		, m_width( 0 )
		, m_height( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TileMap::~TileMap()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void TileMap::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "TileMap", "Name", m_resourcename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool TileMap::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		m_resourceMap = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceTileMap>( m_resourcename );

		if( m_resourceMap == NULL )
		{
			MENGE_LOG_ERROR( "TileMap::_compile -> compiling resource '%s' failed"
				, m_resourcename.c_str()
				);

			return false;
		}

		m_width = m_resourceMap->getWidth();
		m_height = m_resourceMap->getHeight();

		RigidBody2D* collision = Holder<SceneManager>::hostage()
			->createNodeT<RigidBody2D>( "RigidBody2D" ) ;

		const std::vector< mt::vec2f >& pos = m_resourceMap->_getPhysPos();
		float width = m_resourceMap->_getPhysWidth();
		collision->setName( "WorldPhysObject" );
		for( std::vector< mt::vec2f >::size_type i = 0; i < pos.size(); i++ )
		{
			collision->addShapeBox_( width, width, pos[i], 0.0f );
		}
		collision->compile();
		addChildren( collision );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TileMap::_release()
	{
		Node::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceMap );

		m_resourceMap = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void TileMap::_render( Camera2D * _camera )
	{
		const Viewport& viewport = Holder<Player>::hostage()->getRenderCamera2D()->getViewport();

		const mt::mat3f & wm = getWorldMatrix();
		float tileSize = m_resourceMap->getTileSize();

		std::size_t init_i = std::size_t( ( viewport.begin.x > 0.f ) ? viewport.begin.x / tileSize : 0.f );
		std::size_t init_j = std::size_t( ( viewport.begin.y > 0.f ) ? viewport.begin.y / tileSize : 0.f );
		std::size_t end_i = std::size_t( viewport.end.x / tileSize + 1 );
		std::size_t end_j = std::size_t( viewport.end.y / tileSize + 1 );
		
		if( end_i > m_width ) 
		{
			end_i = m_width;
		}

		if( end_j > m_height ) 
		{
			end_j = m_height;
		}

		for( std::size_t i = init_i; i < end_i; i++ )
		{
			for( std::size_t j = init_j; j < end_j; j++ )
			{
				ImageBlock tile = m_resourceMap->getTile( i, j );

				mt::vec2f offset( i * tileSize, j * tileSize );
				mt::vec2f size( tileSize, tileSize );

				mt::vec2f renderVertex[4];

				mt::mul_v2_m3( renderVertex[0], offset, wm );
				mt::mul_v2_m3( renderVertex[1], offset + mt::vec2f( size.x, 0.0f ), wm );
				mt::mul_v2_m3( renderVertex[2], offset + size, wm );
				mt::mul_v2_m3( renderVertex[3], offset + mt::vec2f( 0.0f, size.y ), wm );

				//Holder<RenderEngine>::hostage()->renderImage(
				//	renderVertex,
				//	tile.uv,
				//	0xFFFFFFFF,
				//	tile.image
				//	);

			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TileMap::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );
		const mt::mat3f & wm = getWorldMatrix();
		float width = m_resourceMap->getWidth() * m_resourceMap->getTileSize();
		float height = m_resourceMap->getHeight() * m_resourceMap->getTileSize();
		mt::vec2f lowerRight;
		mt::mul_v2_m3( lowerRight, mt::vec2f( width, height ), wm );
		mt::add_internal_point( _boundingBox, lowerRight );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namescape Menge
