#include "Arena3DReplay.h"

#include "Arena3DGeneratedCollision.h"

#include <algorithm>
#include <fstream>
#include <iterator>

namespace Arena3D
{
    namespace
    {
        void writeChecksum( CanonicalWriter * writer, const StateChecksum & checksum )
        {
            writer->writeU32( checksum.player ); writer->writeU32( checksum.projectiles ); writer->writeU32( checksum.turrets ); writer->writeU32( checksum.pickups );
            writer->writeU32( checksum.random ); writer->writeU32( checksum.full );
        }

        bool readChecksum( CanonicalReader * reader, StateChecksum * checksum )
        {
            return reader->readU32( &checksum->player ) && reader->readU32( &checksum->projectiles ) && reader->readU32( &checksum->turrets ) && reader->readU32( &checksum->pickups ) &&
                reader->readU32( &checksum->random ) && reader->readU32( &checksum->full );
        }

        void writeEvent( CanonicalWriter * writer, const ServerEvent & event )
        {
            writer->writeU64( event.id ); writer->writeU64( event.tick ); writer->writeU8( static_cast<uint8_t>(event.type) );
            writer->writeU32( event.actorId ); writer->writeU32( event.targetId ); writer->writeU8( static_cast<uint8_t>(event.weapon) );
            writer->writeVector( event.position ); writer->writeI32( event.amount );
        }

        bool readEvent( CanonicalReader * reader, ServerEvent * event )
        {
            uint8_t type = 0;
            uint8_t weapon = 0;
            if( reader->readU64( &event->id ) == false || reader->readU64( &event->tick ) == false || reader->readU8( &type ) == false ||
                reader->readU32( &event->actorId ) == false || reader->readU32( &event->targetId ) == false || reader->readU8( &weapon ) == false ||
                reader->readVector( &event->position ) == false || reader->readI32( &event->amount ) == false ) return false;
            if( type > static_cast<uint8_t>(EventType::CommandRejected) || weapon >= static_cast<uint8_t>(WeaponType::Count) ) return false;
            event->type = static_cast<EventType>(type);
            event->weapon = static_cast<WeaponType>(weapon);
            return true;
        }

        bool equalEvent( const ServerEvent & left, const ServerEvent & right )
        {
            return left.id == right.id && left.tick == right.tick && left.type == right.type && left.actorId == right.actorId &&
                left.targetId == right.targetId && left.weapon == right.weapon && left.position.x == right.position.x &&
                left.position.y == right.position.y && left.position.z == right.position.z && left.amount == right.amount;
        }
    }

    ReplayRecord makeReplayRecord( const Simulation & simulation, uint64_t seed, const std::vector<CommandEnvelope> & journal,
        const std::vector<ServerEvent> & events )
    {
        ReplayRecord replay;
        replay.matchId = simulation.state().matchId;
        replay.seed = seed;
        replay.finalTick = simulation.state().tick;
        replay.levelCrc = Arena3DGenerated::LevelCrc;
        replay.configCrc = Arena3DGenerated::ConfigCrc;
        replay.collisionCrc = Arena3DGenerated::CollisionCrc;
        replay.finalChecksum = simulation.checksum();
        replay.commands = journal;
        replay.events = events;
        replay.checkpoints.push_back( {replay.finalTick, replay.finalChecksum, simulation.snapshot()} );
        return replay;
    }

