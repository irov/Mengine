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

        protected:
            void onFactoryDestroyObject( Factorable * _object ) override
            {
                T * obj = static_cast<T *>(_object);

                (m_self->*m_method)(obj);
            }

        protected:
            void destroy() override
            {
                delete this;
            }

        protected:
            C * m_self;
            M m_method;
        };
        //////////////////////////////////////////////////////////////////////////
        template<class C, class M, class T>
        FactoryDestroyListenerInterfacePtr makeFactoryDestroyListener( C * _class, M _method )
        {
            return new FactorableUnique<MethodFactoryDestroyListener<C, M, T> >( _class, _method );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, class C, class M>
        void setupFactoryDestroyListener( const FactoryWithListenerPtr & _factory, C * _class, M _method )
        {
            FactoryDestroyListenerInterfacePtr destroyListener = new FactorableUnique<MethodFactoryDestroyListener<C, M, T> >( _class, _method );

            _factory->setDestroyListener( destroyListener );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class Type, uint32_t Count, class C, class M>
        FactoryPtr makeFactoryPoolWithListener( C * _self, M _method )
        {
            FactoryWithListenerPtr factory = new FactoryPool<Type, Count, FactoryWithListener>();

            setupFactoryDestroyListener<Type>( factory, _self, _method );

            return factory;
        }
    }
}
