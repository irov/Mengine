#	include "NodeManager.h"

#	include "LoaderEngine.h"
#	include "ScriptEngine.h"

#	include "ResourceManager.h"
#	include "ResourceBinary.h"

#	include "LogEngine.h"

#	include "Node.h"

#	include <memory>

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
			MENGE_LOG_ERROR( "NodeManager::createNode: Invalid Node Type '%s' [Name '%s']"
				, _type.c_str() 
				, _name.c_str()
				);

			return 0;
		}

		node->setName( _name );
		node->setType( _type );
		
		return node;
	}
}
