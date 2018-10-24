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
        , public Eventable
        , public BaseEventation
    {
        DECLARE_EVENTABLE( EntityEventReceiver );

    public:
        EntityPrototypeGenerator();
        ~EntityPrototypeGenerator() override;

    public:
        void setScriptGenerator( const pybind::object & _generator );

    public:
        bool initialize() override;
        void finalize() override;

    protected:
        FactoryPtr _initializeFactory() override;

    public:
        pybind::object preparePythonType();

    protected:
        PointerFactorable generate() override;

    public:
        PyObject * getGenerator() const;

    protected:
        pybind::object m_generator;
        pybind::object m_type;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityPrototypeGenerator> EntityPrototypeGeneratorPtr;

}