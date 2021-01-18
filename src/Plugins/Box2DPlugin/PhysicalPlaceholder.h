#pragma once

#include "Box2DInterface.h"

#include "Box2DBody.h"

#include "Kernel/Node.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    class PhysicalPlaceholder
        : public Node
        , public UnknownPhysicalPlaceholderInterface
        , protected BaseTransformation
    {
        DECLARE_UNKNOWABLE();
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        PhysicalPlaceholder();
        ~PhysicalPlaceholder() override;

    public:
        void setBox2DBody( const Box2DBodyInterfacePtr & _body ) override;
        const Box2DBodyInterfacePtr & getBox2DBody() const override;

    protected:
        void updateLocalMatrix() const override;

    protected:
        Box2DBodyPtr m_body;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PhysicalPlaceholder> PhysicalPlaceholderPtr;
    //////////////////////////////////////////////////////////////////////////
}