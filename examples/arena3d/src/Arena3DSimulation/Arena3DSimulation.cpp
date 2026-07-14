#include "Arena3DSimulation.h"

#include "Arena3DGeneratedCollision.h"

#include <algorithm>
#include <cstring>
#include <utility>

namespace Arena3D
{
    namespace
    {
        constexpr uint8_t inputBit( InputButton button )
        {
            return static_cast<uint8_t>(1u << static_cast<uint8_t>(button));
        }

        constexpr size_t weaponIndex( WeaponType weapon )
        {
            return static_cast<size_t>(weapon);
        }

        Fixed value( const char * text )
        {
            return Fixed::fromDecimal( text );
        }

        bool isAutomatic( WeaponType weapon )
        {
            return weapon == WeaponType::Nailgun || weapon == WeaponType::Plasmagun;
        }

        bool isExplosive( WeaponType weapon )
        {
            return weapon == WeaponType::RocketLauncher || weapon == WeaponType::Plasmagun || weapon == WeaponType::GrenadeLauncher;
        }

        Fixed projectileRadius( WeaponType weapon )
        {
            switch( weapon )
            {
            case WeaponType::RocketLauncher: return value( "0.18" );
            case WeaponType::Plasmagun: return value( "0.12" );
            case WeaponType::GrenadeLauncher: return value( "0.16" );
            case WeaponType::Shotgun: return value( "0.04" );
            default: return value( "0.06" );
            }
        }

        void writePcg( CanonicalWriter * writer, const Pcg32 & random )
        {
            writer->writeU64( random.state() );
            writer->writeU64( random.stream() );
        }

        bool readPcg( CanonicalReader * reader, Pcg32 * random )
        {
            uint64_t state = 0;
            uint64_t stream = 0;
            if( reader->readU64( &state ) == false || reader->readU64( &stream ) == false ) return false;
            random->restore( state, stream );
            return true;
        }

        Fixed rawFixed( int64_t raw )
        {
            return Fixed::fromRaw( raw );
        }

        bool collidesPlayer( const FixedVec3 & position, Fixed radius, Fixed height )
        {
            const kf_capsule_t capsule{toKinefix( position ), radius.raw(), height.raw()};
            return kf_world_overlap_capsule( Arena3DGenerated::CollisionBoxes,
                Arena3DGenerated::CollisionBoxCount, &capsule, nullptr ) != 0;
        }

        bool lineBlocked( const FixedVec3 & start, const FixedVec3 & end )
        {
            const FixedVec3 delta = end - start;
            const Fixed length = fixedLength( delta );
            if( length == Fixed::zero() ) return false;
            const kf_ray_t ray{toKinefix( start ), toKinefix( delta ), length.raw()};
            kf_hit_t hit{};
            if( kf_world_raycast( Arena3DGenerated::CollisionBoxes,
                Arena3DGenerated::CollisionBoxCount, &ray, &hit ) == 0 ) return false;
            return hit.fraction > value( "0.01" ).raw() && hit.fraction < value( "0.99" ).raw();
        }
    }

    SimulationConfig SimulationConfig::defaults()
    {
        SimulationConfig config;
        config.groundAcceleration = rawFixed( Arena3DGenerated::GroundAcceleration );
        config.airAcceleration = rawFixed( Arena3DGenerated::AirAcceleration );
        config.friction = rawFixed( Arena3DGenerated::Friction );
        config.gravity = rawFixed( Arena3DGenerated::Gravity );
        config.jumpImpulse = rawFixed( Arena3DGenerated::JumpImpulse );
        config.standingHeight = rawFixed( Arena3DGenerated::StandingHeight );
        config.crouchedHeight = rawFixed( Arena3DGenerated::CrouchedHeight );
        config.playerRadius = rawFixed( Arena3DGenerated::PlayerRadius );
        config.stepHeight = rawFixed( Arena3DGenerated::StepHeight );
        config.maximumSlopeCosine = rawFixed( Arena3DGenerated::MaximumSlopeCosine );
        config.crouchedSpeedScale = rawFixed( Arena3DGenerated::CrouchedSpeedScale );
        config.slideMinimumSpeed = rawFixed( Arena3DGenerated::SlideMinimumSpeed );
        config.slideFriction = rawFixed( Arena3DGenerated::SlideFriction );
        config.slideAccelerationScale = rawFixed( Arena3DGenerated::SlideAccelerationScale );
        config.slideMaximumSpeed = rawFixed( Arena3DGenerated::SlideMaximumSpeed );
        config.arenaHalfExtent = rawFixed( Arena3DGenerated::ArenaHalfExtent );
        config.maximumPitch = fixedSignedAngleFromRadians( value( "1.5533430342749532" ) );

        static_assert( static_cast<size_t>(WeaponType::Count) == sizeof(Arena3DGenerated::WeaponConfigs) / sizeof(Arena3DGenerated::WeaponConfigs[0]) );
        for( size_t index = 0; index != static_cast<size_t>(WeaponType::Count); ++index )
        {
            const Arena3DGenerated::WeaponConfigRaw & source = Arena3DGenerated::WeaponConfigs[index];
            WeaponConfig & weapon = config.weapons[index];
            weapon.damage = source.damage;
            weapon.cooldownTicks = source.cooldownTicks;
            weapon.speed = rawFixed( source.speed );
            weapon.gravity = rawFixed( source.gravity );
            weapon.lifetimeTicks = source.lifetimeTicks;
            weapon.chargeTicks = source.chargeTicks;
            weapon.pelletCount = source.pelletCount;
            weapon.splashRadius = rawFixed( source.splashRadius );
            weapon.knockback = rawFixed( source.knockback );
        }
        return config;
    }

    bool StateChecksum::operator==( const StateChecksum & other ) const
    {
        return player == other.player && projectiles == other.projectiles && turrets == other.turrets && pickups == other.pickups && random == other.random && full == other.full;
    }
    bool StateChecksum::operator!=( const StateChecksum & other ) const { return (*this == other) == false; }

    Simulation::Simulation( SimulationConfig config )
        : m_config( std::move( config ) )
    {
        m_events.reserve( MaximumEventsPerTick );
        this->reset( 1, UINT64_C(0xA3E4D0197C2B6501) );
    }

    void Simulation::reset( uint64_t matchId, uint64_t seed )
    {
        m_state = SimulationState{};
        m_state.matchId = matchId;
        m_state.seed = seed;
        m_state.nextObjectId = 1000;
        const Arena3DGenerated::SpawnRaw & spawn = Arena3DGenerated::SpawnPoints[0];
        m_state.player.position = {rawFixed( spawn.position[0] ), rawFixed( spawn.position[1] ), rawFixed( spawn.position[2] )};
        m_state.player.yaw = spawn.yaw;
        for( size_t index = 0; index != static_cast<size_t>(WeaponType::Count); ++index )
            m_state.player.ammo[index] = Arena3DGenerated::WeaponConfigs[index].ammo;
        m_state.weaponRandom.seed( splitMix64( seed ^ UINT64_C(0x574541504F4E) ), UINT64_C(0xA3) );
        m_state.pickupRandom.seed( splitMix64( seed ^ UINT64_C(0x5049434B5550) ), UINT64_C(0xB7) );
        this->initializeTurrets_();
        this->initializePickups_();
        m_events.clear();
        m_damageCount = 0;
        m_eventOrdinal = 0;
    }

