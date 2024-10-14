#pragma once

#include "Plugins/Box2DPlugin/Box2DInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Node.h"
#include "Kernel/JSON.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Unit
        : public Factorable
    {
    public:
        Unit();
        ~Unit() override;

    public:
        virtual void initialize( const Box2DWorldInterfacePtr & _world, const NodePtr & _root, const mt::vec2f & _position, const jpp::object & _data );
        virtual void finalize();

    public:
        const Box2DWorldInterfacePtr & getWorld() const;
        const NodePtr & getRoot() const;

    public:
        Box2DBodyInterfacePtr makeBody( const mt::vec2f & _position, const jpp::object & _data );
        NodePtr makeRender( const jpp::object & _data );
        NodePtr makePhysicalPlaceholder( const Box2DBodyInterfacePtr & _body );

    public:
        float getRadius() const;
        const NodePtr & getNode() const;
        const Box2DBodyInterfacePtr & getBody() const;

    public:
        void setMoveDirection( const mt::vec2f & _moveDirection );
        const mt::vec2f & getMoveDirection() const;

    public:
        Box2DWorldInterfacePtr m_world;
        NodePtr m_root;

        float m_radius;

        NodePtr m_node;
        Box2DBodyInterfacePtr m_body;

        mt::vec2f m_moveDirection;
        float m_moveSpeed;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Unit> UnitPtr;
    //////////////////////////////////////////////////////////////////////////
}