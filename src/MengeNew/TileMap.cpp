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

		const std::vector<mt::vec2f> & pos = m_resourceMap->_getPhysPos();
		float width = m_resourceMap->_getPhysWidth();
		collision->setName( "WorldPhysObject" );
		for( std::vector<mt::vec2f>::size_type i = 0; i < pos.size(); i++ )
		{
			collision->addShapeBox( width, width, pos[i], 0.0f );
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
	void TileMap::_render(  const Viewport & _viewport  )
	{
		const mt::mat3f & wm = getWorldMatrix();
		float tileSize = m_resourceMap->getTileSize();

		int init_i = _viewport.begin.x / tileSize;
		int init_j = _viewport.begin.y / tileSize;
		if( init_i < 0 ) init_i = 0;
		if( init_j < 0 ) init_j = 0;
		int end_i = _viewport.end.x / tileSize + 1;
		int end_j = _viewport.end.y / tileSize + 1;
		if( end_i > m_width ) end_i = m_width;
		if( end_j > m_height ) end_j = m_height;

		for( int i = init_i; i < end_i; i++ )
		{
			for( int j = init_j; j < end_j; j++ )
			{
				ImageBlock tile = m_resourceMap->getTile( i, j );

				Holder<RenderEngine>::hostage()->renderImage(
					wm, 
					mt::vec2f( i * (tileSize), j * (tileSize) ),
					tile.uv,
					mt::vec2f( tileSize, tileSize ),
					0xffffffff,
					tile.image,
					BF_SOURCE_ALPHA,
					BF_ONE_MINUS_SOURCE_ALPHA);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TileMap::_updateBoundingBox( mt::box2f & _localBoundingBox )
	{
		float tileSize = m_resourceMap->getTileSize();

		mt::set_box_from_min_max( _localBoundingBox, mt::vec2f(0.f,0.f), mt::vec2f( tileSize * m_width, tileSize * m_height ) );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namescape Menge