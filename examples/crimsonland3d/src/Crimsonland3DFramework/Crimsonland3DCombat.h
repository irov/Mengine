#pragma once

#include "Crimsonland3DDungeonGenerator.h"

#include "Kernel/Vector.h"

#include "math/vec2.h"
#include "math/vec3.h"

#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // Auto-shooter combat system (Crimsonland / Vampire Survivors style) for the
    // procedural 3D dungeon. The whole simulation runs on the XZ plane using
    // mt::vec2f; the owning scene maps the result onto pooled 3D meshes.
    //
    // Box2D is disabled in this example build, so a lightweight self-contained
    // circle-physics layer (separation + radial knockback impulses) is used for
    // enemy crowding and death explosions.
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Weapons always fire on their own; only the aiming policy differs.
    //////////////////////////////////////////////////////////////////////////
    enum class CombatTargeting
    {
        Nearest,        // closest enemy
        Hardest,        // enemy with the most current HP ("toughest")
        Directional,    // fixed fan of directions around the hero (radial spread)
        Random,         // random alive enemy
        Orbit,          // projectiles orbit the hero (melee aura)
        Forward         // hero facing / last movement direction
    };
    //////////////////////////////////////////////////////////////////////////
    struct CombatWeaponDef
    {
        const char * name;
        CombatTargeting targeting;
        float fireInterval;        // seconds between shots
        float damage;
        float projectileSpeed;
        uint32_t projectileCount;  // pellets per shot
        float spread;              // total fan angle (radians)
        float range;               // acquisition range
        uint32_t pierce;           // extra enemies a projectile can pass through
        float areaRadius;          // splash radius on hit (0 = single target)
        float dotDps;              // damage-over-time per second applied on hit
        float dotDuration;         // seconds the DoT lingers (fire/poison)
        float projectileRadius;
        float projectileLife;      // seconds before despawn
        uint32_t color;            // packed RGBA8
    };
    //////////////////////////////////////////////////////////////////////////
    // Enemy behaviour is a bitmask so a single type can mix traits.
    //////////////////////////////////////////////////////////////////////////
    enum CombatEnemyBehavior : uint32_t
    {
        EB_CHASER   = 1u << 0,     // walks straight at the hero
        EB_SHOOTER  = 1u << 1,     // fires projectiles at the hero
        EB_CHARGER  = 1u << 2,     // accelerates when the hero gets close
        EB_EXPLODER = 1u << 3,     // explodes on death (knockback + AoE damage)
        EB_SPLITTER = 1u << 4      // keeps up to N living children near the hero
    };
    //////////////////////////////////////////////////////////////////////////
    struct CombatEnemyDef
    {
        const char * name;
        float maxHp;
        float speed;
        float touchDamage;         // damage per second while overlapping the hero
        float radius;
        uint32_t xpReward;         // 10 / 25 / 50 / 100 -> XP cube tier & colour
        uint32_t behavior;         // CombatEnemyBehavior flags
        uint32_t color;            // packed RGBA8
        float drawScale;           // mesh scale
        // Shooter
        float shootInterval;
        float shootDamage;
        float shootSpeed;
        // Charger
        float chargeRange;
        float chargeMul;
        // Exploder
        float explodeDamage;
        float explodeRadius;
        float explodeImpulse;
        // Splitter
        int32_t childDefIndex;     // -1 = none
        uint32_t maxChildren;
    };
    //////////////////////////////////////////////////////////////////////////
    enum class CombatUpgradeKind
    {
        WeaponDamage,
        WeaponAttackSpeed,
        WeaponArea,
        WeaponDuration,
        WeaponProjectiles,
        NewWeapon,
        Regen,
        MaxHp,
        Armor,
        Dodge,
        XpGain,
        MoveSpeed,
        PickupRadius,
        Luck                       // raises rarity odds of future upgrades
    };
    //////////////////////////////////////////////////////////////////////////
    // Each upgrade rolls a rarity tier. Higher tiers scale the base magnitude
    // up (rarityMagnitudeMul) and tint the level-up card a brighter colour.
    // The hero "luck" stat biases the roll toward the upper tiers.
    //////////////////////////////////////////////////////////////////////////
    enum class CombatUpgradeRarity
    {
        Common,
        Uncommon,
        Rare,
        Epic,
        Legendary,
        Count
    };
    //////////////////////////////////////////////////////////////////////////
    const char * rarityName( CombatUpgradeRarity _rarity );
    float rarityMagnitudeMul( CombatUpgradeRarity _rarity );
    uint32_t rarityColor( CombatUpgradeRarity _rarity );
    //////////////////////////////////////////////////////////////////////////
    struct CombatUpgradeOption
    {
        CombatUpgradeKind kind;
        CombatUpgradeRarity rarity;
        int32_t weaponIndex;       // weapon slot (upgrades) or weapon def (NewWeapon)
        float magnitude;
        uint32_t color;            // card tint
        char label[96];
    };
    //////////////////////////////////////////////////////////////////////////
    struct CombatWeapon
    {
        uint32_t defIndex;
        float cooldown;
        uint32_t level;
        float damageMul;
        float fireRateMul;         // >1 = faster
        float areaMul;
        float durationMul;
        int32_t extraProjectiles;
        float orbitPhase;          // for Orbit weapons
    };
    //////////////////////////////////////////////////////////////////////////
    struct CombatEnemy
    {
        uint32_t defIndex;
        mt::vec2f pos;
        mt::vec2f vel;
        float hp;
        float shootCd;
        float dotRemain;
        float dotDps;
        int32_t parent;            // splitter parent index, or -1
        uint32_t children;
        bool charging;
        bool alive;
        bool dying;                // death animation in progress (reddening + sinking)
        float deathTime;           // seconds elapsed since death
    };
    //////////////////////////////////////////////////////////////////////////
    // How long the death animation runs: the corpse stops, turns red and slowly
    // sinks through the floor before it is removed from the simulation.
    static const float COMBAT_ENEMY_DEATH_DURATION = 1.4f;
    //////////////////////////////////////////////////////////////////////////
    struct CombatProjectile
    {
        mt::vec2f pos;
        mt::vec2f vel;
        float damage;
        float life;
        float radius;
        uint32_t pierceLeft;
        float areaRadius;
        float dotDps;
        float dotDuration;
        float orbitAngle;          // Orbit projectiles only
        float orbitRadius;
        uint32_t color;
        int32_t ownerWeapon;       // -1 = enemy projectile
        bool fromHero;
        bool alive;
    };
    //////////////////////////////////////////////////////////////////////////
    struct CombatXpCube
    {
        mt::vec2f pos;
        mt::vec2f vel;
        uint32_t value;            // 10 / 25 / 50 / 100
        uint32_t color;
        bool alive;
    };
    //////////////////////////////////////////////////////////////////////////
    struct CombatHeroStats
    {
        float maxHp = 100.f;
        float hp = 100.f;
        float armor = 0.f;         // flat damage reduction
        float dodge = 0.f;         // 0..1 chance to ignore a hit
        float regen = 0.f;         // hp per second
        float moveSpeedMul = 1.f;
        float xpGainMul = 0.2f;     // base XP gain is intentionally slow (5x slower)
        float pickupRadius = 160.f;
        float damageMul = 1.f;     // global weapon damage multiplier
        float luck = 0.f;          // 0..1 bias toward higher upgrade rarities
        uint32_t level = 1;
        uint32_t xp = 0;
        uint32_t xpToNext = 10;
    };
    //////////////////////////////////////////////////////////////////////////
    class Crimsonland3DCombat
    {
    public:
        Crimsonland3DCombat();
        ~Crimsonland3DCombat();

    public:
        // dungeon is borrowed (walkability + floor height); not owned.
        void initialize( const Crimsonland3DDungeonBuildResult * _dungeon, const mt::vec2f & _heroXZ, uint32_t _seed );
        void finalize();

    public:
        // Advance the simulation. _heroMove is the hero's intended move dir (for
        // Forward weapons). Returns nothing; query state via the getters.
        void update( float _dt, const mt::vec2f & _heroXZ, const mt::vec2f & _heroMove );

        // True while a level-up choice is pending (caller should pause + darken).
        bool isLevelUpPending() const;
        const CombatUpgradeOption * getUpgradeOptions( uint32_t * _count ) const;
        void applyUpgrade( uint32_t _optionIndex );

    public:
        const CombatHeroStats & getHeroStats() const;
        bool isHeroDead() const;

        const Vector<CombatEnemy> & getEnemies() const;
        const Vector<CombatProjectile> & getProjectiles() const;
        const Vector<CombatXpCube> & getXpCubes() const;
        const Vector<CombatWeapon> & getWeapons() const;

        const CombatEnemyDef & getEnemyDef( uint32_t _defIndex ) const;

        static uint32_t getEnemyDefCount();
        static uint32_t getWeaponDefCount();
        static const CombatWeaponDef & getWeaponDef( uint32_t _defIndex );

    protected:
        uint32_t random_( uint32_t _range );      // [0, range)
        float randomf_();                          // [0,1)
        float randomRange_( float _min, float _max );
        mt::vec2f randomDir_();

        void spawnWave_( const mt::vec2f & _heroXZ );
        void spawnEnemy_( uint32_t _defIndex, const mt::vec2f & _pos, int32_t _parent );
        void killEnemy_( uint32_t _index, const mt::vec2f & _heroXZ );

        void updateEnemies_( float _dt, const mt::vec2f & _heroXZ );
        void updateWeapons_( float _dt, const mt::vec2f & _heroXZ, const mt::vec2f & _heroMove );
        // Flow-field pathfinding: a BFS from the hero's cell over walkable cells,
        // giving every cell a direction that routes around walls toward the hero.
        void rebuildFlowField_( const mt::vec2f & _heroXZ );
        mt::vec2f sampleFlowDir_( const mt::vec2f & _pos ) const;
        bool enemyWalkable_( float _x, float _z, float _margin ) const;
        bool anyEnemyInRange_( float _range, const mt::vec2f & _heroXZ ) const;
        bool anyEnemyInRangeVisible_( float _range, const mt::vec2f & _heroXZ ) const;
        bool hasLineOfSight_( const mt::vec2f & _from, const mt::vec2f & _to ) const;
        void fireWeapon_( CombatWeapon & _weapon, const mt::vec2f & _heroXZ, const mt::vec2f & _heroMove );
        void updateProjectiles_( float _dt, const mt::vec2f & _heroXZ );
        void updateXpCubes_( float _dt, const mt::vec2f & _heroXZ );
        void resolvePhysics_( float _dt );

        int32_t acquireTarget_( const CombatWeaponDef & _def, const mt::vec2f & _heroXZ );
        void emitProjectile_( const mt::vec2f & _pos, const mt::vec2f & _dir, const CombatWeaponDef & _def
            , const CombatWeapon & _weapon, int32_t _ownerWeapon, bool _fromHero, float _damage );
        void applyExplosion_( const mt::vec2f & _center, float _radius, float _damage, float _impulse, const mt::vec2f & _heroXZ );
        void damageHero_( float _damage );
        void grantXp_( uint32_t _value );
        CombatUpgradeRarity rollRarity_();
        void buildUpgradeOptions_();
        void addWeapon_( uint32_t _defIndex );
        bool hasWeapon_( uint32_t _defIndex ) const;
        void dropXp_( const mt::vec2f & _pos, uint32_t _value );

    protected:
        const Crimsonland3DDungeonBuildResult * m_dungeon;

        uint32_t m_rngState;

        CombatHeroStats m_hero;
        bool m_heroDead;

        Vector<CombatWeapon> m_weapons;
        Vector<CombatEnemy> m_enemies;
        Vector<CombatProjectile> m_projectiles;
        Vector<CombatXpCube> m_xpCubes;

        float m_spawnTimer;
        float m_spawnInterval;
        float m_elapsed;            // total survival time (drives difficulty)
        float m_regenAccum;
        mt::vec2f m_heroFacing;     // last non-zero move dir (Forward weapons)

        bool m_levelUpPending;
        Vector<CombatUpgradeOption> m_upgradeOptions;

        // Flow-field pathfinding state (rebuilt when the hero changes cell).
        Vector<mt::vec2f> m_flowDir;   // per-cell direction toward the hero
        Vector<uint16_t> m_flowDist;   // BFS distance buffer (0xFFFF = unreachable)
        Vector<uint32_t> m_flowQueue;  // reused BFS queue buffer
        int32_t m_flowHeroCellX;
        int32_t m_flowHeroCellZ;
        float m_flowTimer;
    };
    //////////////////////////////////////////////////////////////////////////
}