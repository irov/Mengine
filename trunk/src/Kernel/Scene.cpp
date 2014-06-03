#	include "Scene.h"

#	include "Interface/ScriptSystemInterface.h"

#	include "Interface/RenderSystemInterface.h"
#   include "Interface/ResourceInterface.h"
#   include "Interface/NodeInterface.h"

#	include "Logger/Logger.h"

#   include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scene::Scene()
		: m_mainLayer(nullptr)
		, m_parentScene(nullptr)
		, m_offsetPosition(0.f, 0.f)
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
	void Scene::_changeParent( Node * _oldParent, Node * _newParent )
	{
        (void)_oldParent;        

		if( _newParent == nullptr )
		{
			return;
		}

		m_parentScene = _newParent->getScene();

		if( m_parentScene == nullptr )
		{
			return;
		}

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_ON_SUB_SCENE)( "(O)", m_parentScene->getEmbed() );
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Scene::getParentScene() const
	{
		return m_parentScene;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::isSubScene() const
	{
		return m_parentScene != nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Scene::getScene()
	{
		return this;
	}
	//////////////////////////////////////////////////////////////////////////
	Layer * Scene::getMainLayer() const
	{
		return m_mainLayer;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_addChildren( Node * _node )
	{
		if( Layer * layer = dynamic_cast<Layer*>(_node) )
		{
			layer->setScene( this );

			if( layer->isMain() == true )
			{
				this->setMainLayer( layer ); 
			}
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void Scene::_removeChildren( Node * _node )
    {
        if( Layer * layer = dynamic_cast<Layer*>(_node) )
        {   
			layer->setScene( nullptr );

            if( layer->isMain() == true )
            {
                this->setMainLayer( nullptr );
            }
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void Scene::_setEventListener( PyObject * _embed )
	{
		Entity::_setEventListener( _embed );

		this->registerEvent( EVENT_APP_MOUSE_LEAVE, ("onAppMouseLeave"), _embed );
		this->registerEvent( EVENT_APP_MOUSE_ENTER, ("onAppMouseEnter"), _embed );
		this->registerEvent( EVENT_FOCUS, ("onFocus"), _embed );

		this->registerEvent( EVENT_ON_SUB_SCENE, ("onSubScene"), _embed );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onAppMouseLeave()
	{
		bool handle = false;

		//if( askEvent( handle, EVENT_APP_MOUSE_LEAVE, "()" ) == false )
		//{
		//	handle = false;
		//}

		EVENTABLE_ASK(m_serviceProvider, this, EVENT_APP_MOUSE_LEAVE)( handle, false, "()" );

		if( handle == false )
		{
			for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
			{
				Scene * subScene = dynamic_cast<Scene*>( *it );

				if( subScene != nullptr )
				{
					subScene->onAppMouseLeave();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onAppMouseEnter()
	{
		bool handle = false;

		//if( askEvent( handle, EVENT_APP_MOUSE_ENTER, "()" ) )
		//{
		//	handle = false;
		//}

		EVENTABLE_ASK(m_serviceProvider, this, EVENT_APP_MOUSE_ENTER)( handle, false, "()" );

		if( handle == false )
		{
			for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
			{
				Scene* subScene = dynamic_cast<Scene*>( *it );

				if( subScene != nullptr )
				{
					subScene->onAppMouseEnter();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onFocus( bool _focus )
	{
		bool handle = false;

		//if( askEvent( handle, EVENT_FOCUS, "(O)", pybind::get_bool(_focus) ) == false )
		//{
		//	handle = false;
		//}

		EVENTABLE_ASK(m_serviceProvider, this, EVENT_FOCUS)( handle, false, "(O)", pybind::get_bool(_focus) );

		if( handle == false )
		{
			for( TSlugChild it(m_child); it.eof() == false; it.next_shuffle() )
			{
				Node * children = *it;
				Scene * subScene = dynamic_cast<Scene *>(children);

				if( subScene != nullptr )
				{
					subScene->onFocus( _focus );
				}
			}			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		(void)_viewport;
        (void)_camera;
		// nothing
	}
}
