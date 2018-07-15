#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/ServantBase.h"

#include "Kernel/FactoryPool.h"

namespace Mengine
{
    template<class T>
    class EncoderFactory
        : public ServantBase<EncoderFactoryInterface>
    {
    protected:
        bool initialize() override
        {
            m_factory = new FactoryPool<T, 8>();

            return true;
        }

    protected:
        EncoderInterfacePtr createEncoder() override
        {
            EncoderInterfacePtr encoder = m_factory->createObject();
            
            return encoder;
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

    namespace Helper
    {
        template<class T>
        inline EncoderFactoryInterfacePtr registerEncoder( const Char * _type )
        {
            EncoderFactoryInterfacePtr encoder = new EncoderFactory<T>();

			if( encoder->initialize() == false )
			{
				return nullptr;
			}

            CODEC_SERVICE()
                ->registerEncoder( Helper::stringizeString( _type ), encoder );

            return encoder;
        }

        inline void unregisterEncoder( const Char * _type )
        {
            CODEC_SERVICE()
                ->unregisterEncoder( Helper::stringizeString( _type ) );
        }
    }
}