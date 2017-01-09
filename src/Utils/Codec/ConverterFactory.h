#   pragma once

#   include "Interface/ConverterInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Core/ServantBase.h"
#   include "Core/ConstString.h"

#   include "Factory/FactoryStore.h"

namespace Menge
{
    template<class T>
    class ConverterFactory
        : public ServantBase<ConverterFactoryInterface>
    {
    protected:
        ConverterInterfacePtr createConverter() override
        {
			ConverterInterface * converter = m_factory.createObject();

			converter->setServiceProvider( m_serviceProvider );

            return converter;
        }

    protected:
        void destroy() override
        {
            delete this;
        }

    protected:
		typedef FactoryPoolStore<T, 8> TFactoryConverter;
		TFactoryConverter m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        inline void registerConverter( ServiceProviderInterface * _serviceProvider, const Char * _type )
        {
            ConverterFactoryInterfacePtr converter = new ConverterFactory<T>();

            converter->setServiceProvider( _serviceProvider );

            CONVERTER_SERVICE( _serviceProvider )
                ->registerConverter( Helper::stringizeString( _serviceProvider, _type ), converter );
        }
        
        inline void unregisterConverter( ServiceProviderInterface * _serviceProvider, const Char * _type )
        {
            CONVERTER_SERVICE( _serviceProvider )
                ->unregisterConverter( Helper::stringizeString( _serviceProvider, _type ) );
        }
    }
}