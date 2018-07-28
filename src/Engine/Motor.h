#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Updatable.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "math/vec3.h"

#include "Config/Map.h"

namespace Mengine
{
    class Node;

    class Motor
        : public Factorable
        , public Updatable
    {
    public:
        Motor();
        ~Motor() override;

    public:
        void setNode( Node * _node );
        Node * getNode() const;

    public:
        void addVelocity( const ConstString & _name, const mt::vec3f & _velocity );
        bool hasVelocity( const ConstString & _name ) const;
        mt::vec3f getVelocity( const ConstString & _name ) const;

    protected:
        void _update( float _current, float _time ) override;

    protected:
        Node * m_node;

        struct VelocityDesc
        {
            mt::vec3f velocity;
        };

        typedef Map<ConstString, VelocityDesc> MapVelocity;
        MapVelocity m_velocities;
    };
}