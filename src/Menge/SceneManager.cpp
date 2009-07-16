#	include "SceneManager.h"

#	include "XmlEngine.h"

#	include "ScriptEngine.h"

#	include "Entity.h"

#	include "LogEngine.h"

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
			MENGE_LOG_ERROR( "Invalid Node Type \"%s\"", _type.c_str() );
		}
		else
		{
			node->setType( _type );
		}

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneManager::loadNode( Node *_node, const String& _xml )
	{
		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _xml, _node, &Node::loader ) == false )
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
	Node * SceneManager::createNodeFromXml( const String& _xml )
	{
		Node * node = 0;

		XmlNodeLoaderListener * nodeLoader = new XmlNodeLoaderListener( &node, this );

		if(  Holder<XmlEngine>::hostage()
			->parseXmlFile( _xml, nodeLoader ) == false )
		{
			MENGE_LOG_ERROR( "Invalid parse external node \"%s\""
				, _xml.c_str() );

			return 0;
		}

		if( node == 0 )
		{
			MENGE_LOG_ERROR( "This xml file \"%s\" has invalid external node format"
				, _xml.c_str() );
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
			MENGE_LOG_ERROR( "Invalid parse external xml data \"%s\""
				, _xml_data.c_str() );
			return 0;
		}

		if( node == 0 )
		{
			MENGE_LOG_ERROR( "This xml have invalid external node format" );
		}

		return node;
	}
}
