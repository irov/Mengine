#pragma once

#include "kinefix/kinefix.hpp"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    bool isCollisionHit( kf_status_t _status );
    kf_shape_t makeCapsuleShape( kf_fixed_t _radius, kf_fixed_t _height );
    kf_shape_t makeSphereShape( kf_fixed_t _radius );
    //////////////////////////////////////////////////////////////////////////
    class CollisionWorld
    {
    public:
        CollisionWorld();
        CollisionWorld( const CollisionWorld & _other );
        ~CollisionWorld();

        CollisionWorld & operator=( const CollisionWorld & _other );

        kf_world_t * getWorld();

    private:
        kf_world_t * m_world;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
