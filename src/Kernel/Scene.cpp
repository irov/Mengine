#	include "Scene.h"

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
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Scene::~Scene()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	enum SceneEventFlag
	{
		EVENT_APP_MOUSE_LEAVE = 0,
		EVENT_APP_MOUSE_ENTER,
		EVENT_FOCUS,
		EVENT_ON_SUB_SCENE,
	};
	//////////////////////////////////////////////////////////////////////////
	void Scene::_setEventListener( const pybind::dict & _embed )
	{
		Entity::_setEventListener( _embed );

		this->registerEvent( EVENT_APP_MOUSE_LEAVE, ("onAppMouseLeave"), _embed );
		this->registerEvent( EVENT_APP_MOUSE_ENTER, ("onAppMouseEnter"), _embed );
		this->registerEvent( EVENT_FOCUS, ("onFocus"), _embed );

		this->registerEvent( EVENT_ON_SUB_SCENE, ("onSubScene"), _embed );
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

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_ON_SUB_SCENE)( m_parentScene );
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
	void Scene::_addChild( Node * _node )
	{
		Layer * layer = dynamic_cast<Layer *>(_node);

		if( layer != nullptr )
		{
			layer->setScene( this );

			if( layer->isMain() == true )
			{
				this->setMainLayer( layer ); 
			}
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void Scene::_removeChild( Node * _node )
    {
		Layer * layer = dynamic_cast<Layer *>(_node);

        if( layer != nullptr )
        {   
			layer->setScene( nullptr );

            if( layer->isMain() == true )
            {
                this->setMainLayer( nullptr );
            }
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void Scene::onAppMouseLeave()
	{
		bool handle = false;

		EVENTABLE_ASK( m_serviceProvider, this, EVENT_APP_MOUSE_LEAVE, handle )();

		if( handle == false )
		{
			for( TSlugChild it(m_children); it.eof() == false; )
			{
				Scene * subScene = dynamic_cast<Scene *>( *it );

				it.next_shuffle();

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

		EVENTABLE_ASK( m_serviceProvider, this, EVENT_APP_MOUSE_ENTER, handle )();

		if( handle == false )
		{
			for( TSlugChild it(m_children); it.eof() == false; )
			{
				Scene* subScene = dynamic_cast<Scene*>( *it );

				it.next_shuffle();

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

		EVENTABLE_ASK( m_serviceProvider, this, EVENT_FOCUS, handle )(_focus);

		if( handle == false )
		{
			for( TSlugChild it(m_children); it.eof() == false; )
			{
				Node * children = *it;

				it.next_shuffle();

				Scene * subScene = dynamic_cast<Scene *>(children);

				if( subScene != nullptr )
				{
					subScene->onFocus( _focus );
				}
			}			
		}
	}
}