    void Simulation::initializeTurrets_()
    {
        static_assert( MaximumTurrets == sizeof(Arena3DGenerated::Turrets) / sizeof(Arena3DGenerated::Turrets[0]) );

        for( size_t index = 0; index != MaximumTurrets; ++index )
        {
            const Arena3DGenerated::TurretRaw & source = Arena3DGenerated::Turrets[index];
            TurretStateData & turret = m_state.turrets[index];
            turret.id = source.id;
            turret.weapon = static_cast<WeaponType>(source.weapon);
            turret.position = {rawFixed( source.position[0] ), rawFixed( source.position[1] ), rawFixed( source.position[2] )};
            turret.yaw = source.yaw;
            turret.state = TurretState::Searching;
            turret.cooldownTicks = 200u + static_cast<uint32_t>(index) * 17u;
            turret.health = 100;
            turret.aimRandom.seed( splitMix64( m_state.seed ^ turret.id ), turret.id );
        }
    }

    void Simulation::initializePickups_()
    {
        static_assert( MaximumPickups == sizeof(Arena3DGenerated::Pickups) / sizeof(Arena3DGenerated::Pickups[0]) );
        for( size_t index = 0; index != MaximumPickups; ++index )
        {
            const Arena3DGenerated::PickupRaw & source = Arena3DGenerated::Pickups[index];
            PickupStateData & pickup = m_state.pickups[index];
            pickup.id = source.id;
            pickup.type = static_cast<PickupType>(source.type);
            pickup.weapon = static_cast<WeaponType>(source.weapon);
            pickup.position = {rawFixed( source.position[0] ), rawFixed( source.position[1] ), rawFixed( source.position[2] )};
            pickup.amount = source.amount;
        }
    }

    bool Simulation::step( const std::vector<CommandEnvelope> & commands )
    {
        m_events.clear();
        m_damageCount = 0;
        m_eventOrdinal = 0;
        m_state.fault.clear();
        FixedFaultScope faultScope( &m_state.fault );

        this->applyCommands_( commands );
        if( m_state.fault.failed() == true ) return false;
        if( m_state.paused == true )
        {
            this->finalizeEvents_();
            return true;
        }

        this->updateWeapons_();
        this->updateTurrets_();
        this->updatePlayer_();
        this->updateProjectiles_();
        this->applyDamage_();
        this->updatePickups_();
        this->updateRespawns_();
        this->finalizeEvents_();
        ++m_state.tick;
        return m_state.fault.failed() == false;
    }

    void Simulation::applyCommands_( const std::vector<CommandEnvelope> & commands )
    {
        std::vector<const CommandEnvelope *> ordered;
        ordered.reserve( commands.size() );
        for( const CommandEnvelope & command : commands ) ordered.emplace_back( &command );
        std::stable_sort( ordered.begin(), ordered.end(), []( const CommandEnvelope * left, const CommandEnvelope * right )
        {
            if( left->playerId != right->playerId ) return left->playerId < right->playerId;
            return left->sequence < right->sequence;
        } );

        for( const CommandEnvelope * commandPtr : ordered )
        {
            const CommandEnvelope & command = *commandPtr;
            if( command.accepted == false || command.matchId != m_state.matchId || command.playerId != m_state.player.id )
            {
                this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                continue;
            }

            PlayerState & player = m_state.player;
            if( m_state.paused == true && command.type != CommandType::Resume && command.type != CommandType::Restart )
            {
                this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                continue;
            }

            switch( command.type )
            {
            case CommandType::ButtonDown:
            {
                const uint8_t buttonValue = static_cast<uint8_t>(command.payload.button);
                if( buttonValue >= 6u || (player.inputMask & inputBit( command.payload.button )) != 0 )
                {
                    this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                    break;
                }
                player.inputMask |= inputBit( command.payload.button );
                break;
            }
            case CommandType::ButtonUp:
            {
                const uint8_t buttonValue = static_cast<uint8_t>(command.payload.button);
                if( buttonValue >= 6u || (player.inputMask & inputBit( command.payload.button )) == 0 )
                {
                    this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                    break;
                }
                player.inputMask &= static_cast<uint8_t>(~inputBit( command.payload.button ));
                break;
            }
            case CommandType::LookDelta:
                player.yaw = fixedAngleAdd( player.yaw, command.payload.lookYaw );
                player.pitch = fixedSignedAngleAddClamped( player.pitch, command.payload.lookPitch,
                    static_cast<FixedSignedAngle>(-m_config.maximumPitch), m_config.maximumPitch );
                break;
            case CommandType::FireDown:
                if( player.fireHeld == true )
                {
                    this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                    break;
                }
                player.fireHeld = true;
                player.fireConsumed = false;
                break;
            case CommandType::FireUp:
                if( player.fireHeld == false )
                {
                    this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                    break;
                }
                player.fireHeld = false;
                player.fireConsumed = false;
                player.railChargeTicks = 0;
                break;
            case CommandType::SelectWeapon:
                if( command.payload.weapon < WeaponType::Count && player.ammo[weaponIndex( command.payload.weapon )] > 0 )
                {
                    player.selectedWeapon = command.payload.weapon;
                    player.railChargeTicks = 0;
                    player.fireConsumed = false;
                }
                else
                {
                    this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                }
                break;
            case CommandType::Pause: m_state.paused = true; break;
            case CommandType::Resume:
                if( m_state.paused == false )
                {
                    this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                    break;
                }
                m_state.paused = false;
                break;
            case CommandType::Restart:
            {
                const uint64_t nextMatch = m_state.matchId + 1;
                const uint64_t nextSeed = splitMix64( m_state.seed ^ nextMatch );
                this->reset( nextMatch, nextSeed );
                break;
            }
            case CommandType::Noclip: player.noclip = command.payload.enabled; break;
            }
        }
    }

    void Simulation::updateWeapons_()
    {
        PlayerState & player = m_state.player;
        for( uint32_t & cooldown : player.cooldown ) if( cooldown > 0 ) --cooldown;

        if( player.fireHeld == false )
        {
            player.nailHeat = fixedMax( Fixed::zero(), player.nailHeat - value( "0.02" ) );
            player.railChargeTicks = 0;
            return;
        }
        if( player.alive == false ) return;

        const WeaponType weapon = player.selectedWeapon;
        const size_t index = weaponIndex( weapon );
        if( player.ammo[index] == 0 || player.cooldown[index] != 0 ) return;

        if( weapon == WeaponType::Railgun )
        {
            if( player.fireConsumed == true ) return;
            ++player.railChargeTicks;
            if( player.railChargeTicks == m_config.weapons[weaponIndex( WeaponType::Railgun )].chargeTicks )
            {
                this->firePlayerWeapon_( weapon );
                player.fireConsumed = true;
                player.railChargeTicks = 0;
            }
            return;
        }

        if( isAutomatic( weapon ) == false && player.fireConsumed == true ) return;
        this->firePlayerWeapon_( weapon );
        if( isAutomatic( weapon ) == false ) player.fireConsumed = true;
    }

    FixedVec3 Simulation::viewDirection_() const
    {
        Fixed sinYaw;
        Fixed cosYaw;
        Fixed sinPitch;
        Fixed cosPitch;
        fixedAngleSinCos( m_state.player.yaw, &sinYaw, &cosYaw );
        fixedSignedAngleSinCos( m_state.player.pitch, &sinPitch, &cosPitch );
        return fixedNormalize( {sinYaw * cosPitch, sinPitch, cosYaw * cosPitch} );
    }

