#	include "Scene.h"

#	include "SceneManager.h"
#	include "ResourceManager.h"
#	include "MousePickerSystem.h"

#	include "ScriptEngine.h"
#	include "XmlEngine.h"
#	include "PhysicEngine2D.h"

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
		Layer * layer = getChildrenT<Layer>( _name, false );
		if( layer )
		{
			return layer->getSize();
		}

		return mt::vec2f::zero_v2;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::layerAppend( const std::string & _layer, Node * _node )
	{
		Layer * layer = getChildrenT<Layer>( _layer, false );
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
				askEvent( handle, "KEY", "(IIb)", _key, _char, _isDown );
			}
		}

		if( handle == false )
		{
			for( TListChildren::reverse_iterator 
				it = m_listChildren.rbegin(),
				it_end = m_listChildren.rend();
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
				askEvent( handle, "MOUSE_BUTTON", "(Ib)", _button, _isDown );
			}
		}

		if( handle == false )
		{
			for( TListChildren::reverse_iterator 
				it = m_listChildren.rbegin(),
				it_end = m_listChildren.rend();
			it != it_end;
			++it)
			{
				if( handle = (*it)->handleMouseButtonEvent( _button, _isDown ) )
				{
					break;
				}
			}
		}

		/*if( updatable() )
		{
			askEvent( handle, "MOUSE_BUTTON_END", "(Ib)", _button, _isDown );
		}*/

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
				askEvent( handle, "MOUSE_MOVE", "(ffi)", _x, _y, _whell );
			}
		}
	
		if( handle == false )
		{
			for( TListChildren::reverse_iterator 
				it = m_listChildren.rbegin(),
				it_end = m_listChildren.rend();
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
		registerEvent( "MOUSE_BUTTON_END", "onHandleMouseButtonEventEnd", this->getEmbedding() );

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
			Holder<PhysicEngine2D>::hostage()->destroyScene();
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
		//NodeRenderable::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "OffsetPosition", "Value", m_offsetPosition );
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
	void Scene::_addChildren( Layer * _layer )
	{
		_layer->setOffsetPosition( m_offsetPosition );
		_layer->setScene( this );

		if( _layer->isMain() )
		{
			m_mainLayer = _layer;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setOffsetPosition( const mt::vec2f& _offset )
	{
		m_offsetPosition = _offset;

		for( TListChildren::iterator
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			static_cast<Layer*>(*it)->setOffsetPosition( m_offsetPosition );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	/*void Scene::_render()
	{

	}*/
	//////////////////////////////////////////////////////////////////////////
	void Scene::setRenderTarget( const std::string& _cameraName )
	{
		for( TListChildren::iterator
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			static_cast<Layer*>(*it)->setRenderTarget( _cameraName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::compile()
	{
		if( m_physWorld2D )
		{
			Holder<PhysicEngine2D>::hostage()->createScene( mt::vec2f( m_physWorldBox2D.x, m_physWorldBox2D.y ),mt::vec2f( m_physWorldBox2D.z, m_physWorldBox2D.w ), m_gravity2D );
		}
		return Node::compile();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		for( TListChildren::reverse_iterator 
			it = m_listChildren.rbegin(),
			it_end = m_listChildren.rend();
		it != it_end;
		++it)
		{
			(*it)->handleMouseButtonEventEnd( _button, _isDown );
		}

		bool handle = false;
		if( updatable() )
		{
			askEvent( handle, "MOUSE_BUTTON_END", "(Ib)", _button, _isDown );
		}
		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
}