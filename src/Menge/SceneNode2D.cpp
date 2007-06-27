#	include "SceneNode2D.h"

#	include "SceneManager.h"
#	include "ScriptEngine.h"

#	include "FileEngine.h"
#	include "XmlParser.h"

#	include "ErrorMessage.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SceneNode2D::SceneNode2D()
		: m_parent(0)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::destroy()
	{
		for each( SceneNode2D * children in m_listSceneNode2D )
		{
			if( children->isScriptable() )
			{
				Holder<ScriptEngine>::hostage()
					->decref( children );
			}

			children->destroy();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneNode2D::activate()
	{
		for each( SceneNode2D * children in m_listSceneNode2D )
		{
			children->activate();
		}

		return NodeCore::activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::deactivate()
	{
		for each( SceneNode2D * children in m_listSceneNode2D )
		{
			children->deactivate();
		}

		NodeCore::deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::setParent( SceneNode2D * _parent )
	{
		m_parent = _parent;
	}
	//////////////////////////////////////////////////////////////////////////
	SceneNode2D * SceneNode2D::getParent()
	{
		return m_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneNode2D::addChildren( SceneNode2D *_node)
	{
		if( isChildren( _node ) )
		{
			return false;
		}

		_node->setParent( this );

		Holder<ScriptEngine>::hostage()->
			incref( _node );

		m_listSceneNode2D.push_back( _node );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	SceneNode2D * SceneNode2D::getChildren( const std::string & _name )
	{
		for each( SceneNode2D * children in m_listSceneNode2D )
		{
			if( children->getName() == _name )
			{
				return children;
			}
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::removeChildren( SceneNode2D *_node)
	{
		TListSceneNode2D::iterator it_find = 
			std::find( m_listSceneNode2D.begin(), m_listSceneNode2D.end(), _node );

		if( it_find != m_listSceneNode2D.end() )
		{
			m_listSceneNode2D.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneNode2D::isChildren( SceneNode2D *_node)
	{
		TListSceneNode2D::iterator it_find = 
			std::find( m_listSceneNode2D.begin(), m_listSceneNode2D.end(), _node );

		return it_find != m_listSceneNode2D.end();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::update( float _timing )
	{
		NodeCore::update( _timing );

		if( m_active == false )
		{
			return;
		}

		for each( SceneNode2D * children in m_listSceneNode2D )
		{
			children->update( _timing );
		}
	}
	/////////////////////////////////////////////////////////////////////////
	SceneNode2D * SceneNode2D::createChildren( const std::string &_type )
	{
		SceneNode2D *node = SceneManager::createNodeT<SceneNode2D>( _type );

		if( node )
		{
			addChildren( node );
		}

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::loader( TiXmlElement * _xml )
	{
		Allocator2D::loader( _xml );
		Renderable::loader( _xml );

		XML_FOR_EACH_TREE(_xml)
		{
			Eventable::loader( XML_CURRENT_NODE );

			XML_CHECK_NODE("Node")
			{
				XML_DEF_ATTRIBUTES_NODE(Name);
				XML_DEF_ATTRIBUTES_NODE(Type);

				Node *node = createChildren( Type );

				if(node == 0)
				{
					continue;
				}

				node->setName( Name );
				node->loader(XML_CURRENT_NODE);
			}

			XML_CHECK_NODE("External")
			{
				XML_DEF_ATTRIBUTES_NODE(File);

				XML_PARSE_FILE_EX(File)
				{
					XML_CHECK_NODE("Node")
					{
						XML_DEF_ATTRIBUTES_NODE(Name);
						XML_DEF_ATTRIBUTES_NODE(Type);

						Node *node = createChildren( Type );

						if(node == 0)
						{
							continue;
						}

						node->setName( Name );

						node->loader( XML_CURRENT_NODE );
					}				
				}
				XML_INVALID_PARSE()
				{
					ErrorMessage("Invalid parse external node %s for %s", File.c_str(), m_name.c_str());
				}
			}
		}

	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & SceneNode2D::getWorldMatrix()
	{
		if( m_parent == 0 )
		{
			return getLocalMatrix();
		}

		updateMatrix( m_parent );	
		
		return Allocator2D::getWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::changePivot()
	{
		Allocator2D::changePivot();

		for each( SceneNode2D * children in m_listSceneNode2D )
		{
			children->changePivot();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneNode2D::render( const Viewport & _viewPort )
	{
		const mt::mat3f & wm = getWorldMatrix();

		const Viewport & vp = 
			Renderable::updateViewport( _viewPort );

		renderSelf( wm, vp );

		for each( SceneNode2D * children in m_listSceneNode2D )
		{
			children->render( vp );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode2D::debugRender()
	{
		for each( SceneNode2D * children in m_listSceneNode2D )
		{
			children->debugRender();
		}

		NodeCore::debugRender();
	}


}