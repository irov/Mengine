#include "Arena3DSimulation.h"

#include "Arena3DGeneratedCollision.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdAssert.h"
#include "Config/StdString.h"
#include "Config/StdUtility.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        constexpr size_t SpawnPointCount = sizeof(Arena3DGenerated::SpawnPoints) / sizeof(Arena3DGenerated::SpawnPoints[0]);
        //////////////////////////////////////////////////////////////////////////
        static constexpr uint8_t inputBit( InputButton button )
        {
            return static_cast<uint8_t>(1u << static_cast<uint8_t>(button));
        }
        //////////////////////////////////////////////////////////////////////////
        static constexpr size_t weaponIndex( WeaponType weapon )
        {
            return static_cast<size_t>(weapon);
        }
        //////////////////////////////////////////////////////////////////////////
        static bool collidesPlayer( const kf_vec3_t & position, kf_fixed_t radius, kf_fixed_t height )
        {
            const kf_capsule_t capsule{position, radius, height};
            return kf_world_overlap_capsule( Arena3DGenerated::CollisionBoxes,
                Arena3DGenerated::CollisionBoxCount, &capsule, nullptr ) == KF_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static void accelerateQuake( kf_vec3_t * velocity, const kf_vec3_t & wishDirection, kf_fixed_t wishSpeed, kf_fixed_t acceleration, kf_fixed_t tickSeconds )
        {
            const kf_fixed_t currentSpeed = kf_vec3_dot( {velocity->x, 0, velocity->z}, {wishDirection.x, 0, wishDirection.z} );
            const kf_fixed_t addSpeed = kf_fixed_sub( wishSpeed, currentSpeed );
            if( addSpeed <= 0 )
            {
                return;
            }

            const kf_fixed_t accelerationSpeed = kf_fixed_min( kf_fixed_mul( acceleration, tickSeconds ), addSpeed );
            velocity->x = kf_fixed_add_mul( velocity->x, wishDirection.x, accelerationSpeed );
            velocity->z = kf_fixed_add_mul( velocity->z, wishDirection.z, accelerationSpeed );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SimulationConfig SimulationConfig::defaults()
    {
        SimulationConfig config;
        config.groundAcceleration = Arena3DGenerated::GroundAcceleration;
        config.airAcceleration = Arena3DGenerated::AirAcceleration;
        config.friction = Arena3DGenerated::Friction;
        config.gravity = Arena3DGenerated::Gravity;
        config.jumpImpulse = Arena3DGenerated::JumpImpulse;
        config.standingHeight = Arena3DGenerated::StandingHeight;
        config.crouchedHeight = Arena3DGenerated::CrouchedHeight;
        config.playerRadius = Arena3DGenerated::PlayerRadius;
        config.stepHeight = Arena3DGenerated::StepHeight;
        config.maximumSlopeCosine = Arena3DGenerated::MaximumSlopeCosine;
        config.crouchedSpeedScale = Arena3DGenerated::CrouchedSpeedScale;
        config.slideMinimumSpeed = Arena3DGenerated::SlideMinimumSpeed;
        config.slideFriction = Arena3DGenerated::SlideFriction;
        config.slideAccelerationScale = Arena3DGenerated::SlideAccelerationScale;
        config.slideMaximumSpeed = Arena3DGenerated::SlideMaximumSpeed;
        config.arenaHalfExtent = Arena3DGenerated::ArenaHalfExtent;
        config.maximumPitch = INT16_C(16202);
        // 89 degrees in binary-angle units.

        static_assert( WeaponTypeCount == sizeof(Arena3DGenerated::WeaponConfigs) / sizeof(Arena3DGenerated::WeaponConfigs[0]) );
        for( size_t index = 0; index != WeaponTypeCount; ++index )
        {
            const Arena3DGenerated::WeaponConfigRaw & source = Arena3DGenerated::WeaponConfigs[index];
            WeaponConfig & weapon = config.weapons[index];
            weapon.damage = source.damage;
            weapon.cooldownTicks = source.cooldownTicks;
            weapon.speed = source.speed;
            weapon.launchUpwardSpeed = source.launchUpwardSpeed;
            weapon.gravity = source.gravity;
            weapon.lifetimeTicks = source.lifetimeTicks;
            weapon.chargeTicks = source.chargeTicks;
            weapon.pelletCount = source.pelletCount;
            weapon.splashRadius = source.splashRadius;
            weapon.knockback = source.knockback;
        }
        return config;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StateChecksum::operator==( const StateChecksum & other ) const
    {
        return player == other.player && projectiles == other.projectiles && turrets == other.turrets && pickups == other.pickups && random == other.random && full == other.full;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StateChecksum::operator!=( const StateChecksum & other ) const
    {
        return ( *this == other ) == false;
    }
    //////////////////////////////////////////////////////////////////////////
    Simulation::Simulation( SimulationConfig config )
        : m_config( Mengine::StdUtility::move( config ) )
    {
        m_events.reserve( MaximumEventsPerTick );
        this->reset( 1, UINT64_C(0xA3E4D0197C2B6501) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::reset( uint64_t matchId, uint64_t seed )
    {
        m_state = SimulationState{};
        m_state.matchId = matchId;
        m_state.seed = seed;
        m_state.nextPlayerId = 1;
        m_state.nextObjectId = 1000;
        m_state.weaponRandom.seed( splitMix64( seed ^ UINT64_C(0x574541504F4E) ), UINT64_C(0xA3) );
        m_state.pickupRandom.seed( splitMix64( seed ^ UINT64_C(0x5049434B5550) ), UINT64_C(0xB7) );
        this->initializeTurrets_();
        this->initializePickups_();
        this->addPlayer();
        m_events.clear();
        m_damageCount = 0;
        m_eventOrdinal = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Simulation::addPlayer( uint32_t playerId )
    {
        if( playerId != 0 && this->findPlayer( playerId ) != nullptr )
        {
            return 0;
        }
        for( PlayerState & player : m_state.players )
        {
            if( player.active == true )
            {
                continue;
            }
            player = PlayerState{};
            player.active = true;
            player.id = playerId == 0 ? m_state.nextPlayerId++ : playerId;
            if( player.id >= m_state.nextPlayerId )
            {
                m_state.nextPlayerId = player.id + 1;
            }
            for( size_t index = 0; index != WeaponTypeCount; ++index )
            {
                player.ammo[index] = Arena3DGenerated::WeaponConfigs[index].ammo;
            }
            this->spawnPlayer_( &player );
            return player.id;
        }
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Simulation::removePlayer( uint32_t playerId )
    {
        PlayerState * player = this->findPlayer( playerId );
        if( player == nullptr )
        {
            return false;
        }
        *player = PlayerState{};
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PlayerState * Simulation::findPlayer( uint32_t playerId )
    {
        for( PlayerState & player : m_state.players )
        {
            if( player.active == true && player.id == playerId )
            {
                return &player;
            }
        }
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const PlayerState * Simulation::findPlayer( uint32_t playerId ) const
    {
        for( const PlayerState & player : m_state.players )
        {
            if( player.active == true && player.id == playerId )
            {
                return &player;
            }
        }
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::spawnPlayer_( PlayerState * player )
    {
        size_t selected = 0;
        kf_fixed_t selectedDistance = -1;
        for( size_t spawnIndex = 0; spawnIndex != Detail::SpawnPointCount; ++spawnIndex )
        {
            const Arena3DGenerated::SpawnRaw & spawn = Arena3DGenerated::SpawnPoints[spawnIndex];
            const kf_vec3_t position{spawn.position[0], spawn.position[1], spawn.position[2]};
            kf_fixed_t minimumDistance = kf_fixed_from_int( 2047 );
            for( const PlayerState & other : m_state.players )
            {
                if( other.active == false || other.alive == false || other.id == player->id )
                {
                    continue;
                }
                minimumDistance = kf_fixed_min( minimumDistance, kf_vec3_length_squared( kf_vec3_sub( position, other.position ) ) );
            }
            if( minimumDistance > selectedDistance )
            {
                selectedDistance = minimumDistance;
                selected = spawnIndex;
            }
        }

        const Arena3DGenerated::SpawnRaw & spawn = Arena3DGenerated::SpawnPoints[selected];
        player->position = {spawn.position[0], spawn.position[1], spawn.position[2]};
        player->velocity = {};
        player->yaw = spawn.yaw;
        player->pitch = 0;
        player->grounded = true;
        player->crouched = false;
        player->alive = true;
        player->health = 100;
        player->armor = 0;
        player->respawnTicks = 0;
        player->inputMask = 0;
        player->fireHeld = false;
        player->fireConsumed = false;
        player->nailHeat = 0;
        player->railChargeTicks = 0;
        player->selectedWeapon = WeaponType::Nailgun;
        for( size_t index = 0; index != WeaponTypeCount; ++index )
        {
            player->ammo[index] = Arena3DGenerated::WeaponConfigs[index].ammo;
            player->cooldown[index] = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::initializeTurrets_()
    {
        static_assert( MaximumTurrets == sizeof(Arena3DGenerated::Turrets) / sizeof(Arena3DGenerated::Turrets[0]) );

        for( size_t index = 0; index != MaximumTurrets; ++index )
        {
            const Arena3DGenerated::TurretRaw & source = Arena3DGenerated::Turrets[index];
            TurretStateData & turret = m_state.turrets[index];
            turret.id = source.id;
            turret.weapon = static_cast<WeaponType>(source.weapon);
            turret.position = {source.position[0], source.position[1], source.position[2]};
            turret.yaw = source.yaw;
            turret.state = m_config.turretCombat == true ? TurretState::Searching : TurretState::Idle;
            turret.cooldownTicks = 200u + static_cast<uint32_t>(index) * 17u;
            turret.health = 100;
            turret.aimRandom.seed( splitMix64( m_state.seed ^ turret.id ), turret.id );
        }
    }
    //////////////////////////////////////////////////////////////////////////
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
            pickup.position = {source.position[0], source.position[1], source.position[2]};
            pickup.amount = source.amount;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::step( const VectorCommandEnvelope & commands )
    {
        m_events.clear();
        m_damageCount = 0;
        m_eventOrdinal = 0;

        this->applyCommands_( commands );
        if( m_state.paused == true )
        {
            this->finalizeEvents_();
            return;
        }

        this->updateWeapons_();
        this->updateTurrets_();
        for( PlayerState & player : m_state.players )
        {
            if( player.active == true )
            {
                this->updatePlayer_( &player );
            }
        }
        this->updateProjectiles_();
        this->applyDamage_();
        this->updatePickups_();
        this->updateRespawns_();
        this->finalizeEvents_();
        ++m_state.tick;
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::applyCommands_( const VectorCommandEnvelope & commands )
    {
        VectorCommandEnvelopePtr ordered;
        ordered.reserve( commands.size() );
        for( const CommandEnvelope & command : commands )
        {
            ordered.emplace_back( &command );
        }
        Mengine::StdAlgorithm::stable_sort( ordered.begin(), ordered.end(),
                                            []( const CommandEnvelope * left, const CommandEnvelope * right )
                                            {
                                                if( left->playerId != right->playerId )
                                                {
                                                    return left->playerId < right->playerId;
                                                }
                                                return left->sequence < right->sequence;
                                            } );

        for( const CommandEnvelope * commandPtr : ordered )
        {
            const CommandEnvelope & command = *commandPtr;
            if( command.accepted == false || command.matchId != m_state.matchId )
            {
                this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                continue;
            }

            if( command.type == CommandType::JoinPlayer )
            {
                if( command.playerId == 0 || this->addPlayer( command.playerId ) == 0 )
                {
                    this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                }
                continue;
            }
            if( command.type == CommandType::LeavePlayer )
            {
                if( this->removePlayer( command.playerId ) == false )
                {
                    this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                }
                continue;
            }

            PlayerState * playerPtr = this->findPlayer( command.playerId );
            if( playerPtr == nullptr )
            {
                this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                continue;
            }
            PlayerState & player = *playerPtr;
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
                if( buttonValue >= 6u || (player.inputMask & Detail::inputBit( command.payload.button )) != 0 )
                {
                    this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                    break;
                }
                player.inputMask |= Detail::inputBit( command.payload.button );
                break;
            }
            case CommandType::ButtonUp:
            {
                const uint8_t buttonValue = static_cast<uint8_t>(command.payload.button);
                if( buttonValue >= 6u || (player.inputMask & Detail::inputBit( command.payload.button )) == 0 )
                {
                    this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
                    break;
                }
                player.inputMask &= static_cast<uint8_t>(~Detail::inputBit( command.payload.button ));
                break;
            }
            case CommandType::LookDelta:
                player.yaw = kf_angle16_add( player.yaw, command.payload.lookYaw );
                player.pitch = kf_sangle16_add_clamped( player.pitch, command.payload.lookPitch,
                    static_cast<kf_sangle16_t>(-m_config.maximumPitch), m_config.maximumPitch );
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
                if( command.payload.weapon < WeaponType::Count && player.ammo[Detail::weaponIndex( command.payload.weapon )] > 0 )
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
            case CommandType::Pause:
                m_state.paused = true;
                break;
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
                Mengine::Array<uint32_t, MaximumPlayers> playerIds{};
                size_t playerCount = 0;
                for( const PlayerState & existing : m_state.players )
                {
                    if( existing.active == true )
                    {
                        playerIds[playerCount++] = existing.id;
                    }
                }
                const uint64_t nextMatch = m_state.matchId + 1;
                const uint64_t nextSeed = splitMix64( m_state.seed ^ nextMatch );
                this->reset( nextMatch, nextSeed );
                for( PlayerState & existing : m_state.players )
                {
                    existing = PlayerState{};
                }
                for( size_t index = 0; index != playerCount; ++index )
                {
                    this->addPlayer( playerIds[index] );
                }
                break;
            }
            case CommandType::Noclip:
                player.noclip = command.payload.enabled;
                break;
            case CommandType::JoinPlayer:
            case CommandType::LeavePlayer:
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::updatePlayer_( PlayerState * playerPtr )
    {
        PlayerState & player = *playerPtr;
        if( player.alive == false )
        {
            return;
        }
        const kf_fixed_t tickSeconds = kf_fixed_from_ratio( 1, 100 );
        const kf_vec3_t positionBeforeStep = player.position;

        kf_fixed_t sinYaw;
        kf_fixed_t cosYaw;
        kf_angle16_sin_cos( player.yaw, &sinYaw, &cosYaw );
        kf_vec3_t wish;
        if( player.inputMask & Detail::inputBit( InputButton::Forward ) )
        {
            wish.x = kf_fixed_add( wish.x, sinYaw );
            wish.z = kf_fixed_add( wish.z, cosYaw );
        }
        if( player.inputMask & Detail::inputBit( InputButton::Backward ) )
        {
            wish.x = kf_fixed_sub( wish.x, sinYaw );
            wish.z = kf_fixed_sub( wish.z, cosYaw );
        }
        if( player.inputMask & Detail::inputBit( InputButton::Right ) )
        {
            wish.x = kf_fixed_add( wish.x, cosYaw );
            wish.z = kf_fixed_sub( wish.z, sinYaw );
        }
        if( player.inputMask & Detail::inputBit( InputButton::Left ) )
        {
            wish.x = kf_fixed_sub( wish.x, cosYaw );
            wish.z = kf_fixed_add( wish.z, sinYaw );
        }
        const bool wantsCrouch = (player.inputMask & Detail::inputBit( InputButton::Crouch )) != 0;
        if( wantsCrouch == true )
        {
            player.crouched = true;
        }
        else if( player.crouched == true && Detail::collidesPlayer( player.position, m_config.playerRadius, m_config.standingHeight ) == false )
        {
            player.crouched = false;
        }

        if( player.noclip == true )
        {
            if( player.inputMask & Detail::inputBit( InputButton::Jump ) )
            {
                wish.y = kf_fixed_add( wish.y, KF_FIXED_SCALE );
            }
            if( player.crouched == true )
            {
                wish.y = kf_fixed_sub( wish.y, KF_FIXED_SCALE );
            }
            player.velocity = kf_vec3_mul( kf_vec3_normalize( wish ), kf_fixed_from_int( 12 ) );
            player.position = kf_vec3_add_mul( player.position, player.velocity, tickSeconds );
            return;
        }

        if( player.grounded == true && (player.inputMask & Detail::inputBit( InputButton::Jump )) != 0 )
        {
            player.velocity.y = m_config.jumpImpulse;
            player.grounded = false;
        }

        const kf_fixed_t speedBeforeMove = kf_vec3_length( {player.velocity.x, 0, player.velocity.z} );
        const bool sliding = wantsCrouch == true && player.grounded == true && speedBeforeMove >= m_config.slideMinimumSpeed;
        const bool hasWish = kf_vec3_length_squared( wish ) > 0;
        if( hasWish == true )
        {
            wish = kf_vec3_normalize( wish );
            kf_fixed_t acceleration = player.grounded ? m_config.groundAcceleration : m_config.airAcceleration;
            if( sliding == true )
            {
                acceleration = kf_fixed_mul( acceleration, m_config.slideAccelerationScale );
            }
            else if( player.crouched == true )
            {
                acceleration = kf_fixed_mul( acceleration, m_config.crouchedSpeedScale );
            }

            const kf_fixed_t maximumSpeed = sliding == true
                ? m_config.slideMaximumSpeed
                : kf_fixed_mul( kf_fixed_from_int( 8 ), player.crouched ? m_config.crouchedSpeedScale : KF_FIXED_SCALE );
            Detail::accelerateQuake( &player.velocity, wish, maximumSpeed, acceleration, tickSeconds );

            if( player.grounded == true )
            {
                const kf_fixed_t horizontalSpeed = kf_vec3_length( {player.velocity.x, 0, player.velocity.z} );
                if( horizontalSpeed > maximumSpeed )
                {
                    player.velocity.x = kf_fixed_mul_div( player.velocity.x, maximumSpeed, horizontalSpeed );
                    player.velocity.z = kf_fixed_mul_div( player.velocity.z, maximumSpeed, horizontalSpeed );
                }
            }
        }
        if( sliding == true )
        {
            player.velocity.x = kf_fixed_mul( player.velocity.x, m_config.slideFriction );
            player.velocity.z = kf_fixed_mul( player.velocity.z, m_config.slideFriction );
        }
        else if( hasWish == false && player.grounded == true )
        {
            player.velocity.x = kf_fixed_mul( player.velocity.x, m_config.friction );
            player.velocity.z = kf_fixed_mul( player.velocity.z, m_config.friction );
        }

        const kf_fixed_t height = player.crouched ? m_config.crouchedHeight : m_config.standingHeight;
        kf_character_body_t body{player.position, player.velocity, player.grounded ? KF_TRUE : KF_FALSE};
        const kf_character_config_t characterConfig{
            m_config.playerRadius, height, m_config.stepHeight, m_config.gravity,
            tickSeconds, kf_fixed_from_ratio( 2, 100 ), m_config.arenaHalfExtent};
        kf_character_result_t result{};
        kf_character_step( &body, &characterConfig, Arena3DGenerated::CollisionBoxes,
            Arena3DGenerated::CollisionBoxCount, &result );
        player.position = body.position;
        player.velocity = body.velocity;
        player.grounded = body.grounded == KF_TRUE;

        const kf_capsule_t playerCapsuleAfterStep{player.position, m_config.playerRadius, height};
        for( const PlayerState & other : m_state.players )
        {
            if( other.active == false || other.alive == false || other.id == player.id )
            {
                continue;
            }
            const kf_fixed_t otherHeight = other.crouched ? m_config.crouchedHeight : m_config.standingHeight;
            const kf_capsule_t otherCapsule{other.position, m_config.playerRadius, otherHeight};
            if( kf_overlap_capsule_capsule( &playerCapsuleAfterStep, &otherCapsule ) == KF_FALSE )
            {
                continue;
            }
            player.position = positionBeforeStep;
            player.velocity.x = 0;
            player.velocity.z = 0;
            break;
        }
        const kf_fixed_t previousVertical = result.previous_vertical_velocity;
        if( result.landed == KF_TRUE && previousVertical < kf_fixed_from_int( -12 ) )
        {
            const int32_t fallDamage = kf_fixed_to_int( kf_fixed_sub( kf_fixed_abs( previousVertical ), kf_fixed_from_int( 12 ) ) ) * 5;
            if( fallDamage > 0 )
            {
                this->queueDamage_( player.id, 0, WeaponType::Nailgun, player.position, fallDamage, 0 );
            }
        }

        for( size_t index = 0; index != Arena3DGenerated::JumpPadCount; ++index )
        {
            const Arena3DGenerated::JumpPadRaw & pad = Arena3DGenerated::JumpPads[index];
            const kf_vec3_t padCenter{pad.position[0], pad.position[1], pad.position[2]};
            const kf_vec3_t padHalfSize{pad.halfSize[0], pad.halfSize[1], pad.halfSize[2]};
            const kf_vec3_t padDelta = kf_vec3_sub( player.position, padCenter );
            if( player.grounded == false || kf_fixed_abs( padDelta.x ) > padHalfSize.x || kf_fixed_abs( padDelta.z ) > padHalfSize.z || kf_fixed_abs( padDelta.y ) > kf_fixed_add( padHalfSize.y, m_config.stepHeight ) )
            {
                continue;
            }

            player.velocity = {pad.velocity[0], pad.velocity[1], pad.velocity[2]};
            player.grounded = false;
            this->emit_( EventType::JumpPad, pad.id, player.id, WeaponType::Nailgun, player.position, 0 );
            break;
        }

        const kf_capsule_t playerCapsule{player.position, m_config.playerRadius, height};
        for( size_t index = 0; index != Arena3DGenerated::HazardCount; ++index )
        {
            const Arena3DGenerated::HazardRaw & hazard = Arena3DGenerated::Hazards[index];
            if( hazard.intervalTicks == 0 || m_state.tick % hazard.intervalTicks != 0 )
            {
                continue;
            }

            const kf_vec3_t center{hazard.position[0], hazard.position[1], hazard.position[2]};
            const kf_vec3_t halfSize{hazard.halfSize[0], hazard.halfSize[1], hazard.halfSize[2]};
            const kf_vec3_t minimum = kf_vec3_sub( center, halfSize );
            const kf_vec3_t maximum = kf_vec3_add( center, halfSize );
            const kf_aabb_t volume{hazard.id, {minimum.x, minimum.y, minimum.z}, {maximum.x, maximum.y, maximum.z}};
            if( kf_overlap_capsule_aabb( &playerCapsule, &volume ) == KF_FALSE )
            {
                continue;
            }

            this->queueDamage_( player.id, hazard.id, WeaponType::Nailgun, center, hazard.damage, 0 );
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::updateRespawns_()
    {
        for( PlayerState & player : m_state.players )
        {
            if( player.active == false || player.alive == true || player.respawnTicks == 0 || --player.respawnTicks != 0 )
            {
                continue;
            }
            this->spawnPlayer_( &player );
            this->emit_( EventType::PlayerRespawned, player.id, player.id, WeaponType::Nailgun, player.position, 0 );
        }

        for( TurretStateData & turret : m_state.turrets )
        {
            if( turret.state != TurretState::Destroyed && turret.state != TurretState::Respawning )
            {
                continue;
            }
            turret.state = TurretState::Respawning;
            if( turret.respawnTicks > 0 && --turret.respawnTicks == 0 )
            {
                turret.health = 100;
                turret.state = m_config.turretCombat == true ? TurretState::Searching : TurretState::Idle;
                turret.cooldownTicks = 100;
                this->emit_( EventType::TurretRespawned, turret.id, turret.id, turret.weapon, turret.position, 0 );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::updatePickups_()
    {
        for( PickupStateData & pickup : m_state.pickups )
        {
            if( pickup.active == false )
            {
                if( pickup.respawnTicks > 0 && --pickup.respawnTicks == 0 )
                {
                    pickup.active = true;
                }
                continue;
            }
            PlayerState * player = nullptr;
            for( PlayerState & candidate : m_state.players )
            {
                if( candidate.active == false || candidate.alive == false || kf_vec3_length_squared( kf_vec3_sub( candidate.position, pickup.position ) ) > kf_fixed_mul( kf_fixed_from_ratio( 9, 10 ), kf_fixed_from_ratio( 9, 10 ) ) )
                {
                    continue;
                }
                if( player == nullptr || candidate.id < player->id )
                {
                    player = &candidate;
                }
            }
            if( player == nullptr )
            {
                continue;
            }

            bool consumed = false;
            switch( pickup.type )
            {
            case PickupType::Health:
                if( player->health < 100 )
                {
                    player->health = Mengine::StdAlgorithm::min( 100, player->health + pickup.amount );
                    consumed = true;
                }
                break;
            case PickupType::Armor:
                if( player->armor < 100 )
                {
                    player->armor = Mengine::StdAlgorithm::min( 100, player->armor + pickup.amount );
                    consumed = true;
                }
                break;
            case PickupType::Ammo:
            {
                const size_t index = Detail::weaponIndex( pickup.weapon );
                const uint32_t maximum = Arena3DGenerated::WeaponConfigs[index].ammo * 2u;
                if( player->ammo[index] < maximum )
                {
                    player->ammo[index] = Mengine::StdAlgorithm::min( maximum, player->ammo[index] + static_cast<uint32_t>(pickup.amount) );
                    consumed = true;
                }
                break;
            }
            }
            if( consumed == false )
            {
                continue;
            }
            pickup.active = false;
            pickup.respawnTicks = 800u + m_state.pickupRandom.bounded( 401u );
            this->emit_( EventType::Pickup, pickup.id, player->id, pickup.weapon, pickup.position, pickup.amount );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::emit_( EventType type, uint32_t actorId, uint32_t targetId, WeaponType weapon, const kf_vec3_t & position, int32_t amount )
    {
        if( m_events.size() == MaximumEventsPerTick )
        {
            assert( false && "Arena3D event queue exhausted" );
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
    //////////////////////////////////////////////////////////////////////////
    void Simulation::finalizeEvents_()
    {
        Mengine::StdAlgorithm::stable_sort( m_events.begin(), m_events.end(),
                                            []( const ServerEvent & left, const ServerEvent & right )
                                            {
                                                if( left.type != right.type )
                                                {
                                                    return left.type < right.type;
                                                }
                                                if( left.actorId != right.actorId )
                                                {
                                                    return left.actorId < right.actorId;
                                                }
                                                if( left.targetId != right.targetId )
                                                {
                                                    return left.targetId < right.targetId;
                                                }
                                                if( left.weapon != right.weapon )
                                                {
                                                    return left.weapon < right.weapon;
                                                }
                                                if( left.position.x != right.position.x )
                                                {
                                                    return left.position.x < right.position.x;
                                                }
                                                if( left.position.y != right.position.y )
                                                {
                                                    return left.position.y < right.position.y;
                                                }
                                                if( left.position.z != right.position.z )
                                                {
                                                    return left.position.z < right.position.z;
                                                }
                                                return left.amount < right.amount;
                                            } );

        uint32_t ordinal = 0;
        for( ServerEvent & event : m_events )
        {
            event.id = ( m_state.tick << 32 ) | ordinal++;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
