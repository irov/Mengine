#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/DecoderFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Factorable.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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
            m_factory = Helper::makeFactoryPool<T, 8>( MENGINE_DOCUMENT_FACTORABLE );

            return true;
        }

        void finalize() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );

            m_factory = nullptr;
        }

    protected:
        DecoderInterfacePtr createDecoder( const DocumentPtr & _doc ) override
        {
            IntrusivePtr<T> decoder = m_factory->createObject( _doc );

            return decoder;
        }

    protected:
        void destroy() override
        {
            Helper::deleteT( this );
        }

    protected:
        FactoryPtr m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE DecoderFactoryInterfacePtr registerDecoder( const ConstString & _type, const DocumentPtr & _doc )
        {
            DecoderFactoryInterfacePtr factory = Helper::makeFactorableUnique<DecoderFactory<T>>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, nullptr );

            if( factory->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize decoder '%s'"
                    , _type.c_str()
                );

                return nullptr;
            }

            VOCABULARY_SET( DecoderFactoryInterface, STRINGIZE_STRING_LOCAL( "DecoderFactory" ), _type, factory, _doc );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void unregisterDecoder( const ConstString & _type )
        {
            DecoderFactoryInterfacePtr factory = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DecoderFactory" ), _type );

            MENGINE_ASSERTION_MEMORY_PANIC_VOID( factory, "invalid unregister decoder '%s'"
                , _type.c_str()
            );

            factory->finalize();
        }
    }
}
