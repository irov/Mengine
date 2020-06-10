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

            VOCABULARY_SET( ConverterFactoryInterface, STRINGIZE_STRING_LOCAL( "ConverterFactory" ), Helper::stringizeString( _type ), converter, _doc );

            return converter;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void unregisterConverter( const Char * _type )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ConverterFactory" ), Helper::stringizeString( _type ) );
        }
    }
}