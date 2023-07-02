#pragma once

#include "Interface/FactoryInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/FactoryPrototypeGenerator.h"
#include "Kernel/Eventable.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/Entity.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonEntityPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
        DECLARE_FACTORABLE( PythonEntityPrototypeGenerator );

    public:
        PythonEntityPrototypeGenerator();
        ~PythonEntityPrototypeGenerator() override;

    public:
        void setGenerator( const pybind::object & _generator );
        const pybind::object & getGenerator() const;

    public:
        void setPythonType( const pybind::object & _type );
        const pybind::object & getPythonType();

    public:
        bool initialize() override;
        void finalize() override;

    protected:
        FactoryInterfacePtr _initializeFactory() override;
        void _finalizeFactory() override;

    protected:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override;

    protected:
        EventablePtr m_eventable;

        pybind::object m_generator;
        pybind::object m_type;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonEntityPrototypeGenerator> PythonEntityPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////

}