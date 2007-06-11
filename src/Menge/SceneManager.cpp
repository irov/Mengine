#	include "SceneManager.h"
#	include "NodeFactory.h"

#	include "Node.h"
#	include "Renderable.h"

#	include "InputEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "ScriptEngine.h"

#	include "XmlParser.h"
#	include "ErrorMessage.h"

using namespace Menge;

//////////////////////////////////////////////////////////////////////////
Node* SceneManager::createNode(const std::string &_name, const std::string &_type)
{
	Node *node = NodeFactory::genNode(_type);
	
	if( node )
	{
		node->setName(_name);
	}

	return node;
}
//////////////////////////////////////////////////////////////////////////
bool SceneManager::loadNode(Node *_node, const std::string &_xml)
{
	XML_PARSE_FILE_EX(_xml )
	{
		_node->loader(XML_CURRENT_NODE);
	}
	XML_INVALID_PARSE()
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
Node * SceneManager::createNodeFromXml( const std::string &_file)
{
	XML_PARSE_FILE_EX(_file)
	{
		XML_CHECK_NODE("Node")
		{
			XML_DEF_ATTRIBUTES_NODE(Name);
			XML_DEF_ATTRIBUTES_NODE(Type);
			Node *node = createNode(Name,Type);
			node->loader(XML_CURRENT_NODE);			
//			node->setResource(_file);
			return node;
		}
	}
	XML_INVALID_PARSE()
	{
		ErrorMessage("Invalid parse external node `%s`", _file.c_str() );
	}

	ErrorMessage("This xml file `%s` have invalid external node format", _file.c_str() );

	return 0;
}
//////////////////////////////////////////////////////////////////////////
void SceneManager::renderNode( Node * _node, const Viewport & _viewPort )
{
	if( Renderable* ro = dynamic_cast<Renderable*>(_node) )
	{
		const Viewport &uvp = ro->updateViewport( _viewPort );

		if( ro->render( uvp ) == false )
		{
			return;
		}

		for( Node *it = _node->beginChildren();
			it != 0;
			it = _node->nextChildren()
			)
		{
			SceneManager::renderNode( it, uvp );
		}
	}
	else
	{
		for( Node *it = _node->beginChildren(); 
			it != 0;
			it = _node->nextChildren()
			)
		{
			SceneManager::renderNode( it, _viewPort );
		}
	}
}