    bool writeReplay( const std::filesystem::path & path, const ReplayRecord & replay )
    {
        CanonicalWriter payload;
        payload.writeU64( replay.matchId ); payload.writeU64( replay.seed ); payload.writeU64( replay.finalTick ); writeChecksum( &payload, replay.finalChecksum );
        payload.writeU32( static_cast<uint32_t>(replay.commands.size()) );
        for( const CommandEnvelope & command : replay.commands )
        {
            CanonicalWriter encoded; serializeCommand( &encoded, command );
            payload.writeU32( static_cast<uint32_t>(encoded.data().size()) ); payload.writeBytes( encoded.data().data(), encoded.data().size() );
        }
        payload.writeU32( static_cast<uint32_t>(replay.events.size()) );
        for( const ServerEvent & event : replay.events ) writeEvent( &payload, event );
        payload.writeU32( static_cast<uint32_t>(replay.checkpoints.size()) );
        for( const ReplayCheckpoint & checkpoint : replay.checkpoints )
        {
            payload.writeU64( checkpoint.tick );
            writeChecksum( &payload, checkpoint.checksum );
            payload.writeU32( static_cast<uint32_t>(checkpoint.snapshot.size()) );
            payload.writeBytes( checkpoint.snapshot.data(), checkpoint.snapshot.size() );
        }

        CanonicalWriter writer;
        writer.writeBytes( "A3RP", 4 ); writer.writeU32( ReplayFormatVersion ); writer.writeU32( SimulationVersion ); writer.writeU32( DeterministicMathVersion );
        writer.writeU32( ConfigFormatVersion ); writer.writeU32( CollisionFormatVersion ); writer.writeU32( CommandFormatVersion ); writer.writeU32( SnapshotFormatVersion );
        writer.writeU32( TickMilliseconds ); writer.writeU32( replay.levelCrc ); writer.writeU32( replay.configCrc ); writer.writeU32( replay.collisionCrc );
        writer.writeU32( static_cast<uint32_t>(payload.data().size()) ); writer.writeU32( crc32( payload.data() ) );
        writer.writeBytes( payload.data().data(), payload.data().size() );
        std::ofstream output( path, std::ios::binary | std::ios::trunc );
        if( output.is_open() == false ) return false;
        const std::vector<uint8_t> & bytes = writer.data();
        output.write( reinterpret_cast<const char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()) );
        return output.good();
    }

    bool readReplay( const std::filesystem::path & path, ReplayRecord * replay )
    {
        std::ifstream input( path, std::ios::binary );
        if( input.is_open() == false ) return false;
        std::vector<uint8_t> bytes( (std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>() );
        CanonicalReader reader( bytes );
        char magic[4]{};
        uint32_t format = 0, simulation = 0, math = 0, configFormat = 0, collisionFormat = 0, commandFormat = 0, snapshotFormat = 0;
        uint32_t tickMs = 0, levelCrc = 0, configCrc = 0, collisionCrc = 0, payloadSize = 0, expectedCrc = 0;
        if( reader.readBytes( magic, 4 ) == false || std::string( magic, 4 ) != "A3RP" || reader.readU32( &format ) == false ||
            reader.readU32( &simulation ) == false || reader.readU32( &math ) == false || reader.readU32( &configFormat ) == false ||
            reader.readU32( &collisionFormat ) == false || reader.readU32( &commandFormat ) == false || reader.readU32( &snapshotFormat ) == false ||
            reader.readU32( &tickMs ) == false || reader.readU32( &levelCrc ) == false || reader.readU32( &configCrc ) == false || reader.readU32( &collisionCrc ) == false ||
            reader.readU32( &payloadSize ) == false || reader.readU32( &expectedCrc ) == false || format != ReplayFormatVersion ||
            simulation != SimulationVersion || math != DeterministicMathVersion || configFormat != ConfigFormatVersion || collisionFormat != CollisionFormatVersion ||
            commandFormat != CommandFormatVersion || snapshotFormat != SnapshotFormatVersion || tickMs != TickMilliseconds ||
            levelCrc != Arena3DGenerated::LevelCrc || configCrc != Arena3DGenerated::ConfigCrc || collisionCrc != Arena3DGenerated::CollisionCrc ||
            payloadSize != reader.remaining() ) return false;
        std::vector<uint8_t> payload( payloadSize );
        if( reader.readBytes( payload.data(), payload.size() ) == false || crc32( payload ) != expectedCrc ) return false;
        CanonicalReader data( payload );
        ReplayRecord decoded;
        decoded.levelCrc = levelCrc;
        decoded.configCrc = configCrc;
        decoded.collisionCrc = collisionCrc;
        uint32_t commandCount = 0;
        if( data.readU64( &decoded.matchId ) == false || data.readU64( &decoded.seed ) == false || data.readU64( &decoded.finalTick ) == false ||
            readChecksum( &data, &decoded.finalChecksum ) == false || data.readU32( &commandCount ) == false ) return false;
        decoded.commands.reserve( commandCount );
        for( uint32_t index = 0; index != commandCount; ++index )
        {
            uint32_t commandSize = 0;
            if( data.readU32( &commandSize ) == false || commandSize > data.remaining() ) return false;
            std::vector<uint8_t> commandBytes( commandSize );
            if( data.readBytes( commandBytes.data(), commandBytes.size() ) == false ) return false;
            CanonicalReader commandReader( commandBytes );
            CommandEnvelope command;
            if( deserializeCommand( &commandReader, &command ) == false || commandReader.remaining() != 0 ) return false;
            decoded.commands.emplace_back( command );
        }
        uint32_t eventCount = 0;
        if( data.readU32( &eventCount ) == false ) return false;
        decoded.events.resize( eventCount );
        for( ServerEvent & event : decoded.events ) if( readEvent( &data, &event ) == false ) return false;
        uint32_t checkpointCount = 0;
        if( data.readU32( &checkpointCount ) == false ) return false;
        decoded.checkpoints.resize( checkpointCount );
        for( ReplayCheckpoint & checkpoint : decoded.checkpoints )
        {
            uint32_t snapshotSize = 0;
            if( data.readU64( &checkpoint.tick ) == false || readChecksum( &data, &checkpoint.checksum ) == false ||
                data.readU32( &snapshotSize ) == false || snapshotSize > data.remaining() ) return false;
            checkpoint.snapshot.resize( snapshotSize );
            if( data.readBytes( checkpoint.snapshot.data(), checkpoint.snapshot.size() ) == false ) return false;
        }
        if( data.valid() == false || data.remaining() != 0 ) return false;
        *replay = std::move( decoded );
        return true;
    }

    bool runReplay( const ReplayRecord & replay, Simulation * simulation, StateChecksum * finalChecksum, uint64_t faultInjectionTick )
    {
        simulation->reset( replay.matchId, replay.seed );
        size_t commandIndex = 0;
        size_t eventIndex = 0;
        size_t checkpointIndex = 0;
        while( simulation->state().tick < replay.finalTick )
        {
            std::vector<CommandEnvelope> commands;
            while( commandIndex < replay.commands.size() && replay.commands[commandIndex].appliedTick <= simulation->state().tick )
            {
                commands.emplace_back( replay.commands[commandIndex++] );
            }
            if( simulation->step( commands ) == false ) return false;
            if( simulation->state().tick == faultInjectionTick ) simulation->injectFaultForTest( 1 );
            for( const ServerEvent & actual : simulation->events() )
            {
                if( eventIndex >= replay.events.size() || equalEvent( actual, replay.events[eventIndex] ) == false ) return false;
                ++eventIndex;
            }
            while( checkpointIndex < replay.checkpoints.size() && replay.checkpoints[checkpointIndex].tick == simulation->state().tick )
            {
                const ReplayCheckpoint & checkpoint = replay.checkpoints[checkpointIndex++];
                if( simulation->checksum() != checkpoint.checksum ) return false;
                if( checkpoint.snapshot.empty() == false && simulation->snapshot() != checkpoint.snapshot ) return false;
            }
            if( simulation->state().paused == true && commands.empty() == true ) return false;
        }
        const StateChecksum checksum = simulation->checksum();
        if( finalChecksum != nullptr ) *finalChecksum = checksum;
        return commandIndex == replay.commands.size() && eventIndex == replay.events.size() && checkpointIndex == replay.checkpoints.size() && checksum == replay.finalChecksum;
    }
}
