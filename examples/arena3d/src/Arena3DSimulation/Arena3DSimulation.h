#pragma once

#include "Arena3DProtocol.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace Arena3D
{
    constexpr size_t MaximumProjectiles = 256;
    constexpr size_t MaximumTurrets = 10;
    constexpr size_t MaximumPickups = 12;
    constexpr size_t MaximumEventsPerTick = 512;

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
        CommandRejected
    };

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

    enum class PickupType : uint8_t
    {
        Health,
        Armor,
        Ammo
    };

    struct ServerEvent
    {
        uint64_t id = 0;
        uint64_t tick = 0;
        EventType type = EventType::Shot;
        uint32_t actorId = 0;
        uint32_t targetId = 0;
        WeaponType weapon = WeaponType::Nailgun;
        FixedVec3 position;
        int32_t amount = 0;
    };

    struct WeaponConfig
    {
        int32_t damage = 0;
        uint32_t cooldownTicks = 0;
        Fixed speed;
        Fixed gravity;
        uint32_t lifetimeTicks = 0;
        uint32_t chargeTicks = 0;
        uint32_t pelletCount = 1;
        Fixed splashRadius;
        Fixed knockback;
    };

    struct SimulationConfig
    {
        Fixed groundAcceleration;
        Fixed airAcceleration;
        Fixed friction;
        Fixed gravity;
        Fixed jumpImpulse;
        Fixed standingHeight;
        Fixed crouchedHeight;
        Fixed playerRadius;
        Fixed stepHeight;
        Fixed maximumSlopeCosine;
        Fixed crouchedSpeedScale;
        Fixed slideMinimumSpeed;
        Fixed slideFriction;
        Fixed slideAccelerationScale;
        Fixed slideMaximumSpeed;
        Fixed arenaHalfExtent;
        FixedSignedAngle maximumPitch = 0;
        std::array<WeaponConfig, static_cast<size_t>(WeaponType::Count)> weapons;

        static SimulationConfig defaults();
    };

    struct PlayerState
    {
        uint32_t id = 1;
        FixedVec3 position;
        FixedVec3 velocity;
        FixedAngle yaw = 0;
        FixedSignedAngle pitch = 0;
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
        uint32_t respawnTicks = 0;
        WeaponType selectedWeapon = WeaponType::Nailgun;
        std::array<uint32_t, static_cast<size_t>(WeaponType::Count)> ammo{};
        std::array<uint32_t, static_cast<size_t>(WeaponType::Count)> cooldown{};
        Fixed nailHeat;
        uint32_t railChargeTicks = 0;
        uint64_t shotSequence = 0;
    };

    struct ProjectileState
    {
        bool active = false;
        uint32_t id = 0;
        uint32_t ownerId = 0;
        WeaponType weapon = WeaponType::Nailgun;
        FixedVec3 position;
        FixedVec3 velocity;
        Fixed radius;
        Fixed gravity;
        Fixed traveledDistance;
        uint32_t lifetimeTicks = 0;
        int32_t damage = 0;
        uint64_t shotId = 0;
    };

    struct TurretStateData
    {
        uint32_t id = 0;
        WeaponType weapon = WeaponType::Nailgun;
        FixedVec3 position;
        FixedAngle yaw = 0;
        FixedSignedAngle pitch = 0;
        TurretState state = TurretState::Idle;
        int32_t health = 100;
        uint32_t cooldownTicks = 0;
        uint32_t chargeTicks = 0;
        uint32_t targetMemoryTicks = 0;
        uint32_t respawnTicks = 0;
        Pcg32 aimRandom;
    };

    struct PickupStateData
    {
        uint32_t id = 0;
        PickupType type = PickupType::Health;
        WeaponType weapon = WeaponType::Nailgun;
        FixedVec3 position;
        int32_t amount = 0;
        uint32_t respawnTicks = 0;
        bool active = true;
    };

    struct SimulationState
    {
        uint64_t matchId = 0;
        uint64_t tick = 0;
        uint64_t seed = 0;
        uint32_t nextObjectId = 1000;
        bool paused = false;
        FaultState fault;
        PlayerState player;
        std::array<ProjectileState, MaximumProjectiles> projectiles;
        std::array<TurretStateData, MaximumTurrets> turrets;
        std::array<PickupStateData, MaximumPickups> pickups;
        Pcg32 weaponRandom;
        Pcg32 pickupRandom;
    };

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

    class Simulation
    {
    public:
        explicit Simulation( SimulationConfig config = SimulationConfig::defaults() );

        void reset( uint64_t matchId, uint64_t seed );
        bool step( const std::vector<CommandEnvelope> & commands );

        const SimulationState & state() const;
        const std::vector<ServerEvent> & events() const;
        const SimulationConfig & config() const;

        StateChecksum checksum() const;
        std::vector<uint8_t> snapshot() const;
        bool restoreSnapshot( const std::vector<uint8_t> & snapshot );

        void injectFaultForTest( int64_t rawDelta );

    private:
        struct DamageRequest
        {
            uint32_t targetId;
            uint32_t sourceId;
            uint64_t order;
            WeaponType weapon;
            FixedVec3 origin;
            int32_t damage;
            Fixed knockback;
        };

        void initializeTurrets_();
        void initializePickups_();
        void applyCommands_( const std::vector<CommandEnvelope> & commands );
        void updatePlayer_();
        void updateWeapons_();
        void updateTurrets_();
        void updateProjectiles_();
        void applyDamage_();
        void updateRespawns_();
        void updatePickups_();
        void finalizeEvents_();
        void firePlayerWeapon_( WeaponType weapon );
        void fireTurretWeapon_( TurretStateData * turret );
        void fireRail_( uint32_t ownerId, const FixedVec3 & origin, const FixedVec3 & direction, int32_t damage );
        bool spawnProjectile_( uint32_t ownerId, WeaponType weapon, const FixedVec3 & origin, const FixedVec3 & direction, uint64_t shotId );
        FixedVec3 viewDirection_() const;
        FixedVec3 spreadDirection_( const FixedVec3 & direction, Fixed spread, uint64_t shotId ) const;
        void queueDamage_( uint32_t targetId, uint32_t sourceId, WeaponType weapon, const FixedVec3 & origin, int32_t damage, Fixed knockback );
        void queueExplosion_( const ProjectileState & projectile );
        void emit_( EventType type, uint32_t actorId, uint32_t targetId, WeaponType weapon, const FixedVec3 & position, int32_t amount );
        void serializePlayer_( CanonicalWriter * writer ) const;
        void serializeProjectiles_( CanonicalWriter * writer ) const;
        void serializeTurrets_( CanonicalWriter * writer ) const;
        void serializePickups_( CanonicalWriter * writer ) const;
        void serializeRandom_( CanonicalWriter * writer ) const;
        void serializeState_( CanonicalWriter * writer ) const;
        bool deserializeState_( CanonicalReader * reader );

    private:
        SimulationConfig m_config;
        SimulationState m_state;
        std::vector<ServerEvent> m_events;
        std::array<DamageRequest, MaximumEventsPerTick> m_damage;
        size_t m_damageCount = 0;
        uint32_t m_eventOrdinal = 0;
    };

    std::string describeFirstDifference( const SimulationState & expected, const SimulationState & actual );
}
