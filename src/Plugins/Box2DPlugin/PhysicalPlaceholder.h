#pragma once

#include "Box2DInterface.h"

#include "Box2DBody.h"

#include "Kernel/Node.h"

namespace Mengine
{
    class PhysicalPlaceholder
        : public Node
        , public UnknownPhysicalPlaceholderInterface
    {
        DECLARE_UNKNOWABLE();
        DECLARE_VISITABLE( Node );

    public:
        PhysicalPlaceholder();
        ~PhysicalPlaceholder() override;

    public:
        void setBox2DBody( const Box2DBodyInterfacePtr & _body ) override;
        const Box2DBodyInterfacePtr & getBox2DBody() const override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void updateLocalMatrix() const override;

    protected:
        Box2DBodyPtr m_body;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PhysicalPlaceholder> PhysicalBodyPtr;
    //////////////////////////////////////////////////////////////////////////
}