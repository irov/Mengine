#include "Arena3DSimulation.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        constexpr uint32_t FUSE_TICKS = 600;
        //////////////////////////////////////////////////////////////////////////
        static bool testGrenade( int32_t _speed, kf_angle16_t _yaw )
        {
            SimulationConfig config = SimulationConfig::defaults();
            size_t weaponIndex = static_cast<size_t>(WeaponType::GrenadeLauncher);
            config.weapons[weaponIndex].speed = kf_fixed_from_int( _speed );
            config.weapons[weaponIndex].launchUpwardSpeed = 0;
            config.weapons[weaponIndex].lifetimeTicks = FUSE_TICKS;
            Simulation simulation( config );
            simulation.reset( 1, 123 );
            uint32_t playerId = 1;
            PlayerState * player = simulation.findPlayer( playerId );
            player->position = {557056, 589824, 1081344}; // Lower walkway, with feet on the floor.
            player->velocity = {};
            player->yaw = _yaw;
            player->pitch = 0;

            VectorCommandEnvelope commands;
            CommandEnvelope select;
            select.matchId = 1;
            select.playerId = playerId;
            select.type = CommandType::SelectWeapon;
            select.payload.weapon = WeaponType::GrenadeLauncher;
            commands.emplace_back( select );
            CommandEnvelope fire = select;
            fire.type = CommandType::FireDown;
            commands.emplace_back( fire );
            simulation.step( commands );
            commands.clear();
            fire.type = CommandType::FireUp;
            commands.emplace_back( fire );

            Simulation restored( config );
            bool settled = false;
            kf_vec3_t restingPosition{};
            uint32_t settledTick = 0;
            uint32_t impacts = 0;
            uint32_t explosions = 0;
            for( uint32_t tick = 1; tick <= FUSE_TICKS + 2; ++tick )
            {
                simulation.step( commands );
                commands.clear();
                if( settled == true )
                {
                    restored.step( commands );
                    StateChecksum actual = simulation.checksum();
                    StateChecksum expected = restored.checksum();
                    if( actual != expected )
                    {
                        Mengine::StdIO::fprintf( stderr, "Settled grenade snapshot diverged\n" );
                        return false;
                    }
                }
                for( const ServerEvent & event : simulation.events() )
                {
                    if( event.weapon != WeaponType::GrenadeLauncher )
                    {
                        continue;
                    }
                    if( event.type == EventType::Impact )
                    {
                        ++impacts;
                        if( settled == true )
                        {
                            Mengine::StdIO::fprintf( stderr, "Resting grenade emitted another impact\n" );
                            return false;
                        }
                    }
                    if( event.type == EventType::Explosion )
                    {
                        ++explosions;
                        if( tick != FUSE_TICKS )
                        {
                            Mengine::StdIO::fprintf( stderr, "Grenade exploded before its fuse: %u\n", tick );
                            return false;
                        }
                    }
                }
                const SimulationState & state = simulation.state();
                for( const ProjectileState & projectile : state.projectiles )
                {
                    if( projectile.active == false )
                    {
                        continue;
                    }
                    if( settled == true )
                    {
                        bool moved = projectile.position.x != restingPosition.x || projectile.position.y != restingPosition.y || projectile.position.z != restingPosition.z;
                        if( moved == true )
                        {
                            Mengine::StdIO::fprintf( stderr, "Resting grenade moved\n" );
                            return false;
                        }
                    }
                    else if( projectile.gravity == 0 )
                    {
                        settled = true;
                        settledTick = tick;
                        restingPosition = projectile.position;
                        Mengine::Data snapshot = simulation.snapshot();
                        bool restoredSuccessfully = restored.restoreSnapshot( snapshot );
                        if( restoredSuccessfully == false )
                        {
                            return false;
                        }
                    }
                }
            }
            Mengine::StdIO::printf( "grenade speed=%d yaw=%u: settled=%u impacts=%u explosions=%u\n", _speed, _yaw, settledTick, impacts, explosions );
            bool successful = settled == true && settledTick < FUSE_TICKS - 30 && impacts > 0 && impacts < 12 && explosions == 1;
            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
int main()
{
    bool drop = Arena3D::Detail::testGrenade( 0, 0 );
    bool throwForward = Arena3D::Detail::testGrenade( 8, 0 );
    bool throwSideways = Arena3D::Detail::testGrenade( 18, 16384 );
    bool successful = drop == true && throwForward == true && throwSideways == true;
    return successful == true ? 0 : 1;
}
