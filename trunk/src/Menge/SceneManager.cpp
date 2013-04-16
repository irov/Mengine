#	include "SceneManager.h"

#	include "Kernel/Scene.h"

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/NodeInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Consts.h"

#	include "Logger/Logger.h"

#	include <memory>

SERVICE_FACTORY(SceneService, Menge::SceneServiceInterface, Menge::SceneManager);

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SceneManager::SceneManager()
        : m_serviceProvider(NULL)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SceneManager::~SceneManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * SceneManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	Scene * SceneManager::createScene( const ConstString & _name )
	{
		Scene * scene = NULL;

        PyObject * py_prototype;
		if( this->hasPrototype( _name, &py_prototype ) == false )
		{
            scene = NODE_SERVICE(m_serviceProvider)
                ->createNodeT<Scene>( CONST_STRING(m_serviceProvider, Scene) );
        }
        else
        {
            scene = SCRIPT_SERVICE(m_serviceProvider)
                ->createEntityT<Scene>( CONST_STRING(m_serviceProvider, Scene), _name, py_prototype );
        }

        if( scene == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "SceneManager::createScene Can't create scene '%s'"
                , _name.c_str() 
                );

            return 0;
        }

        scene->setName( _name );

        //m_scenes.insert( std::make_pair(_name, scene) );

		return scene;
	}
}