    FixedVec3 Simulation::spreadDirection_( const FixedVec3 & direction, Fixed spread, uint64_t shotId ) const
    {
        if( spread == Fixed::zero() ) return direction;
        Pcg32 random( splitMix64( m_state.seed ^ shotId ), shotId | 1u );
        const int32_t x = static_cast<int32_t>(random.bounded( 2001 )) - 1000;
        const int32_t y = static_cast<int32_t>(random.bounded( 2001 )) - 1000;
        const Fixed scale = spread / Fixed::fromInt( 1000 );
        return fixedNormalize( {direction.x + Fixed::fromInt( x ) * scale, direction.y + Fixed::fromInt( y ) * scale, direction.z} );
    }

    void Simulation::firePlayerWeapon_( WeaponType weapon )
    {
        PlayerState & player = m_state.player;
        const size_t index = weaponIndex( weapon );
        const WeaponConfig & config = m_config.weapons[index];
        const uint64_t shotId = ++player.shotSequence;
        FixedVec3 direction = this->viewDirection_();
        const FixedVec3 origin = player.position + FixedVec3{Fixed::zero(), player.crouched ? value( "1.0" ) : value( "1.6" ), Fixed::zero()};

        --player.ammo[index];
        player.cooldown[index] = config.cooldownTicks;
        this->emit_( EventType::Shot, player.id, 0, weapon, origin, 0 );

        if( weapon == WeaponType::Railgun )
        {
            this->fireRail_( player.id, origin, direction, config.damage );
            return;
        }

        uint32_t count = config.pelletCount;
        Fixed spread = Fixed::zero();
        if( weapon == WeaponType::Nailgun )
        {
            spread = player.nailHeat * value( "0.08" );
            if( player.grounded == false ) spread += value( "0.02" );
            const Fixed increment = player.crouched ? value( "0.04" ) : value( "0.08" );
            player.nailHeat = fixedMin( Fixed::one(), player.nailHeat + increment );
        }
        else if( weapon == WeaponType::Shotgun )
        {
            spread = value( "0.12" );
        }

        for( uint32_t projectile = 0; projectile != count; ++projectile )
        {
            const FixedVec3 shotDirection = this->spreadDirection_( direction, spread, (shotId << 8) | projectile );
            this->spawnProjectile_( player.id, weapon, origin, shotDirection, (shotId << 8) | projectile );
        }
    }

    bool Simulation::spawnProjectile_( uint32_t ownerId, WeaponType weapon, const FixedVec3 & origin, const FixedVec3 & direction, uint64_t shotId )
    {
        for( ProjectileState & projectile : m_state.projectiles )
        {
            if( projectile.active == true ) continue;
            const WeaponConfig & config = m_config.weapons[weaponIndex( weapon )];
            projectile = ProjectileState{};
            projectile.active = true;
            projectile.id = m_state.nextObjectId++;
            projectile.ownerId = ownerId;
            projectile.weapon = weapon;
            projectile.position = origin;
            projectile.velocity = direction * config.speed;
            projectile.radius = projectileRadius( weapon );
            projectile.gravity = config.gravity;
            projectile.lifetimeTicks = config.lifetimeTicks;
            projectile.damage = config.damage;
            if( ownerId != m_state.player.id ) projectile.damage = std::max( 1, projectile.damage / 4 );
            projectile.shotId = shotId;
            return true;
        }

        m_state.fault.raise( FaultCode::Overflow, "projectile pool exhausted" );
        return false;
    }

    void Simulation::fireRail_( uint32_t ownerId, const FixedVec3 & origin, const FixedVec3 & direction, int32_t damage )
    {
        const Fixed maximumDistance = value( "80" );
        const kf_ray_t ray{toKinefix( origin ), toKinefix( direction ), maximumDistance.raw()};
        kf_hit_t worldHit{};
        const bool hitWorld = kf_world_raycast( Arena3DGenerated::CollisionBoxes,
            Arena3DGenerated::CollisionBoxCount, &ray, &worldHit ) != 0;
        const Fixed worldDistance = hitWorld == true ? Fixed::fromRaw( worldHit.distance ) : maximumDistance;

        if( ownerId == m_state.player.id )
        {
            struct RailTarget { Fixed distance; uint32_t id; };
            std::array<RailTarget, MaximumTurrets> targets{};
            size_t count = 0;
            for( const TurretStateData & turret : m_state.turrets )
            {
                if( turret.state == TurretState::Destroyed || turret.state == TurretState::Respawning ) continue;
                const kf_capsule_t target{toKinefix( turret.position ), value( "0.65" ).raw(), value( "1.6" ).raw()};
                kf_hit_t actorHit{};
                if( kf_raycast_capsule( &ray, &target, turret.id, &actorHit ) == 0 || actorHit.distance >= worldDistance.raw() ) continue;
                targets[count++] = {Fixed::fromRaw( actorHit.distance ), turret.id};
            }
            std::sort( targets.begin(), targets.begin() + static_cast<std::ptrdiff_t>(count), []( const RailTarget & left, const RailTarget & right )
            {
                return left.distance == right.distance ? left.id < right.id : left.distance < right.distance;
            } );
            for( size_t index = 0; index != count; ++index )
            {
                this->queueDamage_( targets[index].id, ownerId, WeaponType::Railgun, origin, damage, value( "1.2" ) );
            }
        }
        else if( m_state.player.alive == true )
        {
            const Fixed playerHeight = m_state.player.crouched ? m_config.crouchedHeight : m_config.standingHeight;
            const kf_capsule_t target{toKinefix( m_state.player.position ), m_config.playerRadius.raw(), playerHeight.raw()};
            kf_hit_t actorHit{};
            if( kf_raycast_capsule( &ray, &target, m_state.player.id, &actorHit ) != 0 && actorHit.distance < worldDistance.raw() )
                this->queueDamage_( m_state.player.id, ownerId, WeaponType::Railgun, origin, damage, value( "1.2" ) );
        }
    }

    void Simulation::updatePlayer_()
    {
        PlayerState & player = m_state.player;
        if( player.alive == false ) return;
        const Fixed tickSeconds = value( "0.01" );

        Fixed sinYaw;
        Fixed cosYaw;
        fixedAngleSinCos( player.yaw, &sinYaw, &cosYaw );
        FixedVec3 wish;
        if( player.inputMask & inputBit( InputButton::Forward ) ) { wish.x += sinYaw; wish.z += cosYaw; }
        if( player.inputMask & inputBit( InputButton::Backward ) ) { wish.x -= sinYaw; wish.z -= cosYaw; }
        if( player.inputMask & inputBit( InputButton::Right ) ) { wish.x += cosYaw; wish.z -= sinYaw; }
        if( player.inputMask & inputBit( InputButton::Left ) ) { wish.x -= cosYaw; wish.z += sinYaw; }
        const bool wantsCrouch = (player.inputMask & inputBit( InputButton::Crouch )) != 0;
        const Fixed speedBeforeCrouchSquared = player.velocity.x * player.velocity.x + player.velocity.z * player.velocity.z;
        const bool sliding = wantsCrouch == true && player.grounded == true &&
            speedBeforeCrouchSquared >= m_config.slideMinimumSpeed * m_config.slideMinimumSpeed;
        if( wantsCrouch == true )
        {
            player.crouched = true;
        }
        else if( player.crouched == true && collidesPlayer( player.position, m_config.playerRadius, m_config.standingHeight ) == false )
        {
            player.crouched = false;
        }

        if( player.noclip == true )
        {
            if( player.inputMask & inputBit( InputButton::Jump ) ) wish.y += Fixed::one();
            if( player.crouched == true ) wish.y -= Fixed::one();
            player.velocity = fixedNormalize( wish ) * value( "12" );
            player.position += player.velocity * tickSeconds;
            return;
        }

        const bool hasWish = fixedLengthSquared( wish ) > Fixed::zero();
        if( hasWish == true )
        {
            wish = fixedNormalize( wish );
            Fixed acceleration = player.grounded ? m_config.groundAcceleration : m_config.airAcceleration;
            if( sliding == true ) acceleration *= m_config.slideAccelerationScale;
            else if( player.crouched == true ) acceleration *= m_config.crouchedSpeedScale;
            player.velocity.x += wish.x * acceleration * tickSeconds;
            player.velocity.z += wish.z * acceleration * tickSeconds;

            const Fixed maximumSpeed = sliding == true
                ? m_config.slideMaximumSpeed
                : value( "8" ) * (player.crouched ? m_config.crouchedSpeedScale : Fixed::one());
            const Fixed horizontalSpeedSquared = player.velocity.x * player.velocity.x + player.velocity.z * player.velocity.z;
            if( horizontalSpeedSquared > maximumSpeed * maximumSpeed )
            {
                const Fixed horizontalSpeed = fixedSqrt( horizontalSpeedSquared );
                player.velocity.x = player.velocity.x * maximumSpeed / horizontalSpeed;
                player.velocity.z = player.velocity.z * maximumSpeed / horizontalSpeed;
            }
        }
        if( sliding == true )
        {
            player.velocity.x *= m_config.slideFriction;
            player.velocity.z *= m_config.slideFriction;
        }
        else if( hasWish == false && player.grounded == true )
        {
            player.velocity.x *= m_config.friction;
            player.velocity.z *= m_config.friction;
        }

        if( player.grounded == true && (player.inputMask & inputBit( InputButton::Jump )) != 0 )
        {
            player.velocity.y = m_config.jumpImpulse;
            player.grounded = false;
        }
        const Fixed height = player.crouched ? m_config.crouchedHeight : m_config.standingHeight;
        kf_character_body_t body{toKinefix( player.position ), toKinefix( player.velocity ), player.grounded ? uint8_t{1} : uint8_t{0}};
        const kf_character_config_t characterConfig{
            m_config.playerRadius.raw(), height.raw(), m_config.stepHeight.raw(), m_config.gravity.raw(),
            tickSeconds.raw(), value( "0.02" ).raw(), m_config.arenaHalfExtent.raw()};
        kf_character_result_t result{};
        kf_character_step( &body, &characterConfig, Arena3DGenerated::CollisionBoxes,
            Arena3DGenerated::CollisionBoxCount, &result );
        player.position = fromKinefix( body.position );
        player.velocity = fromKinefix( body.velocity );
        player.grounded = body.grounded != 0;
        const Fixed previousVertical = Fixed::fromRaw( result.previous_vertical_velocity );
        if( result.landed != 0 && previousVertical < value( "-12" ) )
        {
            const int32_t fallDamage = static_cast<int32_t>((fixedAbs( previousVertical ).raw() - value( "12" ).raw()) >> Fixed::FractionBits) * 5;
            if( fallDamage > 0 ) this->queueDamage_( player.id, 0, WeaponType::Nailgun, player.position, fallDamage, Fixed::zero() );
        }

        const FixedVec3 padCenter{rawFixed( Arena3DGenerated::JumpPadPosition[0] ), rawFixed( Arena3DGenerated::JumpPadPosition[1] ), rawFixed( Arena3DGenerated::JumpPadPosition[2] )};
        const FixedVec3 padDelta = player.position - padCenter;
        if( fixedAbs( padDelta.x ) <= value( "1.5" ) && fixedAbs( padDelta.z ) <= value( "1.5" ) && player.position.y <= value( "0.2" ) )
        {
            player.velocity = {rawFixed( Arena3DGenerated::JumpPadVelocity[0] ), rawFixed( Arena3DGenerated::JumpPadVelocity[1] ), rawFixed( Arena3DGenerated::JumpPadVelocity[2] )};
            player.grounded = false;
            this->emit_( EventType::JumpPad, Arena3DGenerated::JumpPadId, player.id, WeaponType::Nailgun, player.position, 0 );
        }
    }

    void Simulation::updateTurrets_()
    {
        const PlayerState & player = m_state.player;
        for( TurretStateData & turret : m_state.turrets )
        {
            if( turret.state == TurretState::Destroyed || turret.state == TurretState::Respawning ) continue;
            if( turret.cooldownTicks > 0 ) --turret.cooldownTicks;
            if( player.alive == false ) { turret.state = TurretState::Searching; continue; }

            const Fixed distance = fixedLength( player.position - turret.position );
            if( distance > value( "30" ) )
            {
                turret.state = TurretState::Searching;
                turret.chargeTicks = 0;
                continue;
            }

            if( turret.targetMemoryTicks > 0 ) --turret.targetMemoryTicks;
            constexpr uint32_t losPeriod = 8;
            if( m_state.tick % losPeriod == turret.id % losPeriod )
            {
                const FixedVec3 origin = turret.position + FixedVec3{Fixed::zero(), value( "0.8" ), Fixed::zero()};
                const FixedVec3 target = player.position + FixedVec3{Fixed::zero(), player.crouched ? value( "0.6" ) : value( "0.9" ), Fixed::zero()};
                if( lineBlocked( origin, target ) == false ) turret.targetMemoryTicks = 24;
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
                if( turret.chargeTicks < m_config.weapons[weaponIndex( WeaponType::Railgun )].chargeTicks ) continue;
                turret.chargeTicks = 0;
            }
            this->fireTurretWeapon_( &turret );
        }
    }

    void Simulation::fireTurretWeapon_( TurretStateData * turret )
    {
        const WeaponConfig & config = m_config.weapons[weaponIndex( turret->weapon )];
        const FixedVec3 origin = turret->position + FixedVec3{Fixed::zero(), value( "0.8" ), Fixed::zero()};
        FixedVec3 target = m_state.player.position + FixedVec3{Fixed::zero(), value( "0.9" ), Fixed::zero()};
        if( config.speed > Fixed::zero() )
        {
            const Fixed travelTime = fixedClamp( fixedLength( target - origin ) / config.speed,
                Fixed::zero(), value( "1.5" ) );
            target += m_state.player.velocity * travelTime;
            target.y += config.gravity * travelTime * travelTime * value( "0.5" );
        }
        FixedVec3 direction = fixedNormalize( target - origin );
        const uint64_t shotId = (m_state.tick << 16) ^ turret->id;
        const uint32_t aim = turret->aimRandom.bounded( 2001 );
        const Fixed error = Fixed::fromInt( static_cast<int32_t>(aim) - 1000 ) * value( "0.00001" );
        direction = fixedNormalize( {direction.x + error, direction.y, direction.z - error} );
        turret->state = TurretState::Firing;
        turret->cooldownTicks = config.cooldownTicks + 20;
        this->emit_( EventType::Shot, turret->id, m_state.player.id, turret->weapon, origin, 0 );

        if( turret->weapon == WeaponType::Railgun )
        {
            this->fireRail_( turret->id, origin, direction, std::max( 1, config.damage / 4 ) );
            return;
        }
        const uint32_t count = config.pelletCount;
        for( uint32_t projectile = 0; projectile != count; ++projectile )
        {
            FixedVec3 projectileDirection = turret->weapon == WeaponType::Shotgun
                ? this->spreadDirection_( direction, value( "0.14" ), (shotId << 8) | projectile )
                : direction;
            this->spawnProjectile_( turret->id, turret->weapon, origin, projectileDirection, (shotId << 8) | projectile );
        }
    }

    void Simulation::updateProjectiles_()
    {
        const Fixed tickSeconds = value( "0.01" );
        for( ProjectileState & projectile : m_state.projectiles )
        {
            if( projectile.active == false ) continue;
            if( projectile.lifetimeTicks == 0 )
            {
                if( isExplosive( projectile.weapon ) == true ) this->queueExplosion_( projectile );
                projectile.active = false;
                continue;
            }
            --projectile.lifetimeTicks;
            projectile.velocity.y -= projectile.gravity * tickSeconds;
            const FixedVec3 previous = projectile.position;
            const FixedVec3 displacement = projectile.velocity * tickSeconds;
            const FixedVec3 desiredPosition = previous + displacement;

            const kf_sphere_t movingSphere{toKinefix( previous ), projectile.radius.raw()};
            kf_hit_t worldHit{};
            const bool hitWorld = kf_world_sweep_sphere_hit( Arena3DGenerated::CollisionBoxes,
                Arena3DGenerated::CollisionBoxCount, &movingSphere, toKinefix( displacement ), &worldHit ) != 0;

            kf_hit_t actorHit{};
            bool hitActor = false;
            uint32_t hitActorId = 0;
            if( projectile.ownerId == m_state.player.id )
            {
                for( const TurretStateData & turret : m_state.turrets )
                {
                    if( turret.state == TurretState::Destroyed || turret.state == TurretState::Respawning ) continue;
                    const kf_capsule_t target{toKinefix( turret.position ), value( "0.65" ).raw(), value( "1.6" ).raw()};
                    kf_hit_t candidate{};
                    if( kf_sweep_sphere_capsule( &movingSphere, toKinefix( displacement ), &target, turret.id, &candidate ) == 0 ) continue;
                    if( hitActor == false || candidate.fraction < actorHit.fraction ||
                        (candidate.fraction == actorHit.fraction && candidate.object_id < actorHit.object_id) )
                    {
                        actorHit = candidate;
                        hitActor = true;
                        hitActorId = turret.id;
                    }
                }
            }
            else if( m_state.player.alive == true )
            {
                const Fixed playerHeight = m_state.player.crouched ? m_config.crouchedHeight : m_config.standingHeight;
                const kf_capsule_t target{toKinefix( m_state.player.position ), m_config.playerRadius.raw(), playerHeight.raw()};
                hitActor = kf_sweep_sphere_capsule( &movingSphere, toKinefix( displacement ),
                    &target, m_state.player.id, &actorHit ) != 0;
                if( hitActor == true ) hitActorId = m_state.player.id;
            }

            const bool actorIsFirst = hitActor == true && (hitWorld == false || actorHit.fraction < worldHit.fraction);
            if( actorIsFirst == true )
            {
                projectile.position = previous + displacement * Fixed::fromRaw( actorHit.fraction );
            }
            else if( hitWorld == true )
            {
                projectile.position = previous + displacement * Fixed::fromRaw( worldHit.fraction );
            }
            else
            {
                projectile.position = desiredPosition;
            }
            projectile.traveledDistance += fixedLength( projectile.position - previous );
            if( projectile.weapon == WeaponType::Plasmagun )
            {
                const int32_t distanceSteps = static_cast<int32_t>(projectile.traveledDistance.raw() >> Fixed::FractionBits) / 5;
                projectile.damage = std::max( 5, m_config.weapons[weaponIndex( WeaponType::Plasmagun )].damage - distanceSteps * 3 );
                if( projectile.ownerId != m_state.player.id ) projectile.damage = std::max( 1, projectile.damage / 4 );
            }

            if( actorIsFirst == true )
            {
                this->queueDamage_( hitActorId, projectile.ownerId, projectile.weapon, projectile.position, projectile.damage,
                    m_config.weapons[weaponIndex( projectile.weapon )].knockback );
                if( isExplosive( projectile.weapon ) == true ) this->queueExplosion_( projectile );
                this->emit_( EventType::Hit, projectile.ownerId, hitActorId, projectile.weapon, projectile.position, projectile.damage );
                projectile.active = false;
                continue;
            }

            if( hitWorld == true && projectile.weapon == WeaponType::GrenadeLauncher )
            {
                const FixedVec3 normal = fromKinefix( worldHit.normal );
                const Fixed normalVelocity = fixedDot( projectile.velocity, normal );
                if( normalVelocity < Fixed::zero() )
                {
                    projectile.velocity -= normal * (normalVelocity * value( "1.55" ));
                }
                else
                {
                    projectile.velocity = projectile.velocity * value( "-0.55" );
                }
                projectile.velocity.x *= value( "0.8" );
                projectile.velocity.z *= value( "0.8" );
                continue;
            }
            if( hitWorld == true )
            {
                if( isExplosive( projectile.weapon ) == true ) this->queueExplosion_( projectile );
                projectile.active = false;
                continue;
            }
        }
    }

    void Simulation::queueExplosion_( const ProjectileState & projectile )
    {
        const WeaponConfig & config = m_config.weapons[weaponIndex( projectile.weapon )];
        this->emit_( EventType::Explosion, projectile.ownerId, 0, projectile.weapon, projectile.position, projectile.damage );
        if( config.splashRadius <= Fixed::zero() ) return;

        const Fixed playerDistance = fixedLength( m_state.player.position - projectile.position );
        const FixedVec3 playerTarget = m_state.player.position + FixedVec3{Fixed::zero(), value( "0.9" ), Fixed::zero()};
        if( m_state.player.alive == true && playerDistance <= config.splashRadius && lineBlocked( projectile.position, playerTarget ) == false )
        {
            const Fixed falloff = Fixed::one() - playerDistance / config.splashRadius;
            int32_t damage = static_cast<int32_t>((Fixed::fromInt( projectile.damage ) * falloff).raw() >> Fixed::FractionBits);
            if( projectile.ownerId == m_state.player.id && projectile.weapon == WeaponType::RocketLauncher ) damage = 0;
            this->queueDamage_( m_state.player.id, projectile.ownerId, projectile.weapon, projectile.position, damage, config.knockback * falloff );
        }
        for( const TurretStateData & turret : m_state.turrets )
        {
            if( turret.state == TurretState::Destroyed || turret.state == TurretState::Respawning ) continue;
            const Fixed distance = fixedLength( turret.position - projectile.position );
            if( distance <= config.splashRadius && lineBlocked( projectile.position, turret.position ) == false )
            {
                const Fixed falloff = Fixed::one() - distance / config.splashRadius;
                const int32_t damage = static_cast<int32_t>((Fixed::fromInt( projectile.damage ) * falloff).raw() >> Fixed::FractionBits);
                this->queueDamage_( turret.id, projectile.ownerId, projectile.weapon, projectile.position, damage, config.knockback * falloff );
            }
        }
    }

    void Simulation::queueDamage_( uint32_t targetId, uint32_t sourceId, WeaponType weapon, const FixedVec3 & origin, int32_t damage, Fixed knockback )
    {
        if( m_damageCount == m_damage.size() )
        {
            m_state.fault.raise( FaultCode::Overflow, "damage queue exhausted" );
            return;
        }
        DamageRequest & request = m_damage[m_damageCount++];
        request.targetId = targetId;
        request.sourceId = sourceId;
        request.order = (m_state.tick << 32) | m_damageCount;
        request.weapon = weapon;
        request.origin = origin;
        request.damage = std::max( 0, damage );
        request.knockback = knockback;
    }

    void Simulation::applyDamage_()
    {
        std::sort( m_damage.begin(), m_damage.begin() + static_cast<std::ptrdiff_t>(m_damageCount), []( const DamageRequest & left, const DamageRequest & right )
        {
            if( left.targetId != right.targetId ) return left.targetId < right.targetId;
            if( left.sourceId != right.sourceId ) return left.sourceId < right.sourceId;
            return left.order < right.order;
        } );

        for( size_t index = 0; index != m_damageCount; ++index )
        {
            const DamageRequest & damage = m_damage[index];
            if( damage.targetId == m_state.player.id )
            {
                PlayerState & player = m_state.player;
                if( player.alive == false ) continue;
                const int32_t absorbed = std::min( player.armor, damage.damage / 2 );
                player.armor -= absorbed;
                player.health -= damage.damage - absorbed;
                if( damage.knockback > Fixed::zero() )
                {
                    const FixedVec3 direction = fixedNormalize( player.position - damage.origin );
                    player.velocity += direction * damage.knockback;
                }
                this->emit_( EventType::Damage, damage.sourceId, player.id, damage.weapon, damage.origin, damage.damage );
                if( player.health <= 0 )
                {
                    player.health = 0;
                    player.alive = false;
                    player.fireHeld = false;
                    player.railChargeTicks = 0;
                    player.respawnTicks = 300;
                    this->emit_( EventType::PlayerDeath, damage.sourceId, player.id, damage.weapon, player.position, 0 );
                }
                continue;
            }

            for( TurretStateData & turret : m_state.turrets )
            {
                if( turret.id != damage.targetId || turret.state == TurretState::Destroyed || turret.state == TurretState::Respawning ) continue;
                turret.health -= damage.damage;
                this->emit_( EventType::Damage, damage.sourceId, turret.id, damage.weapon, damage.origin, damage.damage );
                if( turret.health <= 0 )
                {
                    turret.health = 0;
                    turret.state = TurretState::Destroyed;
                    turret.respawnTicks = 500;
                    ++m_state.player.score;
                    this->emit_( EventType::TurretDestroyed, damage.sourceId, turret.id, damage.weapon, turret.position, 0 );
                }
                break;
            }
        }
    }

    void Simulation::updateRespawns_()
    {
        PlayerState & player = m_state.player;
        if( player.alive == false && player.respawnTicks > 0 && --player.respawnTicks == 0 )
        {
            player.alive = true;
            player.health = 100;
            player.armor = 0;
            const Arena3DGenerated::SpawnRaw & spawn = Arena3DGenerated::SpawnPoints[0];
            player.position = {rawFixed( spawn.position[0] ), rawFixed( spawn.position[1] ), rawFixed( spawn.position[2] )};
            player.velocity = {};
            player.yaw = spawn.yaw;
            player.nailHeat = Fixed::zero();
            this->emit_( EventType::PlayerRespawned, player.id, player.id, WeaponType::Nailgun, player.position, 0 );
        }

        for( TurretStateData & turret : m_state.turrets )
        {
            if( turret.state != TurretState::Destroyed && turret.state != TurretState::Respawning ) continue;
            turret.state = TurretState::Respawning;
            if( turret.respawnTicks > 0 && --turret.respawnTicks == 0 )
            {
                turret.health = 100;
                turret.state = TurretState::Searching;
                turret.cooldownTicks = 100;
                this->emit_( EventType::TurretRespawned, turret.id, turret.id, turret.weapon, turret.position, 0 );
            }
        }
    }

    void Simulation::updatePickups_()
    {
        PlayerState & player = m_state.player;
        for( PickupStateData & pickup : m_state.pickups )
        {
            if( pickup.active == false )
            {
                if( pickup.respawnTicks > 0 && --pickup.respawnTicks == 0 ) pickup.active = true;
                continue;
            }
            if( player.alive == false || fixedLengthSquared( player.position - pickup.position ) > value( "0.9" ) * value( "0.9" ) ) continue;

            bool consumed = false;
            switch( pickup.type )
            {
            case PickupType::Health:
                if( player.health < 100 ) { player.health = std::min( 100, player.health + pickup.amount ); consumed = true; }
                break;
            case PickupType::Armor:
                if( player.armor < 100 ) { player.armor = std::min( 100, player.armor + pickup.amount ); consumed = true; }
                break;
            case PickupType::Ammo:
            {
                const size_t index = weaponIndex( pickup.weapon );
                const uint32_t maximum = Arena3DGenerated::WeaponConfigs[index].ammo * 2u;
                if( player.ammo[index] < maximum )
                {
                    player.ammo[index] = std::min( maximum, player.ammo[index] + static_cast<uint32_t>(pickup.amount) );
                    consumed = true;
                }
                break;
            }
            }
            if( consumed == false ) continue;
            pickup.active = false;
            pickup.respawnTicks = 800u + m_state.pickupRandom.bounded( 401u );
            this->emit_( EventType::Pickup, pickup.id, player.id, pickup.weapon, pickup.position, pickup.amount );
        }
    }

    void Simulation::emit_( EventType type, uint32_t actorId, uint32_t targetId, WeaponType weapon, const FixedVec3 & position, int32_t amount )
    {
        if( m_events.size() == MaximumEventsPerTick )
        {
            m_state.fault.raise( FaultCode::Overflow, "event queue exhausted" );
            return;
        }
        ServerEvent event;
        event.id = (m_state.tick << 32) | m_eventOrdinal++;
        event.tick = m_state.tick;
        event.type = type;
        event.actorId = actorId;
        event.targetId = targetId;
        event.weapon = weapon;
        event.position = position;
        event.amount = amount;
        m_events.emplace_back( event );
    }

    void Simulation::finalizeEvents_()
    {
        std::stable_sort( m_events.begin(), m_events.end(), []( const ServerEvent & left, const ServerEvent & right )
        {
            if( left.type != right.type ) return left.type < right.type;
            if( left.actorId != right.actorId ) return left.actorId < right.actorId;
            if( left.targetId != right.targetId ) return left.targetId < right.targetId;
            if( left.weapon != right.weapon ) return left.weapon < right.weapon;
            if( left.position.x != right.position.x ) return left.position.x < right.position.x;
            if( left.position.y != right.position.y ) return left.position.y < right.position.y;
            if( left.position.z != right.position.z ) return left.position.z < right.position.z;
            return left.amount < right.amount;
        } );

        uint32_t ordinal = 0;
        for( ServerEvent & event : m_events ) event.id = (m_state.tick << 32) | ordinal++;
    }

    const SimulationState & Simulation::state() const { return m_state; }
    const std::vector<ServerEvent> & Simulation::events() const { return m_events; }
    const SimulationConfig & Simulation::config() const { return m_config; }

    void Simulation::serializePlayer_( CanonicalWriter * writer ) const
    {
        const PlayerState & player = m_state.player;
        writer->writeU32( player.id ); writer->writeVector( player.position ); writer->writeVector( player.velocity );
        writer->writeU16( player.yaw ); writer->writeI16( player.pitch ); writer->writeBool( player.grounded ); writer->writeBool( player.crouched );
        writer->writeBool( player.alive ); writer->writeBool( player.noclip ); writer->writeBool( player.fireHeld ); writer->writeBool( player.fireConsumed );
        writer->writeU8( player.inputMask ); writer->writeI32( player.health ); writer->writeI32( player.armor ); writer->writeI32( player.score );
        writer->writeU32( player.respawnTicks ); writer->writeU8( static_cast<uint8_t>(player.selectedWeapon) );
        for( uint32_t ammo : player.ammo ) writer->writeU32( ammo );
        for( uint32_t cooldown : player.cooldown ) writer->writeU32( cooldown );
        writer->writeFixed( player.nailHeat ); writer->writeU32( player.railChargeTicks ); writer->writeU64( player.shotSequence );
    }

    void Simulation::serializeProjectiles_( CanonicalWriter * writer ) const
    {
        writer->writeU32( static_cast<uint32_t>(m_state.projectiles.size()) );
        for( const ProjectileState & projectile : m_state.projectiles )
        {
            writer->writeBool( projectile.active );
            if( projectile.active == false ) continue;
            writer->writeU32( projectile.id ); writer->writeU32( projectile.ownerId ); writer->writeU8( static_cast<uint8_t>(projectile.weapon) );
            writer->writeVector( projectile.position ); writer->writeVector( projectile.velocity ); writer->writeFixed( projectile.radius );
            writer->writeFixed( projectile.gravity ); writer->writeFixed( projectile.traveledDistance ); writer->writeU32( projectile.lifetimeTicks );
            writer->writeI32( projectile.damage ); writer->writeU64( projectile.shotId );
        }
    }

    void Simulation::serializeTurrets_( CanonicalWriter * writer ) const
    {
        writer->writeU32( static_cast<uint32_t>(m_state.turrets.size()) );
        for( const TurretStateData & turret : m_state.turrets )
        {
            writer->writeU32( turret.id ); writer->writeU8( static_cast<uint8_t>(turret.weapon) ); writer->writeVector( turret.position );
            writer->writeU16( turret.yaw ); writer->writeI16( turret.pitch ); writer->writeU8( static_cast<uint8_t>(turret.state) );
            writer->writeI32( turret.health ); writer->writeU32( turret.cooldownTicks ); writer->writeU32( turret.chargeTicks );
            writer->writeU32( turret.targetMemoryTicks ); writer->writeU32( turret.respawnTicks ); writePcg( writer, turret.aimRandom );
        }
    }

    void Simulation::serializePickups_( CanonicalWriter * writer ) const
    {
        writer->writeU32( static_cast<uint32_t>(m_state.pickups.size()) );
        for( const PickupStateData & pickup : m_state.pickups )
        {
            writer->writeU32( pickup.id ); writer->writeU8( static_cast<uint8_t>(pickup.type) ); writer->writeU8( static_cast<uint8_t>(pickup.weapon) );
            writer->writeVector( pickup.position ); writer->writeI32( pickup.amount ); writer->writeU32( pickup.respawnTicks ); writer->writeBool( pickup.active );
        }
    }

    void Simulation::serializeRandom_( CanonicalWriter * writer ) const
    {
        writePcg( writer, m_state.weaponRandom );
        writePcg( writer, m_state.pickupRandom );
    }

    void Simulation::serializeState_( CanonicalWriter * writer ) const
    {
        writer->writeU32( SimulationVersion ); writer->writeU32( DeterministicMathVersion );
        writer->writeU64( m_state.matchId ); writer->writeU64( m_state.tick ); writer->writeU64( m_state.seed );
        writer->writeU32( m_state.nextObjectId ); writer->writeBool( m_state.paused );
        writer->writeU8( static_cast<uint8_t>(m_state.fault.code) );
        this->serializePlayer_( writer ); this->serializeProjectiles_( writer ); this->serializeTurrets_( writer ); this->serializePickups_( writer ); this->serializeRandom_( writer );
    }

    StateChecksum Simulation::checksum() const
    {
        StateChecksum result;
        CanonicalWriter writer;
        this->serializePlayer_( &writer ); result.player = crc32( writer.data() ); writer.clear();
        this->serializeProjectiles_( &writer ); result.projectiles = crc32( writer.data() ); writer.clear();
        this->serializeTurrets_( &writer ); result.turrets = crc32( writer.data() ); writer.clear();
        this->serializePickups_( &writer ); result.pickups = crc32( writer.data() ); writer.clear();
        this->serializeRandom_( &writer ); result.random = crc32( writer.data() ); writer.clear();
        this->serializeState_( &writer ); result.full = crc32( writer.data() );
        return result;
    }

    std::vector<uint8_t> Simulation::snapshot() const
    {
        CanonicalWriter stateWriter;
        this->serializeState_( &stateWriter );
        CanonicalWriter writer;
        writer.writeBytes( "A3SN", 4 ); writer.writeU32( SnapshotFormatVersion ); writer.writeU32( static_cast<uint32_t>(stateWriter.data().size()) );
        writer.writeU32( crc32( stateWriter.data() ) ); writer.writeBytes( stateWriter.data().data(), stateWriter.data().size() );
        return writer.take();
    }

    bool Simulation::deserializeState_( CanonicalReader * reader )
    {
        uint32_t simulationVersion = 0;
        uint32_t mathVersion = 0;
        uint8_t fault = 0;
        if( reader->readU32( &simulationVersion ) == false || reader->readU32( &mathVersion ) == false ||
            simulationVersion != SimulationVersion || mathVersion != DeterministicMathVersion || reader->readU64( &m_state.matchId ) == false ||
            reader->readU64( &m_state.tick ) == false || reader->readU64( &m_state.seed ) == false || reader->readU32( &m_state.nextObjectId ) == false ||
            reader->readBool( &m_state.paused ) == false || reader->readU8( &fault ) == false || fault > static_cast<uint8_t>(FaultCode::InvalidSnapshot) ) return false;
        m_state.fault.code = static_cast<FaultCode>(fault);

        PlayerState & player = m_state.player;
        uint8_t selected = 0;
        if( reader->readU32( &player.id ) == false || reader->readVector( &player.position ) == false || reader->readVector( &player.velocity ) == false ||
            reader->readU16( &player.yaw ) == false || reader->readI16( &player.pitch ) == false || reader->readBool( &player.grounded ) == false ||
            reader->readBool( &player.crouched ) == false || reader->readBool( &player.alive ) == false || reader->readBool( &player.noclip ) == false ||
            reader->readBool( &player.fireHeld ) == false || reader->readBool( &player.fireConsumed ) == false || reader->readU8( &player.inputMask ) == false ||
            reader->readI32( &player.health ) == false || reader->readI32( &player.armor ) == false || reader->readI32( &player.score ) == false ||
            reader->readU32( &player.respawnTicks ) == false || reader->readU8( &selected ) == false || selected >= static_cast<uint8_t>(WeaponType::Count) ) return false;
        player.selectedWeapon = static_cast<WeaponType>(selected);
        for( uint32_t & ammo : player.ammo ) if( reader->readU32( &ammo ) == false ) return false;
        for( uint32_t & cooldown : player.cooldown ) if( reader->readU32( &cooldown ) == false ) return false;
        if( reader->readFixed( &player.nailHeat ) == false || reader->readU32( &player.railChargeTicks ) == false || reader->readU64( &player.shotSequence ) == false ) return false;

        uint32_t projectileCount = 0;
        if( reader->readU32( &projectileCount ) == false || projectileCount != m_state.projectiles.size() ) return false;
        for( ProjectileState & projectile : m_state.projectiles )
        {
            projectile = ProjectileState{};
            if( reader->readBool( &projectile.active ) == false ) return false;
            if( projectile.active == false ) continue;
            uint8_t weapon = 0;
            if( reader->readU32( &projectile.id ) == false || reader->readU32( &projectile.ownerId ) == false || reader->readU8( &weapon ) == false ||
                weapon >= static_cast<uint8_t>(WeaponType::Count) || reader->readVector( &projectile.position ) == false || reader->readVector( &projectile.velocity ) == false ||
                reader->readFixed( &projectile.radius ) == false || reader->readFixed( &projectile.gravity ) == false || reader->readFixed( &projectile.traveledDistance ) == false ||
                reader->readU32( &projectile.lifetimeTicks ) == false || reader->readI32( &projectile.damage ) == false || reader->readU64( &projectile.shotId ) == false ) return false;
            projectile.weapon = static_cast<WeaponType>(weapon);
        }

        uint32_t turretCount = 0;
        if( reader->readU32( &turretCount ) == false || turretCount != m_state.turrets.size() ) return false;
        for( TurretStateData & turret : m_state.turrets )
        {
            uint8_t weapon = 0;
            uint8_t state = 0;
            if( reader->readU32( &turret.id ) == false || reader->readU8( &weapon ) == false || weapon >= static_cast<uint8_t>(WeaponType::Count) ||
                reader->readVector( &turret.position ) == false || reader->readU16( &turret.yaw ) == false || reader->readI16( &turret.pitch ) == false ||
                reader->readU8( &state ) == false || state > static_cast<uint8_t>(TurretState::Respawning) || reader->readI32( &turret.health ) == false ||
                reader->readU32( &turret.cooldownTicks ) == false || reader->readU32( &turret.chargeTicks ) == false ||
                reader->readU32( &turret.targetMemoryTicks ) == false || reader->readU32( &turret.respawnTicks ) == false || readPcg( reader, &turret.aimRandom ) == false ) return false;
            turret.weapon = static_cast<WeaponType>(weapon);
            turret.state = static_cast<TurretState>(state);
        }
        uint32_t pickupCount = 0;
        if( reader->readU32( &pickupCount ) == false || pickupCount != m_state.pickups.size() ) return false;
        for( PickupStateData & pickup : m_state.pickups )
        {
            uint8_t type = 0;
            uint8_t weapon = 0;
            if( reader->readU32( &pickup.id ) == false || reader->readU8( &type ) == false || type > static_cast<uint8_t>(PickupType::Ammo) ||
                reader->readU8( &weapon ) == false || weapon >= static_cast<uint8_t>(WeaponType::Count) || reader->readVector( &pickup.position ) == false ||
                reader->readI32( &pickup.amount ) == false || reader->readU32( &pickup.respawnTicks ) == false || reader->readBool( &pickup.active ) == false ) return false;
            pickup.type = static_cast<PickupType>(type);
            pickup.weapon = static_cast<WeaponType>(weapon);
        }
        return readPcg( reader, &m_state.weaponRandom ) && readPcg( reader, &m_state.pickupRandom );
    }

    bool Simulation::restoreSnapshot( const std::vector<uint8_t> & snapshotData )
    {
        CanonicalReader reader( snapshotData );
        char magic[4]{};
        uint32_t version = 0;
        uint32_t size = 0;
        uint32_t expectedCrc = 0;
        if( reader.readBytes( magic, 4 ) == false || std::memcmp( magic, "A3SN", 4 ) != 0 || reader.readU32( &version ) == false ||
            version != SnapshotFormatVersion || reader.readU32( &size ) == false || reader.readU32( &expectedCrc ) == false || size != reader.remaining() ) return false;
        std::vector<uint8_t> payload( size );
        if( reader.readBytes( payload.data(), payload.size() ) == false || crc32( payload ) != expectedCrc ) return false;
        Simulation candidate( m_config );
        CanonicalReader payloadReader( payload );
        if( candidate.deserializeState_( &payloadReader ) == false || payloadReader.valid() == false || payloadReader.remaining() != 0 ) return false;
        m_state = candidate.m_state;
        m_events.clear();
        m_damageCount = 0;
        return true;
    }

    void Simulation::injectFaultForTest( int64_t rawDelta )
    {
        m_state.player.position.x = Fixed::fromRaw( m_state.player.position.x.raw() + rawDelta );
    }

    std::string describeFirstDifference( const SimulationState & expected, const SimulationState & actual )
    {
#define ARENA3D_COMPARE_FIELD(Object, Field, Label) if( expected.Object.Field != actual.Object.Field ) return Label
        if( expected.matchId != actual.matchId ) return "state.matchId";
        if( expected.tick != actual.tick ) return "state.tick";
        if( expected.seed != actual.seed ) return "state.seed";
        if( expected.nextObjectId != actual.nextObjectId ) return "state.nextObjectId";
        if( expected.paused != actual.paused ) return "state.paused";
        ARENA3D_COMPARE_FIELD( player, position.x, "player.position.x" );
        ARENA3D_COMPARE_FIELD( player, position.y, "player.position.y" );
        ARENA3D_COMPARE_FIELD( player, position.z, "player.position.z" );
        ARENA3D_COMPARE_FIELD( player, velocity.x, "player.velocity.x" );
        ARENA3D_COMPARE_FIELD( player, velocity.y, "player.velocity.y" );
        ARENA3D_COMPARE_FIELD( player, velocity.z, "player.velocity.z" );
        ARENA3D_COMPARE_FIELD( player, yaw, "player.yaw" );
        ARENA3D_COMPARE_FIELD( player, pitch, "player.pitch" );
        ARENA3D_COMPARE_FIELD( player, health, "player.health" );
        ARENA3D_COMPARE_FIELD( player, armor, "player.armor" );
        ARENA3D_COMPARE_FIELD( player, score, "player.score" );
        ARENA3D_COMPARE_FIELD( player, inputMask, "player.inputMask" );
        ARENA3D_COMPARE_FIELD( player, selectedWeapon, "player.selectedWeapon" );
        for( size_t index = 0; index != expected.projectiles.size(); ++index )
        {
            const ProjectileState & left = expected.projectiles[index];
            const ProjectileState & right = actual.projectiles[index];
            const std::string prefix = "projectile[" + std::to_string( left.active ? left.id : static_cast<uint32_t>(index) ) + "].";
            if( left.active != right.active ) return prefix + "active";
            if( left.id != right.id ) return prefix + "id";
            if( left.position.x != right.position.x ) return prefix + "position.x";
            if( left.position.y != right.position.y ) return prefix + "position.y";
            if( left.position.z != right.position.z ) return prefix + "position.z";
            if( left.velocity.x != right.velocity.x ) return prefix + "velocity.x";
            if( left.velocity.y != right.velocity.y ) return prefix + "velocity.y";
            if( left.velocity.z != right.velocity.z ) return prefix + "velocity.z";
            if( left.lifetimeTicks != right.lifetimeTicks ) return prefix + "lifetimeTicks";
        }
        for( size_t index = 0; index != expected.turrets.size(); ++index )
        {
            const TurretStateData & left = expected.turrets[index];
            const TurretStateData & right = actual.turrets[index];
            const std::string prefix = "turret[" + std::to_string( left.id ) + "].";
            if( left.state != right.state ) return prefix + "state";
            if( left.health != right.health ) return prefix + "health";
            if( left.cooldownTicks != right.cooldownTicks ) return prefix + "cooldownTicks";
            if( left.chargeTicks != right.chargeTicks ) return prefix + "chargeTicks";
            if( left.aimRandom.state() != right.aimRandom.state() ) return prefix + "random.state";
        }
        for( size_t index = 0; index != expected.pickups.size(); ++index )
        {
            const PickupStateData & left = expected.pickups[index];
            const PickupStateData & right = actual.pickups[index];
            const std::string prefix = "pickup[" + std::to_string( left.id ) + "].";
            if( left.active != right.active ) return prefix + "active";
            if( left.respawnTicks != right.respawnTicks ) return prefix + "respawnTicks";
        }
        if( expected.weaponRandom.state() != actual.weaponRandom.state() ) return "random.weapon.state";
        if( expected.pickupRandom.state() != actual.pickupRandom.state() ) return "random.pickup.state";
#undef ARENA3D_COMPARE_FIELD
        return {};
    }
}
