#pragma once

#include "Kernel/BasePrototypeGenerator.h"
#include "Kernel/Eventable.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/Entity.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EntityPrototypeGenerator
        : public BasePrototypeGenerator
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

    public:
        pybind::object preparePythonType();

    protected:
        PointerFactorable generate() override;

    public:
        PyObject * getGenerator() const;

    protected:
        uint32_t count() const override;

    protected:
        pybind::object m_generator;
        pybind::object m_type;

        uint32_t m_count;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityPrototypeGenerator> EntityPrototypeGeneratorPtr;

}