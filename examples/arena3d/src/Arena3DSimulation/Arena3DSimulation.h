#pragma once

#include "Arena3DProtocol.h"

#include "Kernel/Array.h"
#include "Kernel/Data.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "Config/StdInt.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    constexpr size_t MaximumPlayers = 16;
    constexpr size_t MaximumProjectiles = 256;
    constexpr size_t MaximumTurrets = 10;
    constexpr size_t MaximumPickups = 12;
    constexpr size_t MaximumEventsPerTick = 512;
    //////////////////////////////////////////////////////////////////////////
    enum class EventType : uint8_t
    {
        Shot,
        Hit,
        Damage,
        Explosion,
        TurretDestroyed,
        TurretRespawned,
        PlayerDeath,
        PlayerRespawned,
        JumpPad,
        TurretWarning,
        Pickup,
        CommandRejected,
        Impact
    };
    //////////////////////////////////////////////////////////////////////////
    enum class ImpactNormal : int32_t
    {
        NegativeX,
        PositiveX,
        NegativeY,
        PositiveY,
        NegativeZ,
        PositiveZ,
        Unknown
    };
    //////////////////////////////////////////////////////////////////////////
    enum class TurretState : uint8_t
    {
        Idle,
        Searching,
        Tracking,
        Charging,
        Firing,
        Cooldown,
        Destroyed,
        Respawning
    };
    //////////////////////////////////////////////////////////////////////////
    enum class PickupType : uint8_t
    {
        Health,
        Armor,
        Ammo
    };
    //////////////////////////////////////////////////////////////////////////
    struct ServerEvent
    {
        uint64_t id = 0;
        uint64_t tick = 0;
        EventType type = EventType::Shot;
        uint32_t actorId = 0;
        uint32_t targetId = 0;
        WeaponType weapon = WeaponType::Nailgun;
        kf_vec3_t position;
        int32_t amount = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Mengine::Vector<ServerEvent> VectorServerEvent;
    //////////////////////////////////////////////////////////////////////////
    struct WeaponConfig
    {
        int32_t damage = 0;
        uint32_t cooldownTicks = 0;
        kf_fixed_t speed;
        kf_fixed_t launchUpwardSpeed;
        kf_fixed_t gravity;
        uint32_t lifetimeTicks = 0;
        uint32_t chargeTicks = 0;
        uint32_t pelletCount = 1;
        kf_fixed_t splashRadius;
        kf_fixed_t knockback;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Mengine::Array<WeaponConfig, WeaponTypeCount> ArrayWeaponConfig;
    typedef Mengine::Array<uint32_t, WeaponTypeCount> ArrayWeaponCounter;
    //////////////////////////////////////////////////////////////////////////
    struct SimulationConfig
    {
        kf_fixed_t groundAcceleration;
        kf_fixed_t airAcceleration;
        kf_fixed_t friction;
        kf_fixed_t gravity;
        kf_fixed_t jumpImpulse;
        kf_fixed_t standingHeight;
        kf_fixed_t crouchedHeight;
        kf_fixed_t playerRadius;
        kf_fixed_t stepHeight;
        kf_fixed_t maximumSlopeCosine;
        kf_fixed_t crouchedSpeedScale;
        kf_fixed_t slideMinimumSpeed;
        kf_fixed_t slideFriction;
        kf_fixed_t slideAccelerationScale;
        kf_fixed_t slideMaximumSpeed;
        kf_fixed_t arenaHalfExtent;
        kf_sangle16_t maximumPitch = 0;
        bool turretCombat = false;
        ArrayWeaponConfig weapons;

        static SimulationConfig defaults();
    };
    //////////////////////////////////////////////////////////////////////////
    struct PlayerState
    {
        bool active = false;
        uint32_t id = 0;
        kf_vec3_t position;
        kf_vec3_t velocity;
        kf_angle16_t yaw = 0;
        kf_sangle16_t pitch = 0;
        bool grounded = true;
        bool crouched = false;
        bool alive = true;
        bool noclip = false;
        bool fireHeld = false;
        bool fireConsumed = false;
        uint8_t inputMask = 0;
        int32_t health = 100;
        int32_t armor = 0;
        int32_t score = 0;
        int32_t deaths = 0;
        uint32_t respawnTicks = 0;
        WeaponType selectedWeapon = WeaponType::Nailgun;
        ArrayWeaponCounter ammo{};
        ArrayWeaponCounter cooldown{};
        kf_fixed_t nailHeat;
        uint32_t railChargeTicks = 0;
        uint64_t shotSequence = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    struct ProjectileState
    {
        bool active = false;
        uint32_t id = 0;
        uint32_t ownerId = 0;
        WeaponType weapon = WeaponType::Nailgun;
        kf_vec3_t position;
        kf_vec3_t velocity;
        kf_fixed_t radius;
        kf_fixed_t gravity;
        kf_fixed_t traveledDistance;
        uint32_t lifetimeTicks = 0;
        uint32_t launchDelayTicks = 0;
        int32_t damage = 0;
        uint64_t shotId = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    struct TurretStateData
    {
        uint32_t id = 0;
        WeaponType weapon = WeaponType::Nailgun;
        kf_vec3_t position;
        kf_angle16_t yaw = 0;
        kf_sangle16_t pitch = 0;
        TurretState state = TurretState::Idle;
        int32_t health = 100;
        uint32_t cooldownTicks = 0;
        uint32_t chargeTicks = 0;
        uint32_t targetMemoryTicks = 0;
        uint32_t respawnTicks = 0;
        Pcg32 aimRandom;
    };
    //////////////////////////////////////////////////////////////////////////
    struct PickupStateData
    {
        uint32_t id = 0;
        PickupType type = PickupType::Health;
        WeaponType weapon = WeaponType::Nailgun;
        kf_vec3_t position;
        int32_t amount = 0;
        uint32_t respawnTicks = 0;
        bool active = true;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Mengine::Array<PlayerState, MaximumPlayers> ArrayPlayerState;
    typedef Mengine::Array<ProjectileState, MaximumProjectiles> ArrayProjectileState;
    typedef Mengine::Array<TurretStateData, MaximumTurrets> ArrayTurretStateData;
    typedef Mengine::Array<PickupStateData, MaximumPickups> ArrayPickupStateData;
    typedef Mengine::Array<uint32_t, MaximumPlayers> ArrayPlayerId;
    //////////////////////////////////////////////////////////////////////////
    struct SimulationState
    {
        uint64_t matchId = 0;
        uint64_t tick = 0;
        uint64_t seed = 0;
        uint32_t nextPlayerId = 1;
        uint32_t nextObjectId = 1000;
        bool paused = false;
        ArrayPlayerState players;
        ArrayProjectileState projectiles;
        ArrayTurretStateData turrets;
        ArrayPickupStateData pickups;
        Pcg32 weaponRandom;
        Pcg32 pickupRandom;
    };
    //////////////////////////////////////////////////////////////////////////
    struct StateChecksum
    {
        uint32_t player = 0;
        uint32_t projectiles = 0;
        uint32_t turrets = 0;
        uint32_t pickups = 0;
        uint32_t random = 0;
        uint32_t full = 0;

        bool operator==( const StateChecksum & other ) const;
        bool operator!=( const StateChecksum & other ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    class Simulation
    {
    public:
        explicit Simulation( SimulationConfig config = SimulationConfig::defaults() );

        void reset( uint64_t matchId, uint64_t seed );
        uint32_t addPlayer( uint32_t playerId = 0 );
        bool removePlayer( uint32_t playerId );
        PlayerState * findPlayer( uint32_t playerId );
        const PlayerState * findPlayer( uint32_t playerId ) const;
        void step( const VectorCommandEnvelope & commands );

        const SimulationState & state() const;
        const VectorServerEvent & events() const;
        const SimulationConfig & config() const;

        StateChecksum checksum() const;
        Mengine::Data snapshot() const;
        bool restoreSnapshot( const Mengine::Data & snapshot );

    private:
        //////////////////////////////////////////////////////////////////////////
        struct DamageRequest
        {
            uint32_t targetId;
            uint32_t sourceId;
            uint64_t order;
            WeaponType weapon;
            kf_vec3_t origin;
            int32_t damage;
            kf_fixed_t knockback;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef Mengine::Array<DamageRequest, MaximumEventsPerTick> ArrayDamageRequest;

        void initializeTurrets_();
        void initializePickups_();
        void spawnPlayer_( PlayerState * player );
        void applyCommands_( const VectorCommandEnvelope & commands );
        void updatePlayer_( PlayerState * player );
        void updateWeapons_();
        void updateTurrets_();
        void updateProjectiles_();
        void applyDamage_();
        void updateRespawns_();
        void updatePickups_();
        void finalizeEvents_();
        void firePlayerWeapon_( PlayerState * player, WeaponType weapon );
        void fireTurretWeapon_( TurretStateData * turret, const PlayerState & targetPlayer );
        void fireRail_( uint32_t ownerId, const kf_vec3_t & origin, const kf_vec3_t & direction, int32_t damage );
        bool spawnProjectile_( uint32_t ownerId, WeaponType weapon, const kf_vec3_t & origin, const kf_vec3_t & direction, uint64_t shotId, uint32_t launchDelayTicks );
        kf_vec3_t viewDirection_( const PlayerState & player ) const;
        kf_vec3_t spreadDirection_( const kf_vec3_t & direction, kf_fixed_t spread, uint64_t shotId ) const;
        void queueDamage_( uint32_t targetId, uint32_t sourceId, WeaponType weapon, const kf_vec3_t & origin, int32_t damage, kf_fixed_t knockback );
        void queueExplosion_( const ProjectileState & projectile, uint32_t ignoredTargetId = 0 );
        void emit_( EventType type, uint32_t actorId, uint32_t targetId, WeaponType weapon, const kf_vec3_t & position, int32_t amount );
        void serializePlayers_( CanonicalWriter * writer ) const;
        void serializeProjectiles_( CanonicalWriter * writer ) const;
        void serializeTurrets_( CanonicalWriter * writer ) const;
        void serializePickups_( CanonicalWriter * writer ) const;
        void serializeRandom_( CanonicalWriter * writer ) const;
        void serializeState_( CanonicalWriter * writer ) const;
        bool deserializeState_( CanonicalReader * reader );

    private:
        SimulationConfig m_config;
        SimulationState m_state;
        VectorServerEvent m_events;
        ArrayDamageRequest m_damage;
        size_t m_damageCount = 0;
        uint32_t m_eventOrdinal = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    Mengine::String describeFirstDifference( const SimulationState & expected, const SimulationState & actual );
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
