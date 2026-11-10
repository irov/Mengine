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
        static bool readEvent( CanonicalReader * _reader, ServerEvent * _event )
        {
            uint8_t type = 0;
            uint8_t weapon = 0;
            if( _reader->readU64( &_event->id ) == false )
            {
                return false;
            }
            if( _reader->readU64( &_event->tick ) == false )
            {
                return false;
            }
            if( _reader->readU8( &type ) == false )
            {
                return false;
            }
            if( _reader->readU32( &_event->actorId ) == false )
            {
                return false;
            }
            if( _reader->readU32( &_event->targetId ) == false )
            {
                return false;
            }
            if( _reader->readU8( &weapon ) == false )
            {
                return false;
            }
            if( _reader->readVector( &_event->position ) == false )
            {
                return false;
            }
            if( _reader->readI32( &_event->amount ) == false )
            {
                return false;
            }
            if( _reader->readVector( &_event->endPosition ) == false )
            {
                return false;
            }
            if( type > static_cast<uint8_t>(EventType::Impact) || weapon >= WeaponTypeCount )
            {
                return false;
            }
            _event->type = static_cast<EventType>(type);
            _event->weapon = static_cast<WeaponType>(weapon);
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
        if( m_socketService == nullptr )
        {
            return false;
        }
        if( m_state != NetworkClientState::Disconnected && m_state != NetworkClientState::Rejected )
        {
            return false;
        }
        if( m_disconnectPeerId != Mengine::InvalidSocketPeerId )
        {
            return false;
        }
        if( name.empty() == true )
        {
            return false;
        }
        if( name.size() > 48 )
        {
            return false;
        }
        if( m_commands.isValid() == false )
        {
            return false;
        }
        Mengine::SocketPeerId peerId = Mengine::InvalidSocketPeerId;
        if( m_socketService->connect( address, &peerId ) == false )
        {
            return false;
        }
        if( peerId == Mengine::InvalidSocketPeerId )
        {
            return false;
        }
        m_name = Mengine::StdUtility::move( name );
        m_kind = kind;
        m_rejection.clear();
        m_peerId = peerId;
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
            if( event.peerId == m_disconnectPeerId && (event.type == Mengine::ESocketServiceEventType::PeerDisconnected || event.type == Mengine::ESocketServiceEventType::TransportError) )
            {
                m_disconnectPeerId = Mengine::InvalidSocketPeerId;
            }
            if( m_peerId == Mengine::InvalidSocketPeerId )
            {
                continue;
            }
            if( event.peerId != m_peerId )
            {
                continue;
            }
            if( event.type == Mengine::ESocketServiceEventType::PeerConnected )
            {
                if( m_state != NetworkClientState::Connecting )
                {
                    continue;
                }
                m_state = NetworkClientState::Handshaking;
                m_helloPending = true;
            }
            else if( event.type == Mengine::ESocketServiceEventType::PeerDisconnected || event.type == Mengine::ESocketServiceEventType::TransportError )
            {
                this->clearConnection_( NetworkClientState::Disconnected );
            }
            else if( event.type == Mengine::ESocketServiceEventType::MessageReceived )
            {
                if( m_state != NetworkClientState::Handshaking && m_state != NetworkClientState::Connected )
                {
                    continue;
                }
                this->onMessage_( event.channel, event.payload );
            }
        }

        this->sendControlMessages_();
        ++m_networkUpdateOrdinal;
        if( m_state == NetworkClientState::Connected && m_commands.getPendingCount() != 0 && m_networkUpdateOrdinal % 3u == 0u )
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
        if( m_state != NetworkClientState::Connected )
        {
            return false;
        }
        ECommandStream stream = getCommandStream( type );
        size_t predictionCount = 0;
        for( const CommandEnvelope & pending : m_predictionCommands )
        {
            if( getCommandStream( pending.type ) == stream )
            {
                ++predictionCount;
            }
        }
        size_t limit = stream == ECommandStream::Control ? MAXIMUM_PENDING_CONTROL_COMMANDS : MAXIMUM_PENDING_GAMEPLAY_COMMANDS;
        if( predictionCount >= limit )
        {
            return false;
        }
        CommandEnvelope command;
        uint64_t tick = m_simulation.state().tick;
        if( m_commands.makeCommand( type, sourceTimeMs, tick, payload, &command ) == false )
        {
            return false;
        }
        m_predictionCommands.emplace_back( command );
        this->sendPendingCommands_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::disconnect( uint32_t reason )
    {
        Mengine::SocketPeerId peerId = m_peerId;
        this->clearConnection_( NetworkClientState::Disconnected );
        if( peerId == Mengine::InvalidSocketPeerId )
        {
            return;
        }
        this->disconnectPeer_( peerId, reason );
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::clearConnection_( NetworkClientState _state )
    {
        m_peerId = Mengine::InvalidSocketPeerId;
        m_state = _state;
        m_helloPending = false;
        m_snapshotRequestPending = false;
        m_playerId = 0;
        m_commands.clear();
        m_lastPredictedSequences = {};
        m_predictionCommands.clear();
        m_events.clear();
        m_roster.clear();
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
        size_t count = m_commands.getPendingCount();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NetworkClient::sendHello_()
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
        if( Detail::sendWire( m_socketService, m_peerId, 0, Mengine::ESocketDelivery::ReliableOrdered, writer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::requestSnapshot_()
    {
        m_snapshotRequestPending = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NetworkClient::sendSnapshotRequest_()
    {
        CanonicalWriter request;
        Detail::writeHeader( &request, NetworkMessage::SnapshotRequest );
        if( Detail::sendWire( m_socketService, m_peerId, 3, Mengine::ESocketDelivery::ReliableOrdered, request ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::sendControlMessages_()
    {
        if( m_disconnectPeerId != Mengine::InvalidSocketPeerId )
        {
            if( m_socketService->disconnect( m_disconnectPeerId, m_disconnectReason ) == true )
            {
                m_disconnectPeerId = Mengine::InvalidSocketPeerId;
            }
        }
        if( m_state == NetworkClientState::Handshaking && m_helloPending == true )
        {
            if( this->sendHello_() == true )
            {
                m_helloPending = false;
            }
        }
        if( m_state == NetworkClientState::Connected && m_snapshotRequestPending == true )
        {
            if( this->sendSnapshotRequest_() == true )
            {
                m_snapshotRequestPending = false;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::disconnectPeer_( Mengine::SocketPeerId _peerId, uint32_t _reason )
    {
        m_disconnectPeerId = _peerId;
        m_disconnectReason = _reason;
        this->sendControlMessages_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool NetworkClient::sendPendingCommands_()
    {
        VectorCommandEnvelope pending = m_commands.getPendingCommands( 64 );
        if( pending.empty() == true )
        {
            return true;
        }
        size_t count = pending.size();
        CanonicalWriter writer;
        Detail::writeHeader( &writer, NetworkMessage::CommandBatch );
        writer.writeU32( static_cast<uint32_t>(count) );
        for( size_t index = 0; index != count; ++index )
        {
            serializeCommand( &writer, pending[index] );
        }
        if( Detail::sendWire( m_socketService, m_peerId, 1, Mengine::ESocketDelivery::UnreliableSequenced, writer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::predictTick_()
    {
        const uint64_t tick = m_simulation.state().tick;
        VectorCommandEnvelope commands;
        for( const CommandEnvelope & pending : m_predictionCommands )
        {
            ECommandStream stream = getCommandStream( pending.type );
            uint64_t & lastPredictedSequence = m_lastPredictedSequences[static_cast<size_t>(stream)];
            if( pending.sequence <= lastPredictedSequence || pending.appliedTick > tick )
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
            lastPredictedSequence = command.sequence;
        }

        m_simulation.step( commands );
    }
    //////////////////////////////////////////////////////////////////////////
    void NetworkClient::predictToTick_( uint64_t tick )
    {
        uint64_t matchId = m_simulation.state().matchId;
        while( m_simulation.state().tick < tick )
        {
            uint64_t previousTick = m_simulation.state().tick;
            this->predictTick_();
            if( m_simulation.state().tick == previousTick )
            {
                break;
            }
            if( m_simulation.state().matchId != matchId )
            {
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool NetworkClient::applyProgress_( uint64_t _acknowledged, uint64_t _completedMask, ECommandStream _stream )
    {
        if( m_commands.confirmProgress( _acknowledged, _completedMask, _stream ) == false )
        {
            return false;
        }
        m_predictionCommands.erase( Mengine::StdAlgorithm::remove_if( m_predictionCommands.begin(), m_predictionCommands.end(),
            [_acknowledged, _completedMask, _stream]( const CommandEnvelope & _command )
            {
                if( getCommandStream( _command.type ) != _stream )
                {
                    return false;
                }
                if( _command.sequence <= _acknowledged )
                {
                    return true;
                }
                uint64_t distance = _command.sequence - _acknowledged;
                if( distance > 64 )
                {
                    return false;
                }
                if( (_completedMask & (UINT64_C(1) << (distance - 1))) == 0 )
                {
                    return false;
                }

                return true;
            } ), m_predictionCommands.end() );

        return true;
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
            if( reader.readU32( &reason ) == false )
            {
                return;
            }
            Mengine::String rejection;
            if( reader.readString( &rejection ) == false )
            {
                return;
            }
            if( reader.remaining() != 0 )
            {
                return;
            }
            m_rejection = Mengine::StdUtility::move( rejection );
            Mengine::SocketPeerId peerId = m_peerId;
            this->clearConnection_( NetworkClientState::Rejected );
            this->disconnectPeer_( peerId, reason );
            return;
        }
        if( message == NetworkMessage::Welcome && channel == 3 )
        {
            uint64_t matchId = 0, seed = 0, tick = 0, acknowledged = 0, completedMask = 0;
            uint64_t controlAcknowledged = 0, controlCompletedMask = 0;
            uint32_t playerId = 0, size = 0, compressedSize = 0;
            if( reader.readU32( &playerId ) == false )
            {
                return;
            }
            if( reader.readU64( &matchId ) == false )
            {
                return;
            }
            if( reader.readU64( &seed ) == false )
            {
                return;
            }
            if( reader.readU64( &tick ) == false )
            {
                return;
            }
            if( reader.readU64( &acknowledged ) == false )
            {
                return;
            }
            if( reader.readU64( &completedMask ) == false )
            {
                return;
            }
            if( reader.readU64( &controlAcknowledged ) == false )
            {
                return;
            }
            if( reader.readU64( &controlCompletedMask ) == false )
            {
                return;
            }
            if( reader.readU32( &size ) == false )
            {
                return;
            }
            if( reader.readU32( &compressedSize ) == false )
            {
                return;
            }
            bool sameSession = m_state == NetworkClientState::Connected && matchId == m_commands.getMatchId() && playerId == m_playerId;
            Mengine::Data snapshot;
            if( Detail::decompressSnapshot( &reader, size, compressedSize, &snapshot ) == false )
            {
                return;
            }
            Simulation authoritative( m_simulation.config() );
            if( authoritative.restoreSnapshot( snapshot ) == false )
            {
                return;
            }
            if( authoritative.state().matchId != matchId || authoritative.state().seed != seed || authoritative.state().tick != tick )
            {
                return;
            }
            if( sameSession == false )
            {
                if( acknowledged != 0 )
                {
                    return;
                }
                if( completedMask != 0 )
                {
                    return;
                }
                if( controlAcknowledged != 0 )
                {
                    return;
                }
                if( controlCompletedMask != 0 )
                {
                    return;
                }
                if( m_commands.reset( matchId, playerId ) == false )
                {
                    return;
                }
                m_predictionCommands.clear();
                m_playerId = playerId;
            }
            if( m_commands.validateProgress( acknowledged, completedMask, ECommandStream::Gameplay ) == false )
            {
                return;
            }
            if( m_commands.validateProgress( controlAcknowledged, controlCompletedMask, ECommandStream::Control ) == false )
            {
                return;
            }
            if( this->applyProgress_( acknowledged, completedMask, ECommandStream::Gameplay ) == false )
            {
                return;
            }
            if( this->applyProgress_( controlAcknowledged, controlCompletedMask, ECommandStream::Control ) == false )
            {
                return;
            }
            uint64_t predictedTick = sameSession == true ? m_simulation.state().tick : tick;
            m_simulation = Mengine::StdUtility::move( authoritative );
            m_lastPredictedSequences = {};
            uint64_t targetTick = Mengine::StdAlgorithm::max( predictedTick, tick );
            this->predictToTick_( targetTick );
            m_helloPending = false;
            m_snapshotRequestPending = false;
            m_state = NetworkClientState::Connected;
            return;
        }
        if( message == NetworkMessage::StateFrame && channel == 2 && m_state == NetworkClientState::Connected )
        {
            uint64_t matchId = 0, tick = 0, acknowledged = 0, completedMask = 0;
            uint64_t controlAcknowledged = 0, controlCompletedMask = 0;
            uint32_t checksum = 0, eventCount = 0, snapshotSize = 0, compressedSize = 0;
            if( reader.readU64( &matchId ) == false )
            {
                return;
            }
            if( reader.readU64( &tick ) == false )
            {
                return;
            }
            if( reader.readU64( &acknowledged ) == false )
            {
                return;
            }
            if( reader.readU64( &completedMask ) == false )
            {
                return;
            }
            if( reader.readU64( &controlAcknowledged ) == false )
            {
                return;
            }
            if( reader.readU64( &controlCompletedMask ) == false )
            {
                return;
            }
            if( reader.readU32( &checksum ) == false )
            {
                return;
            }
            if( reader.readU32( &eventCount ) == false )
            {
                return;
            }
            if( eventCount > NetworkMaximumFrameEvents )
            {
                return;
            }
            if( matchId != m_commands.getMatchId() )
            {
                return;
            }
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
                this->requestSnapshot_();
                return;
            }
            // Validate the authoritative baseline before releasing pending inputs.
            Simulation authoritative( m_simulation.config() );
            if( authoritative.restoreSnapshot( snapshot ) == false )
            {
                this->requestSnapshot_();
                return;
            }
            if( authoritative.state().matchId != matchId || authoritative.state().tick != tick || authoritative.checksum().full != checksum )
            {
                this->requestSnapshot_();
                return;
            }
            if( m_commands.validateProgress( acknowledged, completedMask, ECommandStream::Gameplay ) == false )
            {
                return;
            }
            if( m_commands.validateProgress( controlAcknowledged, controlCompletedMask, ECommandStream::Control ) == false )
            {
                return;
            }
            if( this->applyProgress_( acknowledged, completedMask, ECommandStream::Gameplay ) == false )
            {
                return;
            }
            if( this->applyProgress_( controlAcknowledged, controlCompletedMask, ECommandStream::Control ) == false )
            {
                return;
            }
            const bool predictionMatches = predictedTick == tick && before.full == checksum;
            if( predictionMatches == false )
            {
                m_simulation = Mengine::StdUtility::move( authoritative );
                m_lastPredictedSequences = {};
                uint64_t targetTick = Mengine::StdAlgorithm::max( predictedTick, tick );
                this->predictToTick_( targetTick );
                ++m_reconciliationCount;
            }
            m_snapshotRequestPending = false;
            m_events.swap( events );
            return;
        }
        if( message == NetworkMessage::Confirmations && channel == 1 && m_state == NetworkClientState::Connected )
        {
            uint64_t matchId = 0, sequence = 0;
            int32_t status = 0;
            uint8_t streamValue = 0;
            if( reader.readU64( &matchId ) == false )
            {
                return;
            }
            if( reader.readU8( &streamValue ) == false )
            {
                return;
            }
            if( streamValue > static_cast<uint8_t>(ECommandStream::Control) )
            {
                return;
            }
            if( reader.readU64( &sequence ) == false )
            {
                return;
            }
            if( reader.readI32( &status ) == false )
            {
                return;
            }
            if( reader.remaining() != 0 )
            {
                return;
            }
            if( matchId != m_commands.getMatchId() )
            {
                return;
            }
            ECommandStream stream = static_cast<ECommandStream>(streamValue);
            m_commands.confirm( sequence, status, stream );
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
