#include "Arena3DCollisionWorld.h"

#include "Arena3DGeneratedCollision.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/ExceptionHelper.h"
#include "Kernel/Vector.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * allocateCollision( void * _context, size_t _bytes )
        {
            MENGINE_UNUSED( _context );

            return Mengine::Helper::allocateMemory( _bytes, "Arena3D" );
        }
        //////////////////////////////////////////////////////////////////////////
        static void freeCollision( void * _context, void * _memory )
        {
            MENGINE_UNUSED( _context );

            Mengine::Helper::deallocateMemory( _memory, "Arena3D" );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool isCollisionHit( kf_status_t _status )
    {
        if( _status == KF_NO_HIT )
        {
            return false;
        }
        if( _status != KF_OK )
        {
            MENGINE_THROW_EXCEPTION( "collision query failed: %u", _status );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    kf_shape_t makeCapsuleShape( kf_fixed_t _radius, kf_fixed_t _height )
    {
        kf_shape_t shape{};
        shape.type = KF_SHAPE_CAPSULE;
        shape.data.capsule.radius = _radius;
        shape.data.capsule.height = _height;

        return shape;
    }
    //////////////////////////////////////////////////////////////////////////
    kf_shape_t makeSphereShape( kf_fixed_t _radius )
    {
        kf_shape_t shape{};
        shape.type = KF_SHAPE_SPHERE;
        shape.data.sphere.radius = _radius;

        return shape;
    }
    //////////////////////////////////////////////////////////////////////////
    CollisionWorld::CollisionWorld()
        : m_world( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CollisionWorld::CollisionWorld( const CollisionWorld & _other )
        : CollisionWorld()
    {
        MENGINE_UNUSED( _other );
    }
    //////////////////////////////////////////////////////////////////////////
    CollisionWorld::~CollisionWorld()
    {
        kf_world_destroy( m_world );
    }
    //////////////////////////////////////////////////////////////////////////
    CollisionWorld & CollisionWorld::operator=( const CollisionWorld & _other )
    {
        MENGINE_UNUSED( _other );

        // The immutable map index is a cache, independent of simulation state.
        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    kf_world_t * CollisionWorld::getWorld()
    {
        if( m_world != nullptr )
        {
            return m_world;
        }

        CollisionWorld candidate;
        kf_world_config_t config = kf_world_config_default();
        config.step = {1, 100};
        kf_allocator_t allocator{Detail::allocateCollision, Detail::freeCollision, nullptr};
        kf_status_t status = kf_world_create( &config, &allocator, &candidate.m_world );
        if( status != KF_OK )
        {
            MENGINE_THROW_EXCEPTION( "collision world creation failed: %u", status );
        }

        kf_capacity_t capacity{1, Arena3DGenerated::CollisionColliderCount, 0, 1, 1};
        status = kf_world_reserve( candidate.m_world, capacity );
        if( status != KF_OK )
        {
            MENGINE_THROW_EXCEPTION( "collision world reservation failed: %u", status );
        }

        Mengine::Vector<kf_shape_desc_t> shapes;
        shapes.reserve( Arena3DGenerated::CollisionColliderCount );
        for( const Arena3DGenerated::CollisionBrushRaw & brush : Arena3DGenerated::CollisionColliders )
        {
            kf_shape_desc_t shape{};
            shape.id = brush.id;
            shape.shape.type = KF_SHAPE_AABB;
            shape.shape.data.aabb = brush.bounds;
            shapes.emplace_back( shape );
        }

        kf_body_desc_t body = kf_body_desc_default( 1 );
        body.type = KF_BODY_STATIC;
        kf_handle_t handle{};
        status = kf_world_add_body( candidate.m_world, &body, shapes.data(), shapes.size(), &handle );
        if( status != KF_OK )
        {
            MENGINE_THROW_EXCEPTION( "collision map creation failed: %u", status );
        }
        status = kf_world_rebuild_index( candidate.m_world );
        if( status != KF_OK )
        {
            MENGINE_THROW_EXCEPTION( "collision index creation failed: %u", status );
        }

        m_world = candidate.m_world;
        candidate.m_world = nullptr;

        return m_world;
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
