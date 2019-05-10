#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/DecoderFactoryInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Factorable.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

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
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        inline DecoderFactoryInterfacePtr registerDecoder( const ConstString & _type )
        {
            DecoderFactoryInterfacePtr factory = Helper::makeFactorableUnique<DecoderFactory<T>>();

            MENGINE_ASSERTION_MEMORY_PANIC( factory, nullptr );

            if( factory->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize decoder '%s'"
                    , _type.c_str()
                );

                return nullptr;
            }

            VOCABULARY_SET( DecoderFactoryInterface, STRINGIZE_STRING_LOCAL( "DecoderFactory" ), _type, factory );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
        inline void unregisterDecoder( const ConstString & _type )
        {
            DecoderFactoryInterfacePtr factory = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DecoderFactory" ), _type );

            if( factory == nullptr )
            {
                LOGGER_ERROR( "invalid unregister decoder '%s'"
                    , _type.c_str()
                );

                return;
            }

            factory->finalize();
        }
    }
}
