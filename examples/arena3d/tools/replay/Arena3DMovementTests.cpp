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
            player->position = _ascending == true ? kf_vec3_t{884736, 262144, 524288} : kf_vec3_t{884736, 589824, 1245184};
            player->velocity = {};
            player->yaw = _ascending == true ? 0 : 32768;
            player->inputMask = 1u << static_cast<uint32_t>(InputButton::Forward);
            player->grounded = true;
            VectorCommandEnvelope empty;
            for( uint32_t tick = 0; tick != 160; ++tick )
            {
                simulation.step( empty );
                kf_capsule_t capsule{player->position, config.playerRadius, config.standingHeight};
                if( kf_world_overlap_capsule( Arena3DGenerated::CollisionColliders, Arena3DGenerated::CollisionColliderCount, &capsule, nullptr ) == KF_TRUE )
                {
                    Mengine::StdIO::fprintf( stderr, "Stair movement entered solid geometry at tick %u\n", tick );
                    return false;
                }
            }
            float x = kf_fixed_to_float( player->position.x );
            float y = kf_fixed_to_float( player->position.y );
            float z = kf_fixed_to_float( player->position.z );
            Mengine::StdIO::printf( "stairs ascending=%d position=(%.3f, %.3f, %.3f) grounded=%d\n", _ascending, x, y, z, player->grounded );
            bool reached = _ascending == true ? y >= 8.99f && z > 18.f : y <= 4.01f && z < 9.f;
            bool successful = reached == true && player->grounded == true;
            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testStepLimits()
        {
            SimulationConfig config = SimulationConfig::defaults();
            kf_fixed_t tickSeconds = kf_fixed_from_ratio( 1, 100 );
            kf_fixed_t supportEpsilon = kf_fixed_from_ratio( 2, 100 );
            kf_character_config_t characterConfig{config.playerRadius, config.standingHeight, config.stepHeight,
                config.gravity, tickSeconds, supportEpsilon, config.arenaHalfExtent};
            kf_fixed_t tallStep = kf_fixed_from_ratio( 3, 4 );
            kf_collider_t colliders[] = {
                {1, {{-655360, -65536, -655360}, {655360, 0, 655360}}},
                {2, {{65536, 0, -65536}, {131072, tallStep, 65536}}}
            };
            kf_character_body_t body{{0, 0, 0}, {524288, 0, 0}, KF_TRUE};
            for( uint32_t tick = 0; tick != 50; ++tick )
            {
                body.velocity.x = 524288;
                kf_character_result_t result{};
                kf_character_step( &body, &characterConfig, colliders, 2, &result );
                if( result.stepped == KF_TRUE || body.position.y != 0 )
                {
                    return false;
                }
            }
            colliders[1].bounds.maximum.y = KF_FIXED_SCALE / 2;
            body = {{0, 0, 0}, {524288, 0, 0}, KF_FALSE};
            for( uint32_t tick = 0; tick != 15; ++tick )
            {
                body.grounded = KF_FALSE;
                body.velocity = {524288, 0, 0};
                kf_character_result_t result{};
                kf_character_step( &body, &characterConfig, colliders, 2, &result );
                if( result.stepped == KF_TRUE )
                {
                    return false;
                }
            }
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
int main()
{
    bool ascending = Arena3D::Detail::testMapStairs( true );
    bool descending = Arena3D::Detail::testMapStairs( false );
    bool limits = Arena3D::Detail::testStepLimits();
    bool successful = ascending == true && descending == true && limits == true;
    return successful == true ? 0 : 1;
}
