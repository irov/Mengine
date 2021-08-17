#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Updatable.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/ConstString.h"
#include "Kernel/Node.h"
#include "Kernel/Vector.h"

#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Motor
        : public Factorable
        , public Updatable
        , protected BaseUpdation
    {
        DECLARE_FACTORABLE( Motor );
        DECLARE_UPDATABLE();

    public:
        Motor();
        ~Motor() override;

    public:
        void setNode( const NodePtr & _node );
        const NodePtr & getNode() const;

    public:
        void addVelocity( const ConstString & _name, const mt::vec3f & _velocity );
        bool hasVelocity( const ConstString & _name ) const;
        mt::vec3f getVelocity( const ConstString & _name ) const;

    protected:
        void update( const UpdateContext * _context ) override;

    protected:
        NodePtr m_node;

        struct VelocityDesc
        {
            ConstString name;
            mt::vec3f velocity;
        };

        typedef Vector<VelocityDesc> VectorVelocities;
        VectorVelocities m_velocities;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Motor> MotorPtr;
    //////////////////////////////////////////////////////////////////////////
}