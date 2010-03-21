#	include "NodeManager.h"

#	include "XmlEngine.h"

#	include "ScriptEngine.h"

#	include "Logger/Logger.h"

#	include "Node.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node* NodeManager::createNode( const String& _type )
	{
		Node * node = FactoryManager::createObjectT<Node>( _type );

		if( node == 0 )
		{
			MENGE_LOG_ERROR( "Invalid Node Type '%s'"
				, _type.c_str() 
				);

			return 0;
		}

		node->setType( _type );

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeManager::loadNode( Node *_node, const String& _pakName, const String& _filename )
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
		XmlNodeLoaderListener( Node ** _externalNode, NodeManager * _manager )
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

					if( *m_externalNode == 0 )
					{
						MENGE_LOG_ERROR( "Error: can't create node '%s' invalid type '%s'"
							, name.c_str()
							, type.c_str()
							);

						XML_CASE_SKIP();
					}

					(*m_externalNode)->setName( name );

					XML_PARSE_ELEMENT( (*m_externalNode), &Node::loader );
				}
			}
		}

	protected:
		Node ** m_externalNode;	
		NodeManager * m_manager;
	};
	//////////////////////////////////////////////////////////////////////////
	Node * NodeManager::createNodeFromXml( const String& _pakName, const String& _filename )
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
	Node * NodeManager::createNodeFromXmlData( const String& _xml_data )
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
}
