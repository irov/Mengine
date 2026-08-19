#pragma once

#include "Interface/FactoryInterface.h"

#include "Kernel/ConstString.h"

#include "Kernel/FactoryWithMutex.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/Tuple.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/VocabularyHelper.h"

#include <type_traits>
#include <utility>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T, class ... Args>
        class ConverterFactoryWithArguments
            : public FactoryWithMutex
        {
        public:
            ConverterFactoryWithArguments( Args ... _args )
                : m_args( std::move( _args ) ... )
            {
            }

            ~ConverterFactoryWithArguments() override
            {
            }

        protected:
            Factorable * _createObject() override
            {
                Factorable * object = std::apply( []( const Args & ... _args )
                {
                    return Helper::newT<T>( _args ... );
                }, m_args );

                return object;
            }

            void _destroyObject( Factorable * _object ) override
            {
                T * object = static_cast<T *>(_object);

                Helper::deleteT( object );
            }

        protected:
            Tuple<Args ...> m_args;
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE FactoryInterfacePtr registerConverter( const ConstString & _type, const DocumentInterfacePtr & _doc )
        {
            FactoryInterfacePtr factory = Helper::makeFactoryPoolWithMutex<T, 8>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, "invalid register converter '%s'"
                , _type.c_str()
            );

            VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "ConverterFactory" ), _type, factory, _doc );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, class Arg, class ... Args>
        MENGINE_INLINE FactoryInterfacePtr registerConverter( const ConstString & _type, const DocumentInterfacePtr & _doc, Arg && _arg, Args && ... _args )
        {
            using FactoryType = Mengine::Detail::ConverterFactoryWithArguments<T, std::decay_t<Arg>, std::decay_t<Args> ...>;

            FactoryInterfacePtr factory = Helper::makeFactorableUnique<FactoryType>( _doc, std::forward<Arg>( _arg ), std::forward<Args>( _args ) ... );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, "invalid register converter '%s'"
                , _type.c_str()
            );

            const ConstString & factorableType = T::getFactorableType();

            factory->setFactoryType( factorableType );

            ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( _doc );

            factory->setMutex( mutex );

            VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "ConverterFactory" ), _type, factory, _doc );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void unregisterConverter( const ConstString & _type )
        {
            FactoryInterfacePtr factory = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ConverterFactory" ), _type );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, "invalid unregister decoder '%s'"
                , _type.c_str()
            );

            MENGINE_UNUSED( factory );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
