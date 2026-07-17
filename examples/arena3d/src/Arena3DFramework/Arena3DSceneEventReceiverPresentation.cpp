#include "Arena3DSceneEventReceiver.h"
#include "Arena3DHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdMath.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static constexpr size_t weaponIndex( Arena3D::WeaponType _weapon )
        {
            return static_cast<size_t>(_weapon);
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f interpolate( const kf_vec3_t & _from, const kf_vec3_t & _to, float _alpha )
        {
            const mt::vec3f from = Helper::kinefixVec3ToVec3f( _from );
            const mt::vec3f to = Helper::kinefixVec3ToVec3f( _to );
            return from + (to - from) * _alpha;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncPresentation_( float _alpha )
    {
        this->syncCamera_( this->localPlayer_( m_currentState ), _alpha );
        this->syncPlayers_( _alpha );
        this->syncArenaChunks_();
        this->syncTurrets_();
        this->syncProjectiles_( _alpha );
        this->syncPickups_();
        this->syncViewModels_();
        this->syncHud_();
        const uint32_t turretDraws = this->activeSimulation_().config().turretCombat == true
            ? static_cast<uint32_t>(Arena3D::MaximumTurrets * 2u)
            : 0u;
        m_drawCalls = m_visibleChunks * static_cast<uint32_t>(ArenaVisualLayerCount) + m_visiblePlayers + turretDraws +
            static_cast<uint32_t>(Arena3D::MaximumPickups) + m_visibleProjectiles + m_visibleExplosionClouds +
            m_visibleImpactFlashes + m_visibleDecals + m_visibleDeathShards +
            m_hud->getDrawCallCount();
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncArenaChunks_()
    {
        // Arena chunks contain long walls which cross their nominal quadrant.
        // A center/radius approximation can therefore hide an occluding wall
        // while part of it is still inside the camera frustum. Four low-poly
        // chunks are cheap enough to keep resident until exact generated AABB
        // frustum tests replace the approximation.
        for( const auto & layer : m_arenaChunkNodes )
        {
            for( const NodePtr & node : layer )
            {
                node->getTransformation()->setLocalPosition( {0.f, 0.f, 0.f} );
            }
        }

        m_visibleChunks = static_cast<uint32_t>(ArenaChunkCount);
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncCamera_( const Arena3D::PlayerState & _player, float _alpha )
    {
        const mt::vec3f previousPosition = Helper::kinefixVec3ToVec3f( this->localPlayer_( m_previousState ).position );
        const mt::vec3f currentPosition = Helper::kinefixVec3ToVec3f( _player.position );
        const mt::vec3f positionDelta = currentPosition - previousPosition;
        const float distanceSquared = positionDelta.x * positionDelta.x + positionDelta.y * positionDelta.y + positionDelta.z * positionDelta.z;
        const mt::vec3f position = distanceSquared > 1.f
            ? currentPosition
            : previousPosition + positionDelta * _alpha;
        const float yaw = Helper::kinefixAngleToRadians( _player.yaw );
        const float pitch = Helper::kinefixAngleToRadians( _player.pitch );
        const float cp = StdMath::cosf( pitch );
        const mt::vec3f direction( StdMath::sinf( yaw ) * cp, StdMath::sinf( pitch ), StdMath::cosf( yaw ) * cp );
        mt::vec3f cameraPosition = position + mt::vec3f( 0.f, _player.crouched ? 1.0f : 1.6f, 0.f );
        cameraPosition.y -= m_stepCameraOffset;
        m_worldCamera->setCameraPosition( cameraPosition );
        m_worldCamera->setCameraDirection( direction );
        m_worldCamera->setCameraUp( {0.f, -1.f, 0.f} );
        m_presentedCameraPosition = cameraPosition;
        m_presentedCameraDirection = direction;
        if( m_skyNode != nullptr )
        {
            m_skyNode->getTransformation()->setLocalPosition( cameraPosition );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncPlayers_( float _alpha )
    {
        m_visiblePlayers = 0;
        for( size_t index = 0; index != Arena3D::MaximumPlayers; ++index )
        {
            const Arena3D::PlayerState & player = m_currentState.players[index];
            NodePtr & node = m_playerNodes[index];
            if( player.active == false || player.alive == false || player.id == m_localPlayerId )
            {
                node->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
                continue;
            }

            const Arena3D::PlayerState & previous = m_previousState.players[index];
            const mt::vec3f position = previous.active == true && previous.id == player.id
                ? Detail::interpolate( previous.position, player.position, _alpha )
                : Helper::kinefixVec3ToVec3f( player.position );
            node->getTransformation()->setLocalPosition( position );
            node->getTransformation()->setLocalOrientationY( Helper::kinefixAngleToRadians( player.yaw ) );
            node->getTransformation()->setLocalScale( player.crouched == true
                ? mt::vec3f( 1.f, 0.68f, 1.f )
                : mt::vec3f( 1.f, 1.f, 1.f ) );
            ++m_visiblePlayers;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncTurrets_()
    {
        const bool turretCombat = this->activeSimulation_().config().turretCombat;
        for( size_t index = 0; index != Arena3D::MaximumTurrets; ++index )
        {
            const Arena3D::TurretStateData & turret = m_currentState.turrets[index];
            const bool visible = turretCombat == true && turret.state != Arena3D::TurretState::Destroyed && turret.state != Arena3D::TurretState::Respawning;
            m_turretBaseNodes[index]->getTransformation()->setLocalPosition( visible ? Helper::kinefixVec3ToVec3f( turret.position ) : mt::vec3f( 0.f, -10000.f, 0.f ) );
            float yaw = Helper::kinefixAngleToRadians( turret.yaw );
            float pitch = Helper::kinefixAngleToRadians( turret.pitch );
            const bool tracking = turret.state == Arena3D::TurretState::Tracking || turret.state == Arena3D::TurretState::Charging ||
                turret.state == Arena3D::TurretState::Firing || turret.state == Arena3D::TurretState::Cooldown;
            if( tracking == true && visible == true )
            {
                const mt::vec3f delta = Helper::kinefixVec3ToVec3f( kf_vec3_sub( this->localPlayer_( m_currentState ).position, turret.position ) );
                yaw = StdMath::atan2f( delta.x, delta.z );
                pitch = -StdMath::atan2f( delta.y, StdMath::sqrtf( delta.x * delta.x + delta.z * delta.z ) );
            }
            m_turretBaseNodes[index]->getTransformation()->setLocalOrientationY( yaw );
            m_turretBarrelNodes[index]->getTransformation()->setLocalOrientationX( pitch );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncProjectiles_( float _alpha )
    {
        Array<uint32_t, Arena3D::WeaponTypeCount> active{};
        for( size_t slot = 0; slot != Arena3D::MaximumProjectiles; ++slot )
        {
            const Arena3D::ProjectileState & projectile = m_currentState.projectiles[slot];
            if( projectile.active == false )
            {
                continue;
            }
            if( projectile.launchDelayTicks != 0 )
            {
                continue;
            }
            const bool playerProjectile = projectile.ownerId == this->localPlayer_( m_currentState ).id;
            const float traveledDistance = kf_fixed_to_float( projectile.traveledDistance );
            // The view-model is a depth-hacked HUD layer, so it cannot occlude
            // world projectiles. Do not expose the projectile until it has
            // cleared the virtual barrel.
            if( playerProjectile == true && traveledDistance < 1.5f )
            {
                continue;
            }
            const size_t weapon = Detail::weaponIndex( projectile.weapon );
            Vector<NodePtr> & pool = m_projectilePools[weapon];
            const uint32_t poolIndex = active[weapon]++;
            if( poolIndex == pool.size() )
            {
                NodePtr node = this->createMeshNode_( Helper::stringizeStringFormat( "Projectile_%u_%u", static_cast<uint32_t>(weapon), poolIndex ),
                    m_projectileResources[weapon], m_emissiveTexture, STRINGIZE_STRING_LOCAL( "Arena3D_Emissive" ),
                    m_worldCamera, m_postProcess.get() );
                pool.emplace_back( node );
            }
            const Arena3D::ProjectileState & previous = m_previousState.projectiles[slot];
            const mt::vec3f position = previous.active && previous.id == projectile.id
                ? Detail::interpolate( previous.position, projectile.position, _alpha )
                : Helper::kinefixVec3ToVec3f( projectile.position );
            TransformationInterface * transformation = pool[poolIndex]->getTransformation();
            const mt::vec3f velocity = Helper::kinefixVec3ToVec3f( projectile.velocity );
            const float speedSquared = velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z;
            mt::vec3f presentedPosition = position;
            if( speedSquared > StdMath::constant_eps )
            {
                if( playerProjectile == true )
                {
                    const float horizontal = StdMath::sqrtf( velocity.x * velocity.x + velocity.z * velocity.z );
                    const mt::vec3f right = horizontal > StdMath::constant_eps
                        ? mt::vec3f( velocity.z / horizontal, 0.f, -velocity.x / horizontal )
                        : mt::vec3f( 1.f, 0.f, 0.f );
                    mt::vec3f direction = velocity * (1.f / StdMath::sqrtf( speedSquared ));
                    mt::vec3f up;
                    mt::cross_v3_v3_norm( &up, direction, right );
                    const float muzzleBlend = Mengine::StdAlgorithm::max( 0.f, Mengine::StdAlgorithm::min( 1.f, (3.5f - traveledDistance) * 0.5f ) );
                    presentedPosition += (right * 0.38f - up * 0.22f + direction * 0.8f) * muzzleBlend;
                }
                const mt::vec3f up = velocity.x * velocity.x + velocity.z * velocity.z > StdMath::constant_eps
                    ? mt::vec3f( 0.f, 1.f, 0.f )
                    : mt::vec3f( 0.f, 0.f, 1.f );
                transformation->setDirection( velocity, up );
            }
            transformation->setLocalPosition( presentedPosition );
        }
        m_visibleProjectiles = 0;
        for( size_t weapon = 0; weapon != active.size(); ++weapon )
        {
            this->parkUnused_( m_projectilePools[weapon], active[weapon] );
            m_visibleProjectiles += active[weapon];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncViewModels_()
    {
        const size_t selected = Detail::weaponIndex( m_displayedWeapon );
        float lowered = 0.f;
        if( m_viewModelTransition == ViewModelTransition::Lowering )
        {
            lowered = m_viewModelTransitionTime / 0.2f;
        }
        else if( m_viewModelTransition == ViewModelTransition::Raising )
        {
            lowered = 1.f - m_viewModelTransitionTime / 0.25f;
        }
        lowered = Mengine::StdAlgorithm::max( 0.f, Mengine::StdAlgorithm::min( 1.f, lowered ) );
        const mt::vec3f position( 0.38f, -0.28f - lowered * 1.1f, 1.05f );
        for( size_t index = 0; index != m_viewModels.size(); ++index )
        {
            m_viewModels[index]->getTransformation()->setLocalPosition( index == selected
                ? position
                : mt::vec3f( 0.f, -10000.f, 0.f ) );
            m_viewModels[index]->getTransformation()->setLocalOrientation( {0.f, 0.f, 0.f} );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncPickups_()
    {
        for( size_t index = 0; index != Arena3D::MaximumPickups; ++index )
        {
            const Arena3D::PickupStateData & pickup = m_currentState.pickups[index];
            m_pickupNodes[index]->getTransformation()->setLocalPosition( pickup.active == true
                ? Helper::kinefixVec3ToVec3f( pickup.position )
                : mt::vec3f( 0.f, -10000.f, 0.f ) );
            m_pickupNodes[index]->getTransformation()->setLocalOrientationY( static_cast<float>(m_currentState.tick % 628u) * 0.01f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncHud_()
    {
        m_hud->sync( this->localPlayer_( m_currentState ), m_warningFlash );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::parkUnused_( Vector<NodePtr> & _pool, uint32_t _activeCount )
    {
        for( uint32_t index = _activeCount; index != _pool.size(); ++index )
        {
            _pool[index]->getTransformation()->setLocalPosition( { 0.f, -10000.f, 0.f } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
