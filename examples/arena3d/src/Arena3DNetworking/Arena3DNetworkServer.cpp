#include "Arena3DNetworkServer.h"

#include "Arena3DGeneratedCollision.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdIO.h"
#include "Config/StdUtility.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void writeHeader( CanonicalWriter * writer, NetworkMessage message )
        {
            writer->writeU32( NetworkWireMagic );
            writer->writeU32( NetworkProtocolVersion );
            writer->writeU8( static_cast<uint8_t>(message) );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool readHeader( CanonicalReader * reader, NetworkMessage * message )
        {
            uint32_t magic = 0;
            uint32_t version = 0;
            uint8_t type = 0;
            if( reader->readU32( &magic ) == false || reader->readU32( &version ) == false || reader->readU8( &type ) == false )
            {
                return false;
            }
            if( magic != NetworkWireMagic || version != NetworkProtocolVersion || type < static_cast<uint8_t>(NetworkMessage::Hello) || type > static_cast<uint8_t>(NetworkMessage::Roster) )
            {
                return false;
            }
            *message = static_cast<NetworkMessage>(type);
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool sendWire( Mengine::SocketServiceInterface * socketService, Mengine::SocketPeerId peerId, uint8_t channel, Mengine::ESocketDelivery delivery, const CanonicalWriter & writer )
        {
            return socketService->send( peerId, channel, delivery, writer.data().data(), writer.data().size() );
        }
        //////////////////////////////////////////////////////////////////////////
        static Mengine::Data compressSnapshot( const Mengine::Data & input )
        {
            Mengine::Data output;
            output.reserve( input.size() );
            size_t offset = 0;
            while( offset != input.size() )
            {
                size_t run = 1;
                while( offset + run != input.size() && input[offset + run] == input[offset] && run != 130 )
                {
                    ++run;
                }
                if( run >= 3 )
                {
                    output.push_back( static_cast<uint8_t>(0x80u | static_cast<uint8_t>(run - 3)) );
                    output.push_back( input[offset] );
                    offset += run;
                    continue;
                }

                const size_t literal = offset;
                ++offset;
                while( offset - literal != 128 && offset != input.size() )
                {
                    run = 1;
                    while( offset + run != input.size() && input[offset + run] == input[offset] && run != 130 )
                    {
                        ++run;
                    }
                    if( run >= 3 )
                    {
                        break;
                    }
                    ++offset;
                }
                const size_t size = offset - literal;
                output.push_back( static_cast<uint8_t>(size - 1) );
                output.insert( output.end(), input.begin() + static_cast<ptrdiff_t>(literal), input.begin() + static_cast<ptrdiff_t>(offset) );
            }
            return output;
        }
        //////////////////////////////////////////////////////////////////////////
        static void writeEvent( CanonicalWriter * writer, const ServerEvent & event )
        {
            writer->writeU64( event.id );
            writer->writeU64( event.tick );
            writer->writeU8( static_cast<uint8_t>(event.type) );
            writer->writeU32( event.actorId );
            writer->writeU32( event.targetId );
            writer->writeU8( static_cast<uint8_t>(event.weapon) );
            writer->writeVector( event.position );
            writer->writeI32( event.amount );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    NetworkServer::NetworkServer( Mengine::SocketServiceInterface * socketService, SimulationConfig config )
        : m_socketService( socketService )
        , m_simulation( Mengine::StdUtility::move( config ) )
    {
        const uint32_t defaultPlayer = m_simulation.state().players[0].id;
        m_simulation.removePlayer( defaultPlayer );
    }
    //////////////////////////////////////////////////////////////////////////
    NetworkServer::~NetworkServer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NetworkServer::listen( const Mengine::SocketAddress & address, uint32_t maximumPlayers )
    {
        if( m_socketService == nullptr || maximumPlayers == 0 || maximumPlayers > MaximumPlayers )
        {
            return false;
        }
        m_maximumPlayers = maximumPlayers;
        return m_socketService->listen( address, maximumPlayers );
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::updateNetwork()
    {
        Mengine::SocketServiceEvent event;
        while( m_socketService->pollEvent( &event ) == true )
        {
            switch( event.type )
            {
            case Mengine::ESocketServiceEventType::PeerConnected:
                this->onConnected_( event.peerId );
                break;
            case Mengine::ESocketServiceEventType::PeerDisconnected:
                this->onDisconnected_( event.peerId );
                break;
            case Mengine::ESocketServiceEventType::MessageReceived:
                this->onMessage_( event.peerId, event.channel, event.payload );
                break;
            case Mengine::ESocketServiceEventType::TransportError:
                this->onDisconnected_( event.peerId );
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::tick()
    {
        this->updateNetwork();
        Mengine::StdAlgorithm::stable_sort( m_scheduled.begin(), m_scheduled.end(),
                                            []( const CommandEnvelope & left, const CommandEnvelope & right )
                                            {
                                                if( left.appliedTick != right.appliedTick )
                                                {
                                                    return left.appliedTick < right.appliedTick;
                                                }
                                                if( left.playerId != right.playerId )
                                                {
                                                    return left.playerId < right.playerId;
                                                }
                                                return left.sequence < right.sequence;
                                            } );

        VectorCommandEnvelope commands;
        size_t consumed = 0;
        while( consumed != m_scheduled.size() && m_scheduled[consumed].appliedTick <= m_simulation.state().tick )
        {
            commands.emplace_back( m_scheduled[consumed++] );
        }
        if( consumed != 0 )
        {
            m_scheduled.erase( m_scheduled.begin(), m_scheduled.begin() + static_cast<ptrdiff_t>(consumed) );
        }
        const uint64_t previousMatchId = m_simulation.state().matchId;
        m_simulation.step( commands );
        for( const CommandEnvelope & command : commands )
        {
            for( Peer & peer : m_peers )
            {
                if( peer.playerId != command.playerId )
                {
                    continue;
                }
                peer.lastAppliedSequence = Mengine::StdAlgorithm::max( peer.lastAppliedSequence, command.sequence );
                break;
            }
        }
        if( m_simulation.state().matchId != previousMatchId )
        {
            m_eventHistory.clear();
        }
        const VectorServerEvent & events = m_simulation.events();
        m_eventHistory.insert( m_eventHistory.end(), events.begin(), events.end() );
        const uint64_t minimumEventTick = m_simulation.state().tick > NetworkEventHistoryTicks ? m_simulation.state().tick - NetworkEventHistoryTicks : 0;
        m_eventHistory.erase( Mengine::StdAlgorithm::remove_if( m_eventHistory.begin(), m_eventHistory.end(), [minimumEventTick]( const ServerEvent & event )
        {
            return (event.id >> 32) < minimumEventTick;
        } ), m_eventHistory.end() );
        if( m_eventHistory.size() > NetworkMaximumFrameEvents )
        {
            m_eventHistory.erase( m_eventHistory.begin(), m_eventHistory.end() - static_cast<ptrdiff_t>(NetworkMaximumFrameEvents) );
        }
        if( m_simulation.state().tick % NetworkStateFrameTicks == 0u )
        {
            this->sendStateFrames_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const Simulation & NetworkServer::simulation() const
    {
        return m_simulation;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorNetworkRosterEntry & NetworkServer::roster() const
    {
        return m_roster;
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::onConnected_( Mengine::SocketPeerId peerId )
    {
        if( this->findPeer_( peerId ) != nullptr )
        {
            return;
        }
        Peer peer;
        peer.peerId = peerId;
        m_peers.emplace_back( Mengine::StdUtility::move( peer ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::onDisconnected_( Mengine::SocketPeerId peerId )
    {
        for( auto iterator = m_peers.begin(); iterator != m_peers.end(); ++iterator )
        {
            if( iterator->peerId != peerId )
            {
                continue;
            }
            if( iterator->playerId != 0 )
            {
                m_simulation.removePlayer( iterator->playerId );
                m_roster.erase( Mengine::StdAlgorithm::remove_if( m_roster.begin(), m_roster.end(),
                                                                  [iterator]( const NetworkRosterEntry & entry )
                                                                  {
                                                                      return entry.playerId == iterator->playerId;
                                                                  } ),
                                m_roster.end() );
            }
            m_peers.erase( iterator );
            this->sendRoster_();
            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::onMessage_( Mengine::SocketPeerId peerId, uint8_t channel, const Mengine::Data & payload )
    {
        Peer * peer = this->findPeer_( peerId );
        if( peer == nullptr || payload.empty() == true )
        {
            return;
        }
        CanonicalReader reader( payload.data(), payload.size() );
        NetworkMessage message;
        if( Detail::readHeader( &reader, &message ) == false )
        {
            this->sendReject_( peerId, 1, "invalid wire header" );
            return;
        }

        if( message == NetworkMessage::Hello && channel == 0 && peer->handshaken == false )
        {
            uint32_t simulation = 0, math = 0, config = 0, collision = 0, command = 0, snapshot = 0;
            uint32_t levelCrc = 0, configCrc = 0, collisionCrc = 0;
            uint8_t kind = 0;
            Mengine::String name;
            if( reader.readU32( &simulation ) == false || reader.readU32( &math ) == false || reader.readU32( &config ) == false || reader.readU32( &collision ) == false ||
                reader.readU32( &command ) == false || reader.readU32( &snapshot ) == false || reader.readU32( &levelCrc ) == false || reader.readU32( &configCrc ) == false ||
                reader.readU32( &collisionCrc ) == false || reader.readU8( &kind ) == false || reader.readString( &name ) == false || reader.remaining() != 0 ||
                kind > static_cast<uint8_t>(NetworkClientKind::Bot) || name.empty() == true || name.size() > 48 )
            {
                this->sendReject_( peerId, 2, "invalid hello" );
                return;
            }
            if( simulation != SimulationVersion || math != DeterministicMathVersion || config != ConfigFormatVersion || collision != CollisionFormatVersion ||
                command != CommandFormatVersion || snapshot != SnapshotFormatVersion || levelCrc != Arena3DGenerated::LevelCrc ||
                configCrc != Arena3DGenerated::ConfigCrc || collisionCrc != Arena3DGenerated::CollisionCrc )
            {
                this->sendReject_( peerId, 3, "incompatible Arena3D content or simulation" );
                return;
            }
            if( m_roster.size() >= m_maximumPlayers )
            {
                this->sendReject_( peerId, 4, "server is full" );
                return;
            }
            const uint32_t playerId = m_simulation.addPlayer();
            if( playerId == 0 )
            {
                this->sendReject_( peerId, 4, "server is full" );
                return;
            }
            peer->playerId = playerId;
            peer->kind = static_cast<NetworkClientKind>(kind);
            peer->name = Mengine::StdUtility::move( name );
            peer->handshaken = true;
            m_roster.push_back( {peer->playerId, peer->kind, peer->name} );
            Mengine::StdIO::printf( "Arena3D network join peer=%u player=%u name=%s\n", peer->peerId, peer->playerId, peer->name.c_str() );
            this->sendWelcome_( peer );
            this->sendRoster_();
            return;
        }

        if( message == NetworkMessage::CommandBatch && channel == 1 && peer->handshaken == true )
        {
            uint32_t count = 0;
            if( reader.readU32( &count ) == false || count == 0 || count > 64 )
            {
                return;
            }

            VectorCommandEnvelope commands;
            commands.reserve( count );
            for( uint32_t index = 0; index != count; ++index )
            {
                CommandEnvelope command;
                if( deserializeCommand( &reader, &command ) == false )
                {
                    return;
                }
                commands.emplace_back( Mengine::StdUtility::move( command ) );
            }
            if( reader.remaining() != 0 )
            {
                return;
            }

            for( CommandEnvelope & command : commands )
            {
                if( command.sequence <= peer->lastReceivedSequence || command.type == CommandType::JoinPlayer || command.type == CommandType::LeavePlayer )
                {
                    continue;
                }
                peer->lastReceivedSequence = command.sequence;
                command.matchId = m_simulation.state().matchId;
                command.playerId = peer->playerId;
                command.accepted = true;
                const uint64_t requestedTick = command.sourceTimeMs / TickMilliseconds + (command.sourceTimeMs % TickMilliseconds != 0u ? 1u : 0u);
                command.appliedTick = Mengine::StdAlgorithm::max( requestedTick, m_simulation.state().tick );
                command.tickCorrected = command.appliedTick != requestedTick;
                m_scheduled.emplace_back( command );
            }
            return;
        }

        if( message == NetworkMessage::SnapshotRequest && channel == 3 && peer->handshaken == true )
        {
            this->sendWelcome_( peer );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::sendWelcome_( Peer * peer )
    {
        const Mengine::Data snapshot = m_simulation.snapshot();
        const Mengine::Data compressed = Detail::compressSnapshot( snapshot );
        CanonicalWriter writer;
        Detail::writeHeader( &writer, NetworkMessage::Welcome );
        writer.writeU32( peer->playerId );
        writer.writeU64( m_simulation.state().matchId );
        writer.writeU64( m_simulation.state().seed );
        writer.writeU64( m_simulation.state().tick );
        writer.writeU32( static_cast<uint32_t>(snapshot.size()) );
        writer.writeU32( static_cast<uint32_t>(compressed.size()) );
        writer.writeBytes( compressed.data(), compressed.size() );
        Detail::sendWire( m_socketService, peer->peerId, 3, Mengine::ESocketDelivery::ReliableOrdered, writer );
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::sendReject_( Mengine::SocketPeerId peerId, uint32_t reason, const char * message )
    {
        CanonicalWriter writer;
        Detail::writeHeader( &writer, NetworkMessage::Reject );
        writer.writeU32( reason );
        writer.writeString( message );
        Detail::sendWire( m_socketService, peerId, 0, Mengine::ESocketDelivery::ReliableOrdered, writer );
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::sendRoster_()
    {
        CanonicalWriter writer;
        Detail::writeHeader( &writer, NetworkMessage::Roster );
        writer.writeU32( static_cast<uint32_t>(m_roster.size()) );
        for( const NetworkRosterEntry & entry : m_roster )
        {
            writer.writeU32( entry.playerId );
            writer.writeU8( static_cast<uint8_t>(entry.kind) );
            writer.writeString( entry.name );
        }
        m_socketService->broadcast( 0, Mengine::ESocketDelivery::ReliableOrdered, writer.data().data(), writer.data().size() );
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::sendStateFrames_()
    {
        const Mengine::Data snapshot = m_simulation.snapshot();
        const Mengine::Data compressed = Detail::compressSnapshot( snapshot );
        for( const Peer & peer : m_peers )
        {
            if( peer.handshaken == false )
            {
                continue;
            }
            CanonicalWriter writer;
            Detail::writeHeader( &writer, NetworkMessage::StateFrame );
            writer.writeU64( m_simulation.state().matchId );
            writer.writeU64( m_simulation.state().tick );
            writer.writeU64( peer.lastAppliedSequence );
            writer.writeU32( m_simulation.checksum().full );
            writer.writeU32( static_cast<uint32_t>(m_eventHistory.size()) );
            for( const ServerEvent & event : m_eventHistory )
            {
                Detail::writeEvent( &writer, event );
            }
            writer.writeU32( static_cast<uint32_t>(snapshot.size()) );
            writer.writeU32( static_cast<uint32_t>(compressed.size()) );
            writer.writeBytes( compressed.data(), compressed.size() );
            Detail::sendWire( m_socketService, peer.peerId, 2, Mengine::ESocketDelivery::UnreliableSequenced, writer );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    NetworkServer::Peer * NetworkServer::findPeer_( Mengine::SocketPeerId peerId )
    {
        for( Peer & peer : m_peers )
        {
            if( peer.peerId == peerId )
            {
                return &peer;
            }
        }
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
