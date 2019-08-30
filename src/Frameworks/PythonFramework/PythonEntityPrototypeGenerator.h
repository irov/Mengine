#pragma once

#include "Kernel/FactoryPrototypeGenerator.h"
#include "Kernel/Eventable.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/Factory.h"
#include "Kernel/Entity.h"

#include "pybind/object.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonEntityPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        PythonEntityPrototypeGenerator();
        ~PythonEntityPrototypeGenerator() override;

    public:
        void setGenerator( const pybind::object & _generator );
        const pybind::object & getGenerator() const;

    public:
        const pybind::object & getPythonType();

    public:
        bool initialize() override;
        void finalize() override;

    protected:
        FactoryPtr _initializeFactory() override;
        void _finalizeFactory() override;

    protected:
        FactorablePointer generate( const Char * _doc ) override;

    protected:
        EventablePtr m_eventable;

        pybind::object m_generator;
        pybind::object m_type;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonEntityPrototypeGenerator> PythonEntityPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////

}