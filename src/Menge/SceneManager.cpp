#	include "SceneManager.h"
#	include "NodeFactory.h"

#	include "Node.h"
#	include "Renderable2D.h"

#	include "InputEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "ScriptEngine.h"

#	include "Entity.h"

#	include "ErrorMessage.h"

#	include "XmlParser/XmlParser.h"

using namespace Menge;

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
	TiXmlDocument * document = Holder<FileEngine>::hostage()
		->loadXml( _xml );

	XML_FOR_EACH_DOCUMENT( document )
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
Node * SceneManager::createNodeFromXml( const std::string & _xml )
{
	TiXmlDocument * document = Holder<FileEngine>::hostage()
		->loadXml( _xml );

	XML_FOR_EACH_DOCUMENT( document )
	{
		
		XML_CHECK_NODE("Node")
		{
			XML_DEF_ATTRIBUTES_NODE(Name);
			XML_DEF_ATTRIBUTES_NODE(Type);

			Node *node = createNode( Type );

			node->setName( Name );
			node->loader(XML_CURRENT_NODE);

			return node;
		}
	}
	XML_INVALID_PARSE()
	{
		ErrorMessage("Invalid parse external node `%s`", _xml.c_str() );
	}

	ErrorMessage("This xml file `%s` have invalid external node format", _xml.c_str() );

	return 0;
}