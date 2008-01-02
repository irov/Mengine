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
		Node *node = NodeFactory::genNode(_type);

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
		: public XmlListener
	{
	public:
		XmlNodeLoaderListener()
			: m_node(0)
		{
		}

	public:
		Node * result()
		{
			return m_node;
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

					m_node = SceneManager::createNode( type );

					m_node->setName( name );

					XML_PUSH_CLASS_LISTENER( m_node, &Node::loader );
				}
			}
		}

	protected:
		Node * m_node;
	};
	//////////////////////////////////////////////////////////////////////////
	Node * SceneManager::createNodeFromXml( const std::string & _xml )
	{
		XmlNodeLoaderListener nodeLoader;

		if(  Holder<XmlEngine>::hostage()
			->parseXmlFile( _xml, &nodeLoader ) == false )
		{
			MENGE_LOG("Invalid parse external node `%s`\n", _xml.c_str() );

			return 0;
		}

		Node * node = nodeLoader.result();

		if( node == 0 )
		{
			MENGE_LOG("This xml file `%s` have invalid external node format\n", _xml.c_str() );
		}

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * SceneManager::createNodeFromXmlData( const std::string & _xml_data )
	{
		XmlNodeLoaderListener nodeLoader;

		if(  Holder<XmlEngine>::hostage()
			->parseXmlBuffer( _xml_data, &nodeLoader ) == false )
		{
			MENGE_LOG("Invalid parse external xml data `%s`\n", _xml_data.c_str() );

			return 0;
		}

		Node * node = nodeLoader.result();

		if( node == 0 )
		{
			MENGE_LOG("This xml have invalid external node format\n" );
		}

		return node;
	}

}