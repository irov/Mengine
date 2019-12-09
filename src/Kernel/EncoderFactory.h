#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/EncoderFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/ConstStringHelper.h"

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
            m_factory = Helper::makeFactoryPool<T, 8>();

            return true;
        }

        void finalize() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );

            m_factory = nullptr;
        }

    protected:
        EncoderInterfacePtr createEncoder( const DocumentPtr & _doc ) override
        {
            IntrusivePtr<T> encoder = m_factory->createObject( _doc );

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
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE EncoderFactoryInterfacePtr registerEncoder( const ConstString & _type )
        {
            EncoderFactoryInterfacePtr factory = Helper::makeFactorableUnique<EncoderFactory<T>>();

            MENGINE_ASSERTION_MEMORY_PANIC( factory, nullptr );

            if( factory->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize encoder '%s'"
                    , _type.c_str()
                );

                return nullptr;
            }

            VOCABULARY_SET( EncoderFactoryInterface, STRINGIZE_STRING_LOCAL( "EncoderFactory" ), _type, factory );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void unregisterEncoder( const ConstString & _type )
        {
            EncoderFactoryInterfacePtr factory = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "EncoderFactory" ), _type );

            MENGINE_ASSERTION_MEMORY_PANIC_VOID( factory, "invalid unregister encoder '%s'"
                , _type.c_str()
            );

            factory->finalize();
        }
    }
}