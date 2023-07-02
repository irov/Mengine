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
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class DecoderFactory
        : public DecoderFactoryInterface
        , public Factorable
    {
    public:
        DecoderFactory()
        {
        }

        ~DecoderFactory() override
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
        DecoderInterfacePtr createDecoder( const DocumentInterfacePtr & _doc ) override
        {
            IntrusivePtr<T> decoder = m_factory->createObject( _doc );

            return decoder;
        }

    protected:
        FactoryInterfacePtr m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE DecoderFactoryInterfacePtr registerDecoder( const ConstString & _type, const DocumentInterfacePtr & _doc )
        {
            DecoderFactoryInterfacePtr factory = Helper::makeFactorableUnique<DecoderFactory<T>>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

            if( factory->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize decoder '%s' type '%s'"
                    , _type.c_str()
                    , T::getFactorableType().c_str()
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

            MENGINE_ASSERTION_MEMORY_PANIC( factory, "invalid unregister decoder '%s'"
                , _type.c_str()
            );

            factory->finalize();
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
