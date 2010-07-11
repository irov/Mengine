#	include "NodeManager.h"

#	include "LoaderEngine.h"
#	include "ScriptEngine.h"

#	include "BinParser.h"

#	include "Logger/Logger.h"


#	include "Node.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	NodeManager::NodeManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Node * NodeManager::createNode( const ConstString& _name, const ConstString& _type )
	{
		Node * node = this->createNode( _type );

		if( node == 0 )
		{
			return 0;
		}

		node->setName( _name );

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	Node* NodeManager::createNode( const ConstString& _type )
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
	bool NodeManager::loadNode( Node *_node, const ConstString& _pakName, const String& _filename )
	{
		
		if( LoaderEngine::get()
			->load( _pakName, _filename, _node ) == false )
		//if( XmlEngine::get()
		//	->parseXmlFileM( _pakName, _filename, _node, &Node::loader ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace 
	{
		class NodeLoaderListener
			: public Loadable
		{
		public:
			NodeLoaderListener( Node ** _externalNode, NodeManager * _manager )
				: m_externalNode(_externalNode)
				, m_manager(_manager)
			{
			}

		public:
			void loader( BinParser * _parser ) override
			{
				BIN_SWITCH_ID(_parser)
				{
					BIN_CASE_NODE( Protocol::Node )
					{
						ConstString name;
						ConstString type;

						BIN_FOR_EACH_ATTRIBUTES()
						{
							BIN_CASE_ATTRIBUTE( Protocol::Node_Name, name );
							BIN_CASE_ATTRIBUTE( Protocol::Node_Type, type );
						}

						*m_externalNode = m_manager->createNode( name, type );

						if( *m_externalNode == 0 )
						{
							MENGE_LOG_ERROR( "Error: can't create node '%s' invalid type '%s'"
								, name.c_str()
								, type.c_str()
								);

							BIN_SKIP();
						}

						BIN_PARSE_ELEMENT( (*m_externalNode), &Node::loader );
					}
				}
			}

			void _loaded() override
			{
				if( m_externalNode == 0 )
				{
					return;
				}

				if( *m_externalNode == 0 )
				{
					return;
				}

				(*m_externalNode)->_loaded();
			}

		protected:
			Node ** m_externalNode;	
			NodeManager * m_manager;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	Node * NodeManager::createNodeFromXml( const ConstString& _pakName, const String& _filename )
	{
		Node * node = 0;

		NodeLoaderListener * listener = new NodeLoaderListener( &node, this );

		if(  LoaderEngine::get()
			->loader( _pakName, _filename, listener ) == false )
		//if(  XmlEngine::get()
		//	->parseXmlFile( _pakName, _filename, nodeLoader ) == false )
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

			return 0;
		}

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * NodeManager::createNodeFromXmlData( const ConstString & _binResource )
	{
	
		ResourceBinary * binary = ResourceManager::get()
			->getResourceT<ResourceBinary>( _resource );

		const TBlobject & blob = binary->getBlobject();

		Node * node = 0;

		NodeLoaderListener * nodeLoader = new NodeLoaderListener( &node,this );

		if(  LoaderEngine::get()
			->load( _xml_data, nodeLoader ) == false )
		//if(  XmlEngine::get()
		//	->parseXmlString( _xml_data, nodeLoader ) == false )
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
