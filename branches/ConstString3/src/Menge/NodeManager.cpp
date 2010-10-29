#	include "NodeManager.h"

#	include "LoaderEngine.h"
#	include "ScriptEngine.h"

#	include "ResourceManager.h"
#	include "ResourceBinary.h"

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
	Node* NodeManager::createNode( const ConstString& _name, const ConstString& _type )
	{
		Node * node = FactoryManager::createObjectT<Node>( _type );

		if( node == 0 )
		{
			MENGE_LOG_ERROR( "Invalid Node Type '%s'"
				, _type.c_str() 
				);

			return 0;
		}

		node->setName( _name );
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
			NodeLoaderListener( Node ** _externalNode, const ConstString& _name, NodeManager * _manager )
				: m_externalNode(_externalNode)
				, m_name(_name)
				, m_manager(_manager)
			{
			}

		public:
			void loader( BinParser * _parser ) override
			{
				BIN_SWITCH_ID(_parser)
				{
					BIN_CASE_NODE( Protocol::BinaryNode )
					{
						ConstString type;

						BIN_FOR_EACH_ATTRIBUTES()
						{
							BIN_CASE_ATTRIBUTE( Protocol::BinaryNode_Type, type );
						}

						*m_externalNode = m_manager->createNode( m_name, type );

						if( *m_externalNode == 0 )
						{
							MENGE_LOG_ERROR( "Error: can't create external binary node '%s' invalid type '%s'"
								, m_name.c_str()
								, type.c_str()
								);

							BIN_SKIP();
						}

						BIN_PARSE( (*m_externalNode) );
					}
				}
			}

			void loaded() override
			{
				if( m_externalNode == 0 )
				{
					return;
				}

				if( *m_externalNode == 0 )
				{
					return;
				}

				(*m_externalNode)->loaded();
			}

		protected:
			Node ** m_externalNode;
			ConstString m_name;
			NodeManager * m_manager;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	Node * NodeManager::createNodeFromBinary( const ConstString& _name, const ConstString & _binResource )
	{	
		ResourceBinary * binary = ResourceManager::get()
			->getResourceT<ResourceBinary>( _binResource );

		if( binary == 0 )
		{
			return 0;
		}

		const TBlobject & blob = binary->getBlobject();

		Node * node = 0;

		std::auto_ptr<NodeLoaderListener> loadable( new NodeLoaderListener(&node, _name, this) );

		if(  LoaderEngine::get()
			->loadBinary( blob, loadable.get() ) == false )
		{
			MENGE_LOG_ERROR( "Invalid parse external binary node data '%s'"
				, _binResource.c_str() 
				);

			return 0;
		}

		if( node == 0 )
		{
			MENGE_LOG_ERROR( "This xml have invalid external binary node format" );

			return 0;
		}

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
}
