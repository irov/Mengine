#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/EventationInterface.h"

#include "Kernel/EntityBehaviorInterface.h"

#include "Kernel/Node.h"
#include "Kernel/DummyRender.h"
#include "Kernel/DummyPicker.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Entity
        : public Node
        , protected DummyRender
        , protected DummyPicker
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( Entity );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_PICKERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        Entity();
        ~Entity() override;

    public:
        void setPrototype( const ConstString & _prototype );
        const ConstString & getPrototype() const;

    public:
        void setBehaviorEventable( const EventablePtr & _behaviorEventable );
        const EventablePtr & getBehaviorEventable() const;

    public:
        void setBehavior( const EntityBehaviorInterfacePtr & _behavior );
        const EntityBehaviorInterfacePtr & getBehavior() const;

    public:
        EventationInterface * getEventation() override;
        const EventationInterface * getEventation() const override;

    protected:
        const RenderViewportInterfacePtr & getPickerViewport() const override;
        const RenderCameraInterfacePtr & getPickerCamera() const override;
        const RenderScissorInterfacePtr & getPickerScissor() const override;

    public:
        bool create();

    protected:
        virtual bool _create();

    protected:
        void _dispose() override;

    protected:
        bool _activate() override;
        bool _afterActivate() override;

        void _deactivate() override;
        void _afterDeactivate() override;

        bool _compile() override;
        void _release() override;

    protected:
        void _destroy() override;

    protected:
        ConstString m_prototype;

        EventablePtr m_behaviorEventable;
        EntityBehaviorInterfacePtr m_behavior;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Entity> EntityPtr;
    //////////////////////////////////////////////////////////////////////////
}