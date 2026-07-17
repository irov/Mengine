#include "Arena3DSimulation.h"

#include "Arena3DGeneratedCollision.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdAssert.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static constexpr size_t weaponIndex( WeaponType _weapon )
        {
            return static_cast<size_t>(_weapon);
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isAutomatic( WeaponType weapon )
        {
            return weapon == WeaponType::Nailgun || weapon == WeaponType::Plasmagun;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isExplosive( WeaponType weapon )
        {
            return weapon == WeaponType::RocketLauncher || weapon == WeaponType::Plasmagun || weapon == WeaponType::GrenadeLauncher;
        }
        //////////////////////////////////////////////////////////////////////////
        static kf_fixed_t projectileRadius( WeaponType weapon )
        {
            switch( weapon )
            {
            case WeaponType::RocketLauncher: return kf_fixed_from_ratio( 18, 100 );
            case WeaponType::Plasmagun: return kf_fixed_from_ratio( 12, 100 );
            case WeaponType::GrenadeLauncher: return kf_fixed_from_ratio( 16, 100 );
            case WeaponType::Shotgun: return kf_fixed_from_ratio( 4, 100 );
            default: return kf_fixed_from_ratio( 6, 100 );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t shotgunLaunchDelayTicks( uint64_t shotId, uint32_t pelletIndex )
        {
            constexpr uint32_t Delays[12] = {0, 3, 0, 6, 3, 0, 6, 0, 3, 6, 0, 3};
            const uint32_t rotation = static_cast<uint32_t>(shotId % 3u) * 4u;
            return Delays[(pelletIndex + rotation) % 12u];
        }
        //////////////////////////////////////////////////////////////////////////
        static ImpactNormal impactNormal( const kf_vec3_t & normal )
        {
            if( normal.x < 0 )
            {
                return ImpactNormal::NegativeX;
            }
            if( normal.x > 0 )
            {
                return ImpactNormal::PositiveX;
            }
            if( normal.y < 0 )
            {
                return ImpactNormal::NegativeY;
            }
            if( normal.y > 0 )
            {
                return ImpactNormal::PositiveY;
            }
            if( normal.z < 0 )
            {
                return ImpactNormal::NegativeZ;
            }
            if( normal.z > 0 )
            {
                return ImpactNormal::PositiveZ;
            }
            return ImpactNormal::Unknown;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool lineBlocked( const kf_vec3_t & start, const kf_vec3_t & end )
        {
            const kf_vec3_t delta = kf_vec3_sub( end, start );
            const kf_fixed_t length = kf_vec3_length( delta );
            if( length == 0 )
            {
                return false;
            }
            const kf_ray_t ray{start, delta, length};
            kf_hit_t hit{};
            if( kf_world_raycast( Arena3DGenerated::CollisionBoxes,
                Arena3DGenerated::CollisionBoxCount, &ray, &hit ) == KF_FALSE ) return false;
            return hit.fraction > kf_fixed_from_ratio( 1, 100 ) && hit.fraction < kf_fixed_from_ratio( 99, 100 );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::updateWeapons_()
    {
        for( PlayerState & player : m_state.players )
        {
            if( player.active == false )
            {
                continue;
            }
            for( uint32_t & cooldown : player.cooldown )
            {
                if( cooldown > 0 )
                {
                    --cooldown;
                }
            }

            if( player.fireHeld == false )
            {
                player.nailHeat = kf_fixed_max( 0, kf_fixed_sub( player.nailHeat, kf_fixed_from_ratio( 2, 100 ) ) );
                player.railChargeTicks = 0;
                continue;
            }
            if( player.alive == false )
            {
                continue;
            }

            const WeaponType weapon = player.selectedWeapon;
            const size_t index = Detail::weaponIndex( weapon );
            if( player.ammo[index] == 0 || player.cooldown[index] != 0 )
            {
                continue;
            }

            if( weapon == WeaponType::Railgun )
            {
                if( player.fireConsumed == true )
                {
                    continue;
                }
                ++player.railChargeTicks;
                if( player.railChargeTicks == m_config.weapons[Detail::weaponIndex( WeaponType::Railgun )].chargeTicks )
                {
                    this->firePlayerWeapon_( &player, weapon );
                    player.fireConsumed = true;
                    player.railChargeTicks = 0;
                }
                continue;
            }

            if( Detail::isAutomatic( weapon ) == false && player.fireConsumed == true )
            {
                continue;
            }
            this->firePlayerWeapon_( &player, weapon );
            if( Detail::isAutomatic( weapon ) == false )
            {
                player.fireConsumed = true;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    kf_vec3_t Simulation::viewDirection_( const PlayerState & player ) const
    {
        kf_fixed_t sinYaw;
        kf_fixed_t cosYaw;
        kf_fixed_t sinPitch;
        kf_fixed_t cosPitch;
        kf_angle16_sin_cos( player.yaw, &sinYaw, &cosYaw );
        kf_sangle16_sin_cos( player.pitch, &sinPitch, &cosPitch );
        return kf_vec3_normalize( {kf_fixed_mul( sinYaw, cosPitch ), sinPitch, kf_fixed_mul( cosYaw, cosPitch )} );
    }
    //////////////////////////////////////////////////////////////////////////
    kf_vec3_t Simulation::spreadDirection_( const kf_vec3_t & direction, kf_fixed_t spread, uint64_t shotId ) const
    {
        if( spread == 0 )
        {
            return direction;
        }
        Pcg32 random( splitMix64( m_state.seed ^ shotId ), shotId | 1u );
        const int32_t x = static_cast<int32_t>(random.bounded( 2001 )) - 1000;
        const int32_t y = static_cast<int32_t>(random.bounded( 2001 )) - 1000;
        const kf_fixed_t xScale = kf_fixed_mul_div( spread, kf_fixed_from_int( x ), kf_fixed_from_int( 1000 ) );
        const kf_fixed_t yScale = kf_fixed_mul_div( spread, kf_fixed_from_int( y ), kf_fixed_from_int( 1000 ) );
        kf_vec3_t right{direction.z, 0, kf_fixed_neg( direction.x )};
        if( kf_vec3_length_squared( right ) <= 1 )
        {
            right = {KF_FIXED_SCALE, 0, 0};
        }
        else
        {
            right = kf_vec3_normalize( right );
        }
        const kf_vec3_t up = kf_vec3_normalize( kf_vec3_cross( direction, right ) );
        return kf_vec3_normalize( kf_vec3_add_mul2( direction, right, xScale, up, yScale ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::firePlayerWeapon_( PlayerState * playerPtr, WeaponType weapon )
    {
        PlayerState & player = *playerPtr;
        const size_t index = Detail::weaponIndex( weapon );
        const WeaponConfig & config = m_config.weapons[index];
        const uint64_t shotId = ++player.shotSequence;
        kf_vec3_t direction = this->viewDirection_( player );
        const kf_vec3_t origin = kf_vec3_add( player.position, {0, player.crouched ? KF_FIXED_SCALE : kf_fixed_from_ratio( 16, 10 ), 0} );

        --player.ammo[index];
        player.cooldown[index] = config.cooldownTicks;
        this->emit_( EventType::Shot, player.id, 0, weapon, origin, 0 );

        if( weapon == WeaponType::Railgun )
        {
            this->fireRail_( player.id, origin, direction, config.damage );
            return;
        }

        uint32_t count = config.pelletCount;
        kf_fixed_t spread = 0;
        if( weapon == WeaponType::Nailgun )
        {
            spread = kf_fixed_mul( player.nailHeat, kf_fixed_from_ratio( 8, 100 ) );
            if( player.grounded == false )
            {
                spread = kf_fixed_add( spread, kf_fixed_from_ratio( 2, 100 ) );
            }
            const kf_fixed_t increment = player.crouched ? kf_fixed_from_ratio( 4, 100 ) : kf_fixed_from_ratio( 8, 100 );
            player.nailHeat = kf_fixed_min( KF_FIXED_SCALE, kf_fixed_add( player.nailHeat, increment ) );
        }
        else if( weapon == WeaponType::Shotgun )
        {
            spread = kf_fixed_from_ratio( 12, 100 );
        }

        for( uint32_t projectile = 0; projectile != count; ++projectile )
        {
            const uint64_t stableShotId = (shotId << 8) | projectile;
            const kf_vec3_t shotDirection = this->spreadDirection_( direction, spread, stableShotId );
            const uint32_t launchDelayTicks = weapon == WeaponType::Shotgun ? Detail::shotgunLaunchDelayTicks( shotId, projectile ) : 0;
            this->spawnProjectile_( player.id, weapon, origin, shotDirection, stableShotId, launchDelayTicks );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Simulation::spawnProjectile_( uint32_t ownerId, WeaponType weapon, const kf_vec3_t & origin, const kf_vec3_t & direction, uint64_t shotId, uint32_t launchDelayTicks )
    {
        for( ProjectileState & projectile : m_state.projectiles )
        {
            if( projectile.active == true )
            {
                continue;
            }
            const WeaponConfig & config = m_config.weapons[Detail::weaponIndex( weapon )];
            projectile = ProjectileState{};
            projectile.active = true;
            projectile.id = m_state.nextObjectId++;
            projectile.ownerId = ownerId;
            projectile.weapon = weapon;
            projectile.position = origin;
            projectile.velocity = kf_vec3_mul( direction, config.speed );
            projectile.velocity.y = kf_fixed_add( projectile.velocity.y, config.launchUpwardSpeed );
            projectile.radius = Detail::projectileRadius( weapon );
            projectile.gravity = config.gravity;
            projectile.lifetimeTicks = config.lifetimeTicks;
            projectile.launchDelayTicks = launchDelayTicks;
            projectile.damage = config.damage;
            if( this->findPlayer( ownerId ) == nullptr )
            {
                projectile.damage = Mengine::StdAlgorithm::max( 1, projectile.damage / 4 );
            }
            projectile.shotId = shotId;
            return true;
        }

        assert( false && "Arena3D projectile pool exhausted" );
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::fireRail_( uint32_t ownerId, const kf_vec3_t & origin, const kf_vec3_t & direction, int32_t damage )
    {
        const kf_fixed_t maximumDistance = kf_fixed_from_int( 80 );
        const kf_ray_t ray{origin, direction, maximumDistance};
        kf_hit_t worldHit{};
        const bool hitWorld = kf_world_raycast( Arena3DGenerated::CollisionBoxes,
            Arena3DGenerated::CollisionBoxCount, &ray, &worldHit ) == KF_TRUE;
        const kf_fixed_t worldDistance = hitWorld == true ? worldHit.distance : maximumDistance;

        struct RailTarget
        {
            kf_fixed_t distance;
            uint32_t id;
            kf_vec3_t position;
        };
        typedef Mengine::Array<RailTarget, MaximumPlayers + MaximumTurrets> ArrayRailTarget;

        ArrayRailTarget targets{};
        size_t count = 0;
        for( const PlayerState & player : m_state.players )
        {
            if( player.active == false || player.alive == false || player.id == ownerId )
            {
                continue;
            }
            const kf_fixed_t height = player.crouched ? m_config.crouchedHeight : m_config.standingHeight;
            const kf_capsule_t target{player.position, m_config.playerRadius, height};
            kf_hit_t actorHit{};
            if( kf_raycast_capsule( &ray, &target, player.id, &actorHit ) == KF_FALSE || actorHit.distance >= worldDistance )
            {
                continue;
            }
            targets[count++] = {actorHit.distance, player.id, actorHit.position};
        }

        if( this->findPlayer( ownerId ) != nullptr )
        {
            for( const TurretStateData & turret : m_state.turrets )
            {
                if( turret.state == TurretState::Destroyed || turret.state == TurretState::Respawning )
                {
                    continue;
                }
                const kf_capsule_t target{turret.position, kf_fixed_from_ratio( 65, 100 ), kf_fixed_from_ratio( 16, 10 )};
                kf_hit_t actorHit{};
                if( kf_raycast_capsule( &ray, &target, turret.id, &actorHit ) == KF_FALSE || actorHit.distance >= worldDistance )
                {
                    continue;
                }
                targets[count++] = {actorHit.distance, turret.id, actorHit.position};
            }
        }

        Mengine::StdAlgorithm::sort( targets.begin(), targets.begin() + static_cast<ptrdiff_t>(count), []( const RailTarget & left, const RailTarget & right )
        {
            return left.distance == right.distance ? left.id < right.id : left.distance < right.distance;
        } );
        const kf_fixed_t knockback = m_config.weapons[Detail::weaponIndex( WeaponType::Railgun )].knockback;
        for( size_t index = 0; index != count; ++index )
        {
            this->queueDamage_( targets[index].id, ownerId, WeaponType::Railgun, origin, damage, knockback );
            this->emit_( EventType::Hit, ownerId, targets[index].id, WeaponType::Railgun, targets[index].position, damage );
        }

        if( hitWorld == true )
        {
            this->emit_( EventType::Impact, ownerId, worldHit.object_id, WeaponType::Railgun,
                worldHit.position, static_cast<int32_t>(Detail::impactNormal( worldHit.normal )) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::updateTurrets_()
    {
        if( m_config.turretCombat == false )
        {
            return;
        }

        for( TurretStateData & turret : m_state.turrets )
        {
            if( turret.state == TurretState::Destroyed || turret.state == TurretState::Respawning )
            {
                continue;
            }
            if( turret.cooldownTicks > 0 )
            {
                --turret.cooldownTicks;
            }

            const PlayerState * targetPlayer = nullptr;
            kf_fixed_t targetDistance = kf_fixed_from_int( 2047 );
            for( const PlayerState & candidate : m_state.players )
            {
                if( candidate.active == false || candidate.alive == false )
                {
                    continue;
                }
                const kf_fixed_t distance = kf_vec3_length( kf_vec3_sub( candidate.position, turret.position ) );
                if( targetPlayer == nullptr || distance < targetDistance || (distance == targetDistance && candidate.id < targetPlayer->id) )
                {
                    targetPlayer = &candidate;
                    targetDistance = distance;
                }
            }
            if( targetPlayer == nullptr )
            {
                turret.state = TurretState::Searching;
                continue;
            }
            const PlayerState & player = *targetPlayer;

            if( targetDistance > kf_fixed_from_int( 30 ) )
            {
                turret.state = TurretState::Searching;
                turret.chargeTicks = 0;
                continue;
            }

            if( turret.targetMemoryTicks > 0 )
            {
                --turret.targetMemoryTicks;
            }
            constexpr uint32_t losPeriod = 8;
            if( m_state.tick % losPeriod == turret.id % losPeriod )
            {
                const kf_vec3_t origin = kf_vec3_add( turret.position, {0, kf_fixed_from_ratio( 8, 10 ), 0} );
                const kf_vec3_t target = kf_vec3_add( player.position, {0, player.crouched ? kf_fixed_from_ratio( 6, 10 ) : kf_fixed_from_ratio( 9, 10 ), 0} );
                if( Detail::lineBlocked( origin, target ) == false )
                {
                    turret.targetMemoryTicks = 24;
                }
            }
            if( turret.targetMemoryTicks == 0 )
            {
                turret.state = TurretState::Searching;
                turret.chargeTicks = 0;
                continue;
            }

            turret.state = TurretState::Tracking;
            if( turret.cooldownTicks != 0 )
            {
                turret.state = TurretState::Cooldown;
                continue;
            }
            if( turret.weapon == WeaponType::Railgun )
            {
                turret.state = TurretState::Charging;
                ++turret.chargeTicks;
                if( turret.chargeTicks == 1 )
                {
                    this->emit_( EventType::TurretWarning, turret.id, player.id, turret.weapon, turret.position, 0 );
                }
                if( turret.chargeTicks < m_config.weapons[Detail::weaponIndex( WeaponType::Railgun )].chargeTicks )
                {
                    continue;
                }
                turret.chargeTicks = 0;
            }
            this->fireTurretWeapon_( &turret, player );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::fireTurretWeapon_( TurretStateData * turret, const PlayerState & targetPlayer )
    {
        const WeaponConfig & config = m_config.weapons[Detail::weaponIndex( turret->weapon )];
        const kf_vec3_t origin = kf_vec3_add( turret->position, {0, kf_fixed_from_ratio( 8, 10 ), 0} );
        kf_vec3_t target = kf_vec3_add( targetPlayer.position, {0, kf_fixed_from_ratio( 9, 10 ), 0} );
        if( config.speed > 0 )
        {
            const kf_fixed_t travelTime = kf_fixed_clamp( kf_fixed_div( kf_vec3_length( kf_vec3_sub( target, origin ) ), config.speed ),
                0, kf_fixed_from_ratio( 15, 10 ) );
            target = kf_vec3_add_mul( target, targetPlayer.velocity, travelTime );
            const kf_fixed_t halfGravity = kf_fixed_mul( config.gravity, kf_fixed_from_ratio( 5, 10 ) );
            target.y = kf_fixed_add_mul( target.y, halfGravity, kf_fixed_mul( travelTime, travelTime ) );
            target.y = kf_fixed_sub_mul( target.y, config.launchUpwardSpeed, travelTime );
        }
        kf_vec3_t direction = kf_vec3_normalize( kf_vec3_sub( target, origin ) );
        const uint64_t shotId = (m_state.tick << 16) ^ turret->id;
        const uint32_t aim = turret->aimRandom.bounded( 2001 );
        const kf_fixed_t error = kf_fixed_from_ratio( static_cast<int32_t>(aim) - 1000, 100000 );
        direction = kf_vec3_normalize( {kf_fixed_add( direction.x, error ), direction.y, kf_fixed_sub( direction.z, error )} );
        turret->state = TurretState::Firing;
        turret->cooldownTicks = config.cooldownTicks + 20;
        this->emit_( EventType::Shot, turret->id, targetPlayer.id, turret->weapon, origin, 0 );

        if( turret->weapon == WeaponType::Railgun )
        {
            this->fireRail_( turret->id, origin, direction, Mengine::StdAlgorithm::max( 1, config.damage / 4 ) );
            return;
        }
        const uint32_t count = config.pelletCount;
        for( uint32_t projectile = 0; projectile != count; ++projectile )
        {
            const uint64_t stableShotId = (shotId << 8) | projectile;
            kf_vec3_t projectileDirection = turret->weapon == WeaponType::Shotgun
                ? this->spreadDirection_( direction, kf_fixed_from_ratio( 14, 100 ), stableShotId )
                : direction;
            const uint32_t launchDelayTicks = turret->weapon == WeaponType::Shotgun ? Detail::shotgunLaunchDelayTicks( shotId, projectile ) : 0;
            this->spawnProjectile_( turret->id, turret->weapon, origin, projectileDirection, stableShotId, launchDelayTicks );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::updateProjectiles_()
    {
        const kf_fixed_t tickSeconds = kf_fixed_from_ratio( 1, 100 );
        for( ProjectileState & projectile : m_state.projectiles )
        {
            if( projectile.active == false )
            {
                continue;
            }
            if( projectile.launchDelayTicks != 0 )
            {
                --projectile.launchDelayTicks;
                continue;
            }
            if( projectile.lifetimeTicks == 0 )
            {
                if( Detail::isExplosive( projectile.weapon ) == true )
                {
                    this->queueExplosion_( projectile );
                }
                projectile.active = false;
                continue;
            }
            --projectile.lifetimeTicks;
            projectile.velocity.y = kf_fixed_sub_mul( projectile.velocity.y, projectile.gravity, tickSeconds );
            const kf_vec3_t previous = projectile.position;
            const kf_vec3_t displacement = kf_vec3_mul( projectile.velocity, tickSeconds );
            const kf_vec3_t desiredPosition = kf_vec3_add_mul( previous, projectile.velocity, tickSeconds );

            const kf_sphere_t movingSphere{previous, projectile.radius};
            kf_hit_t worldHit{};
            const bool hitWorld = kf_world_sweep_sphere_hit( Arena3DGenerated::CollisionBoxes,
                Arena3DGenerated::CollisionBoxCount, &movingSphere, displacement, &worldHit ) == KF_TRUE;

            kf_hit_t actorHit{};
            bool hitActor = false;
            uint32_t hitActorId = 0;
            for( const PlayerState & player : m_state.players )
            {
                if( player.active == false || player.alive == false || player.id == projectile.ownerId )
                {
                    continue;
                }
                const kf_fixed_t playerHeight = player.crouched ? m_config.crouchedHeight : m_config.standingHeight;
                const kf_capsule_t target{player.position, m_config.playerRadius, playerHeight};
                kf_hit_t candidate{};
                if( kf_sweep_sphere_capsule( &movingSphere, displacement, &target, player.id, &candidate ) == KF_FALSE )
                {
                    continue;
                }
                if( hitActor == false || candidate.fraction < actorHit.fraction ||
                    (candidate.fraction == actorHit.fraction && candidate.object_id < actorHit.object_id) )
                {
                    actorHit = candidate;
                    hitActor = true;
                    hitActorId = player.id;
                }
            }
            if( this->findPlayer( projectile.ownerId ) != nullptr )
            {
                for( const TurretStateData & turret : m_state.turrets )
                {
                    if( turret.state == TurretState::Destroyed || turret.state == TurretState::Respawning )
                    {
                        continue;
                    }
                    const kf_capsule_t target{turret.position, kf_fixed_from_ratio( 65, 100 ), kf_fixed_from_ratio( 16, 10 )};
                    kf_hit_t candidate{};
                    if( kf_sweep_sphere_capsule( &movingSphere, displacement, &target, turret.id, &candidate ) == KF_FALSE )
                    {
                        continue;
                    }
                    if( hitActor == false || candidate.fraction < actorHit.fraction ||
                        (candidate.fraction == actorHit.fraction && candidate.object_id < actorHit.object_id) )
                    {
                        actorHit = candidate;
                        hitActor = true;
                        hitActorId = turret.id;
                    }
                }
            }

            const bool actorIsFirst = hitActor == true && (hitWorld == false || actorHit.fraction < worldHit.fraction);
            if( actorIsFirst == true )
            {
                projectile.position = kf_vec3_add_mul( previous, displacement, actorHit.fraction );
            }
            else if( hitWorld == true )
            {
                projectile.position = kf_vec3_add_mul( previous, displacement, worldHit.fraction );
            }
            else
            {
                projectile.position = desiredPosition;
            }
            projectile.traveledDistance = kf_fixed_add( projectile.traveledDistance, kf_vec3_length( kf_vec3_sub( projectile.position, previous ) ) );
            if( projectile.weapon == WeaponType::Plasmagun )
            {
                const int32_t distanceSteps = kf_fixed_to_int( projectile.traveledDistance ) / 5;
                projectile.damage = Mengine::StdAlgorithm::max( 5, m_config.weapons[Detail::weaponIndex( WeaponType::Plasmagun )].damage - distanceSteps * 3 );
                if( this->findPlayer( projectile.ownerId ) == nullptr )
                {
                    projectile.damage = Mengine::StdAlgorithm::max( 1, projectile.damage / 4 );
                }
            }

            if( actorIsFirst == true )
            {
                this->queueDamage_( hitActorId, projectile.ownerId, projectile.weapon, projectile.position, projectile.damage,
                    m_config.weapons[Detail::weaponIndex( projectile.weapon )].knockback );
                if( Detail::isExplosive( projectile.weapon ) == true )
                {
                    this->queueExplosion_( projectile, hitActorId );
                }
                this->emit_( EventType::Hit, projectile.ownerId, hitActorId, projectile.weapon, projectile.position, projectile.damage );
                projectile.active = false;
                continue;
            }

            if( hitWorld == true && projectile.weapon == WeaponType::GrenadeLauncher )
            {
                const kf_vec3_t normal = worldHit.normal;
                const kf_fixed_t normalVelocity = kf_vec3_dot( projectile.velocity, normal );
                if( normalVelocity < kf_fixed_from_ratio( -1, 10 ) )
                {
                    const kf_vec3_t contact = kf_vec3_sub_mul( projectile.position, normal, projectile.radius );
                    this->emit_( EventType::Impact, projectile.ownerId, worldHit.object_id, projectile.weapon,
                        contact, static_cast<int32_t>(Detail::impactNormal( worldHit.normal )) );
                }
                if( normalVelocity < 0 )
                {
                    projectile.velocity = kf_vec3_sub_mul( projectile.velocity, normal, kf_fixed_mul( normalVelocity, kf_fixed_from_ratio( 155, 100 ) ) );
                }
                else
                {
                    projectile.velocity = kf_vec3_mul( projectile.velocity, kf_fixed_from_ratio( -55, 100 ) );
                }
                projectile.velocity.x = kf_fixed_mul( projectile.velocity.x, kf_fixed_from_ratio( 8, 10 ) );
                projectile.velocity.z = kf_fixed_mul( projectile.velocity.z, kf_fixed_from_ratio( 8, 10 ) );
                continue;
            }
            if( hitWorld == true )
            {
                const kf_vec3_t normal = worldHit.normal;
                const kf_vec3_t contact = kf_vec3_sub_mul( projectile.position, normal, projectile.radius );
                this->emit_( EventType::Impact, projectile.ownerId, worldHit.object_id, projectile.weapon,
                    contact, static_cast<int32_t>(Detail::impactNormal( worldHit.normal )) );
                if( Detail::isExplosive( projectile.weapon ) == true )
                {
                    this->queueExplosion_( projectile );
                }
                projectile.active = false;
                continue;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::queueExplosion_( const ProjectileState & projectile, uint32_t ignoredTargetId )
    {
        const WeaponConfig & config = m_config.weapons[Detail::weaponIndex( projectile.weapon )];
        this->emit_( EventType::Explosion, projectile.ownerId, 0, projectile.weapon, projectile.position, projectile.damage );
        if( config.splashRadius <= 0 )
        {
            return;
        }

        for( const PlayerState & player : m_state.players )
        {
            if( player.active == false || player.alive == false || player.id == ignoredTargetId )
            {
                continue;
            }
            const kf_fixed_t playerHeight = player.crouched == true ? m_config.crouchedHeight : m_config.standingHeight;
            const kf_vec3_t playerTarget = kf_vec3_add( player.position, {0, kf_fixed_mul( playerHeight, kf_fixed_from_ratio( 5, 10 ) ), 0} );
            const kf_fixed_t playerDistance = kf_vec3_length( kf_vec3_sub( playerTarget, projectile.position ) );
            if( playerDistance > config.splashRadius || Detail::lineBlocked( projectile.position, playerTarget ) == true )
            {
                continue;
            }
            const kf_fixed_t falloff = kf_fixed_sub( KF_FIXED_SCALE, kf_fixed_div( playerDistance, config.splashRadius ) );
            const int32_t damage = kf_fixed_to_int( kf_fixed_mul( kf_fixed_from_int( projectile.damage ), falloff ) );
            this->queueDamage_( player.id, projectile.ownerId, projectile.weapon, projectile.position, damage, kf_fixed_mul( config.knockback, falloff ) );
        }
        for( const TurretStateData & turret : m_state.turrets )
        {
            if( turret.state == TurretState::Destroyed || turret.state == TurretState::Respawning )
            {
                continue;
            }
            if( turret.id == ignoredTargetId )
            {
                continue;
            }
            const kf_fixed_t distance = kf_vec3_length( kf_vec3_sub( turret.position, projectile.position ) );
            if( distance <= config.splashRadius && Detail::lineBlocked( projectile.position, turret.position ) == false )
            {
                const kf_fixed_t falloff = kf_fixed_sub( KF_FIXED_SCALE, kf_fixed_div( distance, config.splashRadius ) );
                const int32_t damage = kf_fixed_to_int( kf_fixed_mul( kf_fixed_from_int( projectile.damage ), falloff ) );
                this->queueDamage_( turret.id, projectile.ownerId, projectile.weapon, projectile.position, damage, kf_fixed_mul( config.knockback, falloff ) );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::queueDamage_( uint32_t targetId, uint32_t sourceId, WeaponType weapon, const kf_vec3_t & origin, int32_t damage, kf_fixed_t knockback )
    {
        if( m_damageCount == m_damage.size() )
        {
            assert( false && "Arena3D damage queue exhausted" );
            return;
        }
        DamageRequest & request = m_damage[m_damageCount++];
        request.targetId = targetId;
        request.sourceId = sourceId;
        request.order = (m_state.tick << 32) | m_damageCount;
        request.weapon = weapon;
        request.origin = origin;
        request.damage = Mengine::StdAlgorithm::max( 0, damage );
        request.knockback = knockback;
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::applyDamage_()
    {
        Mengine::StdAlgorithm::sort( m_damage.begin(), m_damage.begin() + static_cast<ptrdiff_t>(m_damageCount),
                                     []( const DamageRequest & left, const DamageRequest & right )
                                     {
                                         if( left.targetId != right.targetId )
                                         {
                                             return left.targetId < right.targetId;
                                         }
                                         if( left.sourceId != right.sourceId )
                                         {
                                             return left.sourceId < right.sourceId;
                                         }
                                         return left.order < right.order;
                                     } );

        for( size_t index = 0; index != m_damageCount; ++index )
        {
            const DamageRequest & damage = m_damage[index];
            PlayerState * playerPtr = this->findPlayer( damage.targetId );
            if( playerPtr != nullptr )
            {
                PlayerState & player = *playerPtr;
                if( player.alive == false )
                {
                    continue;
                }
                const bool selfHit = damage.sourceId == player.id;
                const int32_t appliedDamage = selfHit == true ? 0 : damage.damage;
                const int32_t absorbed = Mengine::StdAlgorithm::min( player.armor, appliedDamage / 2 );
                player.armor -= absorbed;
                player.health -= appliedDamage - absorbed;

                kf_fixed_t knockback = damage.knockback;
                if( player.grounded == true )
                {
                    if( Detail::isExplosive( damage.weapon ) == true )
                    {
                        knockback = kf_fixed_mul( knockback, kf_fixed_from_ratio( 5, 10 ) );
                    }
                    else
                    {
                        knockback = 0;
                    }
                }
                if( knockback > 0 )
                {
                    const kf_fixed_t playerHeight = player.crouched == true ? m_config.crouchedHeight : m_config.standingHeight;
                    kf_vec3_t impulseTarget = kf_vec3_add( player.position, {0, kf_fixed_mul( playerHeight, kf_fixed_from_ratio( 5, 10 ) ), 0} );
                    if( Detail::isExplosive( damage.weapon ) == true )
                    {
                        impulseTarget.y = kf_fixed_add( impulseTarget.y, m_config.playerRadius );
                    }
                    const kf_vec3_t direction = kf_vec3_normalize( kf_vec3_sub( impulseTarget, damage.origin ) );
                    player.velocity = kf_vec3_add_mul( player.velocity, direction, knockback );
                    if( Detail::isExplosive( damage.weapon ) == true && direction.y > 0 )
                    {
                        player.grounded = false;
                    }
                }
                if( appliedDamage > 0 )
                {
                    this->emit_( EventType::Damage, damage.sourceId, player.id, damage.weapon, damage.origin, appliedDamage );
                }
                if( player.health <= 0 )
                {
                    player.health = 0;
                    player.alive = false;
                    player.fireHeld = false;
                    player.railChargeTicks = 0;
                    player.respawnTicks = 300;
                    ++player.deaths;
                    PlayerState * killer = this->findPlayer( damage.sourceId );
                    if( killer != nullptr && killer->id != player.id )
                    {
                        ++killer->score;
                    }
                    this->emit_( EventType::PlayerDeath, damage.sourceId, player.id, damage.weapon, player.position, 0 );
                }
                continue;
            }

            for( TurretStateData & turret : m_state.turrets )
            {
                if( turret.id != damage.targetId || turret.state == TurretState::Destroyed || turret.state == TurretState::Respawning )
                {
                    continue;
                }
                turret.health -= damage.damage;
                this->emit_( EventType::Damage, damage.sourceId, turret.id, damage.weapon, damage.origin, damage.damage );
                if( turret.health <= 0 )
                {
                    turret.health = 0;
                    turret.state = TurretState::Destroyed;
                    turret.respawnTicks = 500;
                    PlayerState * scorer = this->findPlayer( damage.sourceId );
                    if( scorer != nullptr )
                    {
                        ++scorer->score;
                    }
                    this->emit_( EventType::TurretDestroyed, damage.sourceId, turret.id, damage.weapon, turret.position, 0 );
                }
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
