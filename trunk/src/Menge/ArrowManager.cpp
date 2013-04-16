#	include "ArrowManager.h"
#	include "Arrow.h"

#	include <memory>

#	include "Interface/ScriptSystemInterface.h"

#	include "Consts.h"
#	include "Logger/Logger.h"

SERVICE_FACTORY(ArrowService, Menge::ArrowServiceInterface, Menge::ArrowManager);

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ArrowManager::ArrowManager()
        : m_serviceProvider(NULL)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ArrowManager::~ArrowManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ArrowManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	Arrow * ArrowManager::createArrow( const ConstString & _prototype )
	{
		PyObject * py_prototype = this->getPrototype( _prototype );

		if( py_prototype == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ArrowManager.createArrow: arrow prototype '%s' not found"
				, _prototype.c_str()
				);

			return 0;
		}

		Arrow * arrow = SCRIPT_SERVICE(m_serviceProvider)
			->createEntityT<Arrow>( CONST_STRING(m_serviceProvider, Arrow), _prototype, py_prototype );

		if( arrow == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ArrowManager.createArrow: can't create arrow from prototype '%s'"
				, _prototype.c_str()
				); 

			return 0;
		}

		return arrow;
	}
}