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
        void setGenerator( const pybind::object & _generator );
        const pybind::object & getGenerator() const;

    public:
        bool initialize() override;
        void finalize() override;

    protected:
        FactoryPtr _initializeFactory() override;

    public:
        pybind::object preparePythonType();

    protected:
        FactorablePointer generate() override;

    protected:
        pybind::object m_generator;
        pybind::object m_type;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityPrototypeGenerator> EntityPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////

}