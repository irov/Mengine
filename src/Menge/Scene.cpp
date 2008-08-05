#	include "Scene.h"

#	include "ScriptEngine.h"
#	include "XmlEngine.h"
#	include "PhysicEngine2D.h"
#	include "RenderEngine.h"

#	include "Layer2D.h"
#	include "Player.h"
#	include "Camera2D.h"
#	include "Application.h"

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
	, m_renderTarget( "defaultCamera" )
	, m_onUpdateEvent(false)
	, m_blockInput( false )
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
	void Scene::setParentScene( Scene * _scene )
	{
		callMethod( "onSubScene", "(O)", _scene->getEmbedding() );

		m_isSubScene = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::isSubScene() const
	{
		return m_isSubScene;
	}
	//////////////////////////////////////////////////////////////////////////
	Layer * Scene::getMainLayer()
	{
		return m_mainLayer;
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
	bool Scene::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( isActivate() == false || m_blockInput == true )
		{
			return false;
		}

		bool handle = false;

		if( handle == false )
		{
			if( updatable() )
			{
				handle = askEvent( handle, EVENT_KEY, "(IIb)", _key, _char, _isDown );
			}
		}

		if( handle == false )
		{
			for( TContainerChildren::reverse_iterator 
				it = m_children.rbegin(),
				it_end = m_children.rend();
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
		if( isActivate() == false || m_blockInput == true )
		{
			return false;
		}

		bool handle = false;
		
		if( handle == false )
		{
			if( updatable() )
			{
				handle = askEvent( handle, EVENT_MOUSE_BUTTON, "(Ib)", _button, _isDown );
			}
		}

		if( handle == false )
		{
			for( TContainerChildren::reverse_iterator 
				it = m_children.rbegin(),
				it_end = m_children.rend();
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
		if( isActivate() == false || m_blockInput == true)
		{
			return false;
		}

		bool handle = false;

		if( updatable() )
		{
			if( handle == false )
			{
				handle = askEvent( handle, EVENT_MOUSE_MOVE, "(ffi)", _x, _y, _whell );
			}
		}

		if( handle == false )
		{
			for( TContainerChildren::reverse_iterator 
				it = m_children.rbegin(),
				it_end = m_children.rend();
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

		m_onUpdateEvent = registerEvent( EVENT_UPDATE, "onUpdate", this->getEmbedding() );

		registerEvent( EVENT_KEY, "onHandleKeyEvent", this->getEmbedding() );
		registerEvent( EVENT_MOUSE_BUTTON, "onHandleMouseButtonEvent", this->getEmbedding() );
		registerEvent( EVENT_MOUSE_MOVE, "onHandleMouseMove", this->getEmbedding() );
		registerEvent( EVENT_MOUSE_BUTTON_END, "onHandleMouseButtonEventEnd", this->getEmbedding() );
		registerEvent( EVENT_LEAVE, "onMouseLeave", this->getEmbedding() );
		registerEvent( EVENT_ENTER, "onMouseEnter", this->getEmbedding() );

		callMethod( "onActivate", "() " );

		bool result = Node::_activate();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_deactivate()
	{
		callMethod( "onDeactivate", "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_release()
	{
		if( m_physWorld2D )
		{
			Holder<PhysicEngine2D>::hostage()->destroyScene();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_update( float _timing )
	{
		Node::_update( _timing );
		if( m_onUpdateEvent )
		{
			callEvent( EVENT_UPDATE, "(f)", _timing );
		}
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
			XML_CASE_ATTRIBUTE_NODE( "Gravity2D", "Value", m_gravity2D );
			XML_CASE_NODE( "PhysicWorld2DBox" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Value", m_physWorldBox2D );
				}
				m_physWorld2D = true;
			}
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
			layer->setScene( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setRenderTarget( const std::string& _cameraName )
	{
		m_renderTarget = _cameraName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::compile()
	{
		if( m_physWorld2D )
		{
			Holder<PhysicEngine2D>::hostage()->createScene( mt::vec2f( m_physWorldBox2D.x, m_physWorldBox2D.y ),mt::vec2f( m_physWorldBox2D.z, m_physWorldBox2D.w ), m_gravity2D );
		}

		if( m_mainLayer )
		{
			mt::vec2f mainSize = m_mainLayer->getSize();

			Camera2D * camera2D = Holder<Player>::hostage()
				->getRenderCamera2D();

			const Viewport & viewport = camera2D->getViewport();

			mt::vec2f viewport_size = viewport.end - viewport.begin;

			mainSize -= viewport_size;

			//if( fabsf( mainSize.x /* mainSize.y*/) > 0.0001f )
			{
				for( TContainerChildren::reverse_iterator 
					it = m_children.rbegin(),
					it_end = m_children.rend();
				it != it_end;
				++it)
				{
					if( Layer2D * layer2D = dynamic_cast<Layer2D*>( *it ) )
					{
						mt::vec2f layerSize = layer2D->getSize();

						layerSize -= viewport_size;

						float factorX = ( mainSize.x > 0.0001f ) ? ( layerSize.x / mainSize.x ) : 0.0f;
						float factorY = ( mainSize.y > 0.0001f ) ? ( layerSize.y / mainSize.y ) : 0.0f;

						mt::vec2f parallaxFactor( factorX, factorY );

						layer2D->setParallaxFactor( parallaxFactor );
					}
				}
			}
		}

		bool result = Node::compile();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if(  m_blockInput == true )
		{
			return false;
		}

		if( updatable() )
		{
			if( handle == false )
			{
				askEvent( handle, EVENT_MOUSE_BUTTON_END, "(Ib)", _button, _isDown );
			}
		}

		if( handle == false )
		{
			for( TContainerChildren::reverse_iterator 
				it = m_children.rbegin(),
				it_end = m_children.rend();
			it != it_end;
			++it)
			{
				if( handle = (*it)->handleMouseButtonEventEnd( _button, _isDown ) )
				{
					break;
				}
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::render( unsigned int _debugMask )
	{
		if( isRenderable() == false )
		{
			return;
		}

		const mt::vec2f& main_size = m_mainLayer->getSize();

		Camera2D * camera2D = Holder<Player>::hostage()->getRenderCamera2D();

		mt::vec2f camPos = camera2D->getLocalPosition();
		const Viewport & vp = camera2D->getViewport();

		/*const mt::mat4f & viewMatrix = camera2D->getViewMatrix();

		Holder<RenderEngine>::hostage()->setViewMatrix( viewMatrix );*/

		mt::vec2f vp_size = vp.end - vp.begin;
		if( ( camPos.y - vp_size.y * 0.5f ) < 0.0f )
		{
			Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( mt::vec2f( camPos.x, vp_size.y * 0.5f ) );
		}
		else if( ( camPos.y + vp_size.y * 0.5f ) > main_size.y )
		{
			Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( mt::vec2f( camPos.x, main_size.y - vp_size.y * 0.5f ) );
			//viewport.begin.y = main_size.y - viewport_size.y;
		}

		_render( _debugMask );


		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			Holder<RenderEngine>::hostage()
				->setRenderTarget( m_renderTarget );

			(*it)->render( _debugMask );
		}

		Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( camPos );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::renderSelf()
	{
		unsigned int debugMask = 0;
		debugMask = Holder<Application>::hostage()->getDebugMask();
		
		render( debugMask );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onMouseLeave()
	{
		callEvent( EVENT_LEAVE, "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onMouseEnter()
	{
		callEvent( EVENT_ENTER, "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Scene::getRenderTarget() const
	{
		return m_renderTarget;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::blockInput( bool _block )
	{
		m_blockInput = _block;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::getBlockInput() const
	{
		return m_blockInput;
	}
	//////////////////////////////////////////////////////////////////////////
}