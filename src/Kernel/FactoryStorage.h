#pragma once

#include "Interface/FactoryInterface.h"

#include "Kernel/AssertionFactory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class FactoryStorage
    {
    public:
        static void initialize( const FactoryInterfacePtr & _factory )
        {
            m_factory = _factory;
        }

        static void finalize()
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );

            m_factory = nullptr;
        }

    public:
        static const FactoryInterfacePtr & getFactory()
        {
            return m_factory;
        }

    public:
        static FactorablePointer createObject( const DocumentInterfacePtr & _doc )
        {
            FactorablePointer p = m_factory->createObject( _doc );

            return p;
        }

    protected:
        static FactoryInterfacePtr m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    FactoryInterfacePtr FactoryStorage<T>::m_factory;
    //////////////////////////////////////////////////////////////////////////
}
