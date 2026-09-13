#include "Arena3DSceneEventReceiver.h"

#include "Arena3DGeneratedModels.h"
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
        constexpr float STEP_CAMERA_RESPONSE = 18.f;
        constexpr float CAMERA_TELEPORT_DISTANCE = 4.f;

        const mt::vec3f WEAPON_POSITION( 0.28f, -0.30f, 0.68f );
        constexpr float WEAPON_RECOIL_DISTANCE = 0.09f;
        constexpr float WEAPON_RECOIL_ANGLE = 0.08f;
        constexpr float WEAPON_BOB_FREQUENCY = 8.f;
        constexpr float WEAPON_IDLE_FREQUENCY = 1.8f;
        constexpr float WEAPON_IDLE_HEIGHT = 0.003f;
        constexpr float WEAPON_WALK_SPEED = 9.f;
        constexpr float WEAPON_LOWER_DISTANCE = 0.9f;
        const mt::vec3f WEAPON_BOB_AMPLITUDE( 0.012f, 0.01f, 0.f );
        const mt::vec3f WEAPON_SWAY_ANGLES( 0.f, 0.008f, 0.02f );
        constexpr float MUZZLE_SPIN_SPEED = 91.f;
        constexpr float PICKUP_BOB_HEIGHT = 0.12f;
        constexpr float PICKUP_BOB_FREQUENCY = 2.f;

        constexpr float PICKUP_PHASE_STEP = 0.73f;
        constexpr float PICKUP_LIFT = 0.18f;
        constexpr float PICKUP_SPIN_SPEED = 1.25f;
        constexpr float GRENADE_TUMBLE_SPEED = 9.f;
        constexpr float GRENADE_TUMBLE_RATIO = 0.7f;

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
    void Arena3DSceneEventReceiver::syncPresentation_( float _alpha, float _seconds )
    {
        const Arena3D::PlayerState & player = this->localPlayer_( m_currentState );
        this->syncCamera_( player, _alpha, _seconds );
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
        m_drawCalls = m_visibleChunks + m_visiblePlayers + turretDraws +
            static_cast<uint32_t>(Arena3D::MaximumPickups) + m_visibleProjectiles + m_visibleExplosionClouds +
            m_visibleImpactFlashes + m_visibleRailTrails + m_visibleDecals + m_visibleDeathShards +
            m_hud->getDrawCallCount();
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncArenaChunks_()
    {
        // Imported chunks retain map coordinates and may span several rooms.
        // Keep them resident until exact generated bounds support frustum culling.
        size_t chunkCount = m_arenaChunkNodes.size();
        m_visibleChunks = static_cast<uint32_t>(chunkCount);
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncCamera_( const Arena3D::PlayerState & _player, float _alpha, float _seconds )
    {
        const Arena3D::PlayerState & previousPlayer = this->localPlayer_( m_previousState );
        mt::vec3f previousPosition = Helper::kinefixVec3ToVec3f( previousPlayer.position );
        mt::vec3f currentPosition = Helper::kinefixVec3ToVec3f( _player.position );

        mt::vec3f positionDelta = currentPosition - previousPosition;
        float distanceSquared = mt::sqrlength_v3( positionDelta );

        mt::vec3f position = distanceSquared > 1.f
            ? currentPosition
            : previousPosition + positionDelta * _alpha;

        mt::vec3f previousCameraFoot( m_presentedCameraPosition.x, m_cameraFootHeight, m_presentedCameraPosition.z );
        mt::vec3f cameraDelta = position - previousCameraFoot;
        float cameraDistanceSquared = mt::sqrlength_v3( cameraDelta );
        constexpr float teleportDistanceSquared = Detail::CAMERA_TELEPORT_DISTANCE * Detail::CAMERA_TELEPORT_DISTANCE;

        if( cameraDistanceSquared > teleportDistanceSquared || _player.noclip == true || _player.alive == false || previousPlayer.id != _player.id )
        {
            m_stepCameraOffset = 0.f;
            m_stepCameraVelocity = 0.f;
        }
        else if( previousPlayer.velocity.y <= 0 && _player.velocity.y <= 0 && cameraDelta.y > 0.f )
        {
            // Stepping can briefly lose ground contact at a rounded capsule edge.
            // A rise without upward velocity is a step, not a jump impulse.
            m_stepCameraOffset += cameraDelta.y;
        }

        m_cameraFootHeight = position.y;

        // A critically damped response preserves upward velocity across risers.
        // Its closed form remains stable when several ticks share a frame.
        float decayExponent = -Detail::STEP_CAMERA_RESPONSE * _seconds;
        float decay = StdMath::expf( decayExponent );
        float response = m_stepCameraVelocity + Detail::STEP_CAMERA_RESPONSE * m_stepCameraOffset;

        m_stepCameraOffset = (m_stepCameraOffset + response * _seconds) * decay;
        m_stepCameraVelocity = (m_stepCameraVelocity - Detail::STEP_CAMERA_RESPONSE * response * _seconds) * decay;

        float yaw = Helper::kinefixAngleToRadians( _player.yaw );
        float pitch = Helper::kinefixAngleToRadians( _player.pitch );
        float cp = StdMath::cosf( pitch );
        float directionX = StdMath::sinf( yaw ) * cp;
        float directionY = StdMath::sinf( pitch );
        float directionZ = StdMath::cosf( yaw ) * cp;
        mt::vec3f direction( directionX, directionY, directionZ );

        float eyeHeight = _player.crouched == true ? 1.f : 1.6f;
        mt::vec3f cameraPosition = position;
        cameraPosition.y += eyeHeight;
        cameraPosition.y -= m_stepCameraOffset;

        m_worldCamera->setCameraPosition( cameraPosition );
        m_worldCamera->setCameraDirection( direction );
        m_worldCamera->setCameraUp( {0.f, -1.f, 0.f} );

        m_presentedCameraPosition = cameraPosition;
        m_presentedCameraDirection = direction;

        if( m_skyNode != nullptr )
        {
            TransformationInterface * skyTransformation = m_skyNode->getTransformation();
            skyTransformation->setLocalPosition( cameraPosition );
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
                bool grenade = projectile.weapon == Arena3D::WeaponType::GrenadeLauncher;
                const ResourceImagePtr & texture = grenade == true ? m_grenadeTexture : m_emissiveTexture;
                ConstString material = grenade == true ? STRINGIZE_STRING_LOCAL( "Arena3D_Weapon" ) : STRINGIZE_STRING_LOCAL( "Arena3D_Emissive" );

                uint32_t weaponIndex = static_cast<uint32_t>(weapon);
                ConstString nodeName = Helper::stringizeStringFormat( "Projectile_%u_%u", weaponIndex, poolIndex );
                Node * parent = m_postProcess.get();
                NodePtr node = this->createMeshNode_( nodeName, m_projectileResources[weapon], texture, material, m_worldCamera, parent );

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

            if( projectile.weapon == Arena3D::WeaponType::GrenadeLauncher && speedSquared > 0.01f )
            {
                float spin = m_presentationTime * Detail::GRENADE_TUMBLE_SPEED + static_cast<float>(projectile.id);
                float yawSpin = spin * Detail::GRENADE_TUMBLE_RATIO;
                mt::vec3f orientation( spin, yawSpin, 0.f );

                transformation->setLocalOrientation( orientation );
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

        const Arena3D::PlayerState & player = this->localPlayer_( m_currentState );
        mt::vec3f velocity = Helper::kinefixVec3ToVec3f( player.velocity );
        float horizontalSpeedSquared = velocity.x * velocity.x + velocity.z * velocity.z;
        float horizontalSpeed = StdMath::sqrtf( horizontalSpeedSquared );
        float relativeSpeed = horizontalSpeed / Detail::WEAPON_WALK_SPEED;
        float bob = StdAlgorithm::min( 1.f, relativeSpeed );

        float bobPhase = m_presentationTime * Detail::WEAPON_BOB_FREQUENCY;
        float sway = StdMath::sinf( bobPhase );
        float liftPhase = bobPhase * 2.f;
        float lift = StdMath::cosf( liftPhase );

        float idlePhase = m_presentationTime * Detail::WEAPON_IDLE_FREQUENCY;
        float idle = StdMath::sinf( idlePhase ) * Detail::WEAPON_IDLE_HEIGHT;

        mt::vec3f position = Detail::WEAPON_POSITION;
        position.x += sway * bob * Detail::WEAPON_BOB_AMPLITUDE.x;
        position.y += -lowered * Detail::WEAPON_LOWER_DISTANCE + lift * bob * Detail::WEAPON_BOB_AMPLITUDE.y + idle;
        position.z -= m_weaponKick * Detail::WEAPON_RECOIL_DISTANCE;

        mt::vec3f orientation = Detail::WEAPON_SWAY_ANGLES * (sway * bob);
        orientation.x = -m_weaponKick * Detail::WEAPON_RECOIL_ANGLE;

        for( size_t index = 0; index != m_viewModels.size(); ++index )
        {
            mt::vec3f weaponPosition = index == selected ? position : mt::vec3f( 0.f, -10000.f, 0.f );
            TransformationInterface * transformation = m_viewModels[index]->getTransformation();
            transformation->setLocalPosition( weaponPosition );
            transformation->setLocalOrientation( orientation );

            TransformationInterface * muzzle = m_muzzleNodes[index]->getTransformation();
            mt::vec3f muzzlePosition = m_muzzleTime > 0.f && index == selected
                ? Arena3DGenerated::MUZZLE_POSITIONS[index] : mt::vec3f( 0.f, -10000.f, 0.f );
            float flashRotation = m_presentationTime * Detail::MUZZLE_SPIN_SPEED;

            muzzle->setLocalPosition( muzzlePosition );
            muzzle->setLocalOrientationZ( flashRotation );
        }

        TransformationInterface * barrel = m_barrelNode->getTransformation();
        barrel->setLocalOrientationZ( m_barrelAngle );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::syncPickups_()
    {
        for( size_t index = 0; index != Arena3D::MaximumPickups; ++index )
        {
            const Arena3D::PickupStateData & pickup = m_currentState.pickups[index];
            float phase = m_presentationTime * Detail::PICKUP_BOB_FREQUENCY + static_cast<float>(index) * Detail::PICKUP_PHASE_STEP;
            mt::vec3f position = pickup.active == true
                ? Helper::kinefixVec3ToVec3f( pickup.position ) : mt::vec3f( 0.f, -10000.f, 0.f );
            position.y += Detail::PICKUP_LIFT + StdMath::sinf( phase ) * Detail::PICKUP_BOB_HEIGHT;

            float rotation = m_presentationTime * Detail::PICKUP_SPIN_SPEED + static_cast<float>(index);

            TransformationInterface * transformation = m_pickupNodes[index]->getTransformation();
            transformation->setLocalPosition( position );
            transformation->setLocalOrientationY( rotation );
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
