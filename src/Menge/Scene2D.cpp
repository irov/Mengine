#	include "Scene2D.h"

#	include "Scene2DNode.h"

#	include "Layer.h"

#	include "SceneManager.h"

#	include "ScriptEngine.h"

#	include "XmlParser.h"

#	include "Renderable.h"

#	include "InputEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "ScriptEngine.h"

#	include "NodeFactory.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scene2DNode* Scene2D::createNode(const std::string &_name, const std::string &_type)
	{
		Scene2DNode *node = NodeFactory::genNode(_type);

		if( node )
		{
			node->setName(_name);
		}

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2D::loadNode(Node *_node, const std::string &_xml)
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
	Scene2DNode * Scene2D::createNodeFromXml( const std::string &_file)
	{
		XML_PARSE_FILE_EX(_file)
		{
			XML_CHECK_NODE("Node")
			{
				XML_DEF_ATTRIBUTES_NODE(Name);
				XML_DEF_ATTRIBUTES_NODE(Type);
				Scene2DNode *node = createNode(Name,Type);
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
	//////////////////////////////////////////////////////////////////////////
	void Scene2D::renderNode( Scene2DNode * _node, const Viewport & _viewPort )
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
				renderNode( it, uvp );
			}
		}
		else
		{
			for( Node *it = _node->beginChildren(); 
				it != 0;
				it = _node->nextChildren()
				)
			{
				renderNode( it, _viewPort );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	Scene2D::Scene2D()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Scene2DNode * Scene2D::getEntity( const std::string & _name )
	{
		for( TListChildren::iterator 
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			if( Layer * layer = dynamic_cast<Layer*>(*it) )
			{
				if( Node * node = layer->getChildren( _name ) )
				{
					return node;
				}
			}
			else
			{
				if( (*it)->getName() == _name )
				{
					return *it;
				}
			}
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::compile()
	{
		if( m_scriptFile.empty() == false )
		{
			int Error = Holder<ScriptEngine>::hostage()->doFile(m_scriptFile);

			return Error == 0;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::loader(TiXmlElement *_xml)
	{
		NodeImpl::loader(_xml);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE("Script", "File", m_scriptFile);
		}
	}
}