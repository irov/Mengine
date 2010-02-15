#	include "SceneManager.h"

#	include "XmlEngine.h"

#	include "ScriptEngine.h"

#	include "Entity.h"

#	include "LogEngine.h"

#	include "Animation.h"
#	include "Arrow.h"
#	include "ParticleEmitter.h"
#	include "HotSpot.h"
#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "TilePolygon.h"
#	include "Point.h"
#	include "SoundEmitter.h"
#	include "Sprite.h"
#	include "TextField.h"
#	include "TileMap.h"
#	include "Track.h"
#	include "Video.h"
#	include "Layer2D.h"
#	include "Layer2DLoop.h"
#	include "LayerScene.h"
#	include "RenderMesh.h"
#	include "Camera2D.h"
#	include "Camera3D.h"
#	include "Layer2DAccumulator.h"
#	include "Layer3D.h"
#	include "Window.h"
#	include "HotSpotImage.h"
#	include "Mesh_40_30.h"

#	define OBJECT_FACTORY( Type )\
	this->registerFactory( #Type, Type::genFactory() )

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node* SceneManager::createNode( const String& _type )
	{
		Node * node = FactoryManager::createObjectT<Node>( _type );

		if( node == 0 )
		{
			/*ScriptEngine * scriptEngine = Holder<ScriptEngine>::hostage();

			if( scriptEngine->isEntityType( _type ) )
			{
				node = scriptEngine->createEntity( _type );
			}*/
			MENGE_LOG_ERROR( "Invalid Node Type '%s'"
				, _type.c_str() 
				);

			return 0;
		}

		node->setType( _type );

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneManager::loadNode( Node *_node, const String& _pakName, const String& _filename )
	{
		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _pakName, _filename, _node, &Node::loader ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	class XmlNodeLoaderListener
		: public XmlElementListener
	{
	public:
		XmlNodeLoaderListener( Node ** _externalNode, SceneManager * _manager )
			: m_externalNode(_externalNode)
			, m_manager(_manager)
		{
		}

	public:
		void parseXML( XmlElement * _xml ) override
		{
			XML_SWITCH_NODE( _xml )
			{			
				XML_CASE_NODE( "Node" )
				{
					String name;
					String type;
					XML_FOR_EACH_ATTRIBUTES()
					{
						XML_CASE_ATTRIBUTE( "Name", name );
						XML_CASE_ATTRIBUTE( "Type", type );
					}

					*m_externalNode = m_manager->createNode( type );

					(*m_externalNode)->setName( name );

					XML_PARSE_ELEMENT( (*m_externalNode), &Node::loader );
				}
			}
		}

	protected:
		Node ** m_externalNode;	
		SceneManager * m_manager;
	};
	//////////////////////////////////////////////////////////////////////////
	Node * SceneManager::createNodeFromXml( const String& _pakName, const String& _filename )
	{
		Node * node = 0;

		XmlNodeLoaderListener * nodeLoader = new XmlNodeLoaderListener( &node, this );

		if(  Holder<XmlEngine>::hostage()
			->parseXmlFile( _pakName, _filename, nodeLoader ) == false )
		{
			MENGE_LOG_ERROR( "Invalid parse external node '%s'"
				, _filename.c_str() 
				);

			return 0;
		}

		if( node == 0 )
		{
			MENGE_LOG_ERROR( "This xml file '%s' has invalid external node format"
				, _filename.c_str() 
				);
		}

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * SceneManager::createNodeFromXmlData( const String& _xml_data )
	{
		Node * node = 0;

		XmlNodeLoaderListener * nodeLoader = new XmlNodeLoaderListener( &node,this );

		if(  Holder<XmlEngine>::hostage()
			->parseXmlString( _xml_data, nodeLoader ) == false )
		{
			MENGE_LOG_ERROR( "Invalid parse external xml data '%s'"
				, _xml_data.c_str() 
				);

			return 0;
		}

		if( node == 0 )
		{
			MENGE_LOG_ERROR( "This xml have invalid external node format" );
		}

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneManager::initialize()
	{
		MENGE_LOG( "Creating Object Factory..." );
		OBJECT_FACTORY( Entity );
		OBJECT_FACTORY( Animation );
		OBJECT_FACTORY( Arrow );
		OBJECT_FACTORY( ParticleEmitter );
		OBJECT_FACTORY( HotSpot );
		OBJECT_FACTORY( Light2D );
		OBJECT_FACTORY( ShadowCaster2D );
		OBJECT_FACTORY( TilePolygon );
		OBJECT_FACTORY( Point );
		OBJECT_FACTORY( RigidBody2D );
		OBJECT_FACTORY( SoundEmitter );
		OBJECT_FACTORY( Sprite );
		OBJECT_FACTORY( TextField );
		OBJECT_FACTORY( TileMap );
		OBJECT_FACTORY( Track );
		OBJECT_FACTORY( Video );
		OBJECT_FACTORY( Layer2D );
		OBJECT_FACTORY( Layer2DLoop );
		OBJECT_FACTORY( Layer2DAccumulator );
		OBJECT_FACTORY( Layer3D );
		OBJECT_FACTORY( LayerScene );
		OBJECT_FACTORY( RenderMesh );
		OBJECT_FACTORY( Camera2D );
		OBJECT_FACTORY( Camera3D );
		OBJECT_FACTORY( SceneNode3D );
		OBJECT_FACTORY( Window );
		OBJECT_FACTORY( HotSpotImage );
		OBJECT_FACTORY( Mesh_40_30 );
	}
	//////////////////////////////////////////////////////////////////////////
}
