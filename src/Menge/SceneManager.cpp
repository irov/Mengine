#	include "SceneManager.h"

#	include "XmlEngine.h"

#	include "NodeFactory.h"

#	include "FileEngine.h"

#	include "ScriptEngine.h"

#	include "Entity.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node* SceneManager::createNode( const std::string & _type )
	{
		FactoryGenStruct gs;
		gs.type = _type;

		Node * node = TFactoryNode::generate( _type, gs );

		if( node )
		{
			return node;
		}

		ScriptEngine * scriptEngine = Holder<ScriptEngine>::hostage();
		
		if( scriptEngine->isEntityType( _type ) )
		{
			node = scriptEngine->createEntity( _type );
		}
		
		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneManager::loadNode(Node *_node, const std::string &_xml)
	{
		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _xml, _node, &Node::loader ) == false )
		{
			return false;
		}
		//TiXmlDocument * document = Holder<FileEngine>::hostage()
		//	->loadXml( _xml );

		//XML_FOR_EACH_DOCUMENT( document )
		//{
		//	XML_CHECK_NODE("Node")
		//	{
		//		_node->loader(XML_CURRENT_NODE);
		//	}
		//}
		//XML_INVALID_PARSE()
		//{
		//	return false;
		//}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	class XmlNodeLoaderListener
		: public XmlElementListener
	{
	public:
		XmlNodeLoaderListener( Node ** _externalNode )
			: m_externalNode( _externalNode )
		{
		}

	public:
		void parseXML( XmlElement * _xml ) override
		{
			XML_SWITCH_NODE( _xml )
			{			
				XML_CASE_NODE("Node")
				{
					std::string name;
					std::string type;
					XML_FOR_EACH_ATTRIBUTES()
					{
						XML_CASE_ATTRIBUTE( "Name", name );
						XML_CASE_ATTRIBUTE( "Type", type );
					}

					*m_externalNode = SceneManager::createNode( type );

					(*m_externalNode)->setName( name );

					XML_PARSE_ELEMENT( (*m_externalNode), &Node::loader );
				}
			}
		}

	protected:
		Node ** m_externalNode;	
	};
	//////////////////////////////////////////////////////////////////////////
	Node * SceneManager::createNodeFromXml( const std::string & _xml )
	{
		Node * node = 0;

		XmlNodeLoaderListener * nodeLoader = new XmlNodeLoaderListener( &node );

		if(  Holder<XmlEngine>::hostage()
			->parseXmlFile( _xml, nodeLoader ) == false )
		{
			MENGE_LOG("Invalid parse external node `%s`\n", _xml.c_str() );

			return 0;
		}

		if( node == 0 )
		{
			MENGE_LOG("This xml file `%s` have invalid external node format\n", _xml.c_str() );
		}

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * SceneManager::createNodeFromXmlData( const std::string & _xml_data )
	{
		Node * node = 0;

		XmlNodeLoaderListener * nodeLoader = new XmlNodeLoaderListener( &node );

		if(  Holder<XmlEngine>::hostage()
			->parseXmlString( _xml_data, nodeLoader ) == false )
		{
			MENGE_LOG("Invalid parse external xml data `%s`\n", _xml_data.c_str() );

			return 0;
		}

		if( node == 0 )
		{
			MENGE_LOG("This xml have invalid external node format\n" );
		}

		return node;
	}

}