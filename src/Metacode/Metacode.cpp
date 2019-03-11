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
        return 6;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t get_metacode_protocol_version()
    {
        return 134;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t get_metacode_protocol_crc32()
    {
        return 3256173687; 
    }
    //////////////////////////////////////////////////////////////////////////
    const char * getHeaderErrorMessage( HeaderError _error )
    {
        switch( _error )
        {
        case HEADER_SUCCESSFUL: return "Successful";
        case HEADER_INVALID_MAGIC: return "invalid magic header";
        case HEADER_INVALID_VERSION: return "invalid version";
        case HEADER_INVALID_PROTOCOL_VERSION: return "invalid protocol version";
        case HEADER_INVALID_PROTOCOL_CRC32: return "invalid protocol crc32";
        case HEADER_INVALID_METAVERSION: return "invalid meta version";
        default: return "invalid error";
        }
    }
    //////////////////////////////////////////////////////////////////////////
    HeaderError readHeader( const void * _buff, size_t _size, size_t & _read, uint32_t & _readVersion, uint32_t & _needVersion, uint32_t & _readProtocol, uint32_t & _needProtocol, uint32_t _metaVersion, uint32_t & _readMetaVersion )
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
            return HEADER_INVALID_MAGIC;
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
            return HEADER_INVALID_VERSION;
        }

        if( read_protocol_version != metacode_protocol_version )
        {
            return HEADER_INVALID_PROTOCOL_VERSION;
        }

        if( read_protocol_crc32 != metacode_protocol_crc32 )
        {
            return HEADER_INVALID_PROTOCOL_CRC32;
        }

        if( read_meta_version != _metaVersion )
        {
            return HEADER_INVALID_METAVERSION;
        }

        return HEADER_SUCCESSFUL;
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
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_DataBlock()
            : Metabuf::Metadata()
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
        uint32_t Meta_DataBlock::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::getId() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 5:
                {
                    includes_Meta_FragmentShader.reserve( _count );
                }break;
            case 2:
                {
                    includes_Meta_Include.reserve( _count );
                }break;
            case 7:
                {
                    includes_Meta_Material.reserve( _count );
                }break;
            case 6:
                {
                    includes_Meta_Program.reserve( _count );
                }break;
            case 3:
                {
                    includes_Meta_VertexAttribute.reserve( _count );
                }break;
            case 4:
                {
                    includes_Meta_VertexShader.reserve( _count );
                }break;
            case 8:
                {
                    includes_Meta_Resource.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 5:
                {
                    includes_Meta_FragmentShader.emplace_back( Meta_DataBlock::Meta_FragmentShader() );
                    Meta_DataBlock::Meta_FragmentShader & metadata = includes_Meta_FragmentShader.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 2:
                {
                    includes_Meta_Include.emplace_back( Meta_DataBlock::Meta_Include() );
                    Meta_DataBlock::Meta_Include & metadata = includes_Meta_Include.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 7:
                {
                    includes_Meta_Material.emplace_back( Meta_DataBlock::Meta_Material() );
                    Meta_DataBlock::Meta_Material & metadata = includes_Meta_Material.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 6:
                {
                    includes_Meta_Program.emplace_back( Meta_DataBlock::Meta_Program() );
                    Meta_DataBlock::Meta_Program & metadata = includes_Meta_Program.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 3:
                {
                    includes_Meta_VertexAttribute.emplace_back( Meta_DataBlock::Meta_VertexAttribute() );
                    Meta_DataBlock::Meta_VertexAttribute & metadata = includes_Meta_VertexAttribute.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 4:
                {
                    includes_Meta_VertexShader.emplace_back( Meta_DataBlock::Meta_VertexShader() );
                    Meta_DataBlock::Meta_VertexShader & metadata = includes_Meta_VertexShader.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::_parseGenerators( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 14:
                {
                    Meta_DataBlock::Meta_ResourceAstralax * metadata = new Meta_DataBlock::Meta_ResourceAstralax ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 31:
                {
                    Meta_DataBlock::Meta_ResourceCal3dAnimation * metadata = new Meta_DataBlock::Meta_ResourceCal3dAnimation ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 32:
                {
                    Meta_DataBlock::Meta_ResourceCal3dMesh * metadata = new Meta_DataBlock::Meta_ResourceCal3dMesh ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 30:
                {
                    Meta_DataBlock::Meta_ResourceCal3dSkeleton * metadata = new Meta_DataBlock::Meta_ResourceCal3dSkeleton ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 16:
                {
                    Meta_DataBlock::Meta_ResourceCursorICO * metadata = new Meta_DataBlock::Meta_ResourceCursorICO ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 15:
                {
                    Meta_DataBlock::Meta_ResourceCursorSystem * metadata = new Meta_DataBlock::Meta_ResourceCursorSystem ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 34:
                {
                    Meta_DataBlock::Meta_ResourceExternal * metadata = new Meta_DataBlock::Meta_ResourceExternal ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 17:
                {
                    Meta_DataBlock::Meta_ResourceFile * metadata = new Meta_DataBlock::Meta_ResourceFile ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 25:
                {
                    Meta_DataBlock::Meta_ResourceHIT * metadata = new Meta_DataBlock::Meta_ResourceHIT ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 9:
                {
                    Meta_DataBlock::Meta_ResourceImageData * metadata = new Meta_DataBlock::Meta_ResourceImageData ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 10:
                {
                    Meta_DataBlock::Meta_ResourceImageDefault * metadata = new Meta_DataBlock::Meta_ResourceImageDefault ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 24:
                {
                    Meta_DataBlock::Meta_ResourceImageSequence * metadata = new Meta_DataBlock::Meta_ResourceImageSequence ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 27:
                {
                    Meta_DataBlock::Meta_ResourceImageSolid * metadata = new Meta_DataBlock::Meta_ResourceImageSolid ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 12:
                {
                    Meta_DataBlock::Meta_ResourceImageSubstract * metadata = new Meta_DataBlock::Meta_ResourceImageSubstract ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 11:
                {
                    Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha * metadata = new Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 28:
                {
                    Meta_DataBlock::Meta_ResourceInternalObject * metadata = new Meta_DataBlock::Meta_ResourceInternalObject ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 26:
                {
                    Meta_DataBlock::Meta_ResourceModel3D * metadata = new Meta_DataBlock::Meta_ResourceModel3D ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 22:
                {
                    Meta_DataBlock::Meta_ResourceMovie * metadata = new Meta_DataBlock::Meta_ResourceMovie ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 21:
                {
                    Meta_DataBlock::Meta_ResourceMovie2 * metadata = new Meta_DataBlock::Meta_ResourceMovie2 ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 19:
                {
                    Meta_DataBlock::Meta_ResourceMusic * metadata = new Meta_DataBlock::Meta_ResourceMusic ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 13:
                {
                    Meta_DataBlock::Meta_ResourceParticle * metadata = new Meta_DataBlock::Meta_ResourceParticle ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 29:
                {
                    Meta_DataBlock::Meta_ResourceShape * metadata = new Meta_DataBlock::Meta_ResourceShape ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 18:
                {
                    Meta_DataBlock::Meta_ResourceSound * metadata = new Meta_DataBlock::Meta_ResourceSound ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 33:
                {
                    Meta_DataBlock::Meta_ResourceSpine * metadata = new Meta_DataBlock::Meta_ResourceSpine ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 23:
                {
                    Meta_DataBlock::Meta_ResourceVideo * metadata = new Meta_DataBlock::Meta_ResourceVideo ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 20:
                {
                    Meta_DataBlock::Meta_ResourceWindow * metadata = new Meta_DataBlock::Meta_ResourceWindow ();
                    metadata->parse( _buff, _size, _read, m_userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_FragmentShader::Meta_FragmentShader()
            : Metabuf::Metadata()
            , m_File_Compile_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_FragmentShader::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_FragmentShader::getId() const
        {
            return 5;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_FragmentShader::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Name );
            this->read( _buff, _size, _read, this->m_RenderPlatform );
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_FragmentShader::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 4:
                {
                    this->read( _buff, _size, _read, this->m_File_Compile );
        
                    this->m_File_Compile_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Include::Meta_Include()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_Include::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_Include::getId() const
        {
            return 2;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Include::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Material::Meta_Material()
            : Metabuf::Metadata()
            , m_Debug_successful(false)
            , m_RenderPlatform_successful(false)
            , m_AlphaBlend_Enable_successful(false)
            , m_BlendFactor_Dest_successful(false)
            , m_BlendFactor_Op_successful(false)
            , m_BlendFactor_Source_successful(false)
            , m_Program_Name_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_Material::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_Material::getId() const
        {
            return 7;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 3:
                {
                    this->read( _buff, _size, _read, this->m_Debug );
        
                    this->m_Debug_successful = true;
                }break;
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_RenderPlatform );
        
                    this->m_RenderPlatform_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_AlphaBlend_Enable );
        
                    this->m_AlphaBlend_Enable_successful = true;
                }break;
            case 5:
                {
                    this->read( _buff, _size, _read, this->m_BlendFactor_Dest );
        
                    this->m_BlendFactor_Dest_successful = true;
                }break;
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_BlendFactor_Op );
        
                    this->m_BlendFactor_Op_successful = true;
                }break;
            case 4:
                {
                    this->read( _buff, _size, _read, this->m_BlendFactor_Source );
        
                    this->m_BlendFactor_Source_successful = true;
                }break;
            case 9:
                {
                    this->read( _buff, _size, _read, this->m_Program_Name );
        
                    this->m_Program_Name_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 8:
                {
                    includes_Meta_TextureStages.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 8:
                {
                    includes_Meta_TextureStages.emplace_back( Meta_DataBlock::Meta_Material::Meta_TextureStages() );
                    Meta_DataBlock::Meta_Material::Meta_TextureStages & metadata = includes_Meta_TextureStages.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Material::Meta_TextureStages::Meta_TextureStages()
            : Metabuf::Metadata()
            , m_AddressMode_Border_successful(false)
            , m_AddressMode_U_successful(false)
            , m_AddressMode_V_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_Material::Meta_TextureStages::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_Material::Meta_TextureStages::getId() const
        {
            return 8;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Stage );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 4:
                {
                    this->read( _buff, _size, _read, this->m_AddressMode_Border );
        
                    this->m_AddressMode_Border_successful = true;
                }break;
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_AddressMode_U );
        
                    this->m_AddressMode_U_successful = true;
                }break;
            case 3:
                {
                    this->read( _buff, _size, _read, this->m_AddressMode_V );
        
                    this->m_AddressMode_V_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Program::Meta_Program()
            : Metabuf::Metadata()
            , m_RenderPlatform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_Program::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_Program::getId() const
        {
            return 6;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Program::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Name );
            this->read( _buff, _size, _read, this->m_FragmentShader_Name );
            this->read( _buff, _size, _read, this->m_Sampler_Count );
            this->read( _buff, _size, _read, this->m_VertexAttribute_Name );
            this->read( _buff, _size, _read, this->m_VertexShader_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Program::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_RenderPlatform );
        
                    this->m_RenderPlatform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_VertexAttribute::Meta_VertexAttribute()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_VertexAttribute::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_VertexAttribute::getId() const
        {
            return 3;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexAttribute::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Name );
            this->read( _buff, _size, _read, this->m_RenderPlatform );
            this->read( _buff, _size, _read, this->m_Element_Size );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexAttribute::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 4:
                {
                    includes_Meta_Attribute.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexAttribute::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 4:
                {
                    includes_Meta_Attribute.emplace_back( Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute() );
                    Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute & metadata = includes_Meta_Attribute.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::Meta_Attribute()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::getId() const
        {
            return 4;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Normalized );
            this->read( _buff, _size, _read, this->m_Offset );
            this->read( _buff, _size, _read, this->m_Size );
            this->read( _buff, _size, _read, this->m_Stride );
            this->read( _buff, _size, _read, this->m_Type );
            this->read( _buff, _size, _read, this->m_Uniform );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_VertexShader::Meta_VertexShader()
            : Metabuf::Metadata()
            , m_File_Compile_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_VertexShader::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_VertexShader::getId() const
        {
            return 4;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexShader::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Name );
            this->read( _buff, _size, _read, this->m_RenderPlatform );
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexShader::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 4:
                {
                    this->read( _buff, _size, _read, this->m_File_Compile );
        
                    this->m_File_Compile_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Resource::Meta_Resource()
            : Metabuf::Metadata()
            , m_Precompile_successful(false)
            , m_Unique_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_Resource::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_Resource::getId() const
        {
            return 8;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Resource::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Name );
            this->read( _buff, _size, _read, this->m_Type );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Resource::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 4:
                {
                    this->read( _buff, _size, _read, this->m_Precompile );
        
                    this->m_Precompile_successful = true;
                }break;
            case 3:
                {
                    this->read( _buff, _size, _read, this->m_Unique );
        
                    this->m_Unique_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceAstralax::Meta_ResourceAstralax()
            : Meta_Resource()
            , m_File_Converter_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceAstralax::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceAstralax::getId() const
        {
            return 14;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceAstralax::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_AtlasCount_Value );
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceAstralax::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceAstralax::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
            switch( _id )
            {
            case 8:
                {
                    includes_Meta_Atlas.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceAstralax::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 8:
                {
                    includes_Meta_Atlas.emplace_back( Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas() );
                    Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas & metadata = includes_Meta_Atlas.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::Meta_Atlas()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::getId() const
        {
            return 8;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Index );
            this->read( _buff, _size, _read, this->m_ResourceName );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceCal3dAnimation::Meta_ResourceCal3dAnimation()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceCal3dAnimation::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceCal3dAnimation::getId() const
        {
            return 31;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceCal3dMesh::Meta_ResourceCal3dMesh()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceCal3dMesh::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceCal3dMesh::getId() const
        {
            return 32;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dMesh::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceCal3dSkeleton::Meta_ResourceCal3dSkeleton()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceCal3dSkeleton::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceCal3dSkeleton::getId() const
        {
            return 30;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceCursorICO::Meta_ResourceCursorICO()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceCursorICO::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceCursorICO::getId() const
        {
            return 16;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCursorICO::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceCursorSystem::Meta_ResourceCursorSystem()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceCursorSystem::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceCursorSystem::getId() const
        {
            return 15;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCursorSystem::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceExternal::Meta_ResourceExternal()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceExternal::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceExternal::getId() const
        {
            return 34;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceFile::Meta_ResourceFile()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceFile::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceFile::getId() const
        {
            return 17;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceFile::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceHIT::Meta_ResourceHIT()
            : Meta_Resource()
            , m_File_Codec_successful(false)
            , m_File_Converter_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceHIT::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceHIT::getId() const
        {
            return 25;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceHIT::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceHIT::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageData::Meta_ResourceImageData()
            : Meta_Resource()
            , m_File_Alpha_successful(false)
            , m_File_Codec_successful(false)
            , m_File_NoExist_successful(false)
            , m_File_Offset_successful(false)
            , m_File_Premultiply_successful(false)
            , m_File_Size_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageData::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageData::getId() const
        {
            return 9;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageData::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_MaxSize );
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageData::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_File_Alpha );
        
                    this->m_File_Alpha_successful = true;
                }break;
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 12:
                {
                    this->read( _buff, _size, _read, this->m_File_NoExist );
        
                    this->m_File_NoExist_successful = true;
                }break;
            case 11:
                {
                    this->read( _buff, _size, _read, this->m_File_Offset );
        
                    this->m_File_Offset_successful = true;
                }break;
            case 8:
                {
                    this->read( _buff, _size, _read, this->m_File_Premultiply );
        
                    this->m_File_Premultiply_successful = true;
                }break;
            case 10:
                {
                    this->read( _buff, _size, _read, this->m_File_Size );
        
                    this->m_File_Size_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageDefault::Meta_ResourceImageDefault()
            : Meta_Resource()
            , m_File_Alpha_successful(false)
            , m_File_Codec_successful(false)
            , m_File_Converter_successful(false)
            , m_File_NoExist_successful(false)
            , m_File_Offset_successful(false)
            , m_File_Premultiply_successful(false)
            , m_File_Size_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageDefault::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageDefault::getId() const
        {
            return 10;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageDefault::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_MaxSize );
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageDefault::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 8:
                {
                    this->read( _buff, _size, _read, this->m_File_Alpha );
        
                    this->m_File_Alpha_successful = true;
                }break;
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            case 13:
                {
                    this->read( _buff, _size, _read, this->m_File_NoExist );
        
                    this->m_File_NoExist_successful = true;
                }break;
            case 12:
                {
                    this->read( _buff, _size, _read, this->m_File_Offset );
        
                    this->m_File_Offset_successful = true;
                }break;
            case 9:
                {
                    this->read( _buff, _size, _read, this->m_File_Premultiply );
        
                    this->m_File_Premultiply_successful = true;
                }break;
            case 11:
                {
                    this->read( _buff, _size, _read, this->m_File_Size );
        
                    this->m_File_Size_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageSequence::Meta_ResourceImageSequence()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageSequence::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageSequence::getId() const
        {
            return 24;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSequence::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
            switch( _id )
            {
            case 5:
                {
                    includes_Meta_Sequence.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSequence::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 5:
                {
                    includes_Meta_Sequence.emplace_back( Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence() );
                    Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence & metadata = includes_Meta_Sequence.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::Meta_Sequence()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::getId() const
        {
            return 5;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Delay );
            this->read( _buff, _size, _read, this->m_ResourceImageName );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageSolid::Meta_ResourceImageSolid()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageSolid::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageSolid::getId() const
        {
            return 27;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSolid::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_Color_Value );
            this->read( _buff, _size, _read, this->m_Size_Value );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageSubstract::Meta_ResourceImageSubstract()
            : Meta_Resource()
            , m_Image_Alpha_successful(false)
            , m_Image_Offset_successful(false)
            , m_Image_Premultiply_successful(false)
            , m_Image_Size_successful(false)
            , m_Image_UVRotate_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageSubstract::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageSubstract::getId() const
        {
            return 12;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSubstract::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_Image_MaxSize );
            this->read( _buff, _size, _read, this->m_Image_Name );
            this->read( _buff, _size, _read, this->m_Image_UV );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSubstract::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 8:
                {
                    this->read( _buff, _size, _read, this->m_Image_Alpha );
        
                    this->m_Image_Alpha_successful = true;
                }break;
            case 12:
                {
                    this->read( _buff, _size, _read, this->m_Image_Offset );
        
                    this->m_Image_Offset_successful = true;
                }break;
            case 9:
                {
                    this->read( _buff, _size, _read, this->m_Image_Premultiply );
        
                    this->m_Image_Premultiply_successful = true;
                }break;
            case 11:
                {
                    this->read( _buff, _size, _read, this->m_Image_Size );
        
                    this->m_Image_Size_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_Image_UVRotate );
        
                    this->m_Image_UVRotate_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::Meta_ResourceImageSubstractRGBAndAlpha()
            : Meta_Resource()
            , m_Image_Offset_successful(false)
            , m_Image_Premultiply_successful(false)
            , m_Image_Size_successful(false)
            , m_Image_UVAlphaRotate_successful(false)
            , m_Image_UVRGBRotate_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::getId() const
        {
            return 11;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_Image_MaxSize );
            this->read( _buff, _size, _read, this->m_Image_NameAlpha );
            this->read( _buff, _size, _read, this->m_Image_NameRGB );
            this->read( _buff, _size, _read, this->m_Image_UVAlpha );
            this->read( _buff, _size, _read, this->m_Image_UVRGB );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 14:
                {
                    this->read( _buff, _size, _read, this->m_Image_Offset );
        
                    this->m_Image_Offset_successful = true;
                }break;
            case 11:
                {
                    this->read( _buff, _size, _read, this->m_Image_Premultiply );
        
                    this->m_Image_Premultiply_successful = true;
                }break;
            case 13:
                {
                    this->read( _buff, _size, _read, this->m_Image_Size );
        
                    this->m_Image_Size_successful = true;
                }break;
            case 10:
                {
                    this->read( _buff, _size, _read, this->m_Image_UVAlphaRotate );
        
                    this->m_Image_UVAlphaRotate_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_Image_UVRGBRotate );
        
                    this->m_Image_UVRGBRotate_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceInternalObject::Meta_ResourceInternalObject()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceInternalObject::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceInternalObject::getId() const
        {
            return 28;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceInternalObject::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_Internal_Group );
            this->read( _buff, _size, _read, this->m_Internal_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceModel3D::Meta_ResourceModel3D()
            : Meta_Resource()
            , m_File_Converter_successful(false)
            , m_File_Dataflow_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceModel3D::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceModel3D::getId() const
        {
            return 26;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceModel3D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
            this->read( _buff, _size, _read, this->m_Image_Resource );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceModel3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_File_Dataflow );
        
                    this->m_File_Dataflow_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie::Meta_ResourceMovie()
            : Meta_Resource()
            , m_Anchor_Point_successful(false)
            , m_Bounds_Box_successful(false)
            , m_KeyFramesPackPath_Codec_successful(false)
            , m_KeyFramesPackPath_Converter_successful(false)
            , m_Loop_Segment_successful(false)
            , m_Offset_Point_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie::getId() const
        {
            return 22;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_Duration_Value );
            this->read( _buff, _size, _read, this->m_FrameDuration_Value );
            this->read( _buff, _size, _read, this->m_Height_Value );
            this->read( _buff, _size, _read, this->m_KeyFramesPackPath_Path );
            this->read( _buff, _size, _read, this->m_Width_Value );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 13:
                {
                    this->read( _buff, _size, _read, this->m_Anchor_Point );
        
                    this->m_Anchor_Point_successful = true;
                }break;
            case 15:
                {
                    this->read( _buff, _size, _read, this->m_Bounds_Box );
        
                    this->m_Bounds_Box_successful = true;
                }break;
            case 10:
                {
                    this->read( _buff, _size, _read, this->m_KeyFramesPackPath_Codec );
        
                    this->m_KeyFramesPackPath_Codec_successful = true;
                }break;
            case 11:
                {
                    this->read( _buff, _size, _read, this->m_KeyFramesPackPath_Converter );
        
                    this->m_KeyFramesPackPath_Converter_successful = true;
                }break;
            case 12:
                {
                    this->read( _buff, _size, _read, this->m_Loop_Segment );
        
                    this->m_Loop_Segment_successful = true;
                }break;
            case 14:
                {
                    this->read( _buff, _size, _read, this->m_Offset_Point );
        
                    this->m_Offset_Point_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
            switch( _id )
            {
            case 18:
                {
                    includes_Meta_MovieCamera3D.reserve( _count );
                }break;
            case 16:
                {
                    includes_Meta_MovieLayer2D.reserve( _count );
                }break;
            case 17:
                {
                    includes_Meta_MovieLayer3D.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 18:
                {
                    includes_Meta_MovieCamera3D.emplace_back( Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D() );
                    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D & metadata = includes_Meta_MovieCamera3D.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 16:
                {
                    includes_Meta_MovieLayer2D.emplace_back( Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D() );
                    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D & metadata = includes_Meta_MovieLayer2D.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 17:
                {
                    includes_Meta_MovieLayer3D.emplace_back( Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D() );
                    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D & metadata = includes_Meta_MovieLayer3D.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::Meta_MovieCamera3D()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::getId() const
        {
            return 18;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_CameraAspect );
            this->read( _buff, _size, _read, this->m_CameraFOV );
            this->read( _buff, _size, _read, this->m_CameraInterest );
            this->read( _buff, _size, _read, this->m_CameraPosition );
            this->read( _buff, _size, _read, this->m_Height );
            this->read( _buff, _size, _read, this->m_Width );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::Meta_MovieLayer2D()
            : Metabuf::Metadata()
            , m_AnchorPoint_successful(false)
            , m_BlendingMode_successful(false)
            , m_Loop_successful(false)
            , m_Params_successful(false)
            , m_Parent_successful(false)
            , m_PlayCount_successful(false)
            , m_Polygon_successful(false)
            , m_Position_successful(false)
            , m_Rotation_successful(false)
            , m_Scale_successful(false)
            , m_Shape_successful(false)
            , m_StartInterval_successful(false)
            , m_Stretch_successful(false)
            , m_Switch_successful(false)
            , m_TimeRemap_successful(false)
            , m_Viewport_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::getId() const
        {
            return 16;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_In );
            this->read( _buff, _size, _read, this->m_Index );
            this->read( _buff, _size, _read, this->m_Name );
            this->read( _buff, _size, _read, this->m_Out );
            this->read( _buff, _size, _read, this->m_Source );
            this->read( _buff, _size, _read, this->m_Type );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 17:
                {
                    this->read( _buff, _size, _read, this->m_AnchorPoint );
        
                    this->m_AnchorPoint_successful = true;
                }break;
            case 9:
                {
                    this->read( _buff, _size, _read, this->m_BlendingMode );
        
                    this->m_BlendingMode_successful = true;
                }break;
            case 16:
                {
                    this->read( _buff, _size, _read, this->m_Loop );
        
                    this->m_Loop_successful = true;
                }break;
            case 22:
                {
                    this->read( _buff, _size, _read, this->m_Params );
        
                    this->m_Params_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_Parent );
        
                    this->m_Parent_successful = true;
                }break;
            case 13:
                {
                    this->read( _buff, _size, _read, this->m_PlayCount );
        
                    this->m_PlayCount_successful = true;
                }break;
            case 12:
                {
                    this->read( _buff, _size, _read, this->m_Polygon );
        
                    this->m_Polygon_successful = true;
                }break;
            case 18:
                {
                    this->read( _buff, _size, _read, this->m_Position );
        
                    this->m_Position_successful = true;
                }break;
            case 20:
                {
                    this->read( _buff, _size, _read, this->m_Rotation );
        
                    this->m_Rotation_successful = true;
                }break;
            case 19:
                {
                    this->read( _buff, _size, _read, this->m_Scale );
        
                    this->m_Scale_successful = true;
                }break;
            case 11:
                {
                    this->read( _buff, _size, _read, this->m_Shape );
        
                    this->m_Shape_successful = true;
                }break;
            case 8:
                {
                    this->read( _buff, _size, _read, this->m_StartInterval );
        
                    this->m_StartInterval_successful = true;
                }break;
            case 14:
                {
                    this->read( _buff, _size, _read, this->m_Stretch );
        
                    this->m_Stretch_successful = true;
                }break;
            case 15:
                {
                    this->read( _buff, _size, _read, this->m_Switch );
        
                    this->m_Switch_successful = true;
                }break;
            case 10:
                {
                    this->read( _buff, _size, _read, this->m_TimeRemap );
        
                    this->m_TimeRemap_successful = true;
                }break;
            case 21:
                {
                    this->read( _buff, _size, _read, this->m_Viewport );
        
                    this->m_Viewport_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::Meta_MovieLayer3D()
            : Metabuf::Metadata()
            , m_AnchorPoint_successful(false)
            , m_BlendingMode_successful(false)
            , m_Loop_successful(false)
            , m_Params_successful(false)
            , m_Parent_successful(false)
            , m_PlayCount_successful(false)
            , m_Polygon_successful(false)
            , m_Position_successful(false)
            , m_Rotation_successful(false)
            , m_Scale_successful(false)
            , m_Shape_successful(false)
            , m_StartInterval_successful(false)
            , m_Stretch_successful(false)
            , m_Switch_successful(false)
            , m_TimeRemap_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::getId() const
        {
            return 17;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_In );
            this->read( _buff, _size, _read, this->m_Index );
            this->read( _buff, _size, _read, this->m_Name );
            this->read( _buff, _size, _read, this->m_Out );
            this->read( _buff, _size, _read, this->m_Source );
            this->read( _buff, _size, _read, this->m_Type );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 17:
                {
                    this->read( _buff, _size, _read, this->m_AnchorPoint );
        
                    this->m_AnchorPoint_successful = true;
                }break;
            case 9:
                {
                    this->read( _buff, _size, _read, this->m_BlendingMode );
        
                    this->m_BlendingMode_successful = true;
                }break;
            case 16:
                {
                    this->read( _buff, _size, _read, this->m_Loop );
        
                    this->m_Loop_successful = true;
                }break;
            case 21:
                {
                    this->read( _buff, _size, _read, this->m_Params );
        
                    this->m_Params_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_Parent );
        
                    this->m_Parent_successful = true;
                }break;
            case 13:
                {
                    this->read( _buff, _size, _read, this->m_PlayCount );
        
                    this->m_PlayCount_successful = true;
                }break;
            case 12:
                {
                    this->read( _buff, _size, _read, this->m_Polygon );
        
                    this->m_Polygon_successful = true;
                }break;
            case 18:
                {
                    this->read( _buff, _size, _read, this->m_Position );
        
                    this->m_Position_successful = true;
                }break;
            case 20:
                {
                    this->read( _buff, _size, _read, this->m_Rotation );
        
                    this->m_Rotation_successful = true;
                }break;
            case 19:
                {
                    this->read( _buff, _size, _read, this->m_Scale );
        
                    this->m_Scale_successful = true;
                }break;
            case 11:
                {
                    this->read( _buff, _size, _read, this->m_Shape );
        
                    this->m_Shape_successful = true;
                }break;
            case 8:
                {
                    this->read( _buff, _size, _read, this->m_StartInterval );
        
                    this->m_StartInterval_successful = true;
                }break;
            case 14:
                {
                    this->read( _buff, _size, _read, this->m_Stretch );
        
                    this->m_Stretch_successful = true;
                }break;
            case 15:
                {
                    this->read( _buff, _size, _read, this->m_Switch );
        
                    this->m_Switch_successful = true;
                }break;
            case 10:
                {
                    this->read( _buff, _size, _read, this->m_TimeRemap );
        
                    this->m_TimeRemap_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie2::Meta_ResourceMovie2()
            : Meta_Resource()
            , m_File_Dataflow_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie2::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie2::getId() const
        {
            return 21;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_File_Dataflow );
        
                    this->m_File_Dataflow_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
            switch( _id )
            {
            case 7:
                {
                    includes_Meta_Composition.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 7:
                {
                    includes_Meta_Composition.emplace_back( Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition() );
                    Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition & metadata = includes_Meta_Composition.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Composition()
            : Metabuf::Metadata()
            , m_Bounds_successful(false)
            , m_Master_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::getId() const
        {
            return 7;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Duration );
            this->read( _buff, _size, _read, this->m_FrameDuration );
            this->read( _buff, _size, _read, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 4:
                {
                    this->read( _buff, _size, _read, this->m_Bounds );
        
                    this->m_Bounds_successful = true;
                }break;
            case 5:
                {
                    this->read( _buff, _size, _read, this->m_Master );
        
                    this->m_Master_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 6:
                {
                    includes_Meta_Layer.reserve( _count );
                }break;
            case 7:
                {
                    includes_Meta_SubComposition.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 6:
                {
                    includes_Meta_Layer.emplace_back( Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer() );
                    Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer & metadata = includes_Meta_Layer.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 7:
                {
                    includes_Meta_SubComposition.emplace_back( Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition() );
                    Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition & metadata = includes_Meta_SubComposition.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::Meta_Layer()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::getId() const
        {
            return 6;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Color );
            this->read( _buff, _size, _read, this->m_Index );
            this->read( _buff, _size, _read, this->m_Matrix );
            this->read( _buff, _size, _read, this->m_Name );
            this->read( _buff, _size, _read, this->m_Type );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::Meta_SubComposition()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::getId() const
        {
            return 7;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Duration );
            this->read( _buff, _size, _read, this->m_FrameDuration );
            this->read( _buff, _size, _read, this->m_Index );
            this->read( _buff, _size, _read, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMusic::Meta_ResourceMusic()
            : Meta_Resource()
            , m_DefaultVolume_Value_successful(false)
            , m_File_Codec_successful(false)
            , m_File_Converter_successful(false)
            , m_File_External_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMusic::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceMusic::getId() const
        {
            return 19;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMusic::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMusic::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 9:
                {
                    this->read( _buff, _size, _read, this->m_DefaultVolume_Value );
        
                    this->m_DefaultVolume_Value_successful = true;
                }break;
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            case 8:
                {
                    this->read( _buff, _size, _read, this->m_File_External );
        
                    this->m_File_External_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceParticle::Meta_ResourceParticle()
            : Meta_Resource()
            , m_File_Converter_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceParticle::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceParticle::getId() const
        {
            return 13;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceParticle::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_AtlasCount_Value );
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceParticle::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceParticle::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
            switch( _id )
            {
            case 8:
                {
                    includes_Meta_Atlas.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceParticle::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 8:
                {
                    includes_Meta_Atlas.emplace_back( Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas() );
                    Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas & metadata = includes_Meta_Atlas.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::Meta_Atlas()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::getId() const
        {
            return 8;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Index );
            this->read( _buff, _size, _read, this->m_ResourceName );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceShape::Meta_ResourceShape()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceShape::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceShape::getId() const
        {
            return 29;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceShape::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_Polygon_Value );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceSound::Meta_ResourceSound()
            : Meta_Resource()
            , m_DefaultVolume_Value_successful(false)
            , m_File_Codec_successful(false)
            , m_File_Converter_successful(false)
            , m_IsStreamable_Value_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceSound::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceSound::getId() const
        {
            return 18;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSound::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSound::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 8:
                {
                    this->read( _buff, _size, _read, this->m_DefaultVolume_Value );
        
                    this->m_DefaultVolume_Value_successful = true;
                }break;
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            case 9:
                {
                    this->read( _buff, _size, _read, this->m_IsStreamable_Value );
        
                    this->m_IsStreamable_Value_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceSpine::Meta_ResourceSpine()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceSpine::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceSpine::getId() const
        {
            return 33;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSpine::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_Atlas_Path );
            this->read( _buff, _size, _read, this->m_Skeleton_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSpine::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        
            switch( _id )
            {
            case 7:
                {
                    includes_Meta_Image.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSpine::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 7:
                {
                    includes_Meta_Image.emplace_back( Meta_DataBlock::Meta_ResourceSpine::Meta_Image() );
                    Meta_DataBlock::Meta_ResourceSpine::Meta_Image & metadata = includes_Meta_Image.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceSpine::Meta_Image::Meta_Image()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceSpine::Meta_Image::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceSpine::Meta_Image::getId() const
        {
            return 7;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSpine::Meta_Image::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Name );
            this->read( _buff, _size, _read, this->m_Resource );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceVideo::Meta_ResourceVideo()
            : Meta_Resource()
            , m_File_Alpha_successful(false)
            , m_File_Codec_successful(false)
            , m_File_Converter_successful(false)
            , m_File_Duration_successful(false)
            , m_File_FrameRate_successful(false)
            , m_File_NoSeek_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceVideo::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceVideo::getId() const
        {
            return 23;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceVideo::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceVideo::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_File_Alpha );
        
                    this->m_File_Alpha_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 8:
                {
                    this->read( _buff, _size, _read, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            case 11:
                {
                    this->read( _buff, _size, _read, this->m_File_Duration );
        
                    this->m_File_Duration_successful = true;
                }break;
            case 10:
                {
                    this->read( _buff, _size, _read, this->m_File_FrameRate );
        
                    this->m_File_FrameRate_successful = true;
                }break;
            case 9:
                {
                    this->read( _buff, _size, _read, this->m_File_NoSeek );
        
                    this->m_File_NoSeek_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceWindow::Meta_ResourceWindow()
            : Meta_Resource()
            , m_WindowBackground_ResourceImageName_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceWindow::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_DataBlock::Meta_ResourceWindow::getId() const
        {
            return 20;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceWindow::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
        
            this->read( _buff, _size, _read, this->m_WindowBottom_Offset );
            this->read( _buff, _size, _read, this->m_WindowBottom_ResourceImageName );
            this->read( _buff, _size, _read, this->m_WindowLeft_Offset );
            this->read( _buff, _size, _read, this->m_WindowLeft_ResourceImageName );
            this->read( _buff, _size, _read, this->m_WindowLeftBottom_Offset );
            this->read( _buff, _size, _read, this->m_WindowLeftBottom_ResourceImageName );
            this->read( _buff, _size, _read, this->m_WindowLeftTop_Offset );
            this->read( _buff, _size, _read, this->m_WindowLeftTop_ResourceImageName );
            this->read( _buff, _size, _read, this->m_WindowRight_Offset );
            this->read( _buff, _size, _read, this->m_WindowRight_ResourceImageName );
            this->read( _buff, _size, _read, this->m_WindowRightBottom_Offset );
            this->read( _buff, _size, _read, this->m_WindowRightBottom_ResourceImageName );
            this->read( _buff, _size, _read, this->m_WindowRightTop_Offset );
            this->read( _buff, _size, _read, this->m_WindowRightTop_ResourceImageName );
            this->read( _buff, _size, _read, this->m_WindowTop_Offset );
            this->read( _buff, _size, _read, this->m_WindowTop_ResourceImageName );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceWindow::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
        
            switch( _id )
            {
            case 5:
                {
                    this->read( _buff, _size, _read, this->m_WindowBackground_ResourceImageName );
        
                    this->m_WindowBackground_ResourceImageName_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_KeyFramesPack()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::getId() const
        {
            return 4;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_MaxIndex );
            this->read( _buff, _size, _read, this->m_Version );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 5:
                {
                    includes_Meta_ImageShape.reserve( _count );
                }break;
            case 6:
                {
                    includes_Meta_KeyFrames2D.reserve( _count );
                }break;
            case 7:
                {
                    includes_Meta_KeyFrames3D.reserve( _count );
                }break;
            case 4:
                {
                    includes_Meta_Polygon.reserve( _count );
                }break;
            case 3:
                {
                    includes_Meta_TimeRemap.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 5:
                {
                    includes_Meta_ImageShape.emplace_back( Meta_KeyFramesPack::Meta_ImageShape() );
                    Meta_KeyFramesPack::Meta_ImageShape & metadata = includes_Meta_ImageShape.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 6:
                {
                    includes_Meta_KeyFrames2D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames2D() );
                    Meta_KeyFramesPack::Meta_KeyFrames2D & metadata = includes_Meta_KeyFrames2D.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 7:
                {
                    includes_Meta_KeyFrames3D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames3D() );
                    Meta_KeyFramesPack::Meta_KeyFrames3D & metadata = includes_Meta_KeyFrames3D.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 4:
                {
                    includes_Meta_Polygon.emplace_back( Meta_KeyFramesPack::Meta_Polygon() );
                    Meta_KeyFramesPack::Meta_Polygon & metadata = includes_Meta_Polygon.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 3:
                {
                    includes_Meta_TimeRemap.emplace_back( Meta_KeyFramesPack::Meta_TimeRemap() );
                    Meta_KeyFramesPack::Meta_TimeRemap & metadata = includes_Meta_TimeRemap.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_ImageShape::Meta_ImageShape()
            : Metabuf::Metadata()
            , m_Count_successful(false)
            , m_ImageOffset_successful(false)
            , m_ImageSize_successful(false)
            , m_Immutable_successful(false)
            , m_Subtract_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_ImageShape::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_ImageShape::getId() const
        {
            return 5;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_ImageShape::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_ImageMaxSize );
            this->read( _buff, _size, _read, this->m_LayerIndex );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_ImageShape::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_Count );
        
                    this->m_Count_successful = true;
                }break;
            case 4:
                {
                    this->read( _buff, _size, _read, this->m_ImageOffset );
        
                    this->m_ImageOffset_successful = true;
                }break;
            case 3:
                {
                    this->read( _buff, _size, _read, this->m_ImageSize );
        
                    this->m_ImageSize_successful = true;
                }break;
            case 5:
                {
                    this->read( _buff, _size, _read, this->m_Immutable );
        
                    this->m_Immutable_successful = true;
                }break;
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_Subtract );
        
                    this->m_Subtract_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_ImageShape::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 8:
                {
                    includes_Meta_Shape.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_ImageShape::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 8:
                {
                    includes_Meta_Shape.emplace_back( Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape() );
                    Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape & metadata = includes_Meta_Shape.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::Meta_Shape()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::getId() const
        {
            return 8;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Polygon );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrames2D()
            : Metabuf::Metadata()
            , m_Count_successful(false)
            , m_Immutable_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_KeyFrames2D::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_KeyFrames2D::getId() const
        {
            return 6;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_LayerIndex );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 3:
                {
                    this->read( _buff, _size, _read, this->m_Count );
        
                    this->m_Count_successful = true;
                }break;
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_Immutable );
        
                    this->m_Immutable_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 4:
                {
                    includes_Meta_KeyFrame2D.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 4:
                {
                    includes_Meta_KeyFrame2D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D() );
                    Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & metadata = includes_Meta_KeyFrame2D.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::Meta_KeyFrame2D()
            : Metabuf::Metadata()
            , m_AnchorPoint_successful(false)
            , m_Count_successful(false)
            , m_Opacity_successful(false)
            , m_Position_successful(false)
            , m_Rotation_successful(false)
            , m_Scale_successful(false)
            , m_Volume_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::getId() const
        {
            return 4;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 1:
                {
                    this->read( _buff, _size, _read, this->m_AnchorPoint );
        
                    this->m_AnchorPoint_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_Count );
        
                    this->m_Count_successful = true;
                }break;
            case 5:
                {
                    this->read( _buff, _size, _read, this->m_Opacity );
        
                    this->m_Opacity_successful = true;
                }break;
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_Position );
        
                    this->m_Position_successful = true;
                }break;
            case 4:
                {
                    this->read( _buff, _size, _read, this->m_Rotation );
        
                    this->m_Rotation_successful = true;
                }break;
            case 3:
                {
                    this->read( _buff, _size, _read, this->m_Scale );
        
                    this->m_Scale_successful = true;
                }break;
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_Volume );
        
                    this->m_Volume_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrames3D()
            : Metabuf::Metadata()
            , m_Count_successful(false)
            , m_Immutable_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_KeyFrames3D::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_KeyFrames3D::getId() const
        {
            return 7;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_LayerIndex );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 3:
                {
                    this->read( _buff, _size, _read, this->m_Count );
        
                    this->m_Count_successful = true;
                }break;
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_Immutable );
        
                    this->m_Immutable_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 4:
                {
                    includes_Meta_KeyFrame3D.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 4:
                {
                    includes_Meta_KeyFrame3D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D() );
                    Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D & metadata = includes_Meta_KeyFrame3D.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::Meta_KeyFrame3D()
            : Metabuf::Metadata()
            , m_AnchorPoint_successful(false)
            , m_Count_successful(false)
            , m_Opacity_successful(false)
            , m_Orientation_successful(false)
            , m_Position_successful(false)
            , m_Rotation_successful(false)
            , m_Scale_successful(false)
            , m_Volume_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::getId() const
        {
            return 4;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 1:
                {
                    this->read( _buff, _size, _read, this->m_AnchorPoint );
        
                    this->m_AnchorPoint_successful = true;
                }break;
            case 8:
                {
                    this->read( _buff, _size, _read, this->m_Count );
        
                    this->m_Count_successful = true;
                }break;
            case 6:
                {
                    this->read( _buff, _size, _read, this->m_Opacity );
        
                    this->m_Opacity_successful = true;
                }break;
            case 5:
                {
                    this->read( _buff, _size, _read, this->m_Orientation );
        
                    this->m_Orientation_successful = true;
                }break;
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_Position );
        
                    this->m_Position_successful = true;
                }break;
            case 4:
                {
                    this->read( _buff, _size, _read, this->m_Rotation );
        
                    this->m_Rotation_successful = true;
                }break;
            case 3:
                {
                    this->read( _buff, _size, _read, this->m_Scale );
        
                    this->m_Scale_successful = true;
                }break;
            case 7:
                {
                    this->read( _buff, _size, _read, this->m_Volume );
        
                    this->m_Volume_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_Polygon::Meta_Polygon()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_Polygon::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_Polygon::getId() const
        {
            return 4;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_Polygon::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_LayerIndex );
            this->read( _buff, _size, _read, this->m_Value );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_TimeRemap::Meta_TimeRemap()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_TimeRemap::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_KeyFramesPack::Meta_TimeRemap::getId() const
        {
            return 3;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_TimeRemap::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_LayerIndex );
            this->read( _buff, _size, _read, this->m_Time );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Pak()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::getId() const
        {
            return 2;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 5:
                {
                    includes_Meta_Datas.reserve( _count );
                }break;
            case 2:
                {
                    includes_Meta_Fonts.reserve( _count );
                }break;
            case 6:
                {
                    includes_Meta_Materials.reserve( _count );
                }break;
            case 3:
                {
                    includes_Meta_Resources.reserve( _count );
                }break;
            case 1:
                {
                    includes_Meta_Scripts.reserve( _count );
                }break;
            case 4:
                {
                    includes_Meta_Texts.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 5:
                {
                    includes_Meta_Datas.emplace_back( Meta_Pak::Meta_Datas() );
                    Meta_Pak::Meta_Datas & metadata = includes_Meta_Datas.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 2:
                {
                    includes_Meta_Fonts.emplace_back( Meta_Pak::Meta_Fonts() );
                    Meta_Pak::Meta_Fonts & metadata = includes_Meta_Fonts.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 6:
                {
                    includes_Meta_Materials.emplace_back( Meta_Pak::Meta_Materials() );
                    Meta_Pak::Meta_Materials & metadata = includes_Meta_Materials.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 3:
                {
                    includes_Meta_Resources.emplace_back( Meta_Pak::Meta_Resources() );
                    Meta_Pak::Meta_Resources & metadata = includes_Meta_Resources.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 1:
                {
                    includes_Meta_Scripts.emplace_back( Meta_Pak::Meta_Scripts() );
                    Meta_Pak::Meta_Scripts & metadata = includes_Meta_Scripts.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            case 4:
                {
                    includes_Meta_Texts.emplace_back( Meta_Pak::Meta_Texts() );
                    Meta_Pak::Meta_Texts & metadata = includes_Meta_Texts.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Datas::Meta_Datas()
            : Metabuf::Metadata()
            , m_Platform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Datas::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Datas::getId() const
        {
            return 5;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Datas::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 1:
                {
                    this->read( _buff, _size, _read, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Datas::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 2:
                {
                    includes_Meta_Data.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Datas::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 2:
                {
                    includes_Meta_Data.emplace_back( Meta_Pak::Meta_Datas::Meta_Data() );
                    Meta_Pak::Meta_Datas::Meta_Data & metadata = includes_Meta_Data.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Datas::Meta_Data::Meta_Data()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Datas::Meta_Data::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Datas::Meta_Data::getId() const
        {
            return 2;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Datas::Meta_Data::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Name );
            this->read( _buff, _size, _read, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Fonts::Meta_Fonts()
            : Metabuf::Metadata()
            , m_Platform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Fonts::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Fonts::getId() const
        {
            return 2;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Fonts::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Fonts::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Materials::Meta_Materials()
            : Metabuf::Metadata()
            , m_Platform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Materials::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Materials::getId() const
        {
            return 6;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Materials::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 1:
                {
                    this->read( _buff, _size, _read, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Materials::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 2:
                {
                    includes_Meta_Material.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Materials::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 2:
                {
                    includes_Meta_Material.emplace_back( Meta_Pak::Meta_Materials::Meta_Material() );
                    Meta_Pak::Meta_Materials::Meta_Material & metadata = includes_Meta_Material.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Materials::Meta_Material::Meta_Material()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Materials::Meta_Material::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Materials::Meta_Material::getId() const
        {
            return 2;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Materials::Meta_Material::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Resources::Meta_Resources()
            : Metabuf::Metadata()
            , m_Demand_successful(false)
            , m_Ignored_successful(false)
            , m_Platform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Resources::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Resources::getId() const
        {
            return 3;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Resources::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_Demand );
        
                    this->m_Demand_successful = true;
                }break;
            case 1:
                {
                    this->read( _buff, _size, _read, this->m_Ignored );
        
                    this->m_Ignored_successful = true;
                }break;
            case 3:
                {
                    this->read( _buff, _size, _read, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Resources::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 4:
                {
                    includes_Meta_Resource.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Resources::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 4:
                {
                    includes_Meta_Resource.emplace_back( Meta_Pak::Meta_Resources::Meta_Resource() );
                    Meta_Pak::Meta_Resources::Meta_Resource & metadata = includes_Meta_Resource.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Resources::Meta_Resource::Meta_Resource()
            : Metabuf::Metadata()
            , m_Tags_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Resources::Meta_Resource::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Resources::Meta_Resource::getId() const
        {
            return 4;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Resources::Meta_Resource::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Resources::Meta_Resource::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_Tags );
        
                    this->m_Tags_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Scripts::Meta_Scripts()
            : Metabuf::Metadata()
            , m_Finalizer_successful(false)
            , m_Initializer_successful(false)
            , m_Module_successful(false)
            , m_Platform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Scripts::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Scripts::getId() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Scripts::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Scripts::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 4:
                {
                    this->read( _buff, _size, _read, this->m_Finalizer );
        
                    this->m_Finalizer_successful = true;
                }break;
            case 3:
                {
                    this->read( _buff, _size, _read, this->m_Initializer );
        
                    this->m_Initializer_successful = true;
                }break;
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_Module );
        
                    this->m_Module_successful = true;
                }break;
            case 5:
                {
                    this->read( _buff, _size, _read, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Texts::Meta_Texts()
            : Metabuf::Metadata()
            , m_Platform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Texts::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Texts::getId() const
        {
            return 4;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Texts::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 1:
                {
                    this->read( _buff, _size, _read, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Texts::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 2:
                {
                    includes_Meta_Text.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Texts::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 2:
                {
                    includes_Meta_Text.emplace_back( Meta_Pak::Meta_Texts::Meta_Text() );
                    Meta_Pak::Meta_Texts::Meta_Text & metadata = includes_Meta_Text.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Texts::Meta_Text::Meta_Text()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Texts::Meta_Text::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Pak::Meta_Texts::Meta_Text::getId() const
        {
            return 2;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Texts::Meta_Text::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Texts::Meta_Texts()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Texts::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Texts::getId() const
        {
            return 3;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Texts::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Text.reserve( _count );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Texts::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 1:
                {
                    includes_Meta_Text.emplace_back( Meta_Texts::Meta_Text() );
                    Meta_Texts::Meta_Text & metadata = includes_Meta_Text.back();
        
                    metadata.parse( _buff, _size, _read, m_userData );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Texts::Meta_Text::Meta_Text()
            : Metabuf::Metadata()
            , m_CharOffset_successful(false)
            , m_Font_successful(false)
            , m_LineOffset_successful(false)
            , m_Value_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Texts::Meta_Text::getVersion() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t Meta_Texts::Meta_Text::getId() const
        {
            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Texts::Meta_Text::_parseData( const uint8_t * _buff, size_t _size, size_t & _read )
        {
            this->read( _buff, _size, _read, this->m_Key );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Texts::Meta_Text::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id )
        {
            switch( _id )
            {
            case 4:
                {
                    this->read( _buff, _size, _read, this->m_CharOffset );
        
                    this->m_CharOffset_successful = true;
                }break;
            case 3:
                {
                    this->read( _buff, _size, _read, this->m_Font );
        
                    this->m_Font_successful = true;
                }break;
            case 5:
                {
                    this->read( _buff, _size, _read, this->m_LineOffset );
        
                    this->m_LineOffset_successful = true;
                }break;
            case 2:
                {
                    this->read( _buff, _size, _read, this->m_Value );
        
                    this->m_Value_successful = true;
                }break;
            default:
                break;
            }
        }
    } 
}
