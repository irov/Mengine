#pragma once

#include "Kernel/Factory.h"
#include "Kernel/AssertionFactory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class FactoryStorage
    {
    public:
        static void initialize( const FactoryPtr & _factory )
        {
            m_factory = _factory;
        }

        static void finalize()
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );

            m_factory = nullptr;
        }

    public:
        static const FactoryPtr & getFactory()
        {
            return m_factory;
        }

    public:
        static FactorablePointer createObject( const DocumentPtr & _doc )
        {
            FactorablePointer p = m_factory->createObject( _doc );

            return p;
        }

    protected:
        static FactoryPtr m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    FactoryPtr FactoryStorage<T>::m_factory;
    //////////////////////////////////////////////////////////////////////////
}
