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
            if( magic != NetworkWireMagic || version != NetworkProtocolVersion || type < static_cast<uint8_t>(NetworkMessage::Hello) || type > static_cast<uint8_t>(NetworkMessage::Confirmations) )
            {
                return false;
            }
            *message = static_cast<NetworkMessage>(type);
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool sendWire( Mengine::SocketServiceInterface * socketService, Mengine::SocketPeerId peerId, uint8_t channel, Mengine::ESocketDelivery delivery, const CanonicalWriter & writer )
        {
            const Mengine::Data & data = writer.data();
            const void * bytes = data.data();
            size_t size = data.size();
            if( socketService->send( peerId, channel, delivery, bytes, size ) == false )
            {
                return false;
            }

            return true;
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
        static void writeEvent( CanonicalWriter * _writer, const ServerEvent & _event )
        {
            _writer->writeU64( _event.id );
            _writer->writeU64( _event.tick );
            uint8_t type = static_cast<uint8_t>(_event.type);
            _writer->writeU8( type );
            _writer->writeU32( _event.actorId );
            _writer->writeU32( _event.targetId );
            uint8_t weapon = static_cast<uint8_t>(_event.weapon);
            _writer->writeU8( weapon );
            _writer->writeVector( _event.position );
            _writer->writeI32( _event.amount );
            _writer->writeVector( _event.endPosition );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    NetworkServer::NetworkServer( Mengine::SocketServiceInterface * socketService, SimulationConfig config )
        : m_socketService( socketService )
        , m_simulation( Mengine::StdUtility::move( config ) )
        , m_commands( &m_simulation )
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
        if( m_commands.reset() == false )
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
        this->sendControlMessages_();
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::tick()
    {
        this->updateNetwork();
        uint64_t previousMatchId = m_simulation.state().matchId;
        bool previouslyPaused = m_simulation.state().paused;
        if( m_commands.tick() == false )
        {
            return;
        }
        this->sendConfirmations_();
        if( m_simulation.state().matchId != previousMatchId )
        {
            m_eventHistory.clear();
            if( m_commands.reset() == false )
            {
                return;
            }
            for( Peer & peer : m_peers )
            {
                if( peer.handshaken == false )
                {
                    continue;
                }
                peer.welcomePending = true;
            }
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
        bool paused = m_simulation.state().paused;
        const VectorCommandEnvelope & commands = m_commands.getCommands();
        bool processed = commands.empty() == false;
        ++m_stateFrameTicks;
        bool periodicFrame = m_stateFrameTicks >= NetworkStateFrameTicks;
        if( periodicFrame == true || paused != previouslyPaused || (paused == true && processed == true) )
        {
            this->sendStateFrames_();
            m_stateFrameTicks = 0;
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
                m_commands.unregisterPlayer( iterator->playerId );
                m_simulation.removePlayer( iterator->playerId );
                m_roster.erase( Mengine::StdAlgorithm::remove_if( m_roster.begin(), m_roster.end(),
                                                                  [iterator]( const NetworkRosterEntry & entry )
                                                                  {
                                                                      return entry.playerId == iterator->playerId;
                                                                  } ),
                                m_roster.end() );
            }
            m_peers.erase( iterator );
            this->queueRoster_();
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
        if( peer->rejection.empty() == false )
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
            if( m_commands.registerPlayer( playerId ) == false )
            {
                m_simulation.removePlayer( playerId );
                this->sendReject_( peerId, 4, "command server is full" );
                return;
            }
            peer->playerId = playerId;
            peer->kind = static_cast<NetworkClientKind>(kind);
            peer->name = Mengine::StdUtility::move( name );
            peer->handshaken = true;
            m_roster.push_back( {peer->playerId, peer->kind, peer->name} );
            Mengine::StdIO::printf( "Arena3D network join peer=%u player=%u name=%s\n", peer->peerId, peer->playerId, peer->name.c_str() );
            peer->welcomePending = true;
            this->queueRoster_();
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
                if( command.type == CommandType::JoinPlayer || command.type == CommandType::LeavePlayer )
                {
                    this->sendReject_( peerId, 5, "server-owned command" );
                    return;
                }
                if( command.matchId != m_simulation.state().matchId )
                {
                    continue;
                }
                command.playerId = peer->playerId;
                ackord_status status = m_commands.submit( command );
                if( status == ACKORD_CONFLICT || status == ACKORD_INVALID )
                {
                    this->sendReject_( peerId, 5, "invalid command identity or payload" );
                    return;
                }
                // FULL/OUT_OF_WINDOW retain the client's pending command for retry.
            }
            return;
        }

        if( message == NetworkMessage::SnapshotRequest && channel == 3 && peer->handshaken == true )
        {
            peer->welcomePending = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool NetworkServer::sendWelcome_( Peer * _peer )
    {
        const Mengine::Data snapshot = m_simulation.snapshot();
        const Mengine::Data compressed = Detail::compressSnapshot( snapshot );
        CanonicalWriter writer;
        Detail::writeHeader( &writer, NetworkMessage::Welcome );
        writer.writeU32( _peer->playerId );
        writer.writeU64( m_simulation.state().matchId );
        writer.writeU64( m_simulation.state().seed );
        writer.writeU64( m_simulation.state().tick );
        uint64_t confirmed = m_commands.getConfirmedSequence( _peer->playerId );
        uint64_t completedMask = m_commands.getCompletedMask( _peer->playerId );
        writer.writeU64( confirmed );
        writer.writeU64( completedMask );
        uint64_t controlConfirmed = m_commands.getConfirmedSequence( _peer->playerId, ECommandStream::Control );
        uint64_t controlCompletedMask = m_commands.getCompletedMask( _peer->playerId, ECommandStream::Control );
        writer.writeU64( controlConfirmed );
        writer.writeU64( controlCompletedMask );
        writer.writeU32( static_cast<uint32_t>(snapshot.size()) );
        writer.writeU32( static_cast<uint32_t>(compressed.size()) );
        writer.writeBytes( compressed.data(), compressed.size() );
        if( Detail::sendWire( m_socketService, _peer->peerId, 3, Mengine::ESocketDelivery::ReliableOrdered, writer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::sendReject_( Mengine::SocketPeerId peerId, uint32_t reason, const char * message )
    {
        Peer * peer = this->findPeer_( peerId );
        if( peer == nullptr )
        {
            return;
        }
        if( peer->rejection.empty() == false )
        {
            return;
        }
        CanonicalWriter writer;
        Detail::writeHeader( &writer, NetworkMessage::Reject );
        writer.writeU32( reason );
        writer.writeString( message );
        peer->rejection = writer.take();
        peer->rejectPending = true;
        peer->welcomePending = false;
        peer->rosterPending = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::queueRoster_()
    {
        for( Peer & peer : m_peers )
        {
            if( peer.handshaken == false )
            {
                continue;
            }
            if( peer.rejection.empty() == false )
            {
                continue;
            }
            peer.rosterPending = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool NetworkServer::sendRoster_( Peer * _peer )
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
        if( Detail::sendWire( m_socketService, _peer->peerId, 0, Mengine::ESocketDelivery::ReliableOrdered, writer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::sendControlMessages_()
    {
        for( Peer & peer : m_peers )
        {
            if( peer.rejectPending == true )
            {
                const void * bytes = peer.rejection.data();
                size_t size = peer.rejection.size();
                if( m_socketService->send( peer.peerId, 0, Mengine::ESocketDelivery::ReliableOrdered, bytes, size ) == false )
                {
                    continue;
                }
                peer.rejectPending = false;
            }
            if( peer.rejection.empty() == false )
            {
                continue;
            }
            if( peer.welcomePending == true )
            {
                if( this->sendWelcome_( &peer ) == false )
                {
                    continue;
                }
                peer.welcomePending = false;
            }
            if( peer.rosterPending == true )
            {
                if( this->sendRoster_( &peer ) == false )
                {
                    continue;
                }
                peer.rosterPending = false;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkServer::sendStateFrames_()
    {
        const Mengine::Data snapshot = m_simulation.snapshot();
        const Mengine::Data compressed = Detail::compressSnapshot( snapshot );
        uint32_t checksum = m_simulation.checksum().full;
        for( const Peer & peer : m_peers )
        {
            if( peer.handshaken == false )
            {
                continue;
            }
            if( peer.rejection.empty() == false )
            {
                continue;
            }
            CanonicalWriter writer;
            Detail::writeHeader( &writer, NetworkMessage::StateFrame );
            writer.writeU64( m_simulation.state().matchId );
            writer.writeU64( m_simulation.state().tick );
            uint64_t confirmed = m_commands.getConfirmedSequence( peer.playerId );
            uint64_t completedMask = m_commands.getCompletedMask( peer.playerId );
            writer.writeU64( confirmed );
            writer.writeU64( completedMask );
            uint64_t controlConfirmed = m_commands.getConfirmedSequence( peer.playerId, ECommandStream::Control );
            uint64_t controlCompletedMask = m_commands.getCompletedMask( peer.playerId, ECommandStream::Control );
            writer.writeU64( controlConfirmed );
            writer.writeU64( controlCompletedMask );
            writer.writeU32( checksum );
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
    void NetworkServer::sendConfirmations_()
    {
        ackord_journal_entry entry;
        uint64_t index = 0;
        while( m_commands.getNextJournalEntry( index, &entry ) == true )
        {
            for( const Peer & peer : m_peers )
            {
                if( peer.handshaken == false )
                {
                    continue;
                }
                if( peer.rejection.empty() == false )
                {
                    continue;
                }
                if( entry.confirmation.client != peer.playerId )
                {
                    continue;
                }
                CanonicalWriter writer;
                Detail::writeHeader( &writer, NetworkMessage::Confirmations );
                writer.writeU64( entry.confirmation.session );
                ECommandStream stream = getCommandStream( static_cast<CommandType>(entry.command.type) );
                writer.writeU8( static_cast<uint8_t>(stream) );
                writer.writeU64( entry.confirmation.sequence );
                writer.writeI32( entry.confirmation.status );
                if( Detail::sendWire( m_socketService, peer.peerId, 1, Mengine::ESocketDelivery::ReliableOrdered, writer ) == false )
                {
                    m_commands.trimJournal( index );
                    return;
                }
                break;
            }
            index = entry.index;
        }
        m_commands.trimJournal( index );
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
