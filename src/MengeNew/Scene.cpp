#	include "Scene.h"

#	include "SceneManager.h"
#	include "ResourceManager.h"
#	include "MousePickerSystem.h"

#	include "ScriptEngine.h"
#	include "XmlEngine.h"
#	include "PhysicEngine2D.h"

#	include "Player.h"
#	include "Camera2D.h"
#	include "Layer2D.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scene::Scene()
	: m_mainLayer(0)
	, m_isSubScene(false)
	, m_offsetPosition(0.f,0.f)
	, m_gravity2D( 0.0f, 0.0f )
	, m_physWorldBox2D( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_physWorld2D( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Scene::~Scene()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setMainLayer( Layer * _layer )
	{
		m_mainLayer = _layer;
	}
	//////////////////////////////////////////////////////////////////////////
	Layer * Scene::getMainLayer()
	{
		return m_mainLayer;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setPhysicWorldBox2D( const mt::vec4f & _physWorldBox2D )
	{
		m_physWorldBox2D = _physWorldBox2D;
		m_physWorld2D = true;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & Scene::getPhysicWorldBox2D() const
	{
		return m_physWorldBox2D;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setParentScene( Scene * _scene )
	{
		this->callMethod( "onSubScene", "(O)", _scene->getEmbedding() );

		m_isSubScene = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::isSubScene() const
	{
		return m_isSubScene;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Scene::getNode(const std::string & _name )
	{
		Node * node = getChildren( _name, true );
		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Scene::getLayerSize( const std::string & _name )
	{
		Layer * layer = getLayer_( _name );
		if( layer )
		{
			return layer->getSize();
		}

		return mt::vec2f::zero_v2;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::layerAppend( const std::string & _layer, Node * _node )
	{
		Layer * layer = getLayer_( _layer );
		if( layer )
		{
			layer->addChildren( _node );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( isActivate() == false )
		{
			return false;
		}

		bool handle = false;
		
		if( updatable() )
		{
			if( handle == false )
			{
				handle = askEvent( handle, "KEY", "(IIb)", _key, _char, _isDown );
			}
		}

		if( handle == false )
		{
			for( TContainerChildrens::reverse_iterator 
				it = m_childrens.rbegin(),
				it_end = m_childrens.rend();
			it != it_end;
			++it)
			{
				if( handle = (*it)->handleKeyEvent( _key, _char, _isDown ) )
				{
					break;
				}
			}
		}

		return handle;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		if( isActivate() == false )
		{
			return false;
		}
		
		bool handle = false;
		
		if( updatable() )
		{
			if( handle == false )
			{
				handle = askEvent( handle, "MOUSE_BUTTON", "(Ib)", _button, _isDown );
			}
		}

		if( handle == false )
		{
			for( TContainerChildrens::reverse_iterator 
				it = m_childrens.rbegin(),
				it_end = m_childrens.rend();
			it != it_end;
			++it)
			{
				if( handle = (*it)->handleMouseButtonEvent( _button, _isDown ) )
				{
					break;
				}
			}
		}

		return handle;	
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseMove( float _x, float _y, int _whell )
	{
		if( isActivate() == false )
		{
			return false;
		}
		
		bool handle = false;
		
		if( updatable() )
		{
			if( handle == false )
			{
				handle = askEvent( handle, "MOUSE_MOVE", "(ffi)", _x, _y, _whell );
			}
		}
	
		if( handle == false )
		{
			for( TContainerChildrens::reverse_iterator 
				it = m_childrens.rbegin(),
				it_end = m_childrens.rend();
			it != it_end;
			++it)
			{
				if( handle = (*it)->handleMouseMove( _x, _y, _whell ) )
				{
					break;
				}
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::_activate()
	{
		const std::string & name = this->getName();
		
		if( name.empty() )
		{
			return false;
		}

		registerEvent( "UPDATE", "onUpdate", this->getEmbedding() );

		registerEvent( "KEY", "onHandleKeyEvent", this->getEmbedding() );
		registerEvent( "MOUSE_BUTTON", "onHandleMouseButtonEvent", this->getEmbedding() );
		registerEvent( "MOUSE_MOVE", "onHandleMouseMove", this->getEmbedding() );

		callMethod( "onActivate", "() " );

		return Node::_activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_deactivate()
	{
		callMethod( "onDeactivate", "() ");
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_release()
	{
		if( m_physWorld2D )
		{
			Holder<PhysicEngine2D>::hostage()
				->destroyScene();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_update( float _timing )
	{
		callEvent( "UPDATE", "(f)", _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loader( XmlElement *_xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Scene")
			{
				XML_PARSE_ELEMENT( this, &Scene::loaderScene_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loaderScene_( XmlElement *_xml )
	{
		std::string name;

		Node::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "OffsetPosition", "Value", m_offsetPosition );
			XML_CASE_ATTRIBUTE_NODE( "Gravity2D", "Value", m_gravity2D );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "PhysicWorld2DBox", "Value", &Scene::setPhysicWorldBox2D )
		}
		XML_END_NODE()
		{
			callMethod( "onLoader", "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_addChildren( Node * _node )
	{
		if( Layer * layer = dynamic_cast<Layer*>(_node) )
		{
			layer->setOffsetPosition( m_offsetPosition );
			layer->setScene( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setOffsetPosition( const mt::vec2f& _offset )
	{
		m_offsetPosition = _offset;

		//for( TListChildren::iterator
		//	it = m_listChildren.begin(),
		//	it_end = m_listChildren.end();
		//it != it_end;
		//++it)
		//{
		//	static_cast<Layer*>(*it)->setOffsetPosition( m_offsetPosition );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setRenderTarget( const std::string& _cameraName )
	{
		//for( TListChildren::iterator
		//	it = m_listChildren.begin(),
		//	it_end = m_listChildren.end();
		//it != it_end;
		//++it)
		//{
		//	static_cast<Layer*>(*it)->setRenderTarget( _cameraName );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::compile()
	{
		if( m_physWorld2D )
		{
			Holder<PhysicEngine2D>::hostage()
				->createScene( m_physWorldBox2D.v2_0, m_physWorldBox2D.v2_1, m_gravity2D );
		}

		if( m_mainLayer )
		{
			mt::vec2f mainSize = m_mainLayer->getSize();

			Camera2D * camera2D = Holder<Player>::hostage()
				->getRenderCamera2D();

			const Viewport & viewport = camera2D->getViewport();

			mt::vec2f viewport_size = viewport.end - viewport.begin;

			mainSize -= viewport_size;
		
			for( TContainerChildrens::reverse_iterator 
				it = m_childrens.rbegin(),
				it_end = m_childrens.rend();
			it != it_end;
			++it)
			{
				if( Layer2D * layer2D = dynamic_cast<Layer2D*>( *it ) )
				{
					mt::vec2f layerSize = layer2D->getSize();

					layerSize -= viewport_size;

					mt::vec2f parallaxFactor( layerSize.x / mainSize.x, layerSize.y / mainSize.y );

					layer2D->setParallaxFactor( parallaxFactor );
				}
			}
		}

		return Node::compile();
	}
	//////////////////////////////////////////////////////////////////////////
	Layer * Scene::getLayer_( const std::string & _name )
	{
		Node * children = getChildren( _name, false );

		if( Layer * layer = dynamic_cast<Layer*>(children) )
		{
			return layer;
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
}