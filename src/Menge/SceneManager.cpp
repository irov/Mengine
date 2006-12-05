#	include "SceneManager.h"
#	include "NodeFactory.h"

#	include "Node.h"

#	include "InputEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "ScriptEngine.h"

#	include "ScriptFunction.h"

#	include "XmlParser.h"
#	include "ErrorMessage.h"

using namespace Menge;

//////////////////////////////////////////////////////////////////////////
SceneManager::SceneManager()
{
	Keeper<SceneManager>::keep(this);
}
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
//			node->loader(XML_CURRENT_NODE);			
			node->setResource(_file);
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