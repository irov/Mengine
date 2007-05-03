#	include "Scene2DNode.h"
//#	include "ObjectImplement.h"

#	include "SceneManager.h"

#	include "FileEngine.h"
#	include "XmlParser.h"
#	include "ErrorMessage.h"

#	include <algorithm>

namespace Menge
{

	//////////////////////////////////////////////////////////////////////////
	Scene2DNode::Scene2DNode()
		: m_parent(0)
		, m_active(true)
		, m_compile(false)
		, m_external(false)
		, m_iteratorChildren(m_listChildren.begin())
		, m_changePivot(false)
		, m_scene(0)
	{
		mt::ident_m3(m_localMatrix);
		mt::ident_m3(m_worldMatrix);
	}
	//////////////////////////////////////////////////////////////////////////
	Scene2DNode::~Scene2DNode()
	{
		for( TListChildren::iterator 
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			delete *it;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::activate()
	{
		compile();

		if( m_compile == false )
		{
			return false;
		}

		std::for_each( 
			m_listChildren.begin(), 
			m_listChildren.end(), 
			std::mem_fun( &Scene2DNode::activate) 
			);

		if( m_active )
		{
			return true;
		}

		return _activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::deactivate()
	{
		std::for_each( 
			m_listChildren.begin(), 
			m_listChildren.end(),
			std::mem_fun( &Scene2DNode::deactivate ) 
			);

		if( m_compile )
		{
			_deactivate();

			m_active = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::isActive()
	{
		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::compile()
	{
		if( m_external && m_compile == false )
		{
			if( m_resource.empty() == false )
			{
				SceneManager::loadNode(this, m_resource);
			}
		}

		if( m_compile == false )
		{
			m_compile = _compile();
		}

		std::for_each(
			m_listChildren.begin(), 
			m_listChildren.end(), 
			std::mem_fun( &Node::compile ) 
			);

		return m_compile;	
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::release()
	{
		std::for_each(
			m_listChildren.begin(), 
			m_listChildren.end(),
			std::mem_fun( &Node::release ) 
			);

		if( m_compile == true )
		{
			deactivate();

			_release();

			m_compile = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::isCompile()
	{
		return m_compile;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::setName(const std::string & _name)
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & Scene2DNode::getName()const
	{	
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::setType(const std::string & _type)
	{
		m_type = _type;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & Scene2DNode::getType()const
	{
		return m_type;
	}

	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::isExternal()const
	{
		return m_external;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::setResource(const std::string & _file)
	{
		m_external = true;
		m_resource = _file;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & Scene2DNode::getResource()const
	{
		return m_resource;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Scene2DNode::getParent()
	{
		return m_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::isRoot()
	{
		return m_parent == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::setParent(Node *_node)
	{
		if( m_parent )
		{
			m_parent->_lostChildren(this,true);
		}

		m_parent = _node;
		_updateParent();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::update(float _timing)
	{
		if( m_active == true )
		{
			compile();

			_update(_timing);

			std::for_each(
				m_listChildren.begin(), 
				m_listChildren.end(), 
				std::bind2nd( std::mem_fun( &Node::update ) ,_timing ) 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::loader(TiXmlElement * _xml)
	{
		XML_FOR_EACH_TREE(_xml)
		{
			Eventable::loader( XML_CURRENT_NODE );

			XML_CHECK_NODE("Transformation")
			{
				XML_VALUE_ATTRIBUTE("Value", m_localMatrix);

				changePivot();
			}

			XML_CHECK_NODE("Scene2DNode")
			{
				XML_DEF_ATTRIBUTES_NODE(Name);
				XML_DEF_ATTRIBUTES_NODE(Type);

				Scene2DNode *node = createChildren(Name,Type);

				if(node == 0)
				{
					continue;
				}

				node->loader(XML_CURRENT_NODE);
			}

			XML_CHECK_NODE("External")
			{
				XML_DEF_ATTRIBUTES_NODE(File);

				bool loading = false; 
				XML_VALUE_ATTRIBUTE("Loading",loading);

				XML_PARSE_FILE_EX(File)
				{
					if ( loading == true )
					{
						loader(XML_CURRENT_NODE);
					}
					else
					{
						XML_CHECK_NODE("Scene2DNode")
						{
							XML_DEF_ATTRIBUTES_NODE(Name);
							XML_DEF_ATTRIBUTES_NODE(Type);

							Scene2DNode *node = createChildren(Name,Type);

							node->setResource(File);
						}				
					}
				}
				XML_INVALID_PARSE()
				{
					ErrorMessage("Invalid parse external node %s for %s", File.c_str(), m_name.c_str());
				}
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////
	Scene2DNode::TListChildren::iterator Scene2DNode::_findChildren(const std::string &_name)
	{
		struct FNodeFind
		{
		public:
			FNodeFind(const std::string &_findName)
				: m_findName(_findName)
			{
			}

			bool operator()(Node *_node)const
			{
				const std::string & nodeName = 
					_node->getName();

				if( nodeName == m_findName )
				{
					return true;
				}

				return false;
			}

		private:
			const std::string & m_findName;
		};

		TListChildren::iterator it_find = 
			std::find_if(
			m_listChildren.begin(),
			m_listChildren.end(),
			FNodeFind(_name)
			);

		return it_find;
	}
	/////////////////////////////////////////////////////////////////////////
	Scene2DNode * Scene2DNode::createChildren(const std::string &_name, const std::string &_type)
	{
		Scene2DNode *node = SceneManager::createNode(_name,_type);

		if( node )
		{
			addChildren(node);
		}

		return node;
	}

	/////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::addChildren(Scene2DNode *_node)
	{
		bool bInsert = isChildren(_node);

		if( bInsert == true )
		{
			return false;
		}

		_node->setParent(this);

		m_listChildren.push_back(_node);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::isChildren(Scene2DNode *_node)
	{
		const std::string & name = _node->getName();

		return isChildren(name);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::isChildren(const std::string &_name)
	{
		TListChildren::iterator it_find = _findChildren(_name);

		if( it_find == m_listChildren.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene2DNode * Scene2DNode::getChildren(const std::string &_name)
	{
		TListChildren::iterator it_find = _findChildren(_name);

		if( it_find == m_listChildren.end() )
		{
			return 0;
		}

		return *it_find;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene2DNode * Scene2DNode::nextChildren()
	{
		++m_iteratorChildren;

		if( m_iteratorChildren == m_listChildren.end() )
		{
			return 0;
		}

		return *m_iteratorChildren;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene2DNode * Scene2DNode::beginChildren()
	{
		m_iteratorChildren = m_listChildren.begin();

		if( m_listChildren.empty() )
		{
			return 0;
		}

		return *m_iteratorChildren;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::visitChildren( Visitor *_visitor )
	{
		std::for_each( 
			m_listChildren.begin(), 
			m_listChildren.end(), 
			std::bind1st( std::mem_fun( &Visitor::apply ) , _visitor ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::removeChildren(Scene2DNode *_node)
	{
		if( _node == 0 )
		{
			return;
		}

		const std::string &name = _node->getName();

		removeChildren(name);	
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::removeChildren(const std::string &_name)
	{
		TListChildren::iterator it_find = _findChildren(_name);

		if( it_find != m_listChildren.end() )
		{
			_lostChildren(*it_find,false);
			m_listChildren.erase(it_find);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::debugRender()
	{
		std::for_each(
			m_listChildren.begin(), 
			m_listChildren.end(), 
			std::mem_fun( &Scene2DNode::debugRender )
			);

		_debugRender();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::registerEvent( const std::string &_name, const lua_boost::lua_functor * _func  )
	{
		Eventable::registerEvent( _name, _func );
	}
	//////////////////////////////////////////////////////////////////////////
	const lua_boost::lua_functor * Scene2DNode::event( const std::string &_name )
	{
		return Eventable::event( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::_debugRender()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::_lostChildren(Node *_node, bool _valid)
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::_updateParent()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::_activate()
	{
		//Empty
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::_deactivate()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::_compile()
	{
		//Empty
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::_release()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::_update(float _timing)
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::_visit( Visitor * _visitor )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::changePivot()
	{
		_changePivot();

		ObjectForeach<Allocator2D>(
			m_listChildren.begin(), 
			m_listChildren.end(), 
			std::mem_fun( &Allocator2D::changePivot ) 
			);
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::_changePivot()
	{
		m_changePivot = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::isChangePivot()const
	{
		return m_changePivot;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Scene2DNode::getWorldPosition()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v2.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Scene2DNode::getWorldDirection()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v0.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Scene2DNode::getWorldMatrix()
	{
		if( m_parentAllocator == 0 )
		{
			return getLocalMatrix();
		}

		updateMatrix();

		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Scene2DNode::getLocalPosition()const
	{
		return m_localMatrix.v2.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f & Scene2DNode::getLocalPosition()
	{
		return m_localMatrix.v2.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Scene2DNode::getLocalDirection()const
	{
		return m_localMatrix.v0.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f & Scene2DNode::getLocalDirection()
	{
		return m_localMatrix.v0.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Scene2DNode::getLocalMatrix()const
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::mat3f & Scene2DNode::getLocalMatrix()
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::setPosition( const mt::vec2f &position )
	{
		mt::vec2f & localPosition = getLocalPosition();

		localPosition = position;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::setDirection( const mt::vec2f &direction )
	{
		m_localMatrix.v0.v2 = direction;
		m_localMatrix.v1.v2 = mt::perp(direction);

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::translate( const mt::vec2f &delta )
	{
		m_localMatrix.v2.v2 += delta;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene2DNode::updateMatrix()
	{
		if( m_changePivot == false )
		{
			return;
		}

		if( m_parentAllocator == 0 )
		{
			return;
		}

		if( _updateMatrix() )
		{
			const mt::mat3f & parentMatrix =
				m_parent->getWorldMatrix();

			mt::mul_m3_m3(m_worldMatrix,m_localMatrix,parentMatrix);
		}

		m_changePivot = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene2DNode::_updateMatrix()
	{
		return true;
	}
}