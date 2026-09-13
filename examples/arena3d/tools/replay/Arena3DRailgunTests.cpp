#include "Arena3DSimulation.h"

#include "Arena3DReplay.h"

#include "Arena3DGeneratedCollision.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void command( Simulation * _simulation, CommandType _type )
        {
            CommandEnvelope command;
            command.matchId = 1;
            command.playerId = 1;
            command.type = _type;
            command.payload.weapon = WeaponType::Railgun;
            VectorCommandEnvelope commands;
            commands.emplace_back( command );
            _simulation->step( commands );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testClick()
        {
            SimulationConfig config = SimulationConfig::defaults();
            size_t railIndex = static_cast<size_t>(WeaponType::Railgun);
            if( config.weapons[railIndex].chargeTicks != 0 )
            {
                Mengine::StdIO::fprintf( stderr, "Default Railgun requires a charge instead of firing on click\n" );
                return false;
            }

            Simulation simulation( config );
            simulation.reset( 1, 123 );
            simulation.addPlayer( 2 );
            simulation.addPlayer( 3 );
            PlayerState * shooter = simulation.findPlayer( 1 );
            PlayerState * target = simulation.findPlayer( 2 );
            PlayerState * occluded = simulation.findPlayer( 3 );
            shooter->position = {565248, 589824, 1081344};
            shooter->yaw = 0;
            shooter->pitch = 0;
            shooter->noclip = true;
            target->position = {565248, 589824, 1310720};
            target->noclip = true;
            kf_vec3_t origin = shooter->position;
            kf_fixed_t eyeHeight = kf_fixed_from_ratio( 16, 10 );
            origin.y = kf_fixed_add( origin.y, eyeHeight );
            kf_vec3_t direction{0, 0, KF_FIXED_SCALE};
            kf_fixed_t range = kf_fixed_from_int( 80 );
            kf_ray_t ray{origin, direction, range};
            kf_hit_t wall{};
            if( kf_world_raycast( Arena3DGenerated::CollisionColliders, Arena3DGenerated::CollisionColliderCount, &ray, &wall ) == KF_FALSE )
            {
                Mengine::StdIO::fprintf( stderr, "Railgun test requires a wall beyond the target\n" );
                return false;
            }
            occluded->position = wall.position;
            occluded->position.y = shooter->position.y;
            kf_fixed_t wallClearance = kf_fixed_from_int( 2 );
            occluded->position.z = kf_fixed_add( wall.position.z, wallClearance );
            occluded->noclip = true;
            uint32_t ammo = shooter->ammo[railIndex];

            Detail::command( &simulation, CommandType::SelectWeapon );
            Detail::command( &simulation, CommandType::FireDown );
            uint32_t shots = 0;
            uint32_t hits = 0;
            for( const ServerEvent & event : simulation.events() )
            {
                if( event.weapon != WeaponType::Railgun )
                {
                    continue;
                }
                if( event.type == EventType::Shot )
                {
                    ++shots;
                    if( event.position.x != origin.x || event.position.y != origin.y || event.position.z != origin.z ||
                        event.endPosition.x != wall.position.x || event.endPosition.y != wall.position.y || event.endPosition.z != wall.position.z )
                    {
                        Mengine::StdIO::fprintf( stderr, "Railgun trail does not end at the blocking wall\n" );
                        return false;
                    }
                }
                if( event.type == EventType::Hit && event.targetId == 2 )
                {
                    ++hits;
                }
            }
            if( shots != 1 || hits != 1 || target->alive == true || occluded->health != 100 || shooter->ammo[railIndex] != ammo - 1 )
            {
                Mengine::StdIO::fprintf( stderr, "Railgun click failed: shots=%u hits=%u target=%d occluded=%d ammo=%u\n", shots, hits, target->health, occluded->health, shooter->ammo[railIndex] );
                return false;
            }

            VectorCommandEnvelope empty;
            uint32_t cooldown = config.weapons[railIndex].cooldownTicks;
            for( uint32_t tick = 0; tick != cooldown + 2; ++tick )
            {
                simulation.step( empty );
            }
            if( shooter->ammo[railIndex] != ammo - 1 )
            {
                Mengine::StdIO::fprintf( stderr, "Railgun repeated a consumed click while held\n" );
                return false;
            }

            Detail::command( &simulation, CommandType::FireUp );
            Detail::command( &simulation, CommandType::FireDown );
            Detail::command( &simulation, CommandType::FireUp );
            Detail::command( &simulation, CommandType::FireDown );
            Detail::command( &simulation, CommandType::FireUp );
            bool successful = shooter->ammo[railIndex] == ammo - 2;
            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testTrailReplay( const Mengine::String & _path )
        {
            SimulationConfig config = SimulationConfig::defaults();
            Simulation simulation( config );
            simulation.reset( 1, 789 );
            PlayerState * player = simulation.findPlayer( 1 );
            player->position = {0, 6553600, 0};
            player->yaw = 0;
            player->pitch = 0;
            player->noclip = true;
            Detail::command( &simulation, CommandType::SelectWeapon );
            Detail::command( &simulation, CommandType::FireDown );
            const VectorServerEvent & events = simulation.events();
            if( events.size() != 1 || events[0].type != EventType::Shot )
            {
                return false;
            }
            kf_vec3_t delta = kf_vec3_sub( events[0].endPosition, events[0].position );
            kf_fixed_t maximumRange = kf_fixed_from_int( 80 );
            if( delta.x != 0 || delta.y != 0 || delta.z != maximumRange )
            {
                Mengine::StdIO::fprintf( stderr, "Unobstructed rail trail did not reach maximum range\n" );
                return false;
            }
            VectorCommandEnvelope journal;
            ReplayRecord replay = makeReplayRecord( simulation, 789, journal, events );
            if( writeReplay( _path, replay ) == false )
            {
                return false;
            }
            ReplayRecord restored;
            bool read = readReplay( _path, &restored );
            if( read == false || restored.events.size() != 1 )
            {
                return false;
            }
            const kf_vec3_t & end = restored.events[0].endPosition;
            const kf_vec3_t & expected = events[0].endPosition;
            bool successful = end.x == expected.x && end.y == expected.y && end.z == expected.z;
            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testCharge()
        {
            SimulationConfig config = SimulationConfig::defaults();
            size_t railIndex = static_cast<size_t>(WeaponType::Railgun);
            constexpr uint32_t chargeTicks = 4;
            config.weapons[railIndex].chargeTicks = chargeTicks;
            Simulation simulation( config );
            simulation.reset( 1, 456 );
            PlayerState * player = simulation.findPlayer( 1 );
            uint32_t ammo = player->ammo[railIndex];
            Detail::command( &simulation, CommandType::SelectWeapon );
            Detail::command( &simulation, CommandType::FireDown );
            Detail::command( &simulation, CommandType::FireUp );
            if( player->ammo[railIndex] != ammo )
            {
                return false;
            }

            Detail::command( &simulation, CommandType::FireDown );
            VectorCommandEnvelope empty;
            for( uint32_t tick = 1; tick < chargeTicks - 1; ++tick )
            {
                simulation.step( empty );
            }
            if( player->ammo[railIndex] != ammo )
            {
                return false;
            }
            simulation.step( empty );
            bool successful = player->ammo[railIndex] == ammo - 1;
            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
int main( int _argc, char ** _argv )
{
    if( _argc != 2 )
    {
        return 1;
    }
    Mengine::String replayPath = _argv[1];
    bool click = Arena3D::Detail::testClick();
    bool charge = Arena3D::Detail::testCharge();
    bool trailReplay = Arena3D::Detail::testTrailReplay( replayPath );
    Mengine::StdIO::printf( "Railgun click=%d charge=%d trailReplay=%d\n", click, charge, trailReplay );
    bool successful = click == true && charge == true && trailReplay == true;
    return successful == true ? 0 : 1;
}
