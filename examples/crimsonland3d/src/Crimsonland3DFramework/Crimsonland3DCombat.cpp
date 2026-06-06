#include "Crimsonland3DCombat.h"

#include "Kernel/Logger.h"

#include "Config/StdMath.h"
#include "Config/StdString.h"
#include "Config/StdIO.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static uint32_t combatPackRGBA8( float _r, float _g, float _b, float _a )
        {
            auto pack = []( float _v ) -> uint32_t
            {
                if( _v < 0.f ) _v = 0.f;
                else if( _v > 1.f ) _v = 1.f;

                return (uint32_t)(_v * 255.f + 0.5f);
            };

            return pack( _r ) | (pack( _g ) << 8) | (pack( _b ) << 16) | (pack( _a ) << 24);
        }
        //////////////////////////////////////////////////////////////////////////
        // Ten distinct weapons. They all fire automatically; only the aiming
        // policy and payload differ.
        //////////////////////////////////////////////////////////////////////////
        static const CombatWeaponDef g_weaponDefs[] = {
            // name          targeting                    interval dmg  pspd  cnt spread     range  pierce area  dotDps dotDur pr   life  color
            { "Pistol",       CombatTargeting::Nearest,    0.55f,  12.f, 900.f, 1u, 0.f,      1300.f, 0u,  0.f,   0.f,  0.f,   10.f, 1.8f, combatPackRGBA8( 1.f, 0.9f, 0.4f, 1.f ) },
            { "Shotgun",      CombatTargeting::Forward,    0.95f,  9.f,  820.f, 6u, 0.62f,    800.f,  0u,  0.f,   0.f,  0.f,   9.f,  0.6f, combatPackRGBA8( 1.f, 0.7f, 0.3f, 1.f ) },
            { "SMG",          CombatTargeting::Nearest,    0.16f,  5.f,  1050.f,1u, 0.10f,    1150.f, 0u,  0.f,   0.f,  0.f,   7.f,  1.4f, combatPackRGBA8( 0.7f, 1.f, 0.9f, 1.f ) },
            { "Sniper",       CombatTargeting::Hardest,    1.35f,  60.f, 1700.f,1u, 0.f,      2400.f, 4u,  0.f,   0.f,  0.f,   8.f,  2.2f, combatPackRGBA8( 0.6f, 0.8f, 1.f, 1.f ) },
            { "Flamethrower", CombatTargeting::Forward,    0.10f,  3.f,  600.f, 3u, 0.42f,    700.f,  0u,  40.f,  6.f,  2.0f,  12.f, 0.55f,combatPackRGBA8( 1.f, 0.45f, 0.15f, 1.f ) },
            { "Poison Orb",   CombatTargeting::Nearest,    1.05f,  10.f, 620.f, 1u, 0.f,      1300.f, 1u,  150.f, 10.f, 3.0f,  16.f, 2.4f, combatPackRGBA8( 0.5f, 1.f, 0.3f, 1.f ) },
            { "Rockets",      CombatTargeting::Random,     1.10f,  28.f, 720.f, 1u, 0.f,      1500.f, 0u,  220.f, 0.f,  0.f,   13.f, 2.6f, combatPackRGBA8( 1.f, 0.55f, 0.2f, 1.f ) },
            { "Lightning",    CombatTargeting::Hardest,    0.80f,  22.f, 2200.f,1u, 0.f,      1700.f, 6u,  60.f,  0.f,  0.f,   7.f,  1.0f, combatPackRGBA8( 0.7f, 0.85f, 1.f, 1.f ) },
            { "Orbital Blades",CombatTargeting::Orbit,     2.40f,  16.f, 0.f,   3u, 0.f,      0.f,    9999u,0.f,   0.f,  0.f,   14.f, 2.45f,combatPackRGBA8( 0.85f, 0.9f, 1.f, 1.f ) },
            { "Spread Cannon",CombatTargeting::Directional,1.25f,  11.f, 760.f, 10u,0.f,      0.f,    0u,  0.f,   0.f,  0.f,   9.f,  1.8f, combatPackRGBA8( 1.f, 0.5f, 0.8f, 1.f ) },
        };
        static const uint32_t g_weaponCount = sizeof( g_weaponDefs ) / sizeof( g_weaponDefs[0] );
        //////////////////////////////////////////////////////////////////////////
        // Enemy roster. Behaviours are flag combinations.
        //////////////////////////////////////////////////////////////////////////
        enum EnemyDefId
        {
            ED_GRUNT = 0,
            ED_RUNNER,
            ED_SHOOTER,
            ED_BOMBER,
            ED_BRUTE,
            ED_SPLITTER,
            ED_SPAWNLING,
            ED_TANK
        };
        //////////////////////////////////////////////////////////////////////////
        static const CombatEnemyDef g_enemyDefs[] = {
            // name        hp     spd    touch  rad   xp   behavior                              color                                     scale shootInt sDmg sSpd chargeR chargeM expDmg expR  expImp child       maxCh
            { "Grunt",      22.f,  150.f, 16.f,  28.f, 10u, EB_CHASER,                            combatPackRGBA8( 0.65f, 0.35f, 0.30f, 1.f ),0.55f, 0.f,   0.f, 0.f,  0.f,    1.f,   0.f,   0.f,  0.f,   -1,         0u },
            { "Runner",     14.f,  235.f, 12.f,  24.f, 10u, EB_CHASER | EB_CHARGER,               combatPackRGBA8( 0.85f, 0.55f, 0.20f, 1.f ),0.48f, 0.f,   0.f, 0.f,  520.f,  2.2f,  0.f,   0.f,  0.f,   -1,         0u },
            { "Spitter",    30.f,  120.f, 10.f,  30.f, 25u, EB_CHASER | EB_SHOOTER,               combatPackRGBA8( 0.55f, 0.35f, 0.75f, 1.f ),0.58f, 1.7f,  12.f,220.f, 0.f,    1.f,   0.f,   0.f,  0.f,   -1,         0u },
            { "Bomber",     26.f,  175.f, 0.f,   30.f, 25u, EB_CHASER | EB_CHARGER | EB_EXPLODER, combatPackRGBA8( 0.90f, 0.25f, 0.20f, 1.f ),0.60f, 0.f,   0.f, 0.f,  360.f,  1.8f,  35.f,  260.f,820.f, -1,         0u },
            { "Brute",      130.f, 95.f,  26.f,  46.f, 50u, EB_CHASER,                            combatPackRGBA8( 0.45f, 0.45f, 0.50f, 1.f ),0.95f, 0.f,   0.f, 0.f,  0.f,    1.f,   0.f,   0.f,  0.f,   -1,         0u },
            { "Splitter",   70.f,  130.f, 18.f,  38.f, 50u, EB_CHASER | EB_SPLITTER,              combatPackRGBA8( 0.30f, 0.70f, 0.40f, 1.f ),0.78f, 0.f,   0.f, 0.f,  0.f,    1.f,   0.f,   0.f,  0.f,   ED_SPAWNLING,3u },
            { "Spawnling",  9.f,   205.f, 8.f,   18.f, 10u, EB_CHASER,                            combatPackRGBA8( 0.40f, 0.85f, 0.55f, 1.f ),0.36f, 0.f,   0.f, 0.f,  0.f,    1.f,   0.f,   0.f,  0.f,   -1,         0u },
            { "Tank",       420.f, 80.f,  34.f,  60.f, 100u,EB_CHASER | EB_SHOOTER,               combatPackRGBA8( 0.55f, 0.20f, 0.25f, 1.f ),1.30f, 2.4f,  20.f,185.f, 0.f,    1.f,   0.f,   0.f,  0.f,   -1,         0u },
        };
        static const uint32_t g_enemyCount = sizeof( g_enemyDefs ) / sizeof( g_enemyDefs[0] );
        //////////////////////////////////////////////////////////////////////////
        // XP cube colours per tier value.
        //////////////////////////////////////////////////////////////////////////
        static uint32_t xpColorForValue( uint32_t _value )
        {
            if( _value >= 100u ) return combatPackRGBA8( 1.f, 0.85f, 0.2f, 1.f );   // gold
            if( _value >= 50u )  return combatPackRGBA8( 0.8f, 0.4f, 1.f, 1.f );    // purple
            if( _value >= 25u )  return combatPackRGBA8( 0.3f, 1.f, 0.5f, 1.f );    // green
            return combatPackRGBA8( 0.4f, 0.7f, 1.f, 1.f );                          // blue
        }
        //////////////////////////////////////////////////////////////////////////
        static const float COMBAT_PI = 3.14159265358979323846f;
        static const float COMBAT_TWO_PI = 6.28318530717958647692f;
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Crimsonland3DCombat::Crimsonland3DCombat()
        : m_dungeon( nullptr )
        , m_rngState( 1u )
        , m_heroDead( false )
        , m_spawnTimer( 0.f )
        , m_spawnInterval( 1.6f )
        , m_elapsed( 0.f )
        , m_regenAccum( 0.f )
        , m_heroFacing( 0.f, 1.f )
        , m_levelUpPending( false )
        , m_flowHeroCellX( -1 )
        , m_flowHeroCellZ( -1 )
        , m_flowTimer( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Crimsonland3DCombat::~Crimsonland3DCombat()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::initialize( const Crimsonland3DDungeonBuildResult * _dungeon, const mt::vec2f & _heroXZ, uint32_t _seed )
    {
        m_dungeon = _dungeon;
        m_rngState = (_seed != 0u) ? _seed : 1u;

        m_hero = CombatHeroStats();
        m_heroDead = false;

        m_weapons.clear();
        m_enemies.clear();
        m_projectiles.clear();
        m_xpCubes.clear();

        m_spawnTimer = 0.f;
        m_spawnInterval = 1.6f;
        m_elapsed = 0.f;
        m_regenAccum = 0.f;
        m_heroFacing = mt::vec2f( 0.f, 1.f );
        m_levelUpPending = false;
        m_upgradeOptions.clear();

        // Reset the flow field so it is rebuilt on the first enemy update.
        m_flowDir.clear();
        m_flowDist.clear();
        m_flowQueue.clear();
        m_flowHeroCellX = -1;
        m_flowHeroCellZ = -1;
        m_flowTimer = 0.f;

        MENGINE_UNUSED( _heroXZ );

        // Start with the Pistol.
        this->addWeapon_( 0u );
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::finalize()
    {
        m_dungeon = nullptr;
        m_weapons.clear();
        m_enemies.clear();
        m_projectiles.clear();
        m_xpCubes.clear();
        m_upgradeOptions.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Crimsonland3DCombat::random_( uint32_t _range )
    {
        m_rngState = m_rngState * 1664525u + 1013904223u;

        if( _range == 0u )
        {
            return 0u;
        }

        return (m_rngState >> 8) % _range;
    }
    //////////////////////////////////////////////////////////////////////////
    float Crimsonland3DCombat::randomf_()
    {
        m_rngState = m_rngState * 1664525u + 1013904223u;

        return (float)((m_rngState >> 8) & 0x00FFFFFFu) / (float)0x01000000u;
    }
    //////////////////////////////////////////////////////////////////////////
    float Crimsonland3DCombat::randomRange_( float _min, float _max )
    {
        return _min + (_max - _min) * this->randomf_();
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Crimsonland3DCombat::randomDir_()
    {
        float a = this->randomf_() * Detail::COMBAT_TWO_PI;

        return mt::vec2f( StdMath::cosf( a ), StdMath::sinf( a ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::update( float _dt, const mt::vec2f & _heroXZ, const mt::vec2f & _heroMove )
    {
        if( m_heroDead == true || m_levelUpPending == true )
        {
            return;
        }

        m_elapsed += _dt;

        if( mt::sqrlength_v2( _heroMove ) > 1e-4f )
        {
            mt::norm_safe_v2( &m_heroFacing, _heroMove );
        }

        // Difficulty ramp: spawn faster over time, down to a floor.
        m_spawnInterval = 1.6f - m_elapsed * 0.006f;

        if( m_spawnInterval < 0.35f )
        {
            m_spawnInterval = 0.35f;
        }

        m_spawnTimer -= _dt;

        if( m_spawnTimer <= 0.f )
        {
            m_spawnTimer = m_spawnInterval;

            this->spawnWave_( _heroXZ );
        }

        this->updateEnemies_( _dt, _heroXZ );
        this->updateWeapons_( _dt, _heroXZ, _heroMove );
        this->updateProjectiles_( _dt, _heroXZ );
        this->resolvePhysics_( _dt );
        this->updateXpCubes_( _dt, _heroXZ );

        // Passive regeneration.
        if( m_hero.regen > 0.f && m_hero.hp < m_hero.maxHp )
        {
            m_hero.hp += m_hero.regen * _dt;

            if( m_hero.hp > m_hero.maxHp )
            {
                m_hero.hp = m_hero.maxHp;
            }
        }

        // Trigger a pending level-up once XP crosses the threshold.
        if( m_levelUpPending == false && m_hero.xp >= m_hero.xpToNext )
        {
            m_hero.xp -= m_hero.xpToNext;
            m_hero.level += 1u;
            m_hero.xpToNext = 10u + m_hero.level * 8u + (m_hero.level * m_hero.level) / 2u;

            this->buildUpgradeOptions_();

            m_levelUpPending = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::spawnWave_( const mt::vec2f & _heroXZ )
    {
        // Wave grows with elapsed time.
        uint32_t count = 3u + (uint32_t)(m_elapsed / 12.f);

        if( count > 14u )
        {
            count = 14u;
        }

        // Which enemy types are unlocked depends on survival time.
        for( uint32_t i = 0; i != count; ++i )
        {
            uint32_t roll = this->random_( 100u );
            uint32_t defIndex = Detail::ED_GRUNT;

            if( m_elapsed > 120.f && roll < 6u )       defIndex = Detail::ED_TANK;
            else if( m_elapsed > 75.f && roll < 18u )  defIndex = Detail::ED_SPLITTER;
            else if( m_elapsed > 55.f && roll < 30u )  defIndex = Detail::ED_BRUTE;
            else if( m_elapsed > 30.f && roll < 48u )  defIndex = Detail::ED_BOMBER;
            else if( m_elapsed > 15.f && roll < 66u )  defIndex = Detail::ED_SHOOTER;
            else if( roll < 84u )                      defIndex = Detail::ED_GRUNT;
            else                                       defIndex = Detail::ED_RUNNER;

            // Spawn on a ring well outside the hero's visible area.
            mt::vec2f pos;
            bool placed = false;

            for( uint32_t attempt = 0; attempt != 12u; ++attempt )
            {
                mt::vec2f dir = this->randomDir_();
                float dist = this->randomRange_( 1700.f, 2300.f );

                pos = _heroXZ + dir * dist;

                if( m_dungeon == nullptr || m_dungeon->isWalkableWorld( pos.x, pos.y ) == true )
                {
                    placed = true;
                    break;
                }
            }

            if( placed == false )
            {
                continue;
            }

            this->spawnEnemy_( defIndex, pos, -1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::spawnEnemy_( uint32_t _defIndex, const mt::vec2f & _pos, int32_t _parent )
    {
        if( _defIndex >= Detail::g_enemyCount )
        {
            return;
        }

        const CombatEnemyDef & def = Detail::g_enemyDefs[_defIndex];

        CombatEnemy enemy;
        enemy.defIndex = _defIndex;
        enemy.pos = _pos;
        enemy.vel = mt::vec2f( 0.f, 0.f );
        enemy.hp = def.maxHp;
        enemy.shootCd = def.shootInterval * (0.5f + this->randomf_());
        enemy.dotRemain = 0.f;
        enemy.dotDps = 0.f;
        enemy.parent = _parent;
        enemy.children = 0u;
        enemy.charging = false;
        enemy.alive = true;
        enemy.dying = false;
        enemy.deathTime = 0.f;

        m_enemies.push_back( enemy );

        if( _parent >= 0 && (uint32_t)_parent < m_enemies.size() )
        {
            m_enemies[_parent].children += 1u;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::rebuildFlowField_( const mt::vec2f & _heroXZ )
    {
        if( m_dungeon == nullptr || m_dungeon->gridWidth == 0u || m_dungeon->gridHeight == 0u )
        {
            return;
        }

        const uint32_t w = m_dungeon->gridWidth;
        const uint32_t h = m_dungeon->gridHeight;
        const uint32_t cellCount = w * h;

        // Hero cell.
        int32_t hx = (int32_t)((_heroXZ.x - m_dungeon->originX) / m_dungeon->cellSize);
        int32_t hz = (int32_t)((_heroXZ.y - m_dungeon->originZ) / m_dungeon->cellSize);

        if( hx < 0 || hz < 0 || hx >= (int32_t)w || hz >= (int32_t)h )
        {
            return;
        }

        // Only rebuild when the hero moves to a new cell (or the buffers are empty).
        if( hx == m_flowHeroCellX && hz == m_flowHeroCellZ && m_flowDir.size() == cellCount )
        {
            return;
        }

        m_flowHeroCellX = hx;
        m_flowHeroCellZ = hz;

        m_flowDir.assign( cellCount, mt::vec2f( 0.f, 0.f ) );
        m_flowDist.assign( cellCount, (uint16_t)0xFFFFu );

        const Vector<uint8_t> & cells = m_dungeon->cells;

        uint32_t heroCell = (uint32_t)hz * w + (uint32_t)hx;

        if( cells[heroCell] != 1u )
        {
            return;
        }

        // 4-neighbor BFS over walkable cells (fast, gives shortest step distance).
        m_flowQueue.clear();
        m_flowDist[heroCell] = 0u;
        m_flowQueue.push_back( heroCell );

        const int32_t nx4[4] = { 1, -1, 0, 0 };
        const int32_t nz4[4] = { 0, 0, 1, -1 };

        uint32_t head = 0u;

        while( head < m_flowQueue.size() )
        {
            uint32_t cur = m_flowQueue[head++];

            uint32_t cx = cur % w;
            uint32_t cz = cur / w;
            uint16_t curDist = m_flowDist[cur];

            for( uint32_t k = 0; k != 4u; ++k )
            {
                int32_t tx = (int32_t)cx + nx4[k];
                int32_t tz = (int32_t)cz + nz4[k];

                if( tx < 0 || tz < 0 || tx >= (int32_t)w || tz >= (int32_t)h )
                {
                    continue;
                }

                uint32_t ncell = (uint32_t)tz * w + (uint32_t)tx;

                if( cells[ncell] != 1u || m_flowDist[ncell] != (uint16_t)0xFFFFu )
                {
                    continue;
                }

                m_flowDist[ncell] = (uint16_t)(curDist + 1u);
                m_flowQueue.push_back( ncell );
            }
        }

        // Derive a per-cell direction: point toward the 8-neighbor with the
        // lowest BFS distance (one step closer to the hero). Diagonals are only
        // used when both orthogonal neighbors are open, to avoid corner cutting.
        const int32_t nx8[8] = { 1, -1, 0, 0, 1, 1, -1, -1 };
        const int32_t nz8[8] = { 0, 0, 1, -1, 1, -1, 1, -1 };

        for( uint32_t c = 0; c != cellCount; ++c )
        {
            if( m_flowDist[c] == (uint16_t)0xFFFFu )
            {
                continue;
            }

            uint32_t cx = c % w;
            uint32_t cz = c / w;
            uint16_t bestDist = m_flowDist[c];
            int32_t bestDx = 0;
            int32_t bestDz = 0;

            for( uint32_t k = 0; k != 8u; ++k )
            {
                int32_t tx = (int32_t)cx + nx8[k];
                int32_t tz = (int32_t)cz + nz8[k];

                if( tx < 0 || tz < 0 || tx >= (int32_t)w || tz >= (int32_t)h )
                {
                    continue;
                }

                // Block diagonal moves that would clip a wall corner.
                if( nx8[k] != 0 && nz8[k] != 0 )
                {
                    uint32_t oa = (uint32_t)cz * w + (uint32_t)tx;
                    uint32_t ob = (uint32_t)tz * w + (uint32_t)cx;

                    if( cells[oa] != 1u || cells[ob] != 1u )
                    {
                        continue;
                    }
                }

                uint32_t ncell = (uint32_t)tz * w + (uint32_t)tx;
                uint16_t nd = m_flowDist[ncell];

                if( nd < bestDist )
                {
                    bestDist = nd;
                    bestDx = nx8[k];
                    bestDz = nz8[k];
                }
            }

            if( bestDx != 0 || bestDz != 0 )
            {
                mt::vec2f d( (float)bestDx, (float)bestDz );
                mt::norm_safe_v2( &m_flowDir[c], d );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Crimsonland3DCombat::sampleFlowDir_( const mt::vec2f & _pos ) const
    {
        if( m_dungeon == nullptr || m_flowDir.empty() == true )
        {
            return mt::vec2f( 0.f, 0.f );
        }

        const uint32_t w = m_dungeon->gridWidth;
        const uint32_t h = m_dungeon->gridHeight;

        int32_t gx = (int32_t)((_pos.x - m_dungeon->originX) / m_dungeon->cellSize);
        int32_t gz = (int32_t)((_pos.y - m_dungeon->originZ) / m_dungeon->cellSize);

        if( gx < 0 || gz < 0 || gx >= (int32_t)w || gz >= (int32_t)h )
        {
            return mt::vec2f( 0.f, 0.f );
        }

        return m_flowDir[(uint32_t)gz * w + (uint32_t)gx];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DCombat::enemyWalkable_( float _x, float _z, float _margin ) const
    {
        if( m_dungeon == nullptr )
        {
            return true;
        }

        // Keep enemies a body-width off the cliffs: require the cell at the
        // target plus a small margin in the four directions to be walkable.
        if( m_dungeon->isWalkableWorld( _x, _z ) == false )
        {
            return false;
        }

        if( _margin <= 0.f )
        {
            return true;
        }

        if( m_dungeon->isWalkableWorld( _x + _margin, _z ) == false ||
            m_dungeon->isWalkableWorld( _x - _margin, _z ) == false ||
            m_dungeon->isWalkableWorld( _x, _z + _margin ) == false ||
            m_dungeon->isWalkableWorld( _x, _z - _margin ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::updateEnemies_( float _dt, const mt::vec2f & _heroXZ )
    {
        // Refresh the wall-aware flow field toward the hero (cheap BFS, only
        // rebuilds when the hero crosses into a new grid cell).
        this->rebuildFlowField_( _heroXZ );

        for( uint32_t i = 0; i != m_enemies.size(); ++i )
        {
            CombatEnemy & e = m_enemies[i];

            if( e.alive == false )
            {
                // Advance the death animation; once it finishes the corpse is
                // fully gone (no longer rendered).
                if( e.dying == true )
                {
                    e.deathTime += _dt;

                    if( e.deathTime >= COMBAT_ENEMY_DEATH_DURATION )
                    {
                        e.dying = false;
                    }
                }

                continue;
            }

            const CombatEnemyDef & def = Detail::g_enemyDefs[e.defIndex];

            // Damage-over-time (fire / poison).
            if( e.dotRemain > 0.f )
            {
                e.hp -= e.dotDps * _dt;
                e.dotRemain -= _dt;

                if( e.hp <= 0.f )
                {
                    this->killEnemy_( i, _heroXZ );
                    continue;
                }
            }

            mt::vec2f toHero = _heroXZ - e.pos;
            float dist = mt::length_v2( toHero );

            mt::vec2f dir( 0.f, 0.f );

            if( dist > 1e-3f )
            {
                dir = toHero / dist;
            }

            // When a wall blocks the direct line to the hero, steer along the
            // flow field so the enemy walks around it instead of into it. Close
            // up (or with clear line of sight) keep the straight beeline.
            if( m_dungeon != nullptr && dist > m_dungeon->cellSize )
            {
                if( this->hasLineOfSight_( e.pos, _heroXZ ) == false )
                {
                    mt::vec2f flow = this->sampleFlowDir_( e.pos );

                    if( mt::sqrlength_v2( flow ) > 1e-4f )
                    {
                        dir = flow;
                    }
                }
            }

            float speed = def.speed;

            // Charger: accelerate when the hero gets close.
            if( (def.behavior & EB_CHARGER) != 0u )
            {
                if( dist < def.chargeRange )
                {
                    speed *= def.chargeMul;
                    e.charging = true;
                }
                else
                {
                    e.charging = false;
                }
            }

            // Splitter children orbit the hero instead of piling on top.
            mt::vec2f desired = dir * speed;

            // Steering target velocity blended with knockback velocity (e.vel).
            mt::vec2f step = desired * _dt + e.vel * _dt;

            // Wall-aware movement: try axis-separated so enemies slide on cliffs.
            // A body-radius margin keeps them from clipping into the wall mesh.
            float wallMargin = def.radius;

            mt::vec2f next = e.pos;

            if( this->enemyWalkable_( e.pos.x + step.x, e.pos.y, wallMargin ) == true )
            {
                next.x += step.x;
            }

            if( this->enemyWalkable_( next.x, e.pos.y + step.y, wallMargin ) == true )
            {
                next.y += step.y;
            }

            e.pos = next;

            // Knockback velocity decays.
            float decay = 1.f - 6.f * _dt;

            if( decay < 0.f )
            {
                decay = 0.f;
            }

            e.vel *= decay;

            // Shooter: fire at the hero on interval.
            if( (def.behavior & EB_SHOOTER) != 0u )
            {
                e.shootCd -= _dt;

                if( e.shootCd <= 0.f && dist < 1500.f )
                {
                    // Only fire when the hero is actually visible (no wall in between).
                    if( this->hasLineOfSight_( e.pos, _heroXZ ) == false )
                    {
                        e.shootCd = 0.f;
                    }
                    else
                    {
                        e.shootCd = def.shootInterval;

                        if( dist > 1e-3f )
                        {
                            CombatProjectile p;
                            p.pos = e.pos;
                            p.vel = dir * def.shootSpeed;
                            p.damage = def.shootDamage;
                            // Slow bullets need a longer life to still reach the hero.
                            p.life = 9.0f;
                            p.radius = 12.f;
                            p.pierceLeft = 0u;
                            p.areaRadius = 0.f;
                            p.dotDps = 0.f;
                            p.dotDuration = 0.f;
                            p.orbitAngle = 0.f;
                            p.orbitRadius = 0.f;
                            p.color = Detail::combatPackRGBA8( 1.f, 0.3f, 0.5f, 1.f );
                            p.ownerWeapon = -1;
                            p.fromHero = false;
                            p.alive = true;

                            m_projectiles.push_back( p );
                        }
                    }
                }
            }

            // Splitter: keep up to maxChildren alive, replacing fallen ones.
            if( (def.behavior & EB_SPLITTER) != 0u && def.childDefIndex >= 0 )
            {
                if( e.children < def.maxChildren )
                {
                    // Spawn a replacement child next to the parent.
                    mt::vec2f cpos = e.pos + this->randomDir_() * (def.radius + 40.f);

                    if( m_dungeon == nullptr || m_dungeon->isWalkableWorld( cpos.x, cpos.y ) == true )
                    {
                        this->spawnEnemy_( (uint32_t)def.childDefIndex, cpos, (int32_t)i );
                    }
                }
            }

            // Touch damage while overlapping the hero.
            if( dist < def.radius + 34.f )
            {
                this->damageHero_( def.touchDamage * _dt );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::killEnemy_( uint32_t _index, const mt::vec2f & _heroXZ )
    {
        CombatEnemy & e = m_enemies[_index];

        if( e.alive == false )
        {
            return;
        }

        e.alive = false;

        // Begin the death animation: freeze in place, then redden and sink.
        e.dying = true;
        e.deathTime = 0.f;
        e.vel = mt::vec2f( 0.f, 0.f );

        const CombatEnemyDef & def = Detail::g_enemyDefs[e.defIndex];

        // Notify splitter parent that a child died (so it can respawn one).
        if( e.parent >= 0 && (uint32_t)e.parent < m_enemies.size() )
        {
            CombatEnemy & parent = m_enemies[e.parent];

            if( parent.children > 0u )
            {
                parent.children -= 1u;
            }
        }

        // Exploder: AoE damage + knockback on death.
        if( (def.behavior & EB_EXPLODER) != 0u )
        {
            this->applyExplosion_( e.pos, def.explodeRadius, def.explodeDamage, def.explodeImpulse, _heroXZ );
        }

        // Drop an XP cube whose tier matches this enemy's reward.
        this->dropXp_( e.pos, def.xpReward );
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::applyExplosion_( const mt::vec2f & _center, float _radius, float _damage, float _impulse, const mt::vec2f & _heroXZ )
    {
        float r2 = _radius * _radius;

        for( CombatEnemy & e : m_enemies )
        {
            if( e.alive == false )
            {
                continue;
            }

            mt::vec2f d = e.pos - _center;
            float dist2 = mt::sqrlength_v2( d );

            if( dist2 > r2 )
            {
                continue;
            }

            float dist = StdMath::sqrtf( dist2 );
            float falloff = 1.f - dist / _radius;

            e.hp -= _damage * falloff;

            mt::vec2f push( 0.f, 0.f );

            if( dist > 1e-3f )
            {
                push = (d / dist) * (_impulse * falloff);
            }

            e.vel += push;
        }

        // The hero is shoved/damaged too if within the blast.
        mt::vec2f hd = _heroXZ - _center;
        float hdist2 = mt::sqrlength_v2( hd );

        if( hdist2 < r2 )
        {
            float hdist = StdMath::sqrtf( hdist2 );
            float falloff = 1.f - hdist / _radius;

            this->damageHero_( _damage * 0.5f * falloff );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::updateWeapons_( float _dt, const mt::vec2f & _heroXZ, const mt::vec2f & _heroMove )
    {
        for( CombatWeapon & w : m_weapons )
        {
            const CombatWeaponDef & def = Detail::g_weaponDefs[w.defIndex];

            float interval = def.fireInterval / (w.fireRateMul > 0.01f ? w.fireRateMul : 0.01f);

            w.cooldown -= _dt;

            if( w.cooldown <= 0.f )
            {
                // Aimed weapons (Nearest/Hardest/Random) need a visible enemy in
                // range; weapons that fire in fixed directions (Forward/Directional/
                // Orbit) only need any enemy in range. Otherwise stay ready and
                // retry next frame instead of burning the reload.
                bool aimed = (def.targeting == CombatTargeting::Nearest)
                    || (def.targeting == CombatTargeting::Hardest)
                    || (def.targeting == CombatTargeting::Random);

                bool canFire = aimed
                    ? this->anyEnemyInRangeVisible_( def.range, _heroXZ )
                    : this->anyEnemyInRange_( def.range, _heroXZ );

                if( canFire == false )
                {
                    w.cooldown = 0.f;

                    continue;
                }

                w.cooldown += interval;

                if( w.cooldown < 0.f )
                {
                    w.cooldown = interval;
                }

                this->fireWeapon_( w, _heroXZ, _heroMove );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DCombat::anyEnemyInRange_( float _range, const mt::vec2f & _heroXZ ) const
    {
        float range2 = _range * _range;

        for( const CombatEnemy & e : m_enemies )
        {
            if( e.alive == false )
            {
                continue;
            }

            // range <= 0 means unlimited acquisition range.
            if( _range <= 0.f )
            {
                return true;
            }

            float dist2 = mt::sqrlength_v2_v2( e.pos, _heroXZ );

            if( dist2 <= range2 )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DCombat::anyEnemyInRangeVisible_( float _range, const mt::vec2f & _heroXZ ) const
    {
        float range2 = _range * _range;

        for( const CombatEnemy & e : m_enemies )
        {
            if( e.alive == false )
            {
                continue;
            }

            if( _range > 0.f )
            {
                float dist2 = mt::sqrlength_v2_v2( e.pos, _heroXZ );

                if( dist2 > range2 )
                {
                    continue;
                }
            }

            // Only counts when the hero has an unobstructed line to the enemy.
            if( this->hasLineOfSight_( _heroXZ, e.pos ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DCombat::hasLineOfSight_( const mt::vec2f & _from, const mt::vec2f & _to ) const
    {
        // No dungeon means open space: everything is visible.
        if( m_dungeon == nullptr )
        {
            return true;
        }

        mt::vec2f delta = _to - _from;
        float dist = mt::length_v2( delta );

        if( dist < 1e-3f )
        {
            return true;
        }

        // Sample along the segment at roughly half-cell steps; if any sample
        // lands on a solid (non-walkable) cell, the view is blocked by a wall.
        float stepLen = m_dungeon->cellSize * 0.5f;
        uint32_t steps = (uint32_t)(dist / stepLen) + 1u;

        mt::vec2f stepVec = delta / (float)steps;
        mt::vec2f probe = _from;

        for( uint32_t i = 0; i != steps; ++i )
        {
            probe += stepVec;

            if( m_dungeon->isWalkableWorld( probe.x, probe.y ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t Crimsonland3DCombat::acquireTarget_( const CombatWeaponDef & _def, const mt::vec2f & _heroXZ )
    {
        int32_t best = -1;
        float bestScore = 0.f;
        bool bestSet = false;
        uint32_t randomSeen = 0u;

        float range2 = _def.range * _def.range;

        for( uint32_t i = 0; i != m_enemies.size(); ++i )
        {
            const CombatEnemy & e = m_enemies[i];

            if( e.alive == false )
            {
                continue;
            }

            float dist2 = mt::sqrlength_v2_v2( e.pos, _heroXZ );

            if( _def.range > 0.f && dist2 > range2 )
            {
                continue;
            }

            // Aimed weapons only lock onto enemies the hero can actually see;
            // a wall between the hero and the enemy blocks the shot.
            if( this->hasLineOfSight_( _heroXZ, e.pos ) == false )
            {
                continue;
            }

            if( _def.targeting == CombatTargeting::Random )
            {
                // Reservoir sampling: each in-range enemy has equal pick chance.
                randomSeen += 1u;

                if( this->random_( randomSeen ) == 0u )
                {
                    best = (int32_t)i;
                }

                continue;
            }

            float score = 0.f;

            switch( _def.targeting )
            {
            case CombatTargeting::Nearest:
                score = -dist2;          // closest -> largest score
                break;
            case CombatTargeting::Hardest:
                score = e.hp;            // most HP -> largest score
                break;
            default:
                break;
            }

            if( bestSet == false || score > bestScore )
            {
                bestScore = score;
                best = (int32_t)i;
                bestSet = true;
            }
        }

        return best;
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::fireWeapon_( CombatWeapon & _weapon, const mt::vec2f & _heroXZ, const mt::vec2f & _heroMove )
    {
        const CombatWeaponDef & def = Detail::g_weaponDefs[_weapon.defIndex];

        float damage = def.damage * _weapon.damageMul * m_hero.damageMul;
        uint32_t pellets = def.projectileCount + (uint32_t)(_weapon.extraProjectiles > 0 ? _weapon.extraProjectiles : 0);

        if( def.targeting == CombatTargeting::Directional )
        {
            // Fire evenly in all directions around the hero.
            for( uint32_t i = 0; i != pellets; ++i )
            {
                float a = Detail::COMBAT_TWO_PI * (float)i / (float)pellets;
                mt::vec2f dir( StdMath::cosf( a ), StdMath::sinf( a ) );

                this->emitProjectile_( _heroXZ, dir, def, _weapon, (int32_t)_weapon.defIndex, true, damage );
            }

            return;
        }

        if( def.targeting == CombatTargeting::Orbit )
        {
            // Refresh the orbiting blades: equally spaced around the hero.
            for( uint32_t i = 0; i != pellets; ++i )
            {
                CombatProjectile p;
                p.orbitAngle = _weapon.orbitPhase + Detail::COMBAT_TWO_PI * (float)i / (float)pellets;
                p.orbitRadius = 150.f * _weapon.areaMul;
                p.pos = _heroXZ + mt::vec2f( StdMath::cosf( p.orbitAngle ), StdMath::sinf( p.orbitAngle ) ) * p.orbitRadius;
                p.vel = mt::vec2f( 0.f, 0.f );
                p.damage = damage;
                p.life = def.projectileLife;
                p.radius = def.projectileRadius;
                p.pierceLeft = def.pierce;
                p.areaRadius = 0.f;
                p.dotDps = 0.f;
                p.dotDuration = 0.f;
                p.color = def.color;
                p.ownerWeapon = (int32_t)_weapon.defIndex;
                p.fromHero = true;
                p.alive = true;

                m_projectiles.push_back( p );
            }

            return;
        }

        mt::vec2f aim( 0.f, 0.f );

        if( def.targeting == CombatTargeting::Forward )
        {
            if( mt::sqrlength_v2( _heroMove ) > 1e-4f )
            {
                mt::norm_safe_v2( &aim, _heroMove );
            }
            else
            {
                aim = m_heroFacing;
            }
        }
        else
        {
            int32_t target = this->acquireTarget_( def, _heroXZ );

            if( target < 0 )
            {
                return;
            }

            mt::vec2f toTarget = m_enemies[target].pos - _heroXZ;
            mt::norm_safe_v2( &aim, toTarget );
        }

        // Fan the pellets across the spread angle.
        for( uint32_t i = 0; i != pellets; ++i )
        {
            float offset = 0.f;

            if( pellets > 1u && def.spread > 0.f )
            {
                offset = (-0.5f + (float)i / (float)(pellets - 1u)) * def.spread;
            }
            else if( def.spread > 0.f )
            {
                offset = (this->randomf_() - 0.5f) * def.spread;
            }

            float ca = StdMath::cosf( offset );
            float sa = StdMath::sinf( offset );

            mt::vec2f dir( aim.x * ca - aim.y * sa, aim.x * sa + aim.y * ca );

            this->emitProjectile_( _heroXZ, dir, def, _weapon, (int32_t)_weapon.defIndex, true, damage );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::emitProjectile_( const mt::vec2f & _pos, const mt::vec2f & _dir, const CombatWeaponDef & _def
        , const CombatWeapon & _weapon, int32_t _ownerWeapon, bool _fromHero, float _damage )
    {
        CombatProjectile p;
        p.pos = _pos;
        p.vel = _dir * _def.projectileSpeed;
        p.damage = _damage;
        p.life = _def.projectileLife;
        p.radius = _def.projectileRadius;
        p.pierceLeft = _def.pierce;
        p.areaRadius = _def.areaRadius * _weapon.areaMul;
        p.dotDps = _def.dotDps * _weapon.damageMul;
        p.dotDuration = _def.dotDuration * _weapon.durationMul;
        p.orbitAngle = 0.f;
        p.orbitRadius = 0.f;
        p.color = _def.color;
        p.ownerWeapon = _ownerWeapon;
        p.fromHero = _fromHero;
        p.alive = true;

        m_projectiles.push_back( p );
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::updateProjectiles_( float _dt, const mt::vec2f & _heroXZ )
    {
        for( CombatProjectile & p : m_projectiles )
        {
            if( p.alive == false )
            {
                continue;
            }

            p.life -= _dt;

            if( p.life <= 0.f )
            {
                p.alive = false;
                continue;
            }

            if( p.orbitRadius > 0.f )
            {
                // Orbiting melee aura: revolve around the hero.
                p.orbitAngle += _dt * 4.2f;
                p.pos = _heroXZ + mt::vec2f( StdMath::cosf( p.orbitAngle ), StdMath::sinf( p.orbitAngle ) ) * p.orbitRadius;
            }
            else
            {
                p.pos += p.vel * _dt;
            }

            // Projectiles are stopped by walls (orbiting auras pass through,
            // since they hug the hero who already stands on walkable floor).
            if( p.orbitRadius <= 0.f && m_dungeon != nullptr && m_dungeon->isWalkableWorld( p.pos.x, p.pos.y ) == false )
            {
                p.alive = false;
                continue;
            }

            if( p.fromHero == true )
            {
                // Hero projectile vs enemies.
                for( uint32_t ei = 0; ei != m_enemies.size(); ++ei )
                {
                    CombatEnemy & e = m_enemies[ei];

                    if( e.alive == false )
                    {
                        continue;
                    }

                    const CombatEnemyDef & edef = Detail::g_enemyDefs[e.defIndex];

                    float rr = p.radius + edef.radius;

                    if( mt::sqrlength_v2_v2( p.pos, e.pos ) > rr * rr )
                    {
                        continue;
                    }

                    // Splash damage.
                    if( p.areaRadius > 0.f )
                    {
                        this->applyExplosion_( p.pos, p.areaRadius, p.damage, 280.f, _heroXZ );
                    }
                    else
                    {
                        e.hp -= p.damage;
                    }

                    // Apply lingering DoT (fire / poison).
                    if( p.dotDuration > 0.f )
                    {
                        e.dotDps = p.dotDps;
                        e.dotRemain = p.dotDuration;
                    }

                    if( e.hp <= 0.f )
                    {
                        this->killEnemy_( ei, _heroXZ );
                    }

                    if( p.pierceLeft == 0u )
                    {
                        p.alive = false;
                        break;
                    }

                    p.pierceLeft -= 1u;
                }
            }
            else
            {
                // Enemy projectile vs hero.
                float rr = p.radius + 34.f;

                if( mt::sqrlength_v2_v2( p.pos, _heroXZ ) <= rr * rr )
                {
                    this->damageHero_( p.damage );
                    p.alive = false;
                }
            }
        }

        // Compact dead projectiles occasionally to keep the vector bounded.
        if( m_projectiles.size() > 1024u )
        {
            Vector<CombatProjectile> live;
            live.reserve( m_projectiles.size() );

            for( const CombatProjectile & p : m_projectiles )
            {
                if( p.alive == true )
                {
                    live.push_back( p );
                }
            }

            m_projectiles.swap( live );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::resolvePhysics_( float _dt )
    {
        MENGINE_UNUSED( _dt );

        // Lightweight circle separation so enemies don't fully overlap (the
        // "Box2D-like" crowding without pulling in the disabled physics plugin).
        const uint32_t n = (uint32_t)m_enemies.size();

        for( uint32_t i = 0; i != n; ++i )
        {
            CombatEnemy & a = m_enemies[i];

            if( a.alive == false )
            {
                continue;
            }

            const CombatEnemyDef & adef = Detail::g_enemyDefs[a.defIndex];

            for( uint32_t j = i + 1u; j != n; ++j )
            {
                CombatEnemy & b = m_enemies[j];

                if( b.alive == false )
                {
                    continue;
                }

                const CombatEnemyDef & bdef = Detail::g_enemyDefs[b.defIndex];

                float minDist = adef.radius + bdef.radius;

                mt::vec2f d = b.pos - a.pos;
                float dist2 = mt::sqrlength_v2( d );

                if( dist2 >= minDist * minDist || dist2 < 1e-4f )
                {
                    continue;
                }

                float dist = StdMath::sqrtf( dist2 );
                float overlap = (minDist - dist) * 0.5f;

                mt::vec2f push = (d / dist) * overlap;

                a.pos -= push;
                b.pos += push;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::updateXpCubes_( float _dt, const mt::vec2f & _heroXZ )
    {
        float pickup = m_hero.pickupRadius;
        float pickup2 = pickup * pickup;
        float collect2 = 44.f * 44.f;

        for( CombatXpCube & c : m_xpCubes )
        {
            if( c.alive == false )
            {
                continue;
            }

            mt::vec2f d = _heroXZ - c.pos;
            float dist2 = mt::sqrlength_v2( d );

            // Magnetise toward the hero once within pickup radius.
            if( dist2 < pickup2 )
            {
                float dist = StdMath::sqrtf( dist2 );

                mt::vec2f dir( 0.f, 0.f );
                mt::norm_safe_v2( &dir, d );

                // Tangential component (perpendicular to the pull direction) makes
                // the orb curve in along a bezier-like spiral instead of a straight
                // line. The swirl fades and the pull accelerates near the hero.
                mt::vec2f perp( -dir.y, dir.x );

                float closeness = 1.f - dist / pickup; // 0 far .. 1 touching

                if( closeness < 0.f )
                {
                    closeness = 0.f;
                }

                float pullSpeed = 300.f + 1050.f * closeness;
                float swirlSpeed = 560.f * (1.f - closeness);

                c.pos += (dir * pullSpeed + perp * swirlSpeed) * _dt;
            }

            if( dist2 < collect2 )
            {
                this->grantXp_( c.value );
                c.alive = false;
            }
        }

        if( m_xpCubes.size() > 2048u )
        {
            Vector<CombatXpCube> live;
            live.reserve( m_xpCubes.size() );

            for( const CombatXpCube & c : m_xpCubes )
            {
                if( c.alive == true )
                {
                    live.push_back( c );
                }
            }

            m_xpCubes.swap( live );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::dropXp_( const mt::vec2f & _pos, uint32_t _value )
    {
        CombatXpCube c;
        c.pos = _pos;
        c.vel = mt::vec2f( 0.f, 0.f );
        c.value = _value;
        c.color = Detail::xpColorForValue( _value );
        c.alive = true;

        m_xpCubes.push_back( c );
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::grantXp_( uint32_t _value )
    {
        float scaled = (float)_value * m_hero.xpGainMul;

        m_hero.xp += (uint32_t)(scaled + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::damageHero_( float _damage )
    {
        if( _damage <= 0.f || m_heroDead == true )
        {
            return;
        }

        // Dodge: chance to ignore the hit entirely.
        if( m_hero.dodge > 0.f && this->randomf_() < m_hero.dodge )
        {
            return;
        }

        float dealt = _damage - m_hero.armor * _damage; // armor as fractional reduction

        if( dealt < _damage * 0.1f )
        {
            dealt = _damage * 0.1f; // armor caps at 90% reduction
        }

        m_hero.hp -= dealt;

        if( m_hero.hp <= 0.f )
        {
            m_hero.hp = 0.f;
            m_heroDead = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DCombat::hasWeapon_( uint32_t _defIndex ) const
    {
        for( const CombatWeapon & w : m_weapons )
        {
            if( w.defIndex == _defIndex )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::addWeapon_( uint32_t _defIndex )
    {
        if( _defIndex >= Detail::g_weaponCount || this->hasWeapon_( _defIndex ) == true )
        {
            return;
        }

        CombatWeapon w;
        w.defIndex = _defIndex;
        w.cooldown = 0.f;
        w.level = 1u;
        w.damageMul = 1.f;
        w.fireRateMul = 1.f;
        w.areaMul = 1.f;
        w.durationMul = 1.f;
        w.extraProjectiles = 0;
        w.orbitPhase = 0.f;

        m_weapons.push_back( w );
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    const char * rarityName( CombatUpgradeRarity _rarity )
    {
        switch( _rarity )
        {
        case CombatUpgradeRarity::Common:    return "Common";
        case CombatUpgradeRarity::Uncommon:  return "Uncommon";
        case CombatUpgradeRarity::Rare:      return "Rare";
        case CombatUpgradeRarity::Epic:      return "Epic";
        case CombatUpgradeRarity::Legendary: return "Legendary";
        default:                             return "Common";
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float rarityMagnitudeMul( CombatUpgradeRarity _rarity )
    {
        // Higher rarities multiply the base magnitude of every upgrade.
        switch( _rarity )
        {
        case CombatUpgradeRarity::Common:    return 1.00f;
        case CombatUpgradeRarity::Uncommon:  return 1.35f;
        case CombatUpgradeRarity::Rare:      return 1.80f;
        case CombatUpgradeRarity::Epic:      return 2.50f;
        case CombatUpgradeRarity::Legendary: return 3.50f;
        default:                             return 1.00f;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t rarityColor( CombatUpgradeRarity _rarity )
    {
        switch( _rarity )
        {
        case CombatUpgradeRarity::Common:    return Detail::combatPackRGBA8( 0.70f, 0.70f, 0.70f, 1.f );
        case CombatUpgradeRarity::Uncommon:  return Detail::combatPackRGBA8( 0.40f, 0.85f, 0.40f, 1.f );
        case CombatUpgradeRarity::Rare:      return Detail::combatPackRGBA8( 0.35f, 0.55f, 1.00f, 1.f );
        case CombatUpgradeRarity::Epic:      return Detail::combatPackRGBA8( 0.75f, 0.40f, 0.95f, 1.f );
        case CombatUpgradeRarity::Legendary: return Detail::combatPackRGBA8( 1.00f, 0.65f, 0.20f, 1.f );
        default:                             return Detail::combatPackRGBA8( 0.70f, 0.70f, 0.70f, 1.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    CombatUpgradeRarity Crimsonland3DCombat::rollRarity_()
    {
        // Base tier weights; the hero "luck" stat moves probability mass from
        // Common toward the rarer (and stronger) tiers.
        float luck = m_hero.luck;

        if( luck < 0.f ) luck = 0.f;
        else if( luck > 1.f ) luck = 1.f;

        float weights[5];
        weights[0] = 60.f * (1.f - 0.85f * luck);   // Common
        weights[1] = 25.f + 15.f * luck;            // Uncommon
        weights[2] = 10.f + 22.f * luck;            // Rare
        weights[3] = 4.f + 16.f * luck;             // Epic
        weights[4] = 1.f + 10.f * luck;             // Legendary

        float total = weights[0] + weights[1] + weights[2] + weights[3] + weights[4];

        float roll = this->randomf_() * total;

        float acc = 0.f;
        for( uint32_t i = 0; i != 5u; ++i )
        {
            acc += weights[i];

            if( roll < acc )
            {
                return (CombatUpgradeRarity)i;
            }
        }

        return CombatUpgradeRarity::Common;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        // How an upgrade's magnitude is displayed on its card.
        //////////////////////////////////////////////////////////////////////////
        enum class CombatUpgradeUnit
        {
            Percent,    // "+N%"
            Flat,       // "+N"
            Count,      // "+N" (integer count)
            Regen,      // "+N.N HP/s"
            None        // suffix only (e.g. NEW WEAPON)
        };
        //////////////////////////////////////////////////////////////////////////
        struct CombatUpgradeCandidate
        {
            CombatUpgradeKind kind;
            int32_t weaponIndex;
            float baseMag;
            const char * weaponName;    // nullptr for global/passive upgrades
            const char * suffix;
            CombatUpgradeUnit unit;
            bool scalable;              // does rarity scale the magnitude?
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::buildUpgradeOptions_()
    {
        m_upgradeOptions.clear();

        // Build a candidate pool, then pick 3 distinct entries and roll a rarity
        // for each (which scales the magnitude and tints the card).
        Vector<Detail::CombatUpgradeCandidate> pool;

        // Weapon upgrades for owned weapons.
        for( uint32_t i = 0; i != m_weapons.size(); ++i )
        {
            const CombatWeaponDef & def = Detail::g_weaponDefs[m_weapons[i].defIndex];

            pool.push_back( { CombatUpgradeKind::WeaponDamage, (int32_t)i, 0.25f, def.name, "DMG", Detail::CombatUpgradeUnit::Percent, true } );
            pool.push_back( { CombatUpgradeKind::WeaponAttackSpeed, (int32_t)i, 0.20f, def.name, "rate", Detail::CombatUpgradeUnit::Percent, true } );

            if( def.areaRadius > 0.f )
            {
                pool.push_back( { CombatUpgradeKind::WeaponArea, (int32_t)i, 0.25f, def.name, "area", Detail::CombatUpgradeUnit::Percent, true } );
            }

            if( def.dotDuration > 0.f )
            {
                pool.push_back( { CombatUpgradeKind::WeaponDuration, (int32_t)i, 0.30f, def.name, "duration", Detail::CombatUpgradeUnit::Percent, true } );
            }

            pool.push_back( { CombatUpgradeKind::WeaponProjectiles, (int32_t)i, 1.f, def.name, "projectile", Detail::CombatUpgradeUnit::Count, true } );
        }

        // Offer a brand new weapon only on milestone levels (every N levels),
        // so weapons are earned at a steady pace instead of any level-up.
        const uint32_t c_newWeaponEveryNLevels = 5u;

        if( m_hero.level % c_newWeaponEveryNLevels == 0u )
        {
            for( uint32_t i = 0; i != Detail::g_weaponCount; ++i )
            {
                if( this->hasWeapon_( i ) == true )
                {
                    continue;
                }

                pool.push_back( { CombatUpgradeKind::NewWeapon, (int32_t)i, 0.f, Detail::g_weaponDefs[i].name, "NEW WEAPON", Detail::CombatUpgradeUnit::None, false } );
            }
        }

        // Passive upgrades (global; not tied to a single weapon).
        pool.push_back( { CombatUpgradeKind::Regen,        -1, 1.5f,  nullptr, "regen",        Detail::CombatUpgradeUnit::Regen,   true } );
        pool.push_back( { CombatUpgradeKind::MaxHp,        -1, 25.f,  nullptr, "max HP",       Detail::CombatUpgradeUnit::Flat,    true } );
        pool.push_back( { CombatUpgradeKind::Armor,        -1, 0.06f, nullptr, "armor",        Detail::CombatUpgradeUnit::Percent, true } );
        pool.push_back( { CombatUpgradeKind::Dodge,        -1, 0.05f, nullptr, "dodge",        Detail::CombatUpgradeUnit::Percent, true } );
        pool.push_back( { CombatUpgradeKind::XpGain,       -1, 0.20f, nullptr, "XP gain",      Detail::CombatUpgradeUnit::Percent, true } );
        pool.push_back( { CombatUpgradeKind::MoveSpeed,    -1, 0.10f, nullptr, "move speed",   Detail::CombatUpgradeUnit::Percent, true } );
        pool.push_back( { CombatUpgradeKind::PickupRadius, -1, 0.30f, nullptr, "pickup range", Detail::CombatUpgradeUnit::Percent, true } );
        pool.push_back( { CombatUpgradeKind::Luck,         -1, 0.08f, nullptr, "luck",         Detail::CombatUpgradeUnit::Percent, true } );

        // Pick 3 distinct random options out of the pool.
        uint32_t want = 3u;

        if( want > (uint32_t)pool.size() )
        {
            want = (uint32_t)pool.size();
        }

        for( uint32_t k = 0; k != want; ++k )
        {
            uint32_t idx = this->random_( (uint32_t)pool.size() );

            const Detail::CombatUpgradeCandidate candidate = pool[idx];

            pool[idx] = pool.back();
            pool.pop_back();

            // Every upgrade gets a rarity tier.
            CombatUpgradeRarity rarity = this->rollRarity_();

            float magnitude = candidate.baseMag;

            if( candidate.scalable == true )
            {
                magnitude *= rarityMagnitudeMul( rarity );
            }

            CombatUpgradeOption option;
            option.kind = candidate.kind;
            option.rarity = rarity;
            option.weaponIndex = candidate.weaponIndex;
            option.color = rarityColor( rarity );

            // Format the card label: "[Rarity] Weapon: +X suffix".
            char value[32] = {'\0'};

            switch( candidate.unit )
            {
            case Detail::CombatUpgradeUnit::Percent:
                MENGINE_SNPRINTF( value, sizeof( value ), "+%u%% %s", (uint32_t)(magnitude * 100.f + 0.5f), candidate.suffix );
                break;
            case Detail::CombatUpgradeUnit::Flat:
                MENGINE_SNPRINTF( value, sizeof( value ), "+%u %s", (uint32_t)(magnitude + 0.5f), candidate.suffix );
                break;
            case Detail::CombatUpgradeUnit::Count:
                {
                    uint32_t count = (uint32_t)(magnitude + 0.5f);

                    if( count < 1u )
                    {
                        count = 1u;
                    }

                    magnitude = (float)count;

                    MENGINE_SNPRINTF( value, sizeof( value ), "+%u %s", count, candidate.suffix );
                }
                break;
            case Detail::CombatUpgradeUnit::Regen:
                MENGINE_SNPRINTF( value, sizeof( value ), "+%.1f HP/s %s", magnitude, candidate.suffix );
                break;
            case Detail::CombatUpgradeUnit::None:
            default:
                StdString::strcpy_safe( value, candidate.suffix, sizeof( value ) );
                break;
            }

            option.magnitude = magnitude;

            if( candidate.weaponName != nullptr )
            {
                MENGINE_SNPRINTF( option.label, sizeof( option.label ), "[%s] %s: %s", rarityName( rarity ), candidate.weaponName, value );
            }
            else
            {
                MENGINE_SNPRINTF( option.label, sizeof( option.label ), "[%s] %s", rarityName( rarity ), value );
            }

            m_upgradeOptions.push_back( option );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DCombat::applyUpgrade( uint32_t _optionIndex )
    {
        if( m_levelUpPending == false || _optionIndex >= m_upgradeOptions.size() )
        {
            return;
        }

        const CombatUpgradeOption & op = m_upgradeOptions[_optionIndex];

        switch( op.kind )
        {
        case CombatUpgradeKind::WeaponDamage:
            if( op.weaponIndex >= 0 && (uint32_t)op.weaponIndex < m_weapons.size() )
            {
                m_weapons[op.weaponIndex].damageMul += op.magnitude;
                m_weapons[op.weaponIndex].level += 1u;
            }
            break;
        case CombatUpgradeKind::WeaponAttackSpeed:
            if( op.weaponIndex >= 0 && (uint32_t)op.weaponIndex < m_weapons.size() )
            {
                m_weapons[op.weaponIndex].fireRateMul += op.magnitude;
                m_weapons[op.weaponIndex].level += 1u;
            }
            break;
        case CombatUpgradeKind::WeaponArea:
            if( op.weaponIndex >= 0 && (uint32_t)op.weaponIndex < m_weapons.size() )
            {
                m_weapons[op.weaponIndex].areaMul += op.magnitude;
                m_weapons[op.weaponIndex].level += 1u;
            }
            break;
        case CombatUpgradeKind::WeaponDuration:
            if( op.weaponIndex >= 0 && (uint32_t)op.weaponIndex < m_weapons.size() )
            {
                m_weapons[op.weaponIndex].durationMul += op.magnitude;
                m_weapons[op.weaponIndex].level += 1u;
            }
            break;
        case CombatUpgradeKind::WeaponProjectiles:
            if( op.weaponIndex >= 0 && (uint32_t)op.weaponIndex < m_weapons.size() )
            {
                m_weapons[op.weaponIndex].extraProjectiles += (int32_t)op.magnitude;
                m_weapons[op.weaponIndex].level += 1u;
            }
            break;
        case CombatUpgradeKind::NewWeapon:
            this->addWeapon_( (uint32_t)op.weaponIndex );
            break;
        case CombatUpgradeKind::Regen:
            m_hero.regen += op.magnitude;
            break;
        case CombatUpgradeKind::MaxHp:
            m_hero.maxHp += op.magnitude;
            m_hero.hp += op.magnitude;
            break;
        case CombatUpgradeKind::Armor:
            m_hero.armor += op.magnitude;
            if( m_hero.armor > 0.9f ) m_hero.armor = 0.9f;
            break;
        case CombatUpgradeKind::Dodge:
            m_hero.dodge += op.magnitude;
            if( m_hero.dodge > 0.75f ) m_hero.dodge = 0.75f;
            break;
        case CombatUpgradeKind::XpGain:
            m_hero.xpGainMul += op.magnitude;
            break;
        case CombatUpgradeKind::MoveSpeed:
            m_hero.moveSpeedMul += op.magnitude;
            break;
        case CombatUpgradeKind::PickupRadius:
            m_hero.pickupRadius += m_hero.pickupRadius * op.magnitude;
            break;
        case CombatUpgradeKind::Luck:
            m_hero.luck += op.magnitude;
            if( m_hero.luck > 1.f ) m_hero.luck = 1.f;
            break;
        }

        m_levelUpPending = false;
        m_upgradeOptions.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DCombat::isLevelUpPending() const
    {
        return m_levelUpPending;
    }
    //////////////////////////////////////////////////////////////////////////
    const CombatUpgradeOption * Crimsonland3DCombat::getUpgradeOptions( uint32_t * _count ) const
    {
        if( _count != nullptr )
        {
            *_count = (uint32_t)m_upgradeOptions.size();
        }

        if( m_upgradeOptions.empty() == true )
        {
            return nullptr;
        }

        return m_upgradeOptions.data();
    }
    //////////////////////////////////////////////////////////////////////////
    const CombatHeroStats & Crimsonland3DCombat::getHeroStats() const
    {
        return m_hero;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DCombat::isHeroDead() const
    {
        return m_heroDead;
    }
    //////////////////////////////////////////////////////////////////////////
    const Vector<CombatEnemy> & Crimsonland3DCombat::getEnemies() const
    {
        return m_enemies;
    }
    //////////////////////////////////////////////////////////////////////////
    const Vector<CombatProjectile> & Crimsonland3DCombat::getProjectiles() const
    {
        return m_projectiles;
    }
    //////////////////////////////////////////////////////////////////////////
    const Vector<CombatXpCube> & Crimsonland3DCombat::getXpCubes() const
    {
        return m_xpCubes;
    }
    //////////////////////////////////////////////////////////////////////////
    const Vector<CombatWeapon> & Crimsonland3DCombat::getWeapons() const
    {
        return m_weapons;
    }
    //////////////////////////////////////////////////////////////////////////
    const CombatEnemyDef & Crimsonland3DCombat::getEnemyDef( uint32_t _defIndex ) const
    {
        if( _defIndex >= Detail::g_enemyCount )
        {
            _defIndex = 0u;
        }

        return Detail::g_enemyDefs[_defIndex];
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Crimsonland3DCombat::getEnemyDefCount()
    {
        return Detail::g_enemyCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Crimsonland3DCombat::getWeaponDefCount()
    {
        return Detail::g_weaponCount;
    }
    //////////////////////////////////////////////////////////////////////////
    const CombatWeaponDef & Crimsonland3DCombat::getWeaponDef( uint32_t _defIndex )
    {
        if( _defIndex >= Detail::g_weaponCount )
        {
            _defIndex = 0u;
        }

        return Detail::g_weaponDefs[_defIndex];
    }
    //////////////////////////////////////////////////////////////////////////
}
