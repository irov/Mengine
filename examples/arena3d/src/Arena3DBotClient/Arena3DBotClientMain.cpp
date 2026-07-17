#include "Arena3DHeadless.h"
#include "Arena3DNetworkClient.h"

#include "Interface/SocketServiceInterface.h"

#include "Kernel/String.h"

#include "Config/Atomic.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdIO.h"
#include "Config/StdLib.h"
#include "Config/StdMath.h"
#include "Config/StdSignal.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
int main( int argc, char ** argv )
{
    static Mengine::AtomicBool running{true};

    const auto stopBot = []( int )
    {
        running = false;
    };
    const auto argument = [argc, argv]( const char * name, const char * fallback ) -> const char *
    {
        for( int index = 1; index + 1 < argc; ++index )
        {
            if( Mengine::StdString::strcmp( argv[index], name ) == 0 )
            {
                return argv[index + 1];
            }
        }
        return fallback;
    };
    const auto button = []( Arena3D::InputButton value )
    {
        Arena3D::CommandPayload payload;
        payload.button = value;
        return payload;
    };

    Arena3D::HeadlessEngine engine;
    if( engine.initialize() == false )
    {
        Mengine::StdIO::fprintf( stderr, "Arena3DBotClient: Mengine headless bootstrap failed\n" );
        return EXIT_FAILURE;
    }

    const char * host = argument( "--host", "127.0.0.1" );
    const uint16_t port = static_cast<uint16_t>(Mengine::StdLib::strtoul( argument( "--port", "27960" ), nullptr, 10 ));
    const Mengine::String name = argument( "--name", "bot" );
    const uint64_t maximumTicks = Mengine::StdLib::strtoull( argument( "--ticks", "0" ), nullptr, 10 );
    Mengine::SocketAddress address;
    if( engine.resolve( host, port, &address ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Arena3DBotClient: cannot resolve server\n" );
        engine.finalize();
        return EXIT_FAILURE;
    }

    Arena3D::NetworkClient client( SOCKET_SERVICE() );
    if( client.connect( address, name, Arena3D::NetworkClientKind::Bot ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Arena3DBotClient: connect failed\n" );
        engine.finalize();
        return EXIT_FAILURE;
    }

    Mengine::StdSignal::signal( SIGINT, stopBot );
    Mengine::StdSignal::signal( SIGTERM, stopBot );
    const uint64_t started = engine.getTimeMilliseconds();
    uint64_t nextTick = started;
    bool jumpHeld = false;
    bool fireHeld = false;
    bool wasAlive = false;
    bool hasSpawned = false;
    bool positionInitialized = false;
    kf_vec3_t previousPosition;
    kf_angle16_t avoidYaw = 0;
    uint64_t avoidUntilTick = 0;
    uint64_t lastStateTick = UINT64_MAX;
    uint32_t stuckStateFrames = 0;
    uint32_t observedRespawns = 0;
    uint32_t weaponSwitches = 0;
    double traveledDistance = 0.0;
    uint32_t nameHash = 2166136261u;
    for( char character : name )
    {
        nameHash = ( nameHash ^ static_cast<uint8_t>(character) ) * 16777619u;
    }
    const uint32_t botSeed = nameHash;
    const Arena3D::InputButton strafeButton = (nameHash & 1u) == 0u ? Arena3D::InputButton::Left : Arena3D::InputButton::Right;
    uint64_t nextWeaponSwitchTick = 0;
    uint64_t localTick = 0;
    Arena3D::NetworkClientState previousState = client.state();
    while( running == true && (maximumTicks == 0 || localTick < maximumTicks) )
    {
        nextTick += Arena3D::TickMilliseconds;
        client.updateNetwork();
        if( client.state() != previousState )
        {
            Mengine::StdIO::printf( "Arena3DBotClient %s state=%u player=%u\n", name.c_str(), static_cast<unsigned int>(client.state()), client.playerId() );
            previousState = client.state();
        }
        const uint64_t sourceTime = engine.getTimeMilliseconds() - started;

        if( client.state() == Arena3D::NetworkClientState::Connected )
        {
            const Arena3D::PlayerState * self = client.simulation().findPlayer( client.playerId() );
            const Arena3D::PlayerState * target = nullptr;
            if( self != nullptr && self->alive == true )
            {
                kf_fixed_t best = kf_fixed_from_int( 2047 );
                for( const Arena3D::PlayerState & candidate : client.simulation().state().players )
                {
                    if( candidate.active == false || candidate.alive == false || candidate.id == self->id )
                    {
                        continue;
                    }
                    const kf_fixed_t distance = kf_vec3_length_squared( kf_vec3_sub( candidate.position, self->position ) );
                    if( target == nullptr || distance < best || ( distance == best && candidate.id < target->id ) )
                    {
                        target = &candidate;
                        best = distance;
                    }
                }
            }

            const uint64_t stateTick = client.simulation().state().tick;
            const bool stateAdvanced = stateTick != lastStateTick;
            if( stateAdvanced == true )
            {
                lastStateTick = stateTick;
            }
            const bool decisionTick = stateAdvanced == true && stateTick % Arena3D::NetworkStateFrameTicks == 0u;

            if( self != nullptr && self->alive == true && wasAlive == false )
            {
                if( hasSpawned == true )
                {
                    ++observedRespawns;
                }
                hasSpawned = true;
                client.sendCommand( Arena3D::CommandType::ButtonDown, sourceTime, button( Arena3D::InputButton::Forward ) );
                client.sendCommand( Arena3D::CommandType::ButtonDown, sourceTime, button( strafeButton ) );
                jumpHeld = false;
                fireHeld = false;
                positionInitialized = false;
                stuckStateFrames = 0;
                avoidUntilTick = 0;
                const size_t weaponCount = Arena3D::WeaponTypeCount;
                const size_t spawnWeaponIndex = (static_cast<size_t>(botSeed) + observedRespawns) % weaponCount;
                Arena3D::CommandPayload weapon;
                weapon.weapon = static_cast<Arena3D::WeaponType>(spawnWeaponIndex);
                client.sendCommand( Arena3D::CommandType::SelectWeapon, sourceTime, weapon );
                ++weaponSwitches;
                nextWeaponSwitchTick = stateTick + 300u + static_cast<uint64_t>(botSeed % 151u);
            }
            if( self == nullptr || self->alive == false )
            {
                jumpHeld = false;
                fireHeld = false;
                positionInitialized = false;
            }
            wasAlive = self != nullptr && self->alive == true;

            if( self != nullptr && self->alive == true && decisionTick == true )
            {
                if( stateTick >= nextWeaponSwitchTick )
                {
                    const size_t weaponCount = Arena3D::WeaponTypeCount;
                    const size_t currentWeapon = static_cast<size_t>(self->selectedWeapon);
                    for( size_t offset = 1; offset != weaponCount + 1; ++offset )
                    {
                        const size_t candidate = (currentWeapon + offset) % weaponCount;
                        if( self->ammo[candidate] == 0 )
                        {
                            continue;
                        }
                        Arena3D::CommandPayload weapon;
                        weapon.weapon = static_cast<Arena3D::WeaponType>(candidate);
                        client.sendCommand( Arena3D::CommandType::SelectWeapon, sourceTime, weapon );
                        ++weaponSwitches;
                        break;
                    }
                    nextWeaponSwitchTick = stateTick + 300u + static_cast<uint64_t>((botSeed >> 8) % 201u);
                }

                if( positionInitialized == true )
                {
                    const kf_fixed_t dx = kf_fixed_sub( self->position.x, previousPosition.x );
                    const kf_fixed_t dz = kf_fixed_sub( self->position.z, previousPosition.z );
                    const kf_fixed_t horizontalMovement = kf_vec3_length_squared( {dx, 0, dz} );
                    traveledDistance += Mengine::StdMath::sqrt( kf_fixed_to_double( horizontalMovement ) );
                    stuckStateFrames = horizontalMovement < kf_fixed_from_float( 0.0025f ) ? stuckStateFrames + 1u : 0u;
                }
                previousPosition = self->position;
                positionInitialized = true;
                if( stuckStateFrames >= 6u )
                {
                    const kf_sangle16_t turn = (nameHash & 2u) == 0u ? static_cast<kf_sangle16_t>(16384) : static_cast<kf_sangle16_t>(-16384);
                    avoidYaw = kf_angle16_add( self->yaw, turn );
                    avoidUntilTick = stateTick + 120u;
                    stuckStateFrames = 0;
                    nameHash ^= nameHash << 13;
                    nameHash ^= nameHash >> 17;
                    nameHash ^= nameHash << 5;
                }
            }

            if( self != nullptr && target != nullptr && decisionTick == true )
            {
                const double dx = kf_fixed_to_double( target->position.x ) - kf_fixed_to_double( self->position.x );
                const double dy = kf_fixed_to_double( target->position.y ) - kf_fixed_to_double( self->position.y );
                const double dz = kf_fixed_to_double( target->position.z ) - kf_fixed_to_double( self->position.z );
                const double horizontal = Mengine::StdMath::sqrt( dx * dx + dz * dz );
                const kf_angle16_t targetYaw = kf_angle16_from_fixed_radians( kf_fixed_from_float( static_cast<float>(Mengine::StdMath::atan2( dx, dz )) ) );
                const kf_angle16_t desiredYaw = stateTick < avoidUntilTick ? avoidYaw : targetYaw;
                const kf_sangle16_t desiredPitch = kf_sangle16_from_fixed_radians( kf_fixed_from_float( static_cast<float>(Mengine::StdMath::atan2( dy, horizontal )) ) );
                Arena3D::CommandPayload look;
                const int32_t yawDelta = static_cast<int16_t>(static_cast<uint16_t>(desiredYaw - self->yaw));
                look.lookYaw = static_cast<kf_sangle16_t>(Mengine::StdAlgorithm::clamp<int32_t>( yawDelta, -4096, 4096 ));
                const int32_t pitchDelta = static_cast<int32_t>(desiredPitch) - static_cast<int32_t>(self->pitch);
                look.lookPitch = static_cast<kf_sangle16_t>(Mengine::StdAlgorithm::clamp<int32_t>( pitchDelta, -2048, 2048 ));
                client.sendCommand( Arena3D::CommandType::LookDelta, sourceTime, look );
            }

            const bool wantJump = wasAlive == true && localTick % 180u < 8u;
            if( wantJump != jumpHeld )
            {
                client.sendCommand( wantJump ? Arena3D::CommandType::ButtonDown : Arena3D::CommandType::ButtonUp, sourceTime, button( Arena3D::InputButton::Jump ) );
                jumpHeld = wantJump;
            }
            const bool wantFire = wasAlive == true && target != nullptr && stateTick >= avoidUntilTick && localTick % 45u < 4u;
            if( wantFire != fireHeld )
            {
                client.sendCommand( wantFire ? Arena3D::CommandType::FireDown : Arena3D::CommandType::FireUp, sourceTime );
                fireHeld = wantFire;
            }
            client.tick();
        }
        else if( client.state() == Arena3D::NetworkClientState::Rejected )
        {
            Mengine::StdIO::fprintf( stderr, "Arena3DBotClient rejected: %s\n", client.rejection().c_str() );
            break;
        }

        ++localTick;
        const uint64_t now = engine.getTimeMilliseconds();
        if( nextTick > now )
        {
            engine.sleep( nextTick - now );
        }
    }

    Mengine::StdIO::printf( "Arena3DBotClient %s summary local_ticks=%llu state_tick=%llu traveled=%.2f respawns=%u weapon_switches=%u pending=%zu\n", name.c_str(),
        static_cast<unsigned long long>(localTick), static_cast<unsigned long long>(client.simulation().state().tick), traveledDistance, observedRespawns, weaponSwitches, client.pendingCommandCount() );
    client.disconnect();
    engine.finalize();
    return EXIT_SUCCESS;
}
