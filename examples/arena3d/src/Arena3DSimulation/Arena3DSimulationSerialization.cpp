#include "Arena3DSimulation.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static Mengine::String makeDifferencePrefix( const char * _name, uint32_t _id )
        {
            char buffer[64];
            MENGINE_SNPRINTF( buffer, sizeof( buffer ), "%s[%u].", _name, _id );
            return Mengine::String( buffer );
        }
        //////////////////////////////////////////////////////////////////////////
        static void writePcg( CanonicalWriter * writer, const Pcg32 & random )
        {
            writer->writeU64( random.state() );
            writer->writeU64( random.stream() );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool readPcg( CanonicalReader * reader, Pcg32 * random )
        {
            uint64_t state = 0;
            uint64_t stream = 0;
            if( reader->readU64( &state ) == false || reader->readU64( &stream ) == false )
            {
                return false;
            }
            random->restore( state, stream );
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    const SimulationState & Simulation::state() const
    {
        return m_state;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorServerEvent & Simulation::events() const
    {
        return m_events;
    }
    //////////////////////////////////////////////////////////////////////////
    const SimulationConfig & Simulation::config() const
    {
        return m_config;
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::serializePlayers_( CanonicalWriter * writer ) const
    {
        writer->writeU32( static_cast<uint32_t>(m_state.players.size()) );
        for( const PlayerState & player : m_state.players )
        {
            writer->writeBool( player.active );
            if( player.active == false )
            {
                continue;
            }
            writer->writeU32( player.id );
            writer->writeVector( player.position );
            writer->writeVector( player.velocity );
            writer->writeU16( player.yaw );
            writer->writeI16( player.pitch );
            writer->writeBool( player.grounded );
            writer->writeBool( player.crouched );
            writer->writeBool( player.alive );
            writer->writeBool( player.noclip );
            writer->writeBool( player.fireHeld );
            writer->writeBool( player.fireConsumed );
            writer->writeU8( player.inputMask );
            writer->writeI32( player.health );
            writer->writeI32( player.armor );
            writer->writeI32( player.score );
            writer->writeI32( player.deaths );
            writer->writeU32( player.respawnTicks );
            writer->writeU8( static_cast<uint8_t>(player.selectedWeapon) );
            for( uint32_t ammo : player.ammo )
            {
                writer->writeU32( ammo );
            }
            for( uint32_t cooldown : player.cooldown )
            {
                writer->writeU32( cooldown );
            }
            writer->writeFixed( player.nailHeat );
            writer->writeU32( player.railChargeTicks );
            writer->writeU64( player.shotSequence );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::serializeProjectiles_( CanonicalWriter * writer ) const
    {
        writer->writeU32( static_cast<uint32_t>(m_state.projectiles.size()) );
        for( const ProjectileState & projectile : m_state.projectiles )
        {
            writer->writeBool( projectile.active );
            if( projectile.active == false )
            {
                continue;
            }
            writer->writeU32( projectile.id );
            writer->writeU32( projectile.ownerId );
            writer->writeU8( static_cast<uint8_t>(projectile.weapon) );
            writer->writeVector( projectile.position );
            writer->writeVector( projectile.velocity );
            writer->writeFixed( projectile.radius );
            writer->writeFixed( projectile.gravity );
            writer->writeFixed( projectile.traveledDistance );
            writer->writeU32( projectile.lifetimeTicks );
            writer->writeU32( projectile.launchDelayTicks );
            writer->writeI32( projectile.damage );
            writer->writeU64( projectile.shotId );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::serializeTurrets_( CanonicalWriter * writer ) const
    {
        writer->writeU32( static_cast<uint32_t>(m_state.turrets.size()) );
        for( const TurretStateData & turret : m_state.turrets )
        {
            writer->writeU32( turret.id );
            writer->writeU8( static_cast<uint8_t>(turret.weapon) );
            writer->writeVector( turret.position );
            writer->writeU16( turret.yaw );
            writer->writeI16( turret.pitch );
            writer->writeU8( static_cast<uint8_t>(turret.state) );
            writer->writeI32( turret.health );
            writer->writeU32( turret.cooldownTicks );
            writer->writeU32( turret.chargeTicks );
            writer->writeU32( turret.targetMemoryTicks );
            writer->writeU32( turret.respawnTicks );
            Detail::writePcg( writer, turret.aimRandom );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::serializePickups_( CanonicalWriter * writer ) const
    {
        writer->writeU32( static_cast<uint32_t>(m_state.pickups.size()) );
        for( const PickupStateData & pickup : m_state.pickups )
        {
            writer->writeU32( pickup.id );
            writer->writeU8( static_cast<uint8_t>(pickup.type) );
            writer->writeU8( static_cast<uint8_t>(pickup.weapon) );
            writer->writeVector( pickup.position );
            writer->writeI32( pickup.amount );
            writer->writeU32( pickup.respawnTicks );
            writer->writeBool( pickup.active );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::serializeRandom_( CanonicalWriter * writer ) const
    {
        Detail::writePcg( writer, m_state.weaponRandom );
        Detail::writePcg( writer, m_state.pickupRandom );
    }
    //////////////////////////////////////////////////////////////////////////
    void Simulation::serializeState_( CanonicalWriter * writer ) const
    {
        writer->writeU32( SimulationVersion );
        writer->writeU32( DeterministicMathVersion );
        writer->writeU64( m_state.matchId );
        writer->writeU64( m_state.tick );
        writer->writeU64( m_state.seed );
        writer->writeU32( m_state.nextPlayerId );
        writer->writeU32( m_state.nextObjectId );
        writer->writeBool( m_state.paused );
        this->serializePlayers_( writer );
        this->serializeProjectiles_( writer );
        this->serializeTurrets_( writer );
        this->serializePickups_( writer );
        this->serializeRandom_( writer );
    }
    //////////////////////////////////////////////////////////////////////////
    StateChecksum Simulation::checksum() const
    {
        StateChecksum result;
        CanonicalWriter writer;
        this->serializePlayers_( &writer );
        result.player = crc32( writer.data() );
        writer.clear();
        this->serializeProjectiles_( &writer );
        result.projectiles = crc32( writer.data() );
        writer.clear();
        this->serializeTurrets_( &writer );
        result.turrets = crc32( writer.data() );
        writer.clear();
        this->serializePickups_( &writer );
        result.pickups = crc32( writer.data() );
        writer.clear();
        this->serializeRandom_( &writer );
        result.random = crc32( writer.data() );
        writer.clear();
        this->serializeState_( &writer );
        result.full = crc32( writer.data() );
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    Mengine::Data Simulation::snapshot() const
    {
        CanonicalWriter stateWriter;
        this->serializeState_( &stateWriter );
        CanonicalWriter writer;
        writer.writeBytes( "A3SN", 4 );
        writer.writeU32( SnapshotFormatVersion );
        writer.writeU32( static_cast<uint32_t>(stateWriter.data().size()) );
        writer.writeU32( crc32( stateWriter.data() ) );
        writer.writeBytes( stateWriter.data().data(), stateWriter.data().size() );
        return writer.take();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Simulation::deserializeState_( CanonicalReader * reader )
    {
        uint32_t simulationVersion = 0;
        uint32_t mathVersion = 0;
        if( reader->readU32( &simulationVersion ) == false || reader->readU32( &mathVersion ) == false ||
            simulationVersion != SimulationVersion || mathVersion != DeterministicMathVersion || reader->readU64( &m_state.matchId ) == false ||
            reader->readU64( &m_state.tick ) == false || reader->readU64( &m_state.seed ) == false || reader->readU32( &m_state.nextPlayerId ) == false || reader->readU32( &m_state.nextObjectId ) == false ||
            reader->readBool( &m_state.paused ) == false ) return false;

        uint32_t playerCount = 0;
        if( reader->readU32( &playerCount ) == false || playerCount != m_state.players.size() )
        {
            return false;
        }
        for( PlayerState & player : m_state.players )
        {
            player = PlayerState{};
            if( reader->readBool( &player.active ) == false )
            {
                return false;
            }
            if( player.active == false )
            {
                continue;
            }
            uint8_t selected = 0;
            if( reader->readU32( &player.id ) == false || reader->readVector( &player.position ) == false || reader->readVector( &player.velocity ) == false ||
                reader->readU16( &player.yaw ) == false || reader->readI16( &player.pitch ) == false || reader->readBool( &player.grounded ) == false ||
                reader->readBool( &player.crouched ) == false || reader->readBool( &player.alive ) == false || reader->readBool( &player.noclip ) == false ||
                reader->readBool( &player.fireHeld ) == false || reader->readBool( &player.fireConsumed ) == false || reader->readU8( &player.inputMask ) == false ||
                reader->readI32( &player.health ) == false || reader->readI32( &player.armor ) == false || reader->readI32( &player.score ) == false || reader->readI32( &player.deaths ) == false ||
                reader->readU32( &player.respawnTicks ) == false || reader->readU8( &selected ) == false || selected >= WeaponTypeCount ) return false;
            player.selectedWeapon = static_cast<WeaponType>(selected);
            for( uint32_t & ammo : player.ammo )
            {
                if( reader->readU32( &ammo ) == false )
                {
                    return false;
                }
            }
            for( uint32_t & cooldown : player.cooldown )
            {
                if( reader->readU32( &cooldown ) == false )
                {
                    return false;
                }
            }
            if( reader->readFixed( &player.nailHeat ) == false || reader->readU32( &player.railChargeTicks ) == false || reader->readU64( &player.shotSequence ) == false )
            {
                return false;
            }
        }

        uint32_t projectileCount = 0;
        if( reader->readU32( &projectileCount ) == false || projectileCount != m_state.projectiles.size() )
        {
            return false;
        }
        for( ProjectileState & projectile : m_state.projectiles )
        {
            projectile = ProjectileState{};
            if( reader->readBool( &projectile.active ) == false )
            {
                return false;
            }
            if( projectile.active == false )
            {
                continue;
            }
            uint8_t weapon = 0;
            if( reader->readU32( &projectile.id ) == false || reader->readU32( &projectile.ownerId ) == false || reader->readU8( &weapon ) == false ||
                weapon >= WeaponTypeCount || reader->readVector( &projectile.position ) == false || reader->readVector( &projectile.velocity ) == false ||
                reader->readFixed( &projectile.radius ) == false || reader->readFixed( &projectile.gravity ) == false || reader->readFixed( &projectile.traveledDistance ) == false ||
                reader->readU32( &projectile.lifetimeTicks ) == false || reader->readU32( &projectile.launchDelayTicks ) == false || reader->readI32( &projectile.damage ) == false || reader->readU64( &projectile.shotId ) == false ) return false;
            projectile.weapon = static_cast<WeaponType>(weapon);
        }

        uint32_t turretCount = 0;
        if( reader->readU32( &turretCount ) == false || turretCount != m_state.turrets.size() )
        {
            return false;
        }
        for( TurretStateData & turret : m_state.turrets )
        {
            uint8_t weapon = 0;
            uint8_t state = 0;
            if( reader->readU32( &turret.id ) == false || reader->readU8( &weapon ) == false || weapon >= WeaponTypeCount ||
                reader->readVector( &turret.position ) == false || reader->readU16( &turret.yaw ) == false || reader->readI16( &turret.pitch ) == false ||
                reader->readU8( &state ) == false || state > static_cast<uint8_t>(TurretState::Respawning) || reader->readI32( &turret.health ) == false ||
                reader->readU32( &turret.cooldownTicks ) == false || reader->readU32( &turret.chargeTicks ) == false ||
                reader->readU32( &turret.targetMemoryTicks ) == false || reader->readU32( &turret.respawnTicks ) == false || Detail::readPcg( reader, &turret.aimRandom ) == false ) return false;
            turret.weapon = static_cast<WeaponType>(weapon);
            turret.state = static_cast<TurretState>(state);
        }
        uint32_t pickupCount = 0;
        if( reader->readU32( &pickupCount ) == false || pickupCount != m_state.pickups.size() )
        {
            return false;
        }
        for( PickupStateData & pickup : m_state.pickups )
        {
            uint8_t type = 0;
            uint8_t weapon = 0;
            if( reader->readU32( &pickup.id ) == false || reader->readU8( &type ) == false || type > static_cast<uint8_t>(PickupType::Ammo) ||
                reader->readU8( &weapon ) == false || weapon >= WeaponTypeCount || reader->readVector( &pickup.position ) == false ||
                reader->readI32( &pickup.amount ) == false || reader->readU32( &pickup.respawnTicks ) == false || reader->readBool( &pickup.active ) == false ) return false;
            pickup.type = static_cast<PickupType>(type);
            pickup.weapon = static_cast<WeaponType>(weapon);
        }
        return Detail::readPcg( reader, &m_state.weaponRandom ) && Detail::readPcg( reader, &m_state.pickupRandom );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Simulation::restoreSnapshot( const Mengine::Data & snapshotData )
    {
        CanonicalReader reader( snapshotData );
        char magic[4]{};
        uint32_t version = 0;
        uint32_t size = 0;
        uint32_t expectedCrc = 0;
        if( reader.readBytes( magic, 4 ) == false || Mengine::StdString::memcmp( magic, "A3SN", 4 ) != 0 || reader.readU32( &version ) == false ||
            version != SnapshotFormatVersion || reader.readU32( &size ) == false || reader.readU32( &expectedCrc ) == false || size != reader.remaining() ) return false;
        Mengine::Data payload( size );
        if( reader.readBytes( payload.data(), payload.size() ) == false || crc32( payload ) != expectedCrc )
        {
            return false;
        }
        Simulation candidate( m_config );
        CanonicalReader payloadReader( payload );
        if( candidate.deserializeState_( &payloadReader ) == false || payloadReader.valid() == false || payloadReader.remaining() != 0 )
        {
            return false;
        }
        m_state = candidate.m_state;
        m_events.clear();
        m_damageCount = 0;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Mengine::String describeFirstDifference( const SimulationState & expected, const SimulationState & actual )
    {
        if( expected.matchId != actual.matchId )
        {
            return "state.matchId";
        }
        if( expected.tick != actual.tick )
        {
            return "state.tick";
        }
        if( expected.seed != actual.seed )
        {
            return "state.seed";
        }
        if( expected.nextPlayerId != actual.nextPlayerId )
        {
            return "state.nextPlayerId";
        }
        if( expected.nextObjectId != actual.nextObjectId )
        {
            return "state.nextObjectId";
        }
        if( expected.paused != actual.paused )
        {
            return "state.paused";
        }
        for( size_t index = 0; index != expected.players.size(); ++index )
        {
            const PlayerState & left = expected.players[index];
            const PlayerState & right = actual.players[index];
            const Mengine::String prefix = Detail::makeDifferencePrefix( "player", left.active ? left.id : static_cast<uint32_t>(index) );
            if( left.active != right.active )
            {
                return prefix + "active";
            }
            if( left.id != right.id )
            {
                return prefix + "id";
            }
            if( left.position.x != right.position.x )
            {
                return prefix + "position.x";
            }
            if( left.position.y != right.position.y )
            {
                return prefix + "position.y";
            }
            if( left.position.z != right.position.z )
            {
                return prefix + "position.z";
            }
            if( left.velocity.x != right.velocity.x )
            {
                return prefix + "velocity.x";
            }
            if( left.velocity.y != right.velocity.y )
            {
                return prefix + "velocity.y";
            }
            if( left.velocity.z != right.velocity.z )
            {
                return prefix + "velocity.z";
            }
            if( left.yaw != right.yaw )
            {
                return prefix + "yaw";
            }
            if( left.pitch != right.pitch )
            {
                return prefix + "pitch";
            }
            if( left.health != right.health )
            {
                return prefix + "health";
            }
            if( left.armor != right.armor )
            {
                return prefix + "armor";
            }
            if( left.score != right.score )
            {
                return prefix + "score";
            }
            if( left.deaths != right.deaths )
            {
                return prefix + "deaths";
            }
            if( left.inputMask != right.inputMask )
            {
                return prefix + "inputMask";
            }
            if( left.selectedWeapon != right.selectedWeapon )
            {
                return prefix + "selectedWeapon";
            }
        }
        for( size_t index = 0; index != expected.projectiles.size(); ++index )
        {
            const ProjectileState & left = expected.projectiles[index];
            const ProjectileState & right = actual.projectiles[index];
            const Mengine::String prefix = Detail::makeDifferencePrefix( "projectile", left.active ? left.id : static_cast<uint32_t>(index) );
            if( left.active != right.active )
            {
                return prefix + "active";
            }
            if( left.id != right.id )
            {
                return prefix + "id";
            }
            if( left.position.x != right.position.x )
            {
                return prefix + "position.x";
            }
            if( left.position.y != right.position.y )
            {
                return prefix + "position.y";
            }
            if( left.position.z != right.position.z )
            {
                return prefix + "position.z";
            }
            if( left.velocity.x != right.velocity.x )
            {
                return prefix + "velocity.x";
            }
            if( left.velocity.y != right.velocity.y )
            {
                return prefix + "velocity.y";
            }
            if( left.velocity.z != right.velocity.z )
            {
                return prefix + "velocity.z";
            }
            if( left.lifetimeTicks != right.lifetimeTicks )
            {
                return prefix + "lifetimeTicks";
            }
            if( left.launchDelayTicks != right.launchDelayTicks )
            {
                return prefix + "launchDelayTicks";
            }
        }
        for( size_t index = 0; index != expected.turrets.size(); ++index )
        {
            const TurretStateData & left = expected.turrets[index];
            const TurretStateData & right = actual.turrets[index];
            const Mengine::String prefix = Detail::makeDifferencePrefix( "turret", left.id );
            if( left.state != right.state )
            {
                return prefix + "state";
            }
            if( left.health != right.health )
            {
                return prefix + "health";
            }
            if( left.cooldownTicks != right.cooldownTicks )
            {
                return prefix + "cooldownTicks";
            }
            if( left.chargeTicks != right.chargeTicks )
            {
                return prefix + "chargeTicks";
            }
            if( left.aimRandom.state() != right.aimRandom.state() )
            {
                return prefix + "random.state";
            }
        }
        for( size_t index = 0; index != expected.pickups.size(); ++index )
        {
            const PickupStateData & left = expected.pickups[index];
            const PickupStateData & right = actual.pickups[index];
            const Mengine::String prefix = Detail::makeDifferencePrefix( "pickup", left.id );
            if( left.active != right.active )
            {
                return prefix + "active";
            }
            if( left.respawnTicks != right.respawnTicks )
            {
                return prefix + "respawnTicks";
            }
        }
        if( expected.weaponRandom.state() != actual.weaponRandom.state() )
        {
            return "random.weapon.state";
        }
        if( expected.pickupRandom.state() != actual.pickupRandom.state() )
        {
            return "random.pickup.state";
        }
        return {};
    }}
