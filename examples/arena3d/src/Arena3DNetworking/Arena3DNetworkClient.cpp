#include "Arena3DNetworkClient.h"

#include "Arena3DGeneratedCollision.h"

#include "Config/StdAlgorithm.h"
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
        static bool decompressSnapshot( CanonicalReader * reader, uint32_t uncompressedSize, uint32_t compressedSize, Mengine::Data * output )
        {
            if( uncompressedSize > 64u * 1024u || compressedSize > 64u * 1024u || reader->remaining() != compressedSize )
            {
                return false;
            }
            Mengine::Data compressed( compressedSize );
            if( reader->readBytes( compressed.data(), compressed.size() ) == false )
            {
                return false;
            }
            output->clear();
            output->reserve( uncompressedSize );
            size_t offset = 0;
            while( offset != compressed.size() )
            {
                const uint8_t token = compressed[offset++];
                if( (token & 0x80u) != 0 )
                {
                    if( offset == compressed.size() )
                    {
                        return false;
                    }
                    const size_t count = static_cast<size_t>(token & 0x7Fu) + 3u;
                    if( output->size() + count > uncompressedSize )
                    {
                        return false;
                    }
                    output->insert( output->end(), count, compressed[offset++] );
                }
                else
                {
                    const size_t count = static_cast<size_t>(token) + 1u;
                    if( offset + count > compressed.size() || output->size() + count > uncompressedSize )
                    {
                        return false;
                    }
                    output->insert( output->end(), compressed.begin() + static_cast<ptrdiff_t>(offset), compressed.begin() + static_cast<ptrdiff_t>(offset + count) );
                    offset += count;
                }
            }
            return output->size() == uncompressedSize;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool readEvent( CanonicalReader * reader, ServerEvent * event )
        {
            uint8_t type = 0;
            uint8_t weapon = 0;
            if( reader->readU64( &event->id ) == false || reader->readU64( &event->tick ) == false || reader->readU8( &type ) == false ||
                reader->readU32( &event->actorId ) == false || reader->readU32( &event->targetId ) == false || reader->readU8( &weapon ) == false ||
                reader->readVector( &event->position ) == false || reader->readI32( &event->amount ) == false ) return false;
            if( type > static_cast<uint8_t>(EventType::Impact) || weapon >= WeaponTypeCount )
            {
                return false;
            }
            event->type = static_cast<EventType>(type);
            event->weapon = static_cast<WeaponType>(weapon);
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    NetworkClient::NetworkClient( Mengine::SocketServiceInterface * socketService, SimulationConfig config )
        : m_socketService( socketService )
        , m_simulation( Mengine::StdUtility::move( config ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NetworkClient::~NetworkClient()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NetworkClient::connect( const Mengine::SocketAddress & address, Mengine::String name, NetworkClientKind kind )
    {
        if( m_socketService == nullptr || m_state != NetworkClientState::Disconnected || name.empty() == true || name.size() > 48 )
        {
            return false;
        }
        m_name = Mengine::StdUtility::move( name );
        m_kind = kind;
        if( m_socketService->connect( address, &m_peerId ) == false )
        {
            return false;
        }
        m_state = NetworkClientState::Connecting;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::updateNetwork()
    {
        m_events.clear();
        Mengine::SocketServiceEvent event;
        while( m_socketService->pollEvent( &event ) == true )
        {
            if( event.peerId != m_peerId )
            {
                continue;
            }
            if( event.type == Mengine::ESocketServiceEventType::PeerConnected )
            {
                m_state = NetworkClientState::Handshaking;
                this->sendHello_();
            }
            else if( event.type == Mengine::ESocketServiceEventType::PeerDisconnected || event.type == Mengine::ESocketServiceEventType::TransportError )
            {
                m_state = NetworkClientState::Disconnected;
            }
            else if( event.type == Mengine::ESocketServiceEventType::MessageReceived )
            {
                this->onMessage_( event.channel, event.payload );
            }
        }

        ++m_networkUpdateOrdinal;
        if( m_state == NetworkClientState::Connected && m_pending.empty() == false && m_networkUpdateOrdinal % 3u == 0u )
        {
            this->sendPendingCommands_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::tick()
    {
        if( m_state != NetworkClientState::Connected )
        {
            return;
        }
        this->predictTick_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool NetworkClient::sendCommand( CommandType type, uint64_t sourceTimeMs, const CommandPayload & payload )
    {
        if( m_state != NetworkClientState::Connected || m_pending.size() >= 256u )
        {
            return false;
        }
        CommandEnvelope command;
        command.matchId = m_simulation.state().matchId;
        command.playerId = m_playerId;
        command.sequence = m_nextSequence++;
        command.sourceTimeMs = sourceTimeMs;
        command.appliedTick = m_simulation.state().tick;
        command.type = type;
        command.payload = payload;
        m_pending.emplace_back( command );
        return this->sendPendingCommands_();
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::disconnect( uint32_t reason )
    {
        if( m_peerId != Mengine::InvalidSocketPeerId )
        {
            m_socketService->disconnect( m_peerId, reason );
        }
        m_state = NetworkClientState::Disconnected;
    }
    //////////////////////////////////////////////////////////////////////////
    NetworkClientState NetworkClient::state() const
    {
        return m_state;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t NetworkClient::playerId() const
    {
        return m_playerId;
    }
    //////////////////////////////////////////////////////////////////////////
    const Simulation & NetworkClient::simulation() const
    {
        return m_simulation;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorServerEvent & NetworkClient::events() const
    {
        return m_events;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorNetworkRosterEntry & NetworkClient::roster() const
    {
        return m_roster;
    }
    //////////////////////////////////////////////////////////////////////////
    const Mengine::String & NetworkClient::rejection() const
    {
        return m_rejection;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t NetworkClient::reconciliationCount() const
    {
        return m_reconciliationCount;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t NetworkClient::pendingCommandCount() const
    {
        return m_pending.size();
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::sendHello_()
    {
        CanonicalWriter writer;
        Detail::writeHeader( &writer, NetworkMessage::Hello );
        writer.writeU32( SimulationVersion );
        writer.writeU32( DeterministicMathVersion );
        writer.writeU32( ConfigFormatVersion );
        writer.writeU32( CollisionFormatVersion );
        writer.writeU32( CommandFormatVersion );
        writer.writeU32( SnapshotFormatVersion );
        writer.writeU32( Arena3DGenerated::LevelCrc );
        writer.writeU32( Arena3DGenerated::ConfigCrc );
        writer.writeU32( Arena3DGenerated::CollisionCrc );
        writer.writeU8( static_cast<uint8_t>(m_kind) );
        writer.writeString( m_name );
        Detail::sendWire( m_socketService, m_peerId, 0, Mengine::ESocketDelivery::ReliableOrdered, writer );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NetworkClient::sendPendingCommands_()
    {
        if( m_pending.empty() == true )
        {
            return true;
        }
        const size_t count = Mengine::StdAlgorithm::min<size_t>( 64, m_pending.size() );
        CanonicalWriter writer;
        Detail::writeHeader( &writer, NetworkMessage::CommandBatch );
        writer.writeU32( static_cast<uint32_t>(count) );
        for( size_t index = 0; index != count; ++index )
        {
            serializeCommand( &writer, m_pending[index] );
        }
        return Detail::sendWire( m_socketService, m_peerId, 1, Mengine::ESocketDelivery::UnreliableSequenced, writer );
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::predictTick_()
    {
        const uint64_t tick = m_simulation.state().tick;
        VectorCommandEnvelope commands;
        for( const CommandEnvelope & pending : m_pending )
        {
            if( pending.sequence <= m_lastPredictedSequence || pending.appliedTick > tick )
            {
                continue;
            }
            CommandEnvelope command = pending;
            command.matchId = m_simulation.state().matchId;
            command.playerId = m_playerId;
            command.appliedTick = tick;
            command.tickCorrected = false;
            command.accepted = true;
            commands.emplace_back( command );
            m_lastPredictedSequence = command.sequence;
        }

        m_simulation.step( commands );
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::predictToTick_( uint64_t tick )
    {
        while( m_simulation.state().tick < tick )
        {
            this->predictTick_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::onMessage_( uint8_t channel, const Mengine::Data & payload )
    {
        CanonicalReader reader( payload.data(), payload.size() );
        NetworkMessage message;
        if( Detail::readHeader( &reader, &message ) == false )
        {
            return;
        }
        if( message == NetworkMessage::Reject && channel == 0 )
        {
            uint32_t reason = 0;
            if( reader.readU32( &reason ) == true && reader.readString( &m_rejection ) == true )
            {
                m_state = NetworkClientState::Rejected;
                m_socketService->disconnect( m_peerId, reason );
            }
            return;
        }
        if( message == NetworkMessage::Welcome && channel == 3 )
        {
            uint64_t matchId = 0, seed = 0, tick = 0;
            uint32_t size = 0, compressedSize = 0;
            if( reader.readU32( &m_playerId ) == false || reader.readU64( &matchId ) == false || reader.readU64( &seed ) == false || reader.readU64( &tick ) == false ||
                reader.readU32( &size ) == false || reader.readU32( &compressedSize ) == false ) return;
            Mengine::Data snapshot;
            if( Detail::decompressSnapshot( &reader, size, compressedSize, &snapshot ) == false || m_simulation.restoreSnapshot( snapshot ) == false ||
                m_simulation.state().matchId != matchId || m_simulation.state().seed != seed || m_simulation.state().tick != tick ) return;
            m_pending.clear();
            m_lastAcknowledgedSequence = 0;
            m_lastPredictedSequence = 0;
            m_state = NetworkClientState::Connected;
            return;
        }
        if( message == NetworkMessage::StateFrame && channel == 2 && m_state == NetworkClientState::Connected )
        {
            uint64_t matchId = 0, tick = 0, acknowledged = 0;
            uint32_t checksum = 0, eventCount = 0, snapshotSize = 0, compressedSize = 0;
            if( reader.readU64( &matchId ) == false || reader.readU64( &tick ) == false || reader.readU64( &acknowledged ) == false || reader.readU32( &checksum ) == false ||
                reader.readU32( &eventCount ) == false || eventCount > NetworkMaximumFrameEvents || matchId != m_simulation.state().matchId ) return;
            VectorServerEvent events;
            events.resize( eventCount );
            for( ServerEvent & event : events )
            {
                if( Detail::readEvent( &reader, &event ) == false )
                {
                    return;
                }
            }
            if( reader.readU32( &snapshotSize ) == false || reader.readU32( &compressedSize ) == false )
            {
                return;
            }
            const uint64_t predictedTick = m_simulation.state().tick;
            const StateChecksum before = m_simulation.checksum();
            Mengine::Data snapshot;
            if( Detail::decompressSnapshot( &reader, snapshotSize, compressedSize, &snapshot ) == false )
            {
                CanonicalWriter request;
                Detail::writeHeader( &request, NetworkMessage::SnapshotRequest );
                Detail::sendWire( m_socketService, m_peerId, 3, Mengine::ESocketDelivery::ReliableOrdered, request );
                return;
            }
            m_lastAcknowledgedSequence = acknowledged;
            m_pending.erase( Mengine::StdAlgorithm::remove_if( m_pending.begin(), m_pending.end(),
                                                               [acknowledged]( const CommandEnvelope & command )
                                                               {
                                                                   return command.sequence <= acknowledged;
                                                               } ),
                             m_pending.end() );
            const bool predictionMatches = predictedTick == tick && before.full == checksum;
            if( predictionMatches == false )
            {
                if( m_simulation.restoreSnapshot( snapshot ) == false || m_simulation.state().matchId != matchId || m_simulation.state().tick != tick || m_simulation.checksum().full != checksum )
                {
                    CanonicalWriter request;
                    Detail::writeHeader( &request, NetworkMessage::SnapshotRequest );
                    Detail::sendWire( m_socketService, m_peerId, 3, Mengine::ESocketDelivery::ReliableOrdered, request );
                    return;
                }
                m_lastPredictedSequence = acknowledged;
                this->predictToTick_( Mengine::StdAlgorithm::max( predictedTick, tick ) );
                ++m_reconciliationCount;
            }
            m_events.swap( events );
            return;
        }
        if( message == NetworkMessage::Roster && channel == 0 )
        {
            uint32_t count = 0;
            if( reader.readU32( &count ) == false || count > MaximumPlayers )
            {
                return;
            }
            VectorNetworkRosterEntry roster;
            roster.reserve( count );
            for( uint32_t index = 0; index != count; ++index )
            {
                NetworkRosterEntry entry;
                uint8_t kind = 0;
                if( reader.readU32( &entry.playerId ) == false || reader.readU8( &kind ) == false || kind > static_cast<uint8_t>(NetworkClientKind::Bot) || reader.readString( &entry.name ) == false || entry.name.size() > 48 )
                {
                    return;
                }
                entry.kind = static_cast<NetworkClientKind>(kind);
                roster.emplace_back( Mengine::StdUtility::move( entry ) );
            }
            if( reader.remaining() == 0 )
            {
                m_roster.swap( roster );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
