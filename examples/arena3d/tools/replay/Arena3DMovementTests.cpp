#include "Arena3DSimulation.h"

#include "Arena3DGeneratedCollision.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool testMapStairs( bool _ascending )
        {
            SimulationConfig config = SimulationConfig::defaults();
            Simulation simulation( config );
            simulation.reset( 1, 123 );
            PlayerState * player = simulation.findPlayer( 1 );
            // The lower hall staircase rises from y=4 to y=9 along positive Z.
            player->character.position = _ascending == true ? kf_vec3_t{884736, 262144, 524288} : kf_vec3_t{884736, 589824, 1245184};
            player->character.velocity = {};
            player->yaw = _ascending == true ? 0 : 32768;
            player->inputMask = 1u << static_cast<uint32_t>(InputButton::Forward);
            player->character.grounded = true;
            CollisionWorld world;
            VectorCommandEnvelope empty;
            for( uint32_t tick = 0; tick != 160; ++tick )
            {
                simulation.step( empty );
                if( tick == 23 || tick == 79 )
                {
                    Simulation restored( config );
                    if( restored.restoreSnapshot( simulation.snapshot() ) == false )
                    {
                        return false;
                    }
                    Simulation copied = simulation;
                    Simulation assigned( config );
                    assigned = simulation;
                    for( uint32_t continuation = 0; continuation != 25; ++continuation )
                    {
                        restored.step( empty );
                        copied.step( empty );
                        assigned.step( empty );
                        if( restored.checksum() != copied.checksum() || restored.checksum() != assigned.checksum() )
                        {
                            return false;
                        }
                    }
                }
                kf_shape_t shape = makeCapsuleShape( config.playerRadius, config.standingHeight );
                kf_pose_t pose{player->character.position, {0, 0, 0}};
                kf_contact_t contacts[64];
                size_t count = 0;
                kf_status_t status = kf_world_contacts( world.getWorld(), &shape, pose, nullptr, contacts, 64, &count );
                if( status != KF_OK && status != KF_NO_HIT )
                {
                    return false;
                }
                for( size_t index = 0; index != count; ++index )
                {
                    if( contacts[index].depth <= KF_GEOMETRY_TOLERANCE )
                    {
                        continue;
                    }
                    Mengine::StdIO::fprintf( stderr, "Stair movement entered solid geometry at tick %u\n", tick );
                    return false;
                }
            }
            float x = kf_fixed_to_float( player->character.position.x );
            float y = kf_fixed_to_float( player->character.position.y );
            float z = kf_fixed_to_float( player->character.position.z );
            Mengine::StdIO::printf( "stairs ascending=%d position=(%.3f, %.3f, %.3f) grounded=%d\n", _ascending, x, y, z, player->character.grounded );
            bool reached = _ascending == true ? y >= 8.99f && z > 18.f : y <= 4.01f && z < 9.f;
            bool successful = reached == true && player->character.grounded == true;
            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
int main()
{
    bool ascending = Arena3D::Detail::testMapStairs( true );
    bool descending = Arena3D::Detail::testMapStairs( false );

    return ascending == true && descending == true ? 0 : 1;
}
