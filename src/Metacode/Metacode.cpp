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
        return 7;
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
            : Metabuf::Metaparse()
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
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
                case 5:
                    {
                        includes_Meta_FragmentShader.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_FragmentShader.emplace_back( Meta_DataBlock::Meta_FragmentShader() );
                            Meta_DataBlock::Meta_FragmentShader & metadata = includes_Meta_FragmentShader.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 2:
                    {
                        includes_Meta_Include.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_Include.emplace_back( Meta_DataBlock::Meta_Include() );
                            Meta_DataBlock::Meta_Include & metadata = includes_Meta_Include.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 7:
                    {
                        includes_Meta_Material.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_Material.emplace_back( Meta_DataBlock::Meta_Material() );
                            Meta_DataBlock::Meta_Material & metadata = includes_Meta_Material.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 6:
                    {
                        includes_Meta_Program.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_Program.emplace_back( Meta_DataBlock::Meta_Program() );
                            Meta_DataBlock::Meta_Program & metadata = includes_Meta_Program.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 3:
                    {
                        includes_Meta_VertexAttribute.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_VertexAttribute.emplace_back( Meta_DataBlock::Meta_VertexAttribute() );
                            Meta_DataBlock::Meta_VertexAttribute & metadata = includes_Meta_VertexAttribute.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 4:
                    {
                        includes_Meta_VertexShader.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_VertexShader.emplace_back( Meta_DataBlock::Meta_VertexShader() );
                            Meta_DataBlock::Meta_VertexShader & metadata = includes_Meta_VertexShader.back();
        
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
        void Meta_DataBlock::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_DataBlock::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 5:
                {
                    includes_Meta_FragmentShader.emplace_back( Meta_DataBlock::Meta_FragmentShader() );
                    Meta_DataBlock::Meta_FragmentShader & metadata = includes_Meta_FragmentShader.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 2:
                {
                    includes_Meta_Include.emplace_back( Meta_DataBlock::Meta_Include() );
                    Meta_DataBlock::Meta_Include & metadata = includes_Meta_Include.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 7:
                {
                    includes_Meta_Material.emplace_back( Meta_DataBlock::Meta_Material() );
                    Meta_DataBlock::Meta_Material & metadata = includes_Meta_Material.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 6:
                {
                    includes_Meta_Program.emplace_back( Meta_DataBlock::Meta_Program() );
                    Meta_DataBlock::Meta_Program & metadata = includes_Meta_Program.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 3:
                {
                    includes_Meta_VertexAttribute.emplace_back( Meta_DataBlock::Meta_VertexAttribute() );
                    Meta_DataBlock::Meta_VertexAttribute & metadata = includes_Meta_VertexAttribute.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 4:
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
            case 31:
                {
                    Meta_DataBlock::Meta_ResourceCal3dAnimation * metadata = new Meta_DataBlock::Meta_ResourceCal3dAnimation ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 32:
                {
                    Meta_DataBlock::Meta_ResourceCal3dMesh * metadata = new Meta_DataBlock::Meta_ResourceCal3dMesh ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 30:
                {
                    Meta_DataBlock::Meta_ResourceCal3dSkeleton * metadata = new Meta_DataBlock::Meta_ResourceCal3dSkeleton ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 16:
                {
                    Meta_DataBlock::Meta_ResourceCursorICO * metadata = new Meta_DataBlock::Meta_ResourceCursorICO ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 15:
                {
                    Meta_DataBlock::Meta_ResourceCursorSystem * metadata = new Meta_DataBlock::Meta_ResourceCursorSystem ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 34:
                {
                    Meta_DataBlock::Meta_ResourceExternal * metadata = new Meta_DataBlock::Meta_ResourceExternal ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 17:
                {
                    Meta_DataBlock::Meta_ResourceFile * metadata = new Meta_DataBlock::Meta_ResourceFile ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 25:
                {
                    Meta_DataBlock::Meta_ResourceHIT * metadata = new Meta_DataBlock::Meta_ResourceHIT ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 9:
                {
                    Meta_DataBlock::Meta_ResourceImageData * metadata = new Meta_DataBlock::Meta_ResourceImageData ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 10:
                {
                    Meta_DataBlock::Meta_ResourceImageDefault * metadata = new Meta_DataBlock::Meta_ResourceImageDefault ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 24:
                {
                    Meta_DataBlock::Meta_ResourceImageSequence * metadata = new Meta_DataBlock::Meta_ResourceImageSequence ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 27:
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
            case 28:
                {
                    Meta_DataBlock::Meta_ResourceInternalObject * metadata = new Meta_DataBlock::Meta_ResourceInternalObject ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 26:
                {
                    Meta_DataBlock::Meta_ResourceModel3D * metadata = new Meta_DataBlock::Meta_ResourceModel3D ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 22:
                {
                    Meta_DataBlock::Meta_ResourceMovie * metadata = new Meta_DataBlock::Meta_ResourceMovie ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 21:
                {
                    Meta_DataBlock::Meta_ResourceMovie2 * metadata = new Meta_DataBlock::Meta_ResourceMovie2 ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 19:
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
            case 29:
                {
                    Meta_DataBlock::Meta_ResourceShape * metadata = new Meta_DataBlock::Meta_ResourceShape ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 18:
                {
                    Meta_DataBlock::Meta_ResourceSound * metadata = new Meta_DataBlock::Meta_ResourceSound ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 33:
                {
                    Meta_DataBlock::Meta_ResourceSpine * metadata = new Meta_DataBlock::Meta_ResourceSpine ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 23:
                {
                    Meta_DataBlock::Meta_ResourceVideo * metadata = new Meta_DataBlock::Meta_ResourceVideo ();
                    metadata->parse( _buff, _size, _read, _userData );
        
                    includes_Meta_Resource.push_back(metadata);
                }break;
            case 20:
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
            : Metabuf::Metadata()
            , m_File_Compile_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_FragmentShader::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_FragmentShader::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_RenderPlatform );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_FragmentShader::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 4:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Compile );
        
                    this->m_File_Compile_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_FragmentShader::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_FragmentShader::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Include::Meta_Include()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_Include::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Include::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Include::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Include::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
        bool Meta_DataBlock::Meta_Material::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_TextureStages.reserve( includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    includes_Meta_TextureStages.emplace_back( Meta_DataBlock::Meta_Material::Meta_TextureStages() );
                    Meta_DataBlock::Meta_Material::Meta_TextureStages & metadata = includes_Meta_TextureStages.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 3:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Debug );
        
                    this->m_Debug_successful = true;
                }break;
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_RenderPlatform );
        
                    this->m_RenderPlatform_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_AlphaBlend_Enable );
        
                    this->m_AlphaBlend_Enable_successful = true;
                }break;
            case 5:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_BlendFactor_Dest );
        
                    this->m_BlendFactor_Dest_successful = true;
                }break;
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_BlendFactor_Op );
        
                    this->m_BlendFactor_Op_successful = true;
                }break;
            case 4:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_BlendFactor_Source );
        
                    this->m_BlendFactor_Source_successful = true;
                }break;
            case 9:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Program_Name );
        
                    this->m_Program_Name_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_DataBlock::Meta_Material::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 8:
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
            : Metabuf::Metadata()
            , m_AddressMode_Border_successful(false)
            , m_AddressMode_U_successful(false)
            , m_AddressMode_V_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_Material::Meta_TextureStages::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Stage );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 4:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_AddressMode_Border );
        
                    this->m_AddressMode_Border_successful = true;
                }break;
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_AddressMode_U );
        
                    this->m_AddressMode_U_successful = true;
                }break;
            case 3:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_AddressMode_V );
        
                    this->m_AddressMode_V_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::Meta_TextureStages::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_Program::Meta_Program()
            : Metabuf::Metadata()
            , m_RenderPlatform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_Program::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Program::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_FragmentShader_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Sampler_Count );
            Metabuf::read( _buff, _size, _read, _userData, this->m_VertexAttribute_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_VertexShader_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Program::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_RenderPlatform );
        
                    this->m_RenderPlatform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Program::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Program::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_VertexAttribute::Meta_VertexAttribute()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_VertexAttribute::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Attribute.reserve( includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    includes_Meta_Attribute.emplace_back( Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute() );
                    Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute & metadata = includes_Meta_Attribute.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexAttribute::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_RenderPlatform );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Element_Size );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexAttribute::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_DataBlock::Meta_VertexAttribute::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 4:
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
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Normalized );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Offset );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Size );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Stride );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Type );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Uniform );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_VertexShader::Meta_VertexShader()
            : Metabuf::Metadata()
            , m_File_Compile_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_VertexShader::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexShader::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_RenderPlatform );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexShader::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 4:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Compile );
        
                    this->m_File_Compile_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexShader::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_VertexShader::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
        Meta_DataBlock::Meta_Resource::~Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_Resource::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Resource::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Type );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Resource::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 4:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Precompile );
        
                    this->m_Precompile_successful = true;
                }break;
            case 3:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Unique );
        
                    this->m_Unique_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Resource::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_Resource::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceAstralax::Meta_ResourceAstralax()
            : Meta_Resource()
            , m_File_Converter_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceAstralax::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
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
        void Meta_DataBlock::Meta_ResourceAstralax::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_AtlasCount_Value );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceAstralax::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceAstralax::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_DataBlock::Meta_ResourceAstralax::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 8:
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
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Index );
            Metabuf::read( _buff, _size, _read, _userData, this->m_ResourceName );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dAnimation::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dMesh::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dMesh::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dMesh::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCursorICO::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCursorICO::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCursorICO::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCursorSystem::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCursorSystem::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceCursorSystem::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceExternal::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceExternal::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceFile::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceFile::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceFile::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
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
        bool Meta_DataBlock::Meta_ResourceHIT::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceHIT::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceHIT::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceHIT::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceHIT::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
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
        bool Meta_DataBlock::Meta_ResourceImageData::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageData::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_MaxSize );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageData::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Alpha );
        
                    this->m_File_Alpha_successful = true;
                }break;
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 12:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
        
                    this->m_File_NoExist_successful = true;
                }break;
            case 11:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Offset );
        
                    this->m_File_Offset_successful = true;
                }break;
            case 8:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Premultiply );
        
                    this->m_File_Premultiply_successful = true;
                }break;
            case 10:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Size );
        
                    this->m_File_Size_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageData::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageData::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
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
        bool Meta_DataBlock::Meta_ResourceImageDefault::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageDefault::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_MaxSize );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageDefault::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 8:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Alpha );
        
                    this->m_File_Alpha_successful = true;
                }break;
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            case 13:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoExist );
        
                    this->m_File_NoExist_successful = true;
                }break;
            case 12:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Offset );
        
                    this->m_File_Offset_successful = true;
                }break;
            case 9:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Premultiply );
        
                    this->m_File_Premultiply_successful = true;
                }break;
            case 11:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Size );
        
                    this->m_File_Size_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageDefault::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageDefault::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
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
        void Meta_DataBlock::Meta_ResourceImageSequence::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_DataBlock::Meta_ResourceImageSequence::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 5:
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
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Delay );
            Metabuf::read( _buff, _size, _read, _userData, this->m_ResourceImageName );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSolid::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Color_Value );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Size_Value );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSolid::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSolid::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
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
        bool Meta_DataBlock::Meta_ResourceImageSubstract::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSubstract::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_MaxSize );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_UV );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSubstract::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 8:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Alpha );
        
                    this->m_Image_Alpha_successful = true;
                }break;
            case 12:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Offset );
        
                    this->m_Image_Offset_successful = true;
                }break;
            case 9:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Premultiply );
        
                    this->m_Image_Premultiply_successful = true;
                }break;
            case 11:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Size );
        
                    this->m_Image_Size_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Image_UVRotate );
        
                    this->m_Image_UVRotate_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSubstract::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSubstract::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
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
        bool Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
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
        void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 14:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Offset );
        
                    this->m_Image_Offset_successful = true;
                }break;
            case 11:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Premultiply );
        
                    this->m_Image_Premultiply_successful = true;
                }break;
            case 13:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Size );
        
                    this->m_Image_Size_successful = true;
                }break;
            case 10:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Image_UVAlphaRotate );
        
                    this->m_Image_UVAlphaRotate_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Image_UVRGBRotate );
        
                    this->m_Image_UVRGBRotate_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceInternalObject::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Internal_Group );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Internal_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceInternalObject::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceInternalObject::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
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
        bool Meta_DataBlock::Meta_ResourceModel3D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceModel3D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Image_Resource );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceModel3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Dataflow );
        
                    this->m_File_Dataflow_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceModel3D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceModel3D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
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
        bool Meta_DataBlock::Meta_ResourceMovie::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
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
        void Meta_DataBlock::Meta_ResourceMovie::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 13:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Anchor_Point );
        
                    this->m_Anchor_Point_successful = true;
                }break;
            case 15:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Bounds_Box );
        
                    this->m_Bounds_Box_successful = true;
                }break;
            case 10:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_KeyFramesPackPath_Codec );
        
                    this->m_KeyFramesPackPath_Codec_successful = true;
                }break;
            case 11:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_KeyFramesPackPath_Converter );
        
                    this->m_KeyFramesPackPath_Converter_successful = true;
                }break;
            case 12:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Loop_Segment );
        
                    this->m_Loop_Segment_successful = true;
                }break;
            case 14:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Offset_Point );
        
                    this->m_Offset_Point_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_DataBlock::Meta_ResourceMovie::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 18:
                {
                    includes_Meta_MovieCamera3D.emplace_back( Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D() );
                    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D & metadata = includes_Meta_MovieCamera3D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 16:
                {
                    includes_Meta_MovieLayer2D.emplace_back( Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D() );
                    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D & metadata = includes_Meta_MovieLayer2D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 17:
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
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
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
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
        bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
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
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 17:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_AnchorPoint );
        
                    this->m_AnchorPoint_successful = true;
                }break;
            case 9:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_BlendingMode );
        
                    this->m_BlendingMode_successful = true;
                }break;
            case 16:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Loop );
        
                    this->m_Loop_successful = true;
                }break;
            case 22:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Params );
        
                    this->m_Params_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Parent );
        
                    this->m_Parent_successful = true;
                }break;
            case 13:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_PlayCount );
        
                    this->m_PlayCount_successful = true;
                }break;
            case 12:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Polygon );
        
                    this->m_Polygon_successful = true;
                }break;
            case 18:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Position );
        
                    this->m_Position_successful = true;
                }break;
            case 20:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Rotation );
        
                    this->m_Rotation_successful = true;
                }break;
            case 19:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Scale );
        
                    this->m_Scale_successful = true;
                }break;
            case 11:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Shape );
        
                    this->m_Shape_successful = true;
                }break;
            case 8:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_StartInterval );
        
                    this->m_StartInterval_successful = true;
                }break;
            case 14:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Stretch );
        
                    this->m_Stretch_successful = true;
                }break;
            case 15:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Switch );
        
                    this->m_Switch_successful = true;
                }break;
            case 10:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_TimeRemap );
        
                    this->m_TimeRemap_successful = true;
                }break;
            case 21:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Viewport );
        
                    this->m_Viewport_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
        bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
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
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 17:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_AnchorPoint );
        
                    this->m_AnchorPoint_successful = true;
                }break;
            case 9:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_BlendingMode );
        
                    this->m_BlendingMode_successful = true;
                }break;
            case 16:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Loop );
        
                    this->m_Loop_successful = true;
                }break;
            case 21:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Params );
        
                    this->m_Params_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Parent );
        
                    this->m_Parent_successful = true;
                }break;
            case 13:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_PlayCount );
        
                    this->m_PlayCount_successful = true;
                }break;
            case 12:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Polygon );
        
                    this->m_Polygon_successful = true;
                }break;
            case 18:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Position );
        
                    this->m_Position_successful = true;
                }break;
            case 20:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Rotation );
        
                    this->m_Rotation_successful = true;
                }break;
            case 19:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Scale );
        
                    this->m_Scale_successful = true;
                }break;
            case 11:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Shape );
        
                    this->m_Shape_successful = true;
                }break;
            case 8:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_StartInterval );
        
                    this->m_StartInterval_successful = true;
                }break;
            case 14:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Stretch );
        
                    this->m_Stretch_successful = true;
                }break;
            case 15:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Switch );
        
                    this->m_Switch_successful = true;
                }break;
            case 10:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_TimeRemap );
        
                    this->m_TimeRemap_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie2::Meta_ResourceMovie2()
            : Meta_Resource()
            , m_File_Dataflow_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie2::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
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
        void Meta_DataBlock::Meta_ResourceMovie2::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Dataflow );
        
                    this->m_File_Dataflow_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_DataBlock::Meta_ResourceMovie2::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 7:
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
            : Metabuf::Metadata()
            , m_Bounds_successful(false)
            , m_Master_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
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
                case 6:
                    {
                        includes_Meta_Layer.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_Layer.emplace_back( Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer() );
                            Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer & metadata = includes_Meta_Layer.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 7:
                    {
                        includes_Meta_SubComposition.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_SubComposition.emplace_back( Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition() );
                            Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition & metadata = includes_Meta_SubComposition.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Duration );
            Metabuf::read( _buff, _size, _read, _userData, this->m_FrameDuration );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 4:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Bounds );
        
                    this->m_Bounds_successful = true;
                }break;
            case 5:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Master );
        
                    this->m_Master_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 6:
                {
                    includes_Meta_Layer.emplace_back( Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer() );
                    Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer & metadata = includes_Meta_Layer.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 7:
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
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Color );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Index );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Matrix );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Type );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::Meta_SubComposition()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Duration );
            Metabuf::read( _buff, _size, _read, _userData, this->m_FrameDuration );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Index );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
        bool Meta_DataBlock::Meta_ResourceMusic::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMusic::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMusic::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 9:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_DefaultVolume_Value );
        
                    this->m_DefaultVolume_Value_successful = true;
                }break;
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            case 8:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_External );
        
                    this->m_File_External_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMusic::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceMusic::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceParticle::Meta_ResourceParticle()
            : Meta_Resource()
            , m_File_Converter_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceParticle::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
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
        void Meta_DataBlock::Meta_ResourceParticle::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_AtlasCount_Value );
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceParticle::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceParticle::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_DataBlock::Meta_ResourceParticle::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 8:
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
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Index );
            Metabuf::read( _buff, _size, _read, _userData, this->m_ResourceName );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceShape::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Polygon_Value );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceShape::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceShape::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
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
        bool Meta_DataBlock::Meta_ResourceSound::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSound::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSound::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 8:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_DefaultVolume_Value );
        
                    this->m_DefaultVolume_Value_successful = true;
                }break;
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            case 9:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_IsStreamable_Value );
        
                    this->m_IsStreamable_Value_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSound::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSound::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceSpine::Meta_ResourceSpine()
            : Meta_Resource()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceSpine::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
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
        void Meta_DataBlock::Meta_ResourceSpine::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_Atlas_Path );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Skeleton_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSpine::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_DataBlock::Meta_ResourceSpine::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 7:
                {
                    includes_Meta_Image.emplace_back( Meta_DataBlock::Meta_ResourceSpine::Meta_Image() );
                    Meta_DataBlock::Meta_ResourceSpine::Meta_Image & metadata = includes_Meta_Image.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
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
        bool Meta_DataBlock::Meta_ResourceSpine::Meta_Image::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSpine::Meta_Image::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Resource );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSpine::Meta_Image::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceSpine::Meta_Image::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
        bool Meta_DataBlock::Meta_ResourceVideo::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceVideo::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read, _userData );
        
            Metabuf::read( _buff, _size, _read, _userData, this->m_File_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceVideo::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Alpha );
        
                    this->m_File_Alpha_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Codec );
        
                    this->m_File_Codec_successful = true;
                }break;
            case 8:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Converter );
        
                    this->m_File_Converter_successful = true;
                }break;
            case 11:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_Duration );
        
                    this->m_File_Duration_successful = true;
                }break;
            case 10:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_FrameRate );
        
                    this->m_File_FrameRate_successful = true;
                }break;
            case 9:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_File_NoSeek );
        
                    this->m_File_NoSeek_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceVideo::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceVideo::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_DataBlock::Meta_ResourceWindow::Meta_ResourceWindow()
            : Meta_Resource()
            , m_WindowBackground_ResourceImageName_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_DataBlock::Meta_ResourceWindow::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
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
        void Meta_DataBlock::Meta_ResourceWindow::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id, _userData );
        
            switch( _id )
            {
            case 5:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_WindowBackground_ResourceImageName );
        
                    this->m_WindowBackground_ResourceImageName_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceWindow::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
            Meta_DataBlock::Meta_Resource::_preparationIncludes( _id, _count );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_DataBlock::Meta_ResourceWindow::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _id, _userData );
        
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_KeyFramesPack()
            : Metabuf::Metaparse()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
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
                case 5:
                    {
                        includes_Meta_ImageShape.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_ImageShape.emplace_back( Meta_KeyFramesPack::Meta_ImageShape() );
                            Meta_KeyFramesPack::Meta_ImageShape & metadata = includes_Meta_ImageShape.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 6:
                    {
                        includes_Meta_KeyFrames2D.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_KeyFrames2D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames2D() );
                            Meta_KeyFramesPack::Meta_KeyFrames2D & metadata = includes_Meta_KeyFrames2D.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 7:
                    {
                        includes_Meta_KeyFrames3D.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_KeyFrames3D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames3D() );
                            Meta_KeyFramesPack::Meta_KeyFrames3D & metadata = includes_Meta_KeyFrames3D.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 4:
                    {
                        includes_Meta_Polygon.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_Polygon.emplace_back( Meta_KeyFramesPack::Meta_Polygon() );
                            Meta_KeyFramesPack::Meta_Polygon & metadata = includes_Meta_Polygon.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 3:
                    {
                        includes_Meta_TimeRemap.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_TimeRemap.emplace_back( Meta_KeyFramesPack::Meta_TimeRemap() );
                            Meta_KeyFramesPack::Meta_TimeRemap & metadata = includes_Meta_TimeRemap.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_MaxIndex );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Version );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_KeyFramesPack::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 5:
                {
                    includes_Meta_ImageShape.emplace_back( Meta_KeyFramesPack::Meta_ImageShape() );
                    Meta_KeyFramesPack::Meta_ImageShape & metadata = includes_Meta_ImageShape.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 6:
                {
                    includes_Meta_KeyFrames2D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames2D() );
                    Meta_KeyFramesPack::Meta_KeyFrames2D & metadata = includes_Meta_KeyFrames2D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 7:
                {
                    includes_Meta_KeyFrames3D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames3D() );
                    Meta_KeyFramesPack::Meta_KeyFrames3D & metadata = includes_Meta_KeyFrames3D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 4:
                {
                    includes_Meta_Polygon.emplace_back( Meta_KeyFramesPack::Meta_Polygon() );
                    Meta_KeyFramesPack::Meta_Polygon & metadata = includes_Meta_Polygon.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 3:
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
            : Metabuf::Metadata()
            , m_Count_successful(false)
            , m_ImageOffset_successful(false)
            , m_ImageSize_successful(false)
            , m_Immutable_successful(false)
            , m_Subtract_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_ImageShape::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Shape.reserve( includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    includes_Meta_Shape.emplace_back( Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape() );
                    Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape & metadata = includes_Meta_Shape.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_ImageShape::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_ImageMaxSize );
            Metabuf::read( _buff, _size, _read, _userData, this->m_LayerIndex );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_ImageShape::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Count );
        
                    this->m_Count_successful = true;
                }break;
            case 4:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_ImageOffset );
        
                    this->m_ImageOffset_successful = true;
                }break;
            case 3:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_ImageSize );
        
                    this->m_ImageSize_successful = true;
                }break;
            case 5:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Immutable );
        
                    this->m_Immutable_successful = true;
                }break;
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Subtract );
        
                    this->m_Subtract_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_ImageShape::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_KeyFramesPack::Meta_ImageShape::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 8:
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
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Polygon );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
        bool Meta_KeyFramesPack::Meta_KeyFrames2D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_KeyFrame2D.reserve( includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    includes_Meta_KeyFrame2D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D() );
                    Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & metadata = includes_Meta_KeyFrame2D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_LayerIndex );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 3:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Count );
        
                    this->m_Count_successful = true;
                }break;
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Immutable );
        
                    this->m_Immutable_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 4:
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
        bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 1:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_AnchorPoint );
        
                    this->m_AnchorPoint_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Count );
        
                    this->m_Count_successful = true;
                }break;
            case 5:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Opacity );
        
                    this->m_Opacity_successful = true;
                }break;
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Position );
        
                    this->m_Position_successful = true;
                }break;
            case 4:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Rotation );
        
                    this->m_Rotation_successful = true;
                }break;
            case 3:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Scale );
        
                    this->m_Scale_successful = true;
                }break;
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Volume );
        
                    this->m_Volume_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
        bool Meta_KeyFramesPack::Meta_KeyFrames3D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_KeyFrame3D.reserve( includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    includes_Meta_KeyFrame3D.emplace_back( Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D() );
                    Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D & metadata = includes_Meta_KeyFrame3D.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_LayerIndex );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 3:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Count );
        
                    this->m_Count_successful = true;
                }break;
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Immutable );
        
                    this->m_Immutable_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 4:
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
        bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 1:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_AnchorPoint );
        
                    this->m_AnchorPoint_successful = true;
                }break;
            case 8:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Count );
        
                    this->m_Count_successful = true;
                }break;
            case 6:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Opacity );
        
                    this->m_Opacity_successful = true;
                }break;
            case 5:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Orientation );
        
                    this->m_Orientation_successful = true;
                }break;
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Position );
        
                    this->m_Position_successful = true;
                }break;
            case 4:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Rotation );
        
                    this->m_Rotation_successful = true;
                }break;
            case 3:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Scale );
        
                    this->m_Scale_successful = true;
                }break;
            case 7:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Volume );
        
                    this->m_Volume_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_Polygon::Meta_Polygon()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_Polygon::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_Polygon::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_LayerIndex );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Value );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_Polygon::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_Polygon::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_KeyFramesPack::Meta_TimeRemap::Meta_TimeRemap()
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_KeyFramesPack::Meta_TimeRemap::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_TimeRemap::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_LayerIndex );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Time );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_TimeRemap::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_KeyFramesPack::Meta_TimeRemap::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Pak()
            : Metabuf::Metaparse()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
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
                case 5:
                    {
                        includes_Meta_Datas.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_Datas.emplace_back( Meta_Pak::Meta_Datas() );
                            Meta_Pak::Meta_Datas & metadata = includes_Meta_Datas.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 2:
                    {
                        includes_Meta_Fonts.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_Fonts.emplace_back( Meta_Pak::Meta_Fonts() );
                            Meta_Pak::Meta_Fonts & metadata = includes_Meta_Fonts.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 6:
                    {
                        includes_Meta_Materials.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_Materials.emplace_back( Meta_Pak::Meta_Materials() );
                            Meta_Pak::Meta_Materials & metadata = includes_Meta_Materials.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 3:
                    {
                        includes_Meta_Resources.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_Resources.emplace_back( Meta_Pak::Meta_Resources() );
                            Meta_Pak::Meta_Resources & metadata = includes_Meta_Resources.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 1:
                    {
                        includes_Meta_Scripts.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_Scripts.emplace_back( Meta_Pak::Meta_Scripts() );
                            Meta_Pak::Meta_Scripts & metadata = includes_Meta_Scripts.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                case 4:
                    {
                        includes_Meta_Texts.reserve( includeCount );
        
                        for( uint32_t j = 0; j != includeCount; ++j )
                        {
                            includes_Meta_Texts.emplace_back( Meta_Pak::Meta_Texts() );
                            Meta_Pak::Meta_Texts & metadata = includes_Meta_Texts.back();
        
                            metadata.parse( _buff, _size, _read, _userData );
                        }
                    }break;
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_Pak::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 5:
                {
                    includes_Meta_Datas.emplace_back( Meta_Pak::Meta_Datas() );
                    Meta_Pak::Meta_Datas & metadata = includes_Meta_Datas.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 2:
                {
                    includes_Meta_Fonts.emplace_back( Meta_Pak::Meta_Fonts() );
                    Meta_Pak::Meta_Fonts & metadata = includes_Meta_Fonts.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 6:
                {
                    includes_Meta_Materials.emplace_back( Meta_Pak::Meta_Materials() );
                    Meta_Pak::Meta_Materials & metadata = includes_Meta_Materials.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }break;
            case 3:
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
            case 4:
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
            : Metabuf::Metadata()
            , m_Platform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Datas::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Data.reserve( includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    includes_Meta_Data.emplace_back( Meta_Pak::Meta_Datas::Meta_Data() );
                    Meta_Pak::Meta_Datas::Meta_Data & metadata = includes_Meta_Data.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Datas::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 1:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Datas::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_Pak::Meta_Datas::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 2:
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
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Datas::Meta_Data::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Datas::Meta_Data::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Name );
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Datas::Meta_Data::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Datas::Meta_Data::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Fonts::Meta_Fonts()
            : Metabuf::Metadata()
            , m_Platform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Fonts::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Fonts::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Fonts::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Fonts::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Fonts::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Materials::Meta_Materials()
            : Metabuf::Metadata()
            , m_Platform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Materials::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Material.reserve( includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    includes_Meta_Material.emplace_back( Meta_Pak::Meta_Materials::Meta_Material() );
                    Meta_Pak::Meta_Materials::Meta_Material & metadata = includes_Meta_Material.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Materials::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 1:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Materials::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_Pak::Meta_Materials::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 2:
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
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Materials::Meta_Material::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Materials::Meta_Material::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Materials::Meta_Material::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Materials::Meta_Material::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
        bool Meta_Pak::Meta_Resources::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Resource.reserve( includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    includes_Meta_Resource.emplace_back( Meta_Pak::Meta_Resources::Meta_Resource() );
                    Meta_Pak::Meta_Resources::Meta_Resource & metadata = includes_Meta_Resource.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Resources::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Demand );
        
                    this->m_Demand_successful = true;
                }break;
            case 1:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Ignored );
        
                    this->m_Ignored_successful = true;
                }break;
            case 3:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Resources::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_Pak::Meta_Resources::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 4:
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
            : Metabuf::Metadata()
            , m_Tags_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Resources::Meta_Resource::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Resources::Meta_Resource::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Resources::Meta_Resource::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Tags );
        
                    this->m_Tags_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Resources::Meta_Resource::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Resources::Meta_Resource::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
        bool Meta_Pak::Meta_Scripts::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Scripts::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Scripts::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 4:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Finalizer );
        
                    this->m_Finalizer_successful = true;
                }break;
            case 3:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Initializer );
        
                    this->m_Initializer_successful = true;
                }break;
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Module );
        
                    this->m_Module_successful = true;
                }break;
            case 5:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Scripts::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Scripts::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Pak::Meta_Texts::Meta_Texts()
            : Metabuf::Metadata()
            , m_Platform_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Texts::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Text.reserve( includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    includes_Meta_Text.emplace_back( Meta_Pak::Meta_Texts::Meta_Text() );
                    Meta_Pak::Meta_Texts::Meta_Text & metadata = includes_Meta_Text.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Texts::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 1:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Platform );
        
                    this->m_Platform_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Texts::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_Pak::Meta_Texts::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
            switch( _id )
            {
            case 2:
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
            : Metabuf::Metadata()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Pak::Meta_Texts::Meta_Text::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Texts::Meta_Text::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Path );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Texts::Meta_Text::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Pak::Meta_Texts::Meta_Text::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
        //////////////////////////////////////////////////////////////////////////
        //cppcheck-suppress uninitMemberVar
        Meta_Texts::Meta_Texts()
            : Metabuf::Metaparse()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Texts::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            uint32_t includeCount;
            Metabuf::readSize( _buff, _size, _read, includeCount );
        
            if( includeCount != 0 )
            {
                includes_Meta_Text.reserve( includeCount );
        
                for( uint32_t j = 0; j != includeCount; ++j )
                {
                    includes_Meta_Text.emplace_back( Meta_Texts::Meta_Text() );
                    Meta_Texts::Meta_Text & metadata = includes_Meta_Text.back();
        
                    metadata.parse( _buff, _size, _read, _userData );
                }
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Texts::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
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
        void Meta_Texts::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
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
            : Metabuf::Metadata()
            , m_CharOffset_successful(false)
            , m_Font_successful(false)
            , m_LineOffset_successful(false)
            , m_Value_successful(false)
        {
        }
        //////////////////////////////////////////////////////////////////////////
        bool Meta_Texts::Meta_Text::parse( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_userData;
            this->_parseData( _buff, _size, _read, _userData );
        
            uint32_t attributeCount;
            Metabuf::readSize( _buff, _size, _read, attributeCount );
        
            for( uint32_t i = 0; i != attributeCount; ++i )
            {
                uint32_t id;
                Metabuf::readSize( _buff, _size, _read, id );
        
                this->_parseArguments( _buff, _size, _read, id, _userData );
            }
        
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Texts::Meta_Text::_parseData( const uint8_t * _buff, size_t _size, size_t & _read, void * _userData )
        {
            Metabuf::read( _buff, _size, _read, _userData, this->m_Key );
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Texts::Meta_Text::_parseArguments( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            switch( _id )
            {
            case 4:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_CharOffset );
        
                    this->m_CharOffset_successful = true;
                }break;
            case 3:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Font );
        
                    this->m_Font_successful = true;
                }break;
            case 5:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_LineOffset );
        
                    this->m_LineOffset_successful = true;
                }break;
            case 2:
                {
                    Metabuf::read( _buff, _size, _read, _userData, this->m_Value );
        
                    this->m_Value_successful = true;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Texts::Meta_Text::_preparationIncludes( uint32_t _id, uint32_t _count )
        {
            (void)_id;
            (void)_count;
        }
        //////////////////////////////////////////////////////////////////////////
        void Meta_Texts::Meta_Text::_parseIncludes( const uint8_t * _buff, size_t _size, size_t & _read, uint32_t _id, void * _userData )
        {
            (void)_buff;
            (void)_size;
            (void)_read;
            (void)_id;
            (void)_userData;
        }
    } 
}
