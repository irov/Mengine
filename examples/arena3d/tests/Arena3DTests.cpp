#include "Arena3DFixed.h"
#include "Arena3DProtocol.h"
#include "Arena3DReplay.h"
#include "Arena3DSession.h"
#include "Arena3DSimulation.h"

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace
{
    int g_failures = 0;

    void expect( bool condition, const char * expression, int line )
    {
        if( condition == true ) return;
        std::cerr << "FAILED line " << line << ": " << expression << '\n';
        ++g_failures;
    }

#define EXPECT(Value) expect( (Value), #Value, __LINE__ )

    void testFixed()
    {
        using namespace Arena3D;
        FaultState fault;
        FixedFaultScope scope( &fault );
        const Fixed oneAndHalf = Fixed::fromDecimal( "1.5" );
        const Fixed negative = Fixed::fromDecimal( "-2.25" );
        EXPECT( oneAndHalf.raw() == Fixed::Scale + Fixed::Scale / 2 );
        EXPECT( (oneAndHalf * negative).raw() == Fixed::fromDecimal( "-3.375" ).raw() );
        EXPECT( (negative / oneAndHalf).raw() == Fixed::fromDecimal( "-1.5" ).raw() );
        EXPECT( fixedSqrt( Fixed::fromInt( 9 ) ).raw() == Fixed::fromInt( 3 ).raw() );
        const FixedVec3 normalized = fixedNormalize( {Fixed::fromInt( 3 ), Fixed::fromInt( 4 ), Fixed::zero()} );
        EXPECT( std::abs( fixedLength( normalized ).toDouble() - 1.0 ) < 0.00001 );
        Fixed sine;
        Fixed cosine;
        fixedSinCos( Fixed::pi() / Fixed::fromInt( 4 ), &sine, &cosine );
        EXPECT( std::abs( sine.toDouble() - std::sqrt( 0.5 ) ) < 0.00001 );
        EXPECT( std::abs( cosine.toDouble() - std::sqrt( 0.5 ) ) < 0.00001 );
        const FixedAngle quarter = fixedAngleFromRadians( Fixed::halfPi() );
        EXPECT( quarter == KF_ANGLE16_QUARTER );
        EXPECT( fixedAngleFromRadians( Fixed::twoPi() ) == 0 );
        EXPECT( fixedAngleAdd( UINT16_C(65530), INT16_C(10) ) == UINT16_C(4) );
        fixedAngleSinCos( quarter, &sine, &cosine );
        EXPECT( sine == Fixed::one() && cosine == Fixed::zero() );
        const FixedSignedAngle maximumPitch = fixedSignedAngleFromRadians( Fixed::fromDecimal( "1.5533430342749532" ) );
        EXPECT( fixedSignedAngleAddClamped( maximumPitch, INT16_C(100),
            static_cast<FixedSignedAngle>(-maximumPitch), maximumPitch ) == maximumPitch );
        EXPECT( fault.failed() == false );
    }

    void testProtocol()
    {
        using namespace Arena3D;
        CanonicalWriter writer;
        writer.writeU32( UINT32_C(0x78563412) );
        EXPECT( writer.data().size() == 4 );
        EXPECT( writer.data()[0] == 0x12 && writer.data()[1] == 0x34 && writer.data()[2] == 0x56 && writer.data()[3] == 0x78 );
        writer.clear();
        writer.writeI16( INT16_C(-2) );
        EXPECT( writer.data().size() == 2 && writer.data()[0] == 0xFE && writer.data()[1] == 0xFF );
        EXPECT( crc32( "123456789", 9 ) == UINT32_C(0xCBF43926) );

        CommandEnvelope command;
        command.matchId = 44;
        command.playerId = 3;
        command.sequence = 9;
        command.sourceTimeMs = 101;
        command.appliedTick = 11;
        command.type = CommandType::LookDelta;
        command.payload.lookYaw = fixedSignedAngleFromRadians( Fixed::fromDecimal( "0.125" ) );
        command.payload.lookPitch = fixedSignedAngleFromRadians( Fixed::fromDecimal( "-0.25" ) );
        writer.clear();
        serializeCommand( &writer, command );
        EXPECT( writer.data().size() == 50 );
        CanonicalReader reader( writer.data() );
        CommandEnvelope decoded;
        EXPECT( deserializeCommand( &reader, &decoded ) );
        EXPECT( reader.remaining() == 0 );
        EXPECT( decoded.matchId == command.matchId && decoded.sequence == command.sequence && decoded.payload.lookYaw == command.payload.lookYaw );

        Pcg32 first( 42, 54 );
        Pcg32 second( 42, 54 );
        for( uint32_t index = 0; index != 1000; ++index ) EXPECT( first.next() == second.next() );
    }

    Arena3D::CommandPayload button( Arena3D::InputButton value )
    {
        Arena3D::CommandPayload payload;
        payload.button = value;
        return payload;
    }

    void testSnapshotAndFaultDetection()
    {
        using namespace Arena3D;
        Simulation simulation;
        simulation.reset( 7, 99 );
        const std::vector<uint8_t> initial = simulation.snapshot();
        CommandEnvelope command;
        command.matchId = 7;
        command.playerId = 1;
        command.sequence = 1;
        command.type = CommandType::ButtonDown;
        command.payload = button( InputButton::Forward );
        EXPECT( simulation.step( {command} ) );
        EXPECT( simulation.state().tick == 1 );
        const StateChecksum changed = simulation.checksum();
        EXPECT( simulation.restoreSnapshot( initial ) );
        EXPECT( simulation.state().tick == 0 );
        EXPECT( simulation.checksum() != changed );

        std::vector<uint8_t> corrupt = initial;
        corrupt.back() ^= 1;
        EXPECT( simulation.restoreSnapshot( corrupt ) == false );
    }

    void testLocalSessionAndReplay()
    {
        using namespace Arena3D;
        constexpr uint64_t seed = UINT64_C(0x1020304050607080);
        LocalSession session;
        session.reset( 12, seed );
        session.submit( session.makeCommand( CommandType::ButtonDown, 0, button( InputButton::Forward ) ) );
        session.submit( session.makeCommand( CommandType::FireDown, 0 ) );
        CommandPayload look;
        look.lookYaw = fixedSignedAngleFromRadians( Fixed::fromDecimal( "0.01" ) );
        look.lookPitch = fixedSignedAngleFromRadians( Fixed::fromDecimal( "-0.002" ) );
        session.submit( session.makeCommand( CommandType::LookDelta, 0, look ) );
        EXPECT( session.advanceTicks( 250 ) );
        session.submit( session.makeCommand( CommandType::FireUp, 2500 ) );
        session.submit( session.makeCommand( CommandType::ButtonUp, 2500, button( InputButton::Forward ) ) );
        EXPECT( session.advanceTicks( 50 ) );
        EXPECT( session.synchronized() );
        EXPECT( session.server().checksum() == session.client().checksum() );

        session.injectClientFaultForTest( 1 );
        EXPECT( session.server().checksum() != session.client().checksum() );
        EXPECT( session.tick() );
        EXPECT( session.reconciliationCount() == 1 );
        EXPECT( session.synchronized() );

        ReplayRecord record = makeReplayRecord( session.server(), seed, session.journal(), session.eventJournal() );
        const std::filesystem::path replayPath = std::filesystem::temp_directory_path() / "arena3d-test.a3r";
        EXPECT( writeReplay( replayPath, record ) );
        ReplayRecord loaded;
        EXPECT( readReplay( replayPath, &loaded ) );
        Simulation replayed;
        StateChecksum replayChecksum;
        EXPECT( runReplay( loaded, &replayed, &replayChecksum ) );
        EXPECT( replayChecksum == record.finalChecksum );
        Simulation faulted;
        EXPECT( runReplay( loaded, &faulted, nullptr, 100 ) == false );
        Simulation expected;
        EXPECT( expected.restoreSnapshot( loaded.checkpoints.back().snapshot ) );
        EXPECT( describeFirstDifference( expected.state(), faulted.state() ).empty() == false );
        std::error_code error;
        std::filesystem::remove( replayPath, error );
    }

    void testRailgunChargeAndFpsIndependence()
    {
        using namespace Arena3D;
        SimulationConfig railConfig = SimulationConfig::defaults();
        for( WeaponConfig & weaponConfig : railConfig.weapons ) weaponConfig.damage = 0;
        LocalSession session( railConfig );
        session.reset( 20, 20260714 );
        CommandPayload weapon;
        weapon.weapon = WeaponType::Railgun;
        session.submit( session.makeCommand( CommandType::SelectWeapon, 0, weapon ) );
        session.submit( session.makeCommand( CommandType::FireDown, 0 ) );
        EXPECT( session.advanceTicks( 150 ) );
        bool railShot = false;
        for( const ServerEvent & event : session.server().events() )
        {
            railShot |= event.type == EventType::Shot && event.actorId == 1 && event.weapon == WeaponType::Railgun;
        }
        EXPECT( railShot );

        auto runAtFps = []( uint32_t fps )
        {
            LocalSession local;
            local.reset( 33, 1001 );
            local.submit( local.makeCommand( CommandType::ButtonDown, 0, button( InputButton::Right ) ) );
            uint64_t accumulated = 0;
            uint64_t ticks = 0;
            while( ticks < 500 )
            {
                accumulated += 1000;
                const uint64_t due = accumulated * 100u / fps;
                while( ticks < due && ticks < 500 ) { EXPECT( local.tick() ); ++ticks; }
            }
            return local.server().checksum();
        };
        const StateChecksum reference = runAtFps( 30 );
        EXPECT( reference == runAtFps( 60 ) );
        EXPECT( reference == runAtFps( 144 ) );
        EXPECT( reference == runAtFps( 240 ) );
    }

    void testCommandValidationAndCollision()
    {
        using namespace Arena3D;
        SimulationConfig safeConfig = SimulationConfig::defaults();
        for( WeaponConfig & weaponConfig : safeConfig.weapons )
        {
            weaponConfig.damage = 0;
            weaponConfig.knockback = Fixed::zero();
        }

        LocalSession paused( safeConfig );
        paused.reset( 41, 4001 );
        paused.submit( paused.makeCommand( CommandType::Pause, 0 ) );
        EXPECT( paused.tick() );
        EXPECT( paused.server().state().paused );
        EXPECT( paused.server().state().tick == 0 );
        paused.submit( paused.makeCommand( CommandType::ButtonDown, 0, button( InputButton::Forward ) ) );
        EXPECT( paused.tick() );
        EXPECT( paused.server().state().tick == 0 );
        EXPECT( paused.server().state().player.inputMask == 0 );
        bool rejectedWhilePaused = false;
        for( const ServerEvent & event : paused.server().events() ) rejectedWhilePaused |= event.type == EventType::CommandRejected;
        EXPECT( rejectedWhilePaused );
        paused.submit( paused.makeCommand( CommandType::Resume, 0 ) );
        EXPECT( paused.tick() );
        EXPECT( paused.server().state().paused == false );
        EXPECT( paused.server().state().tick == 1 );

        LocalSession contact( safeConfig );
        contact.reset( 42, 4002 );
        contact.submit( contact.makeCommand( CommandType::ButtonDown, 0, button( InputButton::Forward ) ) );
        bool penetratedCover = false;
        for( uint32_t tick = 0; tick != 350; ++tick )
        {
            EXPECT( contact.tick() );
            const FixedVec3 position = contact.server().state().player.position;
            const bool insideX = position.x > Fixed::fromDecimal( "-12.35" ) && position.x < Fixed::fromDecimal( "-7.65" );
            const bool insideZ = position.z > Fixed::fromDecimal( "-12.35" ) && position.z < Fixed::fromDecimal( "-7.65" );
            penetratedCover |= insideX && insideZ && position.y < Fixed::fromDecimal( "2.5" );
        }
        const FixedVec3 contactPosition = contact.server().state().player.position;
        EXPECT( contactPosition.x > Fixed::fromInt( -24 ) && contactPosition.z > Fixed::fromInt( -24 ) );
        EXPECT( penetratedCover == false );

        SimulationConfig sweepConfig = safeConfig;
        sweepConfig.weapons[static_cast<size_t>(WeaponType::Nailgun)].speed = Fixed::fromInt( 4000 );
        Simulation sweep( sweepConfig );
        sweep.reset( 43, 4003 );
        CommandEnvelope look;
        look.matchId = 43;
        look.playerId = 1;
        look.sequence = 1;
        look.type = CommandType::LookDelta;
        look.payload.lookPitch = fixedSignedAngleFromRadians( Fixed::fromDecimal( "-0.02" ) );
        CommandEnvelope fire = look;
        fire.sequence = 2;
        fire.type = CommandType::FireDown;
        fire.payload = {};
        EXPECT( sweep.step( {fire, look} ) );
        size_t activeProjectiles = 0;
        for( const ProjectileState & projectile : sweep.state().projectiles ) activeProjectiles += projectile.active ? 1u : 0u;
        EXPECT( activeProjectiles == 0 );

        const uint64_t oldMatch = paused.server().state().matchId;
        paused.submit( paused.makeCommand( CommandType::Restart, 10 ) );
        EXPECT( paused.tick() );
        EXPECT( paused.server().state().matchId == oldMatch + 1 );
        EXPECT( paused.server().state().projectiles[0].active == false );
    }

    void testWeaponArchetypes()
    {
        using namespace Arena3D;
        auto fire = []( WeaponType weapon )
        {
            SimulationConfig config = SimulationConfig::defaults();
            for( WeaponConfig & weaponConfig : config.weapons ) weaponConfig.damage = 0;
            Simulation simulation( config );
            simulation.reset( 55, 5500 );
            CommandEnvelope select;
            select.matchId = 55; select.playerId = 1; select.sequence = 1; select.type = CommandType::SelectWeapon; select.payload.weapon = weapon;
            CommandEnvelope trigger = select;
            trigger.sequence = 2; trigger.type = CommandType::FireDown; trigger.payload = {};
            EXPECT( simulation.step( {trigger, select} ) );
            return simulation;
        };

        for( WeaponType weapon : {WeaponType::Nailgun, WeaponType::RocketLauncher, WeaponType::Plasmagun, WeaponType::GrenadeLauncher, WeaponType::Shotgun} )
        {
            Simulation simulation = fire( weapon );
            size_t count = 0;
            for( const ProjectileState & projectile : simulation.state().projectiles )
                if( projectile.active == true && projectile.weapon == weapon ) ++count;
            EXPECT( count == (weapon == WeaponType::Shotgun ? 12u : 1u) );
        }

        Simulation cold = fire( WeaponType::Nailgun );
        const ProjectileState * coldNail = nullptr;
        for( const ProjectileState & projectile : cold.state().projectiles ) if( projectile.active == true ) { coldNail = &projectile; break; }
        EXPECT( coldNail != nullptr );
        if( coldNail != nullptr )
        {
            EXPECT( fixedAbs( fixedLength( coldNail->velocity ) - Fixed::fromInt( 60 ) ) < Fixed::fromDecimal( "0.0001" ) );
            EXPECT( coldNail->shotId == 256u );
        }

        Simulation firstShotgun = fire( WeaponType::Shotgun );
        Simulation secondShotgun = fire( WeaponType::Shotgun );
        for( size_t index = 0; index != MaximumProjectiles; ++index )
        {
            EXPECT( firstShotgun.state().projectiles[index].velocity.x == secondShotgun.state().projectiles[index].velocity.x );
            EXPECT( firstShotgun.state().projectiles[index].velocity.y == secondShotgun.state().projectiles[index].velocity.y );
            EXPECT( firstShotgun.state().projectiles[index].velocity.z == secondShotgun.state().projectiles[index].velocity.z );
        }
    }

    void testGroundSlide()
    {
        using namespace Arena3D;
        SimulationConfig config = SimulationConfig::defaults();
        for( WeaponConfig & weaponConfig : config.weapons )
        {
            weaponConfig.damage = 0;
            weaponConfig.knockback = Fixed::zero();
        }
        LocalSession session( config );
        session.reset( 56, 5600 );
        session.submit( session.makeCommand( CommandType::ButtonDown, 0, button( InputButton::Forward ) ) );
        EXPECT( session.advanceTicks( 100 ) );
        const PlayerState & running = session.server().state().player;
        const Fixed runningSpeed = fixedSqrt( running.velocity.x * running.velocity.x + running.velocity.z * running.velocity.z );
        EXPECT( running.grounded );
        EXPECT( runningSpeed >= config.slideMinimumSpeed );

        session.submit( session.makeCommand( CommandType::ButtonDown, 1000, button( InputButton::Crouch ) ) );
        EXPECT( session.tick() );
        const PlayerState & sliding = session.server().state().player;
        const Fixed slidingSpeed = fixedSqrt( sliding.velocity.x * sliding.velocity.x + sliding.velocity.z * sliding.velocity.z );
        EXPECT( sliding.crouched );
        EXPECT( slidingSpeed > Fixed::fromInt( 8 ) * config.crouchedSpeedScale );
        EXPECT( slidingSpeed <= config.slideMaximumSpeed );
    }
}

int main()
{
    testFixed();
    testProtocol();
    testSnapshotAndFaultDetection();
    testLocalSessionAndReplay();
    testRailgunChargeAndFpsIndependence();
    testCommandValidationAndCollision();
    testWeaponArchetypes();
    testGroundSlide();
    if( g_failures != 0 )
    {
        std::cerr << g_failures << " Arena3D test(s) failed\n";
        return 1;
    }
    std::cout << "Arena3D deterministic tests passed\n";
    return 0;
}
