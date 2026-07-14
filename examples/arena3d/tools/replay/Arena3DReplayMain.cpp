#include "Arena3DReplay.h"
#include "Arena3DSession.h"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <string>

int main( int argc, char ** argv )
{
    if( argc == 3 && std::string( argv[1] ) == "--soak" )
    {
        uint64_t ticks = 0;
        try { ticks = std::stoull( argv[2] ); }
        catch( const std::exception & ) { return 2; }
        Arena3D::SimulationConfig soakConfig = Arena3D::SimulationConfig::defaults();
        for( Arena3D::WeaponConfig & weapon : soakConfig.weapons )
        {
            weapon.damage = 0;
            weapon.knockback = Arena3D::Fixed::zero();
        }
        Arena3D::WeaponConfig & stressWeapon = soakConfig.weapons[static_cast<size_t>(Arena3D::WeaponType::Nailgun)];
        stressWeapon.cooldownTicks = 1;
        stressWeapon.speed = Arena3D::Fixed::one();
        stressWeapon.gravity = Arena3D::Fixed::zero();
        stressWeapon.lifetimeTicks = 1000;
        Arena3D::LocalSession first( soakConfig );
        Arena3D::LocalSession second( soakConfig );
        first.reset( 77, UINT64_C(0xA3E4D01920260714) );
        second.reset( 77, UINT64_C(0xA3E4D01920260714) );
        Arena3D::CommandPayload forward;
        forward.button = Arena3D::InputButton::Forward;
        first.submit( first.makeCommand( Arena3D::CommandType::ButtonDown, 0, forward ) );
        second.submit( second.makeCommand( Arena3D::CommandType::ButtonDown, 0, forward ) );
        first.submit( first.makeCommand( Arena3D::CommandType::FireDown, 0 ) );
        second.submit( second.makeCommand( Arena3D::CommandType::FireDown, 0 ) );
        size_t peakProjectiles = 0;
        for( uint64_t tick = 0; tick != ticks; ++tick )
        {
            if( first.tick() == false || second.tick() == false || first.server().checksum() != second.server().checksum() )
            {
                std::cerr << "Soak desync at tick " << tick << '\n';
                return 6;
            }
            const auto & projectiles = first.server().state().projectiles;
            const size_t active = static_cast<size_t>(std::count_if( projectiles.begin(), projectiles.end(), []( const Arena3D::ProjectileState & projectile )
            {
                return projectile.active;
            }));
            peakProjectiles = std::max( peakProjectiles, active );
        }
        if( ticks >= 6000 && peakProjectiles < 100 )
        {
            std::cerr << "Soak did not reach the required 100 concurrent projectiles (peak=" << peakProjectiles << ")\n";
            return 8;
        }
        for( uint32_t restart = 0; restart != 20; ++restart )
        {
            first.submit( first.makeCommand( Arena3D::CommandType::Restart, ticks * Arena3D::TickMilliseconds ) );
            second.submit( second.makeCommand( Arena3D::CommandType::Restart, ticks * Arena3D::TickMilliseconds ) );
            if( first.tick() == false || second.tick() == false || first.server().checksum() != second.server().checksum() ) return 7;
        }
        const Arena3D::StateChecksum checksum = first.server().checksum();
        std::cout << "Soak OK: ticks=" << ticks << " restarts=20 peak_projectiles=" << std::dec << peakProjectiles
            << " crc=0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << checksum.full << '\n';
        return 0;
    }

    if( argc != 2 && argc != 4 )
    {
        std::cerr << "Usage: Arena3DReplayTool <match.a3r> [--inject-fault <tick>] | --soak <ticks>\n";
        return 2;
    }
    uint64_t faultTick = UINT64_MAX;
    if( argc == 4 )
    {
        if( std::string( argv[2] ) != "--inject-fault" ) return 2;
        try { faultTick = std::stoull( argv[3] ); }
        catch( const std::exception & ) { return 2; }
    }
    Arena3D::ReplayRecord replay;
    if( Arena3D::readReplay( std::filesystem::path( argv[1] ), &replay ) == false )
    {
        std::cerr << "Unable to read compatible Arena3D replay\n";
        return 3;
    }
    Arena3D::Simulation simulation;
    Arena3D::StateChecksum checksum;
    if( Arena3D::runReplay( replay, &simulation, &checksum, faultTick ) == false )
    {
        std::cerr << "Replay desync at or before tick " << simulation.state().tick;
        if( replay.checkpoints.empty() == false && replay.checkpoints.back().snapshot.empty() == false )
        {
            Arena3D::Simulation expected;
            if( expected.restoreSnapshot( replay.checkpoints.back().snapshot ) == true )
            {
                const std::string field = Arena3D::describeFirstDifference( expected.state(), simulation.state() );
                if( field.empty() == false ) std::cerr << ", first differing field: " << field;
            }
        }
        std::cerr << "\n";
        return faultTick == UINT64_MAX ? 4 : 0;
    }
    if( faultTick != UINT64_MAX )
    {
        std::cerr << "Fault injection at tick " << faultTick << " was not detected\n";
        return 5;
    }
    std::cout << "Replay OK: tick=" << simulation.state().tick << " crc=0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << checksum.full << '\n';
    return 0;
}
