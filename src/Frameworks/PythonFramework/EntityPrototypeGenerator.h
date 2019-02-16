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
    class EntityPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        EntityPrototypeGenerator();
        ~EntityPrototypeGenerator() override;

    public:
        void setKernel( pybind::kernel_interface * _kernel );
        pybind::kernel_interface * getKernel() const;

    public:
        void setGenerator( const pybind::object & _generator );
        const pybind::object & getGenerator() const;

    public:
        const pybind::object & getPythonType() const;

    public:
        bool initialize() override;
        void finalize() override;

    protected:
        FactoryPtr _initializeFactory() override;

    protected:
		FactorablePointer generate( const Char * _doc ) override;

    protected:
        const pybind::object & preparePythonType_();

    protected:
        pybind::kernel_interface * m_kernel;

        EventablePtr m_eventable;

        pybind::object m_generator;
        pybind::object m_type;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityPrototypeGenerator> EntityPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////

}