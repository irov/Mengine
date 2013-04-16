#	include "EntityManager.h"

#	include "Kernel/Entity.h"

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Consts.h"

#	include "Logger/Logger.h"

#	include <memory>

SERVICE_FACTORY(EntityService, Menge::EntityServiceInterface, Menge::EntityManager);

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    EntityManager::EntityManager()
        : m_serviceProvider(NULL)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EntityManager::~EntityManager()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void EntityManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * EntityManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	Entity * EntityManager::createEntity( const ConstString & _prototype )
	{		
		PyObject * py_prototype = this->getPrototype( _prototype );

		if( py_prototype == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "EntityManager.createEntity: entity prototype '%s' not found"
				, _prototype.c_str()
				);

			return 0;
		}

		Entity * entity = SCRIPT_SERVICE(m_serviceProvider)
            ->createEntityT<Entity>( CONST_STRING(m_serviceProvider, Entity), _prototype, py_prototype );

		if( entity == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "EntityManager.createEntity: Can't create entity '%s'"
				, _prototype.c_str()
				);

			return 0;
		}

		return entity;
	}
}