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
    template<class T>
    class ConverterFactory
        : public ConverterFactoryInterface
        , public Factorable
    {
    protected:
        bool initialize() override
        {
            m_factory = Helper::makeFactoryPool<T, 8>( MENGINE_DOCUMENT_FACTORABLE );

            return true;
        }

    protected:
        ConverterInterfacePtr createConverter( const DocumentPtr & _doc ) override
        {
            IntrusivePtr<T> converter = m_factory->createObject( _doc );

            return converter;
        }

    protected:
        void destroy() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );

            m_factory = nullptr;

            Helper::deleteT( this );
        }

    protected:
        FactoryPtr m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        MENGINE_INLINE ConverterFactoryInterfacePtr registerConverter( const Char * _type, const DocumentPtr & _doc )
        {
            ConverterFactoryInterfacePtr converter = Helper::makeFactorableUnique<ConverterFactory<T> >( _doc );

            if( converter->initialize() == false )
            {
                return nullptr;
            }

            VOCABULARY_SET( ConverterFactoryInterface, STRINGIZE_STRING_LOCAL( "ConverterFactory" ), Helper::stringizeString( _type ), converter );

            return converter;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void unregisterConverter( const Char * _type )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ConverterFactory" ), Helper::stringizeString( _type ) );
        }
    }
}