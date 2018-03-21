#pragma once

#include "Interface/ConverterInterface.h"
#include "Interface/StringizeInterface.h"

#include "Core/ServantBase.h"
#include "Core/ConstString.h"

#include "Factory/FactoryPool.h"

namespace Mengine
{
    template<class T>
    class ConverterFactory
        : public ServantBase<ConverterFactoryInterface>
    {
    protected:
        bool initialize() override
        {
            m_factory = new FactoryPool<T, 8>();

            return true;
        }

    protected:
        ConverterInterfacePtr createConverter() override
        {
			ConverterInterface * converter = m_factory->createObject();

            return converter;
        }

    protected:
        void destroy() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );

            m_factory = nullptr;

            delete this;
        }

    protected:
		FactoryPtr m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        inline ConverterFactoryInterfacePtr registerConverter( const Char * _type )
        {
            ConverterFactoryInterfacePtr converter = new ConverterFactory<T>();

            if( converter->initialize() == false )
            {
                return nullptr;
            }

            if( CONVERTER_SERVICE()
                ->registerConverter( Helper::stringizeString( _type ), converter ) == false )
            {
                return nullptr;
            }

            return converter;
        }
        
        inline void unregisterConverter( const Char * _type )
        {
            CONVERTER_SERVICE()
                ->unregisterConverter( Helper::stringizeString( _type ) );
        }
    }
}