#include "Metacode.h"

namespace Metacode
{
    //////////////////////////////////////////////////////////////////////////
    uint32_t get_metacode_magic()
    {
        return 3133062829u;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t get_metacode_version()
    {
        return 9;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t get_metacode_protocol_version()
    {
        return 153;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t get_metacode_protocol_crc32()
    {
        return 3746598190; 
    }
    //////////////////////////////////////////////////////////////////////////
    const char * getHeaderErrorMessage( Metabuf::HeaderError _error )
    {
        switch( _error )
        {
        case Metabuf::HEADER_SUCCESSFUL: return "Successful";
        case Metabuf::HEADER_INVALID_MAGIC: return "invalid magic header";
        case Metabuf::HEADER_INVALID_VERSION: return "invalid version";
        case Metabuf::HEADER_INVALID_PROTOCOL_VERSION: return "invalid protocol version";
        case Metabuf::HEADER_INVALID_PROTOCOL_CRC32: return "invalid protocol crc32";
        case Metabuf::HEADER_INVALID_METAVERSION: return "invalid meta version";
        default: return "invalid error";
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Metabuf::HeaderError readHeader( const void * _buff, size_t _size, size_t & _read, uint32_t & _readVersion, uint32_t & _needVersion, uint32_t & _readProtocol, uint32_t & _needProtocol, uint32_t _metaVersion, uint32_t & _readMetaVersion )
    {
        uint32_t metacode_magic = get_metacode_magic();
        uint32_t metacode_version = get_metacode_version();
        uint32_t metacode_protocol_version = get_metacode_protocol_version();
        uint32_t metacode_protocol_crc32 = get_metacode_protocol_crc32();

        Metabuf::Reader ar(_buff, _size, _read);

        uint32_t head;
        ar.readPOD( head );

        if( head != metacode_magic )
        {
            return Metabuf::HEADER_INVALID_MAGIC;
        }

        uint32_t read_version;
        ar.readPOD( read_version );

        uint32_t read_protocol_version;
        ar.readPOD( read_protocol_version );

        uint32_t read_protocol_crc32;
        ar.readPOD( read_protocol_crc32 );

        uint32_t read_meta_version;
        ar.readPOD( read_meta_version );

        _readVersion = read_version;
        _needVersion = metacode_version;
        _readProtocol = read_protocol_version;
        _needProtocol = metacode_protocol_version;
        _readMetaVersion = read_meta_version;

        if( read_version != metacode_version )
        {
            return Metabuf::HEADER_INVALID_VERSION;
        }

        if( read_protocol_version != metacode_protocol_version )
        {
            return Metabuf::HEADER_INVALID_PROTOCOL_VERSION;
        }

        if( read_protocol_crc32 != metacode_protocol_crc32 )
        {
            return Metabuf::HEADER_INVALID_PROTOCOL_CRC32;
        }

        if( read_meta_version != _metaVersion )
        {
            return Metabuf::HEADER_INVALID_METAVERSION;
        }

        return Metabuf::HEADER_SUCCESSFUL;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t getInternalStringsCount()
    {
        return 31;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * getInternalString( uint32_t _index, uint32_t & _stringSize, int64_t & _stringHash )
    {
        struct internal_t
        {
            uint32_t size;
            const char * str;
            uint64_t hash;
        };

        const internal_t internals[] = {
            {17, "ResourceImageData", 3605914952971727146UL},
            {20, "ResourceImageDefault", 3470757930260756242UL},
            {18, "ResourceImageEmpty", 7148372445039453612UL},
            {33, "ResourceImageSubstractRGBAndAlpha", 1723658646726069585UL},
            {22, "ResourceImageSubstract", 16659728524181123102UL},
            {16, "ResourceParticle", 16164975587347302130UL},
            {16, "ResourceAstralax", 15806205534695974182UL},
            {12, "ResourceJSON", 7298292741407897678UL},
            {21, "ResourceTexturepacker", 12701421212601506810UL},
            {20, "ResourceCursorSystem", 4148560451948967337UL},
            {17, "ResourceCursorICO", 1207208513755767748UL},
            {12, "ResourceFile", 13744762813760447274UL},
            {13, "ResourceSound", 843945519076406244UL},
            {13, "ResourceMusic", 7025043015214611656UL},
            {14, "ResourceWindow", 2358163663307596582UL},
            {14, "ResourceMovie2", 4673878932342189948UL},
            {13, "ResourceMovie", 13025064015180611533UL},
            {13, "ResourceVideo", 16146105055103617384UL},
            {21, "ResourceImageSequence", 8213436962902335585UL},
            {11, "ResourceHIT", 8571701938869837048UL},
            {15, "ResourceModel3D", 10779914626559864605UL},
            {18, "ResourceImageSolid", 3375355703810878466UL},
            {22, "ResourceInternalObject", 4413538717023814496UL},
            {13, "ResourceShape", 3843798518083404622UL},
            {21, "ResourceCal3dSkeleton", 13093878591051467045UL},
            {22, "ResourceCal3dAnimation", 13846719098122425571UL},
            {17, "ResourceCal3dMesh", 14426262410430258751UL},
            {18, "ResourceSpineAtlas", 15866872663799921644UL},
            {31, "ResourceSpineAtlasTexturepacker", 6554204108359989850UL},
            {21, "ResourceSpineSkeleton", 8493456877221120145UL},
            {16, "ResourceExternal", 9701561299099324449UL},
        };

        const internal_t & internal = internals[_index];
        _stringSize = internal.size;
        _stringHash = internal.hash;

        return internal.str;
    }
    //////////////////////////////////////////////////////////////////////////
    bool readStrings( const void * _buff, size_t _size, size_t & _read, uint32_t & _stringCount )
    {
        Metabuf::Reader ar(_buff, _size, _read);

        uint32_t count;
        ar.readPOD( count );

        _stringCount = count;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * readString( const void * _buff, size_t _size, size_t & _read, uint32_t & _stringSize, int64_t & _stringHash )
    {
        Metabuf::Reader ar(_buff, _size, _read);

        uint32_t size;
        ar.readSize( size );

        int64_t hash;
        ar.readPOD( hash );

        const char * value = ar.current_buff<char>();
        ar.skip( size );

        _stringSize = size;
        _stringHash = hash;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Meta_Data
    { 
        uint32_t getVersion()
        {
            return 1;
        }
    
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_DataBlock()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        Meta_DataBlock::~Meta_DataBlock()
        {
            for( VectorMeta_Resource::const_iterator
                it = includes_Meta_Resource.begin(),
                it_end = includes_Meta_Resource.end();
            it != it_end;
            ++it )
            {
                delete *it;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t includeTypeCount;
            Metabuf::readSize( _buff, _size, _read, includeTypeCount );
        
            for( uint32_t i = 0; i != includeTypeCount; ++i )
            {
                uint32_t includeCount;
                Metabuf::readSize( _buff, _size, _read, includeCount );
        
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                switch( id )
                {
                case 4:
                    {
                        includes_Meta_FragmentShader.resize( includeCount );
        
                        for( Meta_DataBlock::Meta_FragmentShader & metadata : includes_Meta_FragmentShader )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 1:
                    {
                        includes_Meta_Include.resize( includeCount );
        
                        for( Meta_DataBlock::Meta_Include & metadata : includes_Meta_Include )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 6:
                    {
                        includes_Meta_Material.resize( includeCount );
        
                        for( Meta_DataBlock::Meta_Material & metadata : includes_Meta_Material )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 5:
                    {
                        includes_Meta_Program.resize( includeCount );
        
                        for( Meta_DataBlock::Meta_Program & metadata : includes_Meta_Program )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 2:
                    {
                        includes_Meta_VertexAttribute.resize( includeCount );
        
                        for( Meta_DataBlock::Meta_VertexAttribute & metadata : includes_Meta_VertexAttribute )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 3:
                    {
                        includes_Meta_VertexShader.resize( includeCount );
        
                        for( Meta_DataBlock::Meta_VertexShader & metadata : includes_Meta_VertexShader )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                }
            }
        
            uint32_t generatorTypeCount;
            Metabuf::readSize( _buff, _size, _read, generatorTypeCount );
        
            for( uint32_t i = 0; i != generatorTypeCount; ++i )
            {
                uint32_t generatorCount;
                Metabuf::readSize( _buff, _size, _read, generatorCount );
        
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_preparationIncludes( id, generatorCount );
        
                for( uint32_t j = 0; j != generatorCount; ++j )
                {
                    uint32_t generator_id;
                    Metabuf::readSize( _buff, _size, _read, generator_id );
        
                    this->_parseGenerators( _buff, _size, _read, generator_id, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            switch( _id )
            {
            case 4:
                {
                    includes_Meta_FragmentShader.reserve( _count );
                }break;
            case 1:
                {
                    includes_Meta_Include.reserve( _count );
                }break;
            case 6:
                {
                    includes_Meta_Material.reserve( _count );
                }break;
            case 5:
                {
                    includes_Meta_Program.reserve( _count );
                }break;
            case 2:
                {
                    includes_Meta_VertexAttribute.reserve( _count );
                }break;
            case 3:
                {
                    includes_Meta_VertexShader.reserve( _count );
                }break;
            case 7:
                {
                    includes_Meta_Resource.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 4:
                {
                    includes_Meta_FragmentShader.emplace_back( Meta_DataBlock::Meta_FragmentShader() );
                    Meta_DataBlock::Meta_FragmentShader & metadata = includes_Meta_FragmentShader.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 1:
                {
                    includes_Meta_Include.emplace_back( Meta_DataBlock::Meta_Include() );
                    Meta_DataBlock::Meta_Include & metadata = includes_Meta_Include.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 6:
                {
                    includes_Meta_Material.emplace_back( Meta_DataBlock::Meta_Material() );
                    Meta_DataBlock::Meta_Material & metadata = includes_Meta_Material.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 5:
                {
                    includes_Meta_Program.emplace_back( Meta_DataBlock::Meta_Program() );
                    Meta_DataBlock::Meta_Program & metadata = includes_Meta_Program.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 2:
                {
                    includes_Meta_VertexAttribute.emplace_back( Meta_DataBlock::Meta_VertexAttribute() );
                    Meta_DataBlock::Meta_VertexAttribute & metadata = includes_Meta_VertexAttribute.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 3:
                {
                    includes_Meta_VertexShader.emplace_back( Meta_DataBlock::Meta_VertexShader() );
                    Meta_DataBlock::Meta_VertexShader & metadata = includes_Meta_VertexShader.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::_parseGenerators( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 14:
                {
                    Meta_DataBlock::Meta_ResourceAstralax * metadata = new Meta_DataBlock::Meta_ResourceAstralax ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 33:
                {
                    Meta_DataBlock::Meta_ResourceCal3dAnimation * metadata = new Meta_DataBlock::Meta_ResourceCal3dAnimation ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 34:
                {
                    Meta_DataBlock::Meta_ResourceCal3dMesh * metadata = new Meta_DataBlock::Meta_ResourceCal3dMesh ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 32:
                {
                    Meta_DataBlock::Meta_ResourceCal3dSkeleton * metadata = new Meta_DataBlock::Meta_ResourceCal3dSkeleton ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 18:
                {
                    Meta_DataBlock::Meta_ResourceCursorICO * metadata = new Meta_DataBlock::Meta_ResourceCursorICO ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 17:
                {
                    Meta_DataBlock::Meta_ResourceCursorSystem * metadata = new Meta_DataBlock::Meta_ResourceCursorSystem ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 38:
                {
                    Meta_DataBlock::Meta_ResourceExternal * metadata = new Meta_DataBlock::Meta_ResourceExternal ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 19:
                {
                    Meta_DataBlock::Meta_ResourceFile * metadata = new Meta_DataBlock::Meta_ResourceFile ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 27:
                {
                    Meta_DataBlock::Meta_ResourceHIT * metadata = new Meta_DataBlock::Meta_ResourceHIT ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 8:
                {
                    Meta_DataBlock::Meta_ResourceImageData * metadata = new Meta_DataBlock::Meta_ResourceImageData ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 9:
                {
                    Meta_DataBlock::Meta_ResourceImageDefault * metadata = new Meta_DataBlock::Meta_ResourceImageDefault ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 10:
                {
                    Meta_DataBlock::Meta_ResourceImageEmpty * metadata = new Meta_DataBlock::Meta_ResourceImageEmpty ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 26:
                {
                    Meta_DataBlock::Meta_ResourceImageSequence * metadata = new Meta_DataBlock::Meta_ResourceImageSequence ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 29:
                {
                    Meta_DataBlock::Meta_ResourceImageSolid * metadata = new Meta_DataBlock::Meta_ResourceImageSolid ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 12:
                {
                    Meta_DataBlock::Meta_ResourceImageSubstract * metadata = new Meta_DataBlock::Meta_ResourceImageSubstract ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 11:
                {
                    Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha * metadata = new Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 30:
                {
                    Meta_DataBlock::Meta_ResourceInternalObject * metadata = new Meta_DataBlock::Meta_ResourceInternalObject ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 15:
                {
                    Meta_DataBlock::Meta_ResourceJSON * metadata = new Meta_DataBlock::Meta_ResourceJSON ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 28:
                {
                    Meta_DataBlock::Meta_ResourceModel3D * metadata = new Meta_DataBlock::Meta_ResourceModel3D ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 24:
                {
                    Meta_DataBlock::Meta_ResourceMovie * metadata = new Meta_DataBlock::Meta_ResourceMovie ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 23:
                {
                    Meta_DataBlock::Meta_ResourceMovie2 * metadata = new Meta_DataBlock::Meta_ResourceMovie2 ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 21:
                {
                    Meta_DataBlock::Meta_ResourceMusic * metadata = new Meta_DataBlock::Meta_ResourceMusic ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 13:
                {
                    Meta_DataBlock::Meta_ResourceParticle * metadata = new Meta_DataBlock::Meta_ResourceParticle ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 31:
                {
                    Meta_DataBlock::Meta_ResourceShape * metadata = new Meta_DataBlock::Meta_ResourceShape ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 20:
                {
                    Meta_DataBlock::Meta_ResourceSound * metadata = new Meta_DataBlock::Meta_ResourceSound ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 35:
                {
                    Meta_DataBlock::Meta_ResourceSpineAtlas * metadata = new Meta_DataBlock::Meta_ResourceSpineAtlas ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 36:
                {
                    Meta_DataBlock::Meta_ResourceSpineAtlasTexturepacker * metadata = new Meta_DataBlock::Meta_ResourceSpineAtlasTexturepacker ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 37:
                {
                    Meta_DataBlock::Meta_ResourceSpineSkeleton * metadata = new Meta_DataBlock::Meta_ResourceSpineSkeleton ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 16:
                {
                    Meta_DataBlock::Meta_ResourceTexturepacker * metadata = new Meta_DataBlock::Meta_ResourceTexturepacker ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 25:
                {
                    Meta_DataBlock::Meta_ResourceVideo * metadata = new Meta_DataBlock::Meta_ResourceVideo ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 22:
                {
                    Meta_DataBlock::Meta_ResourceWindow * metadata = new Meta_DataBlock::Meta_ResourceWindow ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_FragmentShader::Meta_FragmentShader()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_FragmentShader::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_FragmentShader::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_RenderPlatform );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_FragmentShader::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_File_Compile) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Compile );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_FragmentShader::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_FragmentShader::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Include::Meta_Include()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_Include::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Include::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Include::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Include::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Material::Meta_Material()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_Material::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_TextureStages.resize( includeCount );
        
                for( Meta_DataBlock::Meta_Material::Meta_TextureStages & metadata : includes_Meta_TextureStages )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Material::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Material::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Debug) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Debug );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_RenderPlatform) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_RenderPlatform );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_AlphaBlend_Enable) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_AlphaBlend_Enable );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_BlendFactor_Dest) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_BlendFactor_Dest );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_BlendFactor_Op) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_BlendFactor_Op );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_BlendFactor_Source) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_BlendFactor_Source );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_DepthBufferTest_Enable) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_DepthBufferTest_Enable );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_DepthBufferWrite_Enable) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_DepthBufferWrite_Enable );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Program_Name) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Program_Name );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlend_Enable) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_SeparateAlphaBlend_Enable );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Dest) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_SeparateAlphaBlendFactor_Dest );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Op) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_SeparateAlphaBlendFactor_Op );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_SeparateAlphaBlendFactor_Source) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_SeparateAlphaBlendFactor_Source );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Material::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Material::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_TextureStages.emplace_back( Meta_DataBlock::Meta_Material::Meta_TextureStages() );
                    Meta_DataBlock::Meta_Material::Meta_TextureStages & metadata = includes_Meta_TextureStages.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Material::Meta_TextureStages::Meta_TextureStages()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_Material::Meta_TextureStages::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Stage );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_AddressMode_Border) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_AddressMode_Border );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_AddressMode_U) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_AddressMode_U );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_AddressMode_V) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_AddressMode_V );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Material::Meta_TextureStages::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Program::Meta_Program()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_Program::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Program::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_FragmentShader_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Sampler_Count );
            Metabuf::read( _buff, _size, _read, _userData, this->m_VertexAttribute_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_VertexShader_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Program::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_RenderPlatform) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_RenderPlatform );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Program::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Program::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_VertexAttribute::Meta_VertexAttribute()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_VertexAttribute::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Attribute.resize( includeCount );
        
                for( Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute & metadata : includes_Meta_Attribute )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_VertexAttribute::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_RenderPlatform );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Element_Size );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_VertexAttribute::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_VertexAttribute::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Attribute.emplace_back( Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute() );
                    Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute & metadata = includes_Meta_Attribute.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::Meta_Attribute()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Index );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Normalized );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Offset );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Size );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Stride );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Type );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Uniform );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_VertexShader::Meta_VertexShader()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_VertexShader::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_VertexShader::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_RenderPlatform );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_VertexShader::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_File_Compile) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Compile );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_VertexShader::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_VertexShader::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Resource::Meta_Resource()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        Meta_DataBlock::Meta_Resource::~Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_Resource::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Resource::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Type );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Resource::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Precompile) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Precompile );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Unique) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Unique );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Resource::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_Resource::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceAstralax::Meta_ResourceAstralax()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceAstralax::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeTypeCount;
            Metabuf::readSize( _buff, _size, _read, includeTypeCount );
        
            for( uint32_t i = 0; i != includeTypeCount; ++i )
            {
                uint32_t includeCount;
                Metabuf::readSize( _buff, _size, _read, includeCount );
        
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_preparationIncludes( id, includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    this->_parseIncludes( _buff, _size, _read, id, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceAstralax::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_AtlasCount_Value );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceAstralax::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceAstralax::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceAstralax::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Atlas.emplace_back( Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas() );
                    Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas & metadata = includes_Meta_Atlas.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::Meta_Atlas()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Index );
            Metabuf::read( _buff, _size, _read, _userData, this->m_ResourceName );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceCal3dAnimation::Meta_ResourceCal3dAnimation()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceCal3dAnimation::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dAnimation::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceCal3dMesh::Meta_ResourceCal3dMesh()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceCal3dMesh::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dMesh::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dMesh::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dMesh::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dMesh::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceCal3dSkeleton::Meta_ResourceCal3dSkeleton()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceCal3dSkeleton::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceCursorICO::Meta_ResourceCursorICO()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceCursorICO::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCursorICO::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCursorICO::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCursorICO::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCursorICO::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceCursorSystem::Meta_ResourceCursorSystem()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceCursorSystem::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCursorSystem::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCursorSystem::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCursorSystem::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceCursorSystem::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceExternal::Meta_ResourceExternal()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceExternal::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceExternal::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceExternal::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceFile::Meta_ResourceFile()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceFile::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceFile::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceFile::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceFile::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceFile::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceHIT::Meta_ResourceHIT()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceHIT::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceHIT::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceHIT::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceHIT::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceHIT::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageData::Meta_ResourceImageData()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceImageData::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageData::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_MaxSize );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageData::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Alpha) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Alpha );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Offset) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Offset );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Premultiply) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Premultiply );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Size) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Size );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_TrimAtlas) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_TrimAtlas );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageData::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageData::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageDefault::Meta_ResourceImageDefault()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceImageDefault::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageDefault::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_MaxSize );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageDefault::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Alpha) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Alpha );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Offset) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Offset );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Premultiply) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Premultiply );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Size) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Size );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_TrimAtlas) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_TrimAtlas );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageDefault::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageDefault::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageEmpty::Meta_ResourceImageEmpty()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceImageEmpty::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageEmpty::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_MaxSize );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageEmpty::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageEmpty::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageSequence::Meta_ResourceImageSequence()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceImageSequence::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeTypeCount;
            Metabuf::readSize( _buff, _size, _read, includeTypeCount );
        
            for( uint32_t i = 0; i != includeTypeCount; ++i )
            {
                uint32_t includeCount;
                Metabuf::readSize( _buff, _size, _read, includeCount );
        
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_preparationIncludes( id, includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    this->_parseIncludes( _buff, _size, _read, id, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSequence::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSequence::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Sequence.emplace_back( Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence() );
                    Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence & metadata = includes_Meta_Sequence.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::Meta_Sequence()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Delay );
            Metabuf::read( _buff, _size, _read, _userData, this->m_ResourceImageName );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageSolid::Meta_ResourceImageSolid()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceImageSolid::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSolid::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Color_Value );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Size_Value );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSolid::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSolid::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageSubstract::Meta_ResourceImageSubstract()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceImageSubstract::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSubstract::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_MaxSize );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_UV );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSubstract::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_Image_Alpha) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Alpha );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Image_Offset) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Offset );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Image_Premultiply) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Premultiply );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Image_Size) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Size );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Image_UVRotate) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Image_UVRotate );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSubstract::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSubstract::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::Meta_ResourceImageSubstractRGBAndAlpha()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_MaxSize );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_NameAlpha );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_NameRGB );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_UVAlpha );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_UVRGB );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_Image_Offset) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Offset );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Image_Premultiply) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Premultiply );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Image_Size) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Size );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Image_UVAlphaRotate) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Image_UVAlphaRotate );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Image_UVRGBRotate) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Image_UVRGBRotate );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceInternalObject::Meta_ResourceInternalObject()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceInternalObject::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceInternalObject::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Internal_Group );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Internal_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceInternalObject::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceInternalObject::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceJSON::Meta_ResourceJSON()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceJSON::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceJSON::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceJSON::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceJSON::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceJSON::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceModel3D::Meta_ResourceModel3D()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceModel3D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceModel3D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Resource );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceModel3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Dataflow) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Dataflow );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceModel3D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceModel3D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie::Meta_ResourceMovie()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeTypeCount;
            Metabuf::readSize( _buff, _size, _read, includeTypeCount );
        
            for( uint32_t i = 0; i != includeTypeCount; ++i )
            {
                uint32_t includeCount;
                Metabuf::readSize( _buff, _size, _read, includeCount );
        
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_preparationIncludes( id, includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    this->_parseIncludes( _buff, _size, _read, id, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Duration_Value );
            Metabuf::read( _buff, _size, _read, _userData, this->m_FrameDuration_Value );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Height_Value );
            Metabuf::read( _buff, _size, _read, _userData, this->m_KeyFramesPackPath_Path );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Width_Value );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_Anchor_Point) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Anchor_Point );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Bounds_Box) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Bounds_Box );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_KeyFramesPackPath_Codec) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_KeyFramesPackPath_Codec );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_KeyFramesPackPath_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_KeyFramesPackPath_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Loop_Segment) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Loop_Segment );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Offset_Point) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Offset_Point );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
            switch( _id )
            {
            case 3:
                {
                    includes_Meta_MovieCamera3D.reserve( _count );
                }break;
            case 1:
                {
                    includes_Meta_MovieLayer2D.reserve( _count );
                }break;
            case 2:
                {
                    includes_Meta_MovieLayer3D.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 3:
                {
                    includes_Meta_MovieCamera3D.emplace_back( Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D() );
                    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D & metadata = includes_Meta_MovieCamera3D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 1:
                {
                    includes_Meta_MovieLayer2D.emplace_back( Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D() );
                    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D & metadata = includes_Meta_MovieLayer2D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 2:
                {
                    includes_Meta_MovieLayer3D.emplace_back( Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D() );
                    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D & metadata = includes_Meta_MovieLayer3D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::Meta_MovieCamera3D()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_CameraAspect );
            Metabuf::read( _buff, _size, _read, _userData, this->m_CameraFOV );
            Metabuf::read( _buff, _size, _read, _userData, this->m_CameraInterest );
            Metabuf::read( _buff, _size, _read, _userData, this->m_CameraPosition );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Height );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Width );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::Meta_MovieLayer2D()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_In );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Index );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Out );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Source );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Type );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_AnchorPoint );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_BlendingMode) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_BlendingMode );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Loop) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Loop );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Params) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Params );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Parent) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Parent );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_PlayCount) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_PlayCount );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Polygon) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Polygon );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Position) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Position );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Rotation) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Rotation );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Scale) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Scale );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Shape) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Shape );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_StartInterval) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_StartInterval );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Stretch) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Stretch );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Switch) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Switch );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_TimeRemap) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_TimeRemap );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Viewport) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Viewport );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::Meta_MovieLayer3D()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_In );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Index );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Out );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Source );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Type );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_AnchorPoint );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_BlendingMode) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_BlendingMode );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Loop) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Loop );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Params) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Params );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Parent) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Parent );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_PlayCount) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_PlayCount );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Polygon) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Polygon );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Position) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Position );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Rotation) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Rotation );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Scale) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Scale );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Shape) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Shape );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_StartInterval) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_StartInterval );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Stretch) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Stretch );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Switch) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Switch );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_TimeRemap) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_TimeRemap );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie2::Meta_ResourceMovie2()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie2::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeTypeCount;
            Metabuf::readSize( _buff, _size, _read, includeTypeCount );
        
            for( uint32_t i = 0; i != includeTypeCount; ++i )
            {
                uint32_t includeCount;
                Metabuf::readSize( _buff, _size, _read, includeCount );
        
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_preparationIncludes( id, includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    this->_parseIncludes( _buff, _size, _read, id, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Dataflow) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Dataflow );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Composition.emplace_back( Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition() );
                    Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition & metadata = includes_Meta_Composition.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Composition()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeTypeCount;
            Metabuf::readSize( _buff, _size, _read, includeTypeCount );
        
            for( uint32_t i = 0; i != includeTypeCount; ++i )
            {
                uint32_t includeCount;
                Metabuf::readSize( _buff, _size, _read, includeCount );
        
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                switch( id )
                {
                case 1:
                    {
                        includes_Meta_Layer.resize( includeCount );
        
                        for( Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer & metadata : includes_Meta_Layer )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 2:
                    {
                        includes_Meta_SubComposition.resize( includeCount );
        
                        for( Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition & metadata : includes_Meta_SubComposition )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Duration );
            Metabuf::read( _buff, _size, _read, _userData, this->m_FrameDuration );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Bounds) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Bounds );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Master) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Master );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Layer.reserve( _count );
                }break;
            case 2:
                {
                    includes_Meta_SubComposition.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Layer.emplace_back( Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer() );
                    Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer & metadata = includes_Meta_Layer.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 2:
                {
                    includes_Meta_SubComposition.emplace_back( Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition() );
                    Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition & metadata = includes_Meta_SubComposition.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::Meta_Layer()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Index );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Type );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Color) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Color );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Dimension) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Dimension );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Matrix) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Matrix );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Options) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Options );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_TrackMatte) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_TrackMatte );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::Meta_SubComposition()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Duration );
            Metabuf::read( _buff, _size, _read, _userData, this->m_FrameDuration );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Index );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMusic::Meta_ResourceMusic()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMusic::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMusic::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMusic::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_DefaultVolume_Value) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_DefaultVolume_Value );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_External) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_External );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMusic::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceMusic::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceParticle::Meta_ResourceParticle()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceParticle::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeTypeCount;
            Metabuf::readSize( _buff, _size, _read, includeTypeCount );
        
            for( uint32_t i = 0; i != includeTypeCount; ++i )
            {
                uint32_t includeCount;
                Metabuf::readSize( _buff, _size, _read, includeCount );
        
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_preparationIncludes( id, includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    this->_parseIncludes( _buff, _size, _read, id, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceParticle::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_AtlasCount_Value );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceParticle::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceParticle::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceParticle::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Atlas.emplace_back( Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas() );
                    Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas & metadata = includes_Meta_Atlas.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::Meta_Atlas()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Index );
            Metabuf::read( _buff, _size, _read, _userData, this->m_ResourceName );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceShape::Meta_ResourceShape()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceShape::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceShape::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Polygon_Value );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceShape::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceShape::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceSound::Meta_ResourceSound()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceSound::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSound::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSound::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_DefaultVolume_Value) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_DefaultVolume_Value );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_IsStreamable_Value) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_IsStreamable_Value );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSound::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSound::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceSpineAtlas::Meta_ResourceSpineAtlas()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceSpineAtlas::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeTypeCount;
            Metabuf::readSize( _buff, _size, _read, includeTypeCount );
        
            for( uint32_t i = 0; i != includeTypeCount; ++i )
            {
                uint32_t includeCount;
                Metabuf::readSize( _buff, _size, _read, includeCount );
        
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_preparationIncludes( id, includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    this->_parseIncludes( _buff, _size, _read, id, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineAtlas::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineAtlas::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineAtlas::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineAtlas::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Image.emplace_back( Meta_DataBlock::Meta_ResourceSpineAtlas::Meta_Image() );
                    Meta_DataBlock::Meta_ResourceSpineAtlas::Meta_Image & metadata = includes_Meta_Image.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceSpineAtlas::Meta_Image::Meta_Image()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceSpineAtlas::Meta_Image::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineAtlas::Meta_Image::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Resource );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineAtlas::Meta_Image::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineAtlas::Meta_Image::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceSpineAtlasTexturepacker::Meta_ResourceSpineAtlasTexturepacker()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceSpineAtlasTexturepacker::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineAtlasTexturepacker::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_Texturepacker_Name) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Texturepacker_Name );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineAtlasTexturepacker::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineAtlasTexturepacker::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceSpineSkeleton::Meta_ResourceSpineSkeleton()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceSpineSkeleton::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineSkeleton::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineSkeleton::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_Atlas_Name) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Atlas_Name );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineSkeleton::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceSpineSkeleton::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceTexturepacker::Meta_ResourceTexturepacker()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceTexturepacker::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceTexturepacker::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_JSON_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceTexturepacker::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceTexturepacker::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceVideo::Meta_ResourceVideo()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceVideo::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceVideo::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceVideo::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Alpha) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Alpha );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Codec) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Converter) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Duration) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Duration );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_FrameRate) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_FrameRate );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoExist) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_NoSeek) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoSeek );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Premultiply) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Premultiply );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_File_Resize) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_File_Resize );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceVideo::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceVideo::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceWindow::Meta_ResourceWindow()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceWindow::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceWindow::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowBottom_Offset );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowBottom_ResourceImageName );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowLeft_Offset );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowLeft_ResourceImageName );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowLeftBottom_Offset );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowLeftBottom_ResourceImageName );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowLeftTop_Offset );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowLeftTop_ResourceImageName );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowRight_Offset );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowRight_ResourceImageName );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowRightBottom_Offset );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowRightBottom_ResourceImageName );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowRightTop_Offset );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowRightTop_ResourceImageName );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowTop_Offset );
            Metabuf::read( _buff, _size, _read, _userData, this->m_WindowTop_ResourceImageName );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceWindow::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _userData );
        
            if( (m_flagNoRequiredAttribute & EMETA_WindowBackground_ResourceImageName) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_WindowBackground_ResourceImageName );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceWindow::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_DataBlock::Meta_ResourceWindow::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_KeyFramesPack()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t includeTypeCount;
            Metabuf::readSize( _buff, _size, _read, includeTypeCount );
        
            for( uint32_t i = 0; i != includeTypeCount; ++i )
            {
                uint32_t includeCount;
                Metabuf::readSize( _buff, _size, _read, includeCount );
        
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                switch( id )
                {
                case 3:
                    {
                        includes_Meta_ImageShape.resize( includeCount );
        
                        for( Meta_KeyFramesPack::Meta_ImageShape & metadata : includes_Meta_ImageShape )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 4:
                    {
                        includes_Meta_KeyFrames2D.resize( includeCount );
        
                        for( Meta_KeyFramesPack::Meta_KeyFrames2D & metadata : includes_Meta_KeyFrames2D )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 5:
                    {
                        includes_Meta_KeyFrames3D.resize( includeCount );
        
                        for( Meta_KeyFramesPack::Meta_KeyFrames3D & metadata : includes_Meta_KeyFrames3D )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 2:
                    {
                        includes_Meta_Polygon.resize( includeCount );
        
                        for( Meta_KeyFramesPack::Meta_Polygon & metadata : includes_Meta_Polygon )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 1:
                    {
                        includes_Meta_TimeRemap.resize( includeCount );
        
                        for( Meta_KeyFramesPack::Meta_TimeRemap & metadata : includes_Meta_TimeRemap )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_MaxIndex );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Version );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            switch( _id )
            {
            case 3:
                {
                    includes_Meta_ImageShape.reserve( _count );
                }break;
            case 4:
                {
                    includes_Meta_KeyFrames2D.reserve( _count );
                }break;
            case 5:
                {
                    includes_Meta_KeyFrames3D.reserve( _count );
                }break;
            case 2:
                {
                    includes_Meta_Polygon.reserve( _count );
                }break;
            case 1:
                {
                    includes_Meta_TimeRemap.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 3:
                {
                    includes_Meta_ImageShape.emplace_back( Meta_KeyFramesPack::Meta_ImageShape() );
                    Meta_KeyFramesPack::Meta_ImageShape & metadata = includes_Meta_ImageShape.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 4:
                {
                    includes_Meta_KeyFrames2D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames2D() );
                    Meta_KeyFramesPack::Meta_KeyFrames2D & metadata = includes_Meta_KeyFrames2D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 5:
                {
                    includes_Meta_KeyFrames3D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames3D() );
                    Meta_KeyFramesPack::Meta_KeyFrames3D & metadata = includes_Meta_KeyFrames3D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 2:
                {
                    includes_Meta_Polygon.emplace_back( Meta_KeyFramesPack::Meta_Polygon() );
                    Meta_KeyFramesPack::Meta_Polygon & metadata = includes_Meta_Polygon.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 1:
                {
                    includes_Meta_TimeRemap.emplace_back( Meta_KeyFramesPack::Meta_TimeRemap() );
                    Meta_KeyFramesPack::Meta_TimeRemap & metadata = includes_Meta_TimeRemap.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_ImageShape::Meta_ImageShape()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_ImageShape::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Shape.resize( includeCount );
        
                for( Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape & metadata : includes_Meta_Shape )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_ImageShape::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_ImageMaxSize );
            Metabuf::read( _buff, _size, _read, _userData, this->m_LayerIndex );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_ImageShape::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Count) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Count );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_ImageOffset) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_ImageOffset );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_ImageSize) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_ImageSize );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Immutable) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Immutable );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Subtract) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Subtract );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_ImageShape::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_ImageShape::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Shape.emplace_back( Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape() );
                    Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape & metadata = includes_Meta_Shape.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::Meta_Shape()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Polygon );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrames2D()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_KeyFrames2D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_KeyFrame2D.resize( includeCount );
        
                for( Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & metadata : includes_Meta_KeyFrame2D )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_LayerIndex );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Count) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Count );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Immutable) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Immutable );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_KeyFrame2D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D() );
                    Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & metadata = includes_Meta_KeyFrame2D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::Meta_KeyFrame2D()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_AnchorPoint );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Count) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Count );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Opacity) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Opacity );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Position) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Position );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Rotation) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Rotation );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Scale) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Scale );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Volume) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Volume );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrames3D()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_KeyFrames3D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_KeyFrame3D.resize( includeCount );
        
                for( Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D & metadata : includes_Meta_KeyFrame3D )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_LayerIndex );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Count) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Count );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Immutable) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Immutable );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_KeyFrame3D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D() );
                    Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D & metadata = includes_Meta_KeyFrame3D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::Meta_KeyFrame3D()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_AnchorPoint) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_AnchorPoint );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Count) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Count );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Opacity) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Opacity );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Orientation) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Orientation );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Position) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Position );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Rotation) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Rotation );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Scale) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Scale );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Volume) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Volume );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_Polygon::Meta_Polygon()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_Polygon::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_Polygon::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_LayerIndex );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Value );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_Polygon::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_Polygon::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_TimeRemap::Meta_TimeRemap()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_TimeRemap::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_TimeRemap::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_LayerIndex );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Time );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_TimeRemap::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_KeyFramesPack::Meta_TimeRemap::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Pak()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            uint32_t includeTypeCount;
            Metabuf::readSize( _buff, _size, _read, includeTypeCount );
        
            for( uint32_t i = 0; i != includeTypeCount; ++i )
            {
                uint32_t includeCount;
                Metabuf::readSize( _buff, _size, _read, includeCount );
        
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                switch( id )
                {
                case 6:
                    {
                        includes_Meta_Datas.resize( includeCount );
        
                        for( Meta_Pak::Meta_Datas & metadata : includes_Meta_Datas )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 3:
                    {
                        includes_Meta_Fonts.resize( includeCount );
        
                        for( Meta_Pak::Meta_Fonts & metadata : includes_Meta_Fonts )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 2:
                    {
                        includes_Meta_Glyphs.resize( includeCount );
        
                        for( Meta_Pak::Meta_Glyphs & metadata : includes_Meta_Glyphs )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 7:
                    {
                        includes_Meta_Materials.resize( includeCount );
        
                        for( Meta_Pak::Meta_Materials & metadata : includes_Meta_Materials )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 4:
                    {
                        includes_Meta_Resources.resize( includeCount );
        
                        for( Meta_Pak::Meta_Resources & metadata : includes_Meta_Resources )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 1:
                    {
                        includes_Meta_Scripts.resize( includeCount );
        
                        for( Meta_Pak::Meta_Scripts & metadata : includes_Meta_Scripts )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 8:
                    {
                        includes_Meta_Settings.resize( includeCount );
        
                        for( Meta_Pak::Meta_Settings & metadata : includes_Meta_Settings )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 5:
                    {
                        includes_Meta_Texts.resize( includeCount );
        
                        for( Meta_Pak::Meta_Texts & metadata : includes_Meta_Texts )
                        {
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
            switch( _id )
            {
            case 6:
                {
                    includes_Meta_Datas.reserve( _count );
                }break;
            case 3:
                {
                    includes_Meta_Fonts.reserve( _count );
                }break;
            case 2:
                {
                    includes_Meta_Glyphs.reserve( _count );
                }break;
            case 7:
                {
                    includes_Meta_Materials.reserve( _count );
                }break;
            case 4:
                {
                    includes_Meta_Resources.reserve( _count );
                }break;
            case 1:
                {
                    includes_Meta_Scripts.reserve( _count );
                }break;
            case 8:
                {
                    includes_Meta_Settings.reserve( _count );
                }break;
            case 5:
                {
                    includes_Meta_Texts.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 6:
                {
                    includes_Meta_Datas.emplace_back( Meta_Pak::Meta_Datas() );
                    Meta_Pak::Meta_Datas & metadata = includes_Meta_Datas.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 3:
                {
                    includes_Meta_Fonts.emplace_back( Meta_Pak::Meta_Fonts() );
                    Meta_Pak::Meta_Fonts & metadata = includes_Meta_Fonts.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 2:
                {
                    includes_Meta_Glyphs.emplace_back( Meta_Pak::Meta_Glyphs() );
                    Meta_Pak::Meta_Glyphs & metadata = includes_Meta_Glyphs.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 7:
                {
                    includes_Meta_Materials.emplace_back( Meta_Pak::Meta_Materials() );
                    Meta_Pak::Meta_Materials & metadata = includes_Meta_Materials.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 4:
                {
                    includes_Meta_Resources.emplace_back( Meta_Pak::Meta_Resources() );
                    Meta_Pak::Meta_Resources & metadata = includes_Meta_Resources.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 1:
                {
                    includes_Meta_Scripts.emplace_back( Meta_Pak::Meta_Scripts() );
                    Meta_Pak::Meta_Scripts & metadata = includes_Meta_Scripts.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 8:
                {
                    includes_Meta_Settings.emplace_back( Meta_Pak::Meta_Settings() );
                    Meta_Pak::Meta_Settings & metadata = includes_Meta_Settings.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 5:
                {
                    includes_Meta_Texts.emplace_back( Meta_Pak::Meta_Texts() );
                    Meta_Pak::Meta_Texts & metadata = includes_Meta_Texts.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Datas::Meta_Datas()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Datas::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Data.resize( includeCount );
        
                for( Meta_Pak::Meta_Datas::Meta_Data & metadata : includes_Meta_Data )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Datas::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Platform) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Datas::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Datas::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Data.emplace_back( Meta_Pak::Meta_Datas::Meta_Data() );
                    Meta_Pak::Meta_Datas::Meta_Data & metadata = includes_Meta_Data.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Datas::Meta_Data::Meta_Data()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Datas::Meta_Data::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Datas::Meta_Data::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Datas::Meta_Data::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Datas::Meta_Data::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Fonts::Meta_Fonts()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Fonts::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Font.resize( includeCount );
        
                for( Meta_Pak::Meta_Fonts::Meta_Font & metadata : includes_Meta_Font )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Fonts::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Platform) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Fonts::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Fonts::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Font.emplace_back( Meta_Pak::Meta_Fonts::Meta_Font() );
                    Meta_Pak::Meta_Fonts::Meta_Font & metadata = includes_Meta_Font.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Fonts::Meta_Font::Meta_Font()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Fonts::Meta_Font::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Fonts::Meta_Font::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Fonts::Meta_Font::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Fonts::Meta_Font::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Glyphs::Meta_Glyphs()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Glyphs::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Glyph.resize( includeCount );
        
                for( Meta_Pak::Meta_Glyphs::Meta_Glyph & metadata : includes_Meta_Glyph )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Glyphs::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Platform) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Glyphs::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Glyphs::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Glyph.emplace_back( Meta_Pak::Meta_Glyphs::Meta_Glyph() );
                    Meta_Pak::Meta_Glyphs::Meta_Glyph & metadata = includes_Meta_Glyph.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Glyphs::Meta_Glyph::Meta_Glyph()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Glyphs::Meta_Glyph::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Glyphs::Meta_Glyph::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Glyphs::Meta_Glyph::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Glyphs::Meta_Glyph::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Materials::Meta_Materials()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Materials::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Material.resize( includeCount );
        
                for( Meta_Pak::Meta_Materials::Meta_Material & metadata : includes_Meta_Material )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Materials::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Platform) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Materials::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Materials::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Material.emplace_back( Meta_Pak::Meta_Materials::Meta_Material() );
                    Meta_Pak::Meta_Materials::Meta_Material & metadata = includes_Meta_Material.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Materials::Meta_Material::Meta_Material()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Materials::Meta_Material::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Materials::Meta_Material::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Materials::Meta_Material::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Materials::Meta_Material::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Resources::Meta_Resources()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Resources::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Resource.resize( includeCount );
        
                for( Meta_Pak::Meta_Resources::Meta_Resource & metadata : includes_Meta_Resource )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Resources::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Demand) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Demand );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Ignored) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Ignored );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Platform) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Resources::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Resources::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Resource.emplace_back( Meta_Pak::Meta_Resources::Meta_Resource() );
                    Meta_Pak::Meta_Resources::Meta_Resource & metadata = includes_Meta_Resource.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Resources::Meta_Resource::Meta_Resource()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Resources::Meta_Resource::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Resources::Meta_Resource::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Resources::Meta_Resource::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Tags) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Tags );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Resources::Meta_Resource::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Resources::Meta_Resource::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Scripts::Meta_Scripts()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Scripts::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Script.resize( includeCount );
        
                for( Meta_Pak::Meta_Scripts::Meta_Script & metadata : includes_Meta_Script )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Scripts::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Platform) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Scripts::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Scripts::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Script.emplace_back( Meta_Pak::Meta_Scripts::Meta_Script() );
                    Meta_Pak::Meta_Scripts::Meta_Script & metadata = includes_Meta_Script.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Scripts::Meta_Script::Meta_Script()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Scripts::Meta_Script::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Scripts::Meta_Script::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Scripts::Meta_Script::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Finalizer) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Finalizer );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Initializer) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Initializer );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Module) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Module );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Scripts::Meta_Script::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Scripts::Meta_Script::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Settings::Meta_Settings()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Settings::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Setting.resize( includeCount );
        
                for( Meta_Pak::Meta_Settings::Meta_Setting & metadata : includes_Meta_Setting )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Settings::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Platform) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Settings::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Settings::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Setting.emplace_back( Meta_Pak::Meta_Settings::Meta_Setting() );
                    Meta_Pak::Meta_Settings::Meta_Setting & metadata = includes_Meta_Setting.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Settings::Meta_Setting::Meta_Setting()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Settings::Meta_Setting::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Settings::Meta_Setting::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Settings::Meta_Setting::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Settings::Meta_Setting::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Texts::Meta_Texts()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Texts::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Text.resize( includeCount );
        
                for( Meta_Pak::Meta_Texts::Meta_Text & metadata : includes_Meta_Text )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Texts::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_Platform) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Texts::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Texts::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Text.emplace_back( Meta_Pak::Meta_Texts::Meta_Text() );
                    Meta_Pak::Meta_Texts::Meta_Text & metadata = includes_Meta_Text.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Texts::Meta_Text::Meta_Text()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Texts::Meta_Text::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Texts::Meta_Text::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Texts::Meta_Text::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Pak::Meta_Texts::Meta_Text::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Texts::Meta_Texts()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Texts::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Text.resize( includeCount );
        
                for( Meta_Texts::Meta_Text & metadata : includes_Meta_Text )
                {
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Texts::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Texts::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Text.emplace_back( Meta_Texts::Meta_Text() );
                    Meta_Texts::Meta_Text & metadata = includes_Meta_Text.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Texts::Meta_Text::Meta_Text()
            : m_flagNoRequiredAttribute(0)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Texts::Meta_Text::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _userData );
            this->_parseData( _buff, _size, _read, _userData );
        
            Metabuf::readSize( _buff, _size, _read, m_flagNoRequiredAttribute );
        
            if( m_flagNoRequiredAttribute != 0 )
            {
                this->_parseArguments( _buff, _size, _read, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Texts::Meta_Text::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Key );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Texts::Meta_Text::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            if( (m_flagNoRequiredAttribute & EMETA_CharOffset) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_CharOffset );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Font) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Font );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_LineOffset) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_LineOffset );
            }
        
            if( (m_flagNoRequiredAttribute & EMETA_Value) != 0 )
            {
                Metabuf::read( _buff, _size, _read, _userData, this->m_Value );
            }
        
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Texts::Meta_Text::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _count );
        }
        //////////////////////////////////////////////////////////////////////////
        // cppcheck-suppress duplInheritedMember
        void Meta_Texts::Meta_Text::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            METABUF_UNUSED( _buff );
            METABUF_UNUSED( _size );
            METABUF_UNUSED( _read );
            METABUF_UNUSED( _id );
            METABUF_UNUSED( _userData );
        }
    } 
}
