#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/EncoderFactoryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class EncoderFactory
        : public EncoderFactoryInterface
        , public Factorable
    {
    public:
        EncoderFactory()
        {
        }

        ~EncoderFactory() override
        {
        }

    protected:
        bool initialize() override
        {
            m_factory = Helper::makeFactoryPool<T, 8>( MENGINE_DOCUMENT_FACTORABLE );

            return true;
        }

        void finalize() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );

            m_factory = nullptr;
        }

    protected:
        EncoderInterfacePtr createEncoder( const DocumentInterfacePtr & _doc ) override
        {
            IntrusivePtr<T> encoder = m_factory->createObject( _doc );

            return encoder;
        }

    protected:
        FactoryInterfacePtr m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE EncoderFactoryInterfacePtr registerEncoder( const ConstString & _type, const DocumentInterfacePtr & _doc )
        {
            EncoderFactoryInterfacePtr factory = Helper::makeFactorableUnique<EncoderFactory<T>>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

            if( factory->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize encoder '%s'"
                    , _type.c_str()
                );

                return nullptr;
            }

            VOCABULARY_SET( EncoderFactoryInterface, STRINGIZE_STRING_LOCAL( "EncoderFactory" ), _type, factory, _doc );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void unregisterEncoder( const ConstString & _type )
        {
            EncoderFactoryInterfacePtr factory = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "EncoderFactory" ), _type );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, "invalid unregister encoder '%s'"
                , _type.c_str()
            );

            factory->finalize();
        }
        //////////////////////////////////////////////////////////////////////////
    }
}