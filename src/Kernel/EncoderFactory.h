#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/Factorable.h"

#include "Kernel/FactoryPool.h"

namespace Mengine
{
    template<class T>
    class EncoderFactory
        : public EncoderFactoryInterface
        , public Factorable
    {
    protected:
        bool initialize() override
        {
            m_factory = new FactoryPool<T, 8>();

            return true;
        }

        void finalize() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );

            m_factory = nullptr;
        }

    protected:
        EncoderInterfacePtr createEncoder() override
        {
            IntrusivePtr<T> encoder = m_factory->createObject();

            return encoder;
        }

    protected:
        void destroy() override
        {
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
            EncoderFactoryInterfacePtr encoder = new FactorableUnique<EncoderFactory<T> >();

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
            EncoderFactoryInterfacePtr encoder = CODEC_SERVICE()
                ->unregisterEncoder( Helper::stringizeString( _type ) );

            if( encoder == nullptr )
            {
                LOGGER_ERROR( "invalid unregister encoder '%s'"
                    , _type
                );

                return;
            }

            encoder->finalize();
        }
    }
}