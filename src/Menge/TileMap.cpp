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
		SceneNode2D::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "TileMap", "Name", m_resourcename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool TileMap::_compile()
	{
		if( SceneNode2D::_compile() == false )
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

		RigidBody2D* collision = dynamic_cast<RigidBody2D*>(SceneManager::createNodeFromXmlData( m_resourceMap->getPhysXml() ) );
		collision->compile();
		addChildren( collision );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TileMap::_release()
	{
		SceneNode2D::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceMap );

		m_resourceMap = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void TileMap::_render()
	{
		const Viewport& viewport = Holder<RenderEngine>::hostage()->getRenderViewport();
		// TODO: viewport filtering
		const mt::mat3f & wm = getWorldMatrix();
		float tileSize = m_resourceMap->getTileSize();

		for( unsigned int i = 0; i < m_width; i++ )
		{
			for( unsigned int j = 0; j < m_height; j++ )
			{
				ImageBlock tile = m_resourceMap->getTile( i, j );

				Holder<RenderEngine>::hostage()->renderImage(
					wm, 
					mt::vec2f( i * tileSize, j * tileSize ),
					tile.uv,
					mt::vec2f( tileSize + 1, tileSize + 1 ),
					0xffffffff,
					tile.image,
					BF_SOURCE_ALPHA,
					BF_ONE_MINUS_SOURCE_ALPHA);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namescape Menge