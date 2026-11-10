#include "Arena3DSimulation.h"

#include "Arena3DGeneratedCollision.h"

#include "Kernel/ExceptionHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdString.h"
#include "Config/StdUtility.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const size_t INITIAL_EVENT_CAPACITY = 512;
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
        static bool collidesPlayer( kf_world_t * const _world, const kf_vec3_t & _position, kf_fixed_t _radius, kf_fixed_t _height )
        {
            kf_shape_t shape = makeCapsuleShape( _radius, _height );
            kf_pose_t pose{_position, {0, 0, 0}};
            kf_contact_t contacts[64];
            size_t count = 0;
            kf_status_t status = kf_world_contacts( _world, &shape, pose, nullptr, contacts, 64, &count );
            isCollisionHit( status );
            for( size_t index = 0; index != count; ++index )
            {
                if( contacts[index].depth > KF_GEOMETRY_TOLERANCE )
                {
                    return true;
                }
            }

            return false;
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
        m_events.reserve( Detail::INITIAL_EVENT_CAPACITY );
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
        uint32_t id = playerId == 0 ? m_state.nextPlayerId : playerId;
        if( id == 0 )
        {
            return 0;
        }
        if( this->findPlayer( id ) != nullptr )
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
            player.id = id;
            if( m_state.nextPlayerId != 0 && player.id >= m_state.nextPlayerId )
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
                minimumDistance = kf_fixed_min( minimumDistance, kf_vec3_length_squared( kf_vec3_sub( position, other.character.position ) ) );
            }
            if( minimumDistance > selectedDistance )
            {
                selectedDistance = minimumDistance;
                selected = spawnIndex;
            }
        }

        const Arena3DGenerated::SpawnRaw & spawn = Arena3DGenerated::SpawnPoints[selected];
        player->character = {};
        player->character.position = {spawn.position[0], spawn.position[1], spawn.position[2]};
        player->character.velocity = {};
        player->yaw = spawn.yaw;
        player->pitch = 0;
        player->character.grounded = true;
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
    void Simulation::step( const VectorCommandEnvelope & commands, VectorCommandResult * const _results )
    {
        m_events.clear();
        m_events.reserve( Detail::INITIAL_EVENT_CAPACITY + commands.size() );
        m_damageCount = 0;
        m_eventOrdinal = 0;

        if( _results != nullptr )
        {
            _results->clear();
            _results->resize( commands.size() );
        }
        this->applyCommands_( commands, _results );
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
    void Simulation::applyCommands_( const VectorCommandEnvelope & commands, VectorCommandResult * const _results )
    {
        for( size_t index = 0; index != commands.size(); ++index )
        {
            const CommandEnvelope & command = commands[index];
            bool applied = this->applyCommand_( command );
            if( applied == false )
            {
                this->emit_( EventType::CommandRejected, command.playerId, 0, WeaponType::Nailgun, {}, 0 );
            }
            if( _results != nullptr )
            {
                (*_results)[index] = {command.playerId, command.sequence, applied == true ? 0 : 1};
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Simulation::applyCommand_( const CommandEnvelope & _command )
    {
        if( _command.accepted == false )
        {
            return false;
        }
        if( _command.matchId != m_state.matchId )
        {
            return false;
        }

        if( _command.type == CommandType::JoinPlayer )
        {
            if( _command.playerId == 0 )
            {
                return false;
            }
            if( this->addPlayer( _command.playerId ) == 0 )
            {
                return false;
            }
            return true;
        }
        if( _command.type == CommandType::LeavePlayer )
        {
            if( this->removePlayer( _command.playerId ) == false )
            {
                return false;
            }
            return true;
        }

        PlayerState * playerPtr = this->findPlayer( _command.playerId );
        if( playerPtr == nullptr )
        {
            return false;
        }
        PlayerState & player = *playerPtr;
        if( m_state.paused == true && _command.type != CommandType::Resume && _command.type != CommandType::Restart )
        {
            return false;
        }

        switch( _command.type )
        {
        case CommandType::ButtonDown:
        {
            uint8_t buttonValue = static_cast<uint8_t>(_command.payload.button);
            if( buttonValue >= 6u )
            {
                return false;
            }
            uint8_t buttonBit = Detail::inputBit( _command.payload.button );
            if( (player.inputMask & buttonBit) != 0 )
            {
                return false;
            }
            player.inputMask |= buttonBit;
            break;
        }
        case CommandType::ButtonUp:
        {
            uint8_t buttonValue = static_cast<uint8_t>(_command.payload.button);
            if( buttonValue >= 6u )
            {
                return false;
            }
            uint8_t buttonBit = Detail::inputBit( _command.payload.button );
            if( (player.inputMask & buttonBit) == 0 )
            {
                return false;
            }
            player.inputMask &= static_cast<uint8_t>(~buttonBit);
            break;
        }
        case CommandType::LookDelta:
            player.yaw = kf_angle16_add( player.yaw, _command.payload.lookYaw );
            player.pitch = kf_sangle16_add_clamped( player.pitch, _command.payload.lookPitch,
                static_cast<kf_sangle16_t>(-m_config.maximumPitch), m_config.maximumPitch );
            break;
        case CommandType::FireDown:
            if( player.fireHeld == true )
            {
                return false;
            }
            player.fireHeld = true;
            player.fireConsumed = false;
            break;
        case CommandType::FireUp:
            if( player.fireHeld == false )
            {
                return false;
            }
            player.fireHeld = false;
            player.fireConsumed = false;
            player.railChargeTicks = 0;
            break;
        case CommandType::SelectWeapon:
            if( _command.payload.weapon < WeaponType::Count && player.ammo[Detail::weaponIndex( _command.payload.weapon )] > 0 )
            {
                player.selectedWeapon = _command.payload.weapon;
                player.railChargeTicks = 0;
                player.fireConsumed = false;
            }
            else
            {
                return false;
            }
            break;
        case CommandType::Pause:
            m_state.paused = true;
            break;
        case CommandType::Resume:
            if( m_state.paused == false )
            {
                return false;
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
            uint64_t nextMatch = m_state.matchId + 1;
            uint64_t nextSeed = splitMix64( m_state.seed ^ nextMatch );
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
        {
            kf_vec3_t position = player.character.position;
            player.character = {};
            player.character.position = position;
            player.noclip = _command.payload.enabled;
            break;
        }
        case CommandType::JoinPlayer:
        case CommandType::LeavePlayer:
            break;
        }
        return true;
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
        const kf_vec3_t positionBeforeStep = player.character.position;

        kf_fixed_t sinYaw;
        kf_fixed_t cosYaw;
        kf_angle16_sin_cos( player.yaw, &sinYaw, &cosYaw );
        kf_vec3_t wish{};
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
        else if( player.crouched == true && Detail::collidesPlayer( m_collisionWorld.getWorld(), player.character.position, m_config.playerRadius, m_config.standingHeight ) == false )
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
            player.character.velocity = kf_vec3_mul( kf_vec3_normalize( wish ), kf_fixed_from_int( 12 ) );
            player.character.position = kf_vec3_add_mul( player.character.position, player.character.velocity, tickSeconds );
            return;
        }

        if( player.character.grounded == true && (player.inputMask & Detail::inputBit( InputButton::Jump )) != 0 )
        {
            player.character.velocity.y = m_config.jumpImpulse;
            player.character.grounded = false;
        }

        const kf_fixed_t speedBeforeMove = kf_vec3_length( {player.character.velocity.x, 0, player.character.velocity.z} );
        const bool sliding = wantsCrouch == true && player.character.grounded == true && speedBeforeMove >= m_config.slideMinimumSpeed;
        const bool hasWish = kf_vec3_length_squared( wish ) > 0;
        if( hasWish == true )
        {
            wish = kf_vec3_normalize( wish );
            kf_fixed_t acceleration = player.character.grounded ? m_config.groundAcceleration : m_config.airAcceleration;
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
            Detail::accelerateQuake( &player.character.velocity, wish, maximumSpeed, acceleration, tickSeconds );

            if( player.character.grounded == true )
            {
                const kf_fixed_t horizontalSpeed = kf_vec3_length( {player.character.velocity.x, 0, player.character.velocity.z} );
                if( horizontalSpeed > maximumSpeed )
                {
                    player.character.velocity.x = kf_fixed_mul_div( player.character.velocity.x, maximumSpeed, horizontalSpeed );
                    player.character.velocity.z = kf_fixed_mul_div( player.character.velocity.z, maximumSpeed, horizontalSpeed );
                }
            }
        }
        if( sliding == true )
        {
            player.character.velocity.x = kf_fixed_mul( player.character.velocity.x, m_config.slideFriction );
            player.character.velocity.z = kf_fixed_mul( player.character.velocity.z, m_config.slideFriction );
        }
        else if( hasWish == false && player.character.grounded == true )
        {
            player.character.velocity.x = kf_fixed_mul( player.character.velocity.x, m_config.friction );
            player.character.velocity.z = kf_fixed_mul( player.character.velocity.z, m_config.friction );
        }

        const kf_fixed_t height = player.crouched ? m_config.crouchedHeight : m_config.standingHeight;
        kf_character_state_t beforeMove = player.character;
        kf_character_settings_t settings = kf_character_settings_default();
        settings.radius = m_config.playerRadius;
        settings.height = height;
        settings.step_height = m_config.stepHeight;
        settings.snap_distance = m_config.stepHeight;
        settings.minimum_ground_normal_y = m_config.maximumSlopeCosine;
        settings.gravity = m_config.gravity;
        settings.constrain_to_region = KF_TRUE;
        settings.region = {{-m_config.arenaHalfExtent, -KF_COORDINATE_LIMIT, -m_config.arenaHalfExtent},
            {m_config.arenaHalfExtent, KF_COORDINATE_LIMIT, m_config.arenaHalfExtent}};
        kf_character_move_result_t result{};
        kf_contact_t contacts[64];
        kf_status_t status = kf_character_move( m_collisionWorld.getWorld(), &player.character, &settings, nullptr, contacts, 64, &result );
        if( status != KF_OK )
        {
            MENGINE_THROW_EXCEPTION( "character movement failed: %u", status );
        }

        kf_shape_t playerShape = makeCapsuleShape( m_config.playerRadius, height );
        kf_pose_t playerPose{player.character.position, {0, 0, 0}};
        for( const PlayerState & other : m_state.players )
        {
            if( other.active == false || other.alive == false || other.id == player.id )
            {
                continue;
            }
            const kf_fixed_t otherHeight = other.crouched ? m_config.crouchedHeight : m_config.standingHeight;
            kf_shape_t otherShape = makeCapsuleShape( m_config.playerRadius, otherHeight );
            kf_pose_t otherPose{other.character.position, {0, 0, 0}};
            kf_contact_t contact{};
            status = kf_shape_contact( &playerShape, playerPose, &otherShape, otherPose, &contact );
            isCollisionHit( status );
            if( status == KF_NO_HIT || contact.depth <= KF_GEOMETRY_TOLERANCE )
            {
                continue;
            }
            player.character = beforeMove;
            player.character.position = positionBeforeStep;
            player.character.velocity.x = 0;
            player.character.velocity.z = 0;
            break;
        }
        kf_fixed_t previousVertical = -result.landing_speed;
        if( result.landed == KF_TRUE && previousVertical < kf_fixed_from_int( -12 ) )
        {
            const int32_t fallDamage = kf_fixed_to_int( kf_fixed_sub( kf_fixed_abs( previousVertical ), kf_fixed_from_int( 12 ) ) ) * 5;
            if( fallDamage > 0 )
            {
                this->queueDamage_( player.id, 0, WeaponType::Nailgun, player.character.position, fallDamage, 0 );
            }
        }

        for( size_t index = 0; index != Arena3DGenerated::JumpPadCount; ++index )
        {
            const Arena3DGenerated::JumpPadRaw & pad = Arena3DGenerated::JumpPads[index];
            const kf_vec3_t padCenter{pad.position[0], pad.position[1], pad.position[2]};
            const kf_vec3_t padHalfSize{pad.halfSize[0], pad.halfSize[1], pad.halfSize[2]};
            const kf_vec3_t padDelta = kf_vec3_sub( player.character.position, padCenter );
            if( player.character.grounded == false || kf_fixed_abs( padDelta.x ) > padHalfSize.x || kf_fixed_abs( padDelta.z ) > padHalfSize.z || kf_fixed_abs( padDelta.y ) > kf_fixed_add( padHalfSize.y, m_config.stepHeight ) )
            {
                continue;
            }

            player.character.velocity = {pad.velocity[0], pad.velocity[1], pad.velocity[2]};
            player.character.grounded = false;
            this->emit_( EventType::JumpPad, pad.id, player.id, WeaponType::Nailgun, player.character.position, 0 );
            break;
        }

        playerPose.position = player.character.position;
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
            kf_shape_t volume{};
            volume.type = KF_SHAPE_AABB;
            volume.data.aabb = {minimum, maximum};
            kf_pose_t identity{};
            kf_contact_t contact{};
            status = kf_shape_contact( &playerShape, playerPose, &volume, identity, &contact );
            isCollisionHit( status );
            if( status == KF_NO_HIT || contact.depth <= 0 )
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
            this->emit_( EventType::PlayerRespawned, player.id, player.id, WeaponType::Nailgun, player.character.position, 0 );
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
                if( candidate.active == false || candidate.alive == false || kf_vec3_length_squared( kf_vec3_sub( candidate.character.position, pickup.position ) ) > kf_fixed_mul( kf_fixed_from_ratio( 9, 10 ), kf_fixed_from_ratio( 9, 10 ) ) )
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
    void Simulation::emit_( EventType _type, uint32_t _actorId, uint32_t _targetId, WeaponType _weapon, const kf_vec3_t & _position, int32_t _amount, const kf_vec3_t & _endPosition )
    {
        ServerEvent event;
        event.id = (m_state.tick << 32) | m_eventOrdinal++;
        event.tick = m_state.tick;
        event.type = _type;
        event.actorId = _actorId;
        event.targetId = _targetId;
        event.weapon = _weapon;
        event.position = _position;
        event.amount = _amount;
        event.endPosition = _endPosition;
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
