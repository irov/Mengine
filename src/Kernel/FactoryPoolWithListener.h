#pragma once

#include "FactoryWithListener.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class C, class M, class T>
        class MethodFactoryDestroyListener
            : public FactoryDestroyListenerInterface
        {
        public:
            MethodFactoryDestroyListener( C * _self, M _method )
                : m_self( _self )
                , m_method( _method )
            {
            }

            ~MethodFactoryDestroyListener() override
            {
            }

        protected:
            void onFactoryDestroyObject( Factorable * _object ) override
            {
                T * obj = static_cast<T *>(_object);

                (m_self->*m_method)(obj);
            }

        protected:
#if defined(MENGINE_DEBUG) //avoid warning C4324
            C * m_self;
            M m_method;
#else            
            M m_method;
            C * m_self;
#endif
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T, class C, class M>
        static FactoryDestroyListenerInterfacePtr makeFactoryDestroyListener( C * _class, M _method, const DocumentPtr & _doc )
        {
            FactoryDestroyListenerInterfacePtr destroyListener = Helper::makeFactorableUnique<MethodFactoryDestroyListener<C, M, T>>( _doc, _class, _method );

            return destroyListener;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, class C, class M>
        static void setupFactoryDestroyListener( const FactoryWithListenerPtr & _factory, C * _class, M _method, const DocumentPtr & _doc )
        {
            FactoryDestroyListenerInterfacePtr destroyListener = Helper::makeFactoryDestroyListener<T, C, M>( _class, _method, _doc );

            _factory->setDestroyListener( destroyListener );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class Type, uint32_t Count, class C, class M>
        FactoryInterfacePtr makeFactoryPoolWithListener( C * _self, M _method, const DocumentPtr & _doc )
        {
            FactoryWithListenerPtr factory = Helper::makeFactoryPool<Type, Count, FactoryWithListener>( _doc );

            Helper::setupFactoryDestroyListener<Type>( factory, _self, _method, _doc );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
