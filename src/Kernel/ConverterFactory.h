#pragma once

#include "Interface/ConverterFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ConverterFactory
        : public ConverterFactoryInterface
        , public Factorable
    {
    public:
        ConverterFactory()
        {
        }

        ~ConverterFactory() override
        {
        }

    protected:
        bool initialize() override
        {
            m_converterFactory = Helper::makeFactoryPool<T, 8>( MENGINE_DOCUMENT_FACTORABLE );

            return true;
        }

        void finalize() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_converterFactory );

            m_converterFactory = nullptr;
        }

    protected:
        ConverterInterfacePtr createConverter( const DocumentInterfacePtr & _doc ) override
        {
            IntrusivePtr<T> converter = m_converterFactory->createObject( _doc );

            return converter;
        }

    protected:
        FactoryInterfacePtr m_converterFactory;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE ConverterFactoryInterfacePtr registerConverter( const Char * _type, const DocumentInterfacePtr & _doc )
        {
            ConverterFactoryInterfacePtr converter = Helper::makeFactorableUnique<ConverterFactory<T>>( _doc );

            if( converter->initialize() == false )
            {
                return nullptr;
            }

            VOCABULARY_SET( ConverterFactoryInterface, STRINGIZE_STRING_LOCAL( "ConverterFactory" ), Helper::stringizeString( _type ), converter, _doc );

            return converter;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void unregisterConverter( const Char * _type )
        {
            ConverterFactoryInterfacePtr converter = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ConverterFactory" ), Helper::stringizeString( _type ) );

            converter->finalize();
        }
        //////////////////////////////////////////////////////////////////////////
    }
}