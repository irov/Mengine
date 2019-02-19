#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Factorable.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

namespace Mengine
{
    template<class T>
    class DecoderFactory
        : public DecoderFactoryInterface
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
        DecoderInterfacePtr createDecoder( const Char * _doc ) override
        {
			IntrusivePtr<T> decoder = m_factory->createObject( _doc );

            return decoder;
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
        inline DecoderFactoryInterfacePtr registerDecoder( const ConstString & _type )
        {
            DecoderFactoryInterfacePtr decoder = Helper::makeFactorableUnique<DecoderFactory<T>>();

            if( decoder->initialize() == false )
            {
                return nullptr;
            }

            if( CODEC_SERVICE()
                ->registerDecoder( _type, decoder ) == false )
            {
                return nullptr;
            }

            return decoder;
        }

        inline void unregisterDecoder( const ConstString & _type )
        {
            DecoderFactoryInterfacePtr decoder = CODEC_SERVICE()
                ->unregisterDecoder( _type );

            if( decoder == nullptr )
            {
                LOGGER_ERROR( "invalid unregister decoder '%s'"
                    , _type.c_str()
                );

                return;
            }

            decoder->finalize();
        }
    }
}
