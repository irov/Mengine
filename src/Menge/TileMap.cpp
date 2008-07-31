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
	OBJECT_IMPLEMENT( TileMap );
	//////////////////////////////////////////////////////////////////////////
	TileMap::TileMap()
		: m_resourceMap( NULL )
		, m_resourcename("")
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
			MENGE_LOG( "TileMap::_compile -> compiling resource %s failed", m_resourcename.c_str() );
			return false;
		}

		m_width = m_resourceMap->getWidth();
		m_height = m_resourceMap->getHeight();

		RigidBody2D* collision = SceneManager::createNodeT<RigidBody2D>( "RigidBody2D" ) ;
		const std::vector< mt::vec2f >& pos = m_resourceMap->_getPhysPos();
		float width = m_resourceMap->_getPhysWidth();
		collision->setName( "WorldPhysObject" );
		for( std::vector< mt::vec2f >::size_type i = 0; i < pos.size(); i++ )
		{
			collision->_addShapeBox( width, width, pos[i], 0.0f );
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
	void TileMap::_render( const Viewport & _viewport, bool _enableDebug )
	{
		//const Viewport& viewport = Holder<RenderEngine>::hostage()->getRenderViewport();
		const Viewport& viewport = Holder<Player>::hostage()->getRenderCamera2D()->getViewport();

		const mt::mat3f & wm = getWorldMatrix();
		float tileSize = m_resourceMap->getTileSize();

		unsigned int init_i = ( viewport.begin.x > 0.f ) ? viewport.begin.x / tileSize : 0.f;
		unsigned int init_j = ( viewport.begin.y > 0.f ) ? viewport.begin.y / tileSize : 0.f;
		unsigned int end_i = viewport.end.x / tileSize + 1;
		unsigned int end_j = viewport.end.y / tileSize + 1;
		if( end_i > m_width ) end_i = m_width;
		if( end_j > m_height ) end_j = m_height;

		for( int i = init_i; i < end_i; i++ )
		{
			for( int j = init_j; j < end_j; j++ )
			{
				ImageBlock tile = m_resourceMap->getTile( i, j );

				mt::vec2f offset( i * tileSize, j * tileSize );
				mt::vec2f size( tileSize, tileSize );
				Holder<RenderEngine>::hostage()->renderImage(
					wm,
					offset,
					offset + mt::vec2f( size.x, 0.0f ),
					offset + size,
					offset + mt::vec2f( 0.0f, size.y ),
					tile.uv,
					0xFFFFFFFF,
					tile.image
					);

			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namescape Menge