#   include "Metacode.h"

namespace Metacode
{
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t metacode_magic = 3133062829u;
    static const uint32_t metacode_version = 3;
    static const uint32_t metacode_protocol = 111;
    //////////////////////////////////////////////////////////////////////////
    uint32_t get_metacode_magic()
    {
        return metacode_magic;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t get_metacode_version()
    {
        return metacode_version;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t get_metacode_protocol()
    {
        return metacode_protocol;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool readHeader2( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _readVersion, uint32_t & _needVersion, uint32_t & _readProtocol, uint32_t & _needProtocol )
    {
        stdex::memory_reader ar(_buff, _size, _read);

        uint32_t head;
        ar.readPOD( head );

        if( head != metacode_magic )
        {
            return false;
        }

        uint32_t version;
        ar.readPOD( version );

        uint32_t protocol;
        ar.readPOD( protocol );

        _readVersion = version;
        _needVersion = metacode_version;
        _readProtocol = protocol;
        _needProtocol = metacode_protocol;

        if( version != metacode_version )
        {
            return false;
        }

        if( protocol != metacode_protocol )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool readHeader( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _readVersion, uint32_t & _needVersion, uint32_t & _readProtocol, uint32_t & _needProtocol )
    {
        bool successful = readHeader2( _buff, _size, _read, _readVersion, _needVersion, _readProtocol, _needProtocol );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool readStrings2( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _stringCount )
    {
        stdex::memory_reader ar(_buff, _size, _read);

        uint32_t count;
        ar.readPOD( count );

        _stringCount = count;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool readStrings( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _stringCount )
    {
        bool successful = readStrings2( _buff, _size, _read, _stringCount );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    static const char * readString2( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _stringSize, int32_t & _stringHash )
    {
        stdex::memory_reader ar(_buff, _size, _read);

        uint32_t size;
        ar.readSize( size );

        int32_t hash;
        ar.readPOD( hash );

        const char * value = ar.current_buff<char>();
        ar.skip( size );

        _stringSize = size;
        _stringHash = hash;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * readString( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _stringSize, int32_t & _stringHash )
    {
        const char * value = readString2( _buff, _size, _read, _stringSize, _stringHash );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_DataBlock()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::~Meta_DataBlock()
    {
        for( TVectorMeta_Resource::const_iterator
            it = includes_Meta_Resource.begin(),
            it_end = includes_Meta_Resource.end();
        it != it_end;
        ++it )
        {
            delete *it;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::getId() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Name );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 4:
            {
                includes_Meta_FragmentShader.reserve( _count );
            }break;
        case 2:
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
        case 3:
            {
                includes_Meta_VertexShader.reserve( _count );
            }break;
        case 7:
            {
                includes_Meta_Resource.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 4:
            {
                Meta_DataBlock::Meta_FragmentShader & metadata = includes_Meta_FragmentShader.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 2:
            {
                Meta_DataBlock::Meta_Include & metadata = includes_Meta_Include.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 6:
            {
                Meta_DataBlock::Meta_Material & metadata = includes_Meta_Material.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 5:
            {
                Meta_DataBlock::Meta_Program & metadata = includes_Meta_Program.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 3:
            {
                Meta_DataBlock::Meta_VertexShader & metadata = includes_Meta_VertexShader.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        switch( _generators )
        {
        case 22:
            {
                Meta_DataBlock::Meta_ResourceAnimation * metadata = new Meta_DataBlock::Meta_ResourceAnimation ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 29:
            {
                Meta_DataBlock::Meta_ResourceCal3dAnimation * metadata = new Meta_DataBlock::Meta_ResourceCal3dAnimation ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 30:
            {
                Meta_DataBlock::Meta_ResourceCal3dMesh * metadata = new Meta_DataBlock::Meta_ResourceCal3dMesh ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 28:
            {
                Meta_DataBlock::Meta_ResourceCal3dSkeleton * metadata = new Meta_DataBlock::Meta_ResourceCal3dSkeleton ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 14:
            {
                Meta_DataBlock::Meta_ResourceCursorICO * metadata = new Meta_DataBlock::Meta_ResourceCursorICO ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 13:
            {
                Meta_DataBlock::Meta_ResourceCursorSystem * metadata = new Meta_DataBlock::Meta_ResourceCursorSystem ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 32:
            {
                Meta_DataBlock::Meta_ResourceExternal * metadata = new Meta_DataBlock::Meta_ResourceExternal ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 15:
            {
                Meta_DataBlock::Meta_ResourceFile * metadata = new Meta_DataBlock::Meta_ResourceFile ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 23:
            {
                Meta_DataBlock::Meta_ResourceHIT * metadata = new Meta_DataBlock::Meta_ResourceHIT ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 8:
            {
                Meta_DataBlock::Meta_ResourceImageData * metadata = new Meta_DataBlock::Meta_ResourceImageData ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 9:
            {
                Meta_DataBlock::Meta_ResourceImageDefault * metadata = new Meta_DataBlock::Meta_ResourceImageDefault ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 25:
            {
                Meta_DataBlock::Meta_ResourceImageSolid * metadata = new Meta_DataBlock::Meta_ResourceImageSolid ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 11:
            {
                Meta_DataBlock::Meta_ResourceImageSubstract * metadata = new Meta_DataBlock::Meta_ResourceImageSubstract ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 10:
            {
                Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha * metadata = new Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 26:
            {
                Meta_DataBlock::Meta_ResourceInternalObject * metadata = new Meta_DataBlock::Meta_ResourceInternalObject ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 24:
            {
                Meta_DataBlock::Meta_ResourceModel3D * metadata = new Meta_DataBlock::Meta_ResourceModel3D ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 20:
            {
                Meta_DataBlock::Meta_ResourceMovie * metadata = new Meta_DataBlock::Meta_ResourceMovie ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 19:
            {
                Meta_DataBlock::Meta_ResourceMovie2 * metadata = new Meta_DataBlock::Meta_ResourceMovie2 ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 17:
            {
                Meta_DataBlock::Meta_ResourceMusic * metadata = new Meta_DataBlock::Meta_ResourceMusic ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 12:
            {
                Meta_DataBlock::Meta_ResourceParticle * metadata = new Meta_DataBlock::Meta_ResourceParticle ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 27:
            {
                Meta_DataBlock::Meta_ResourceShape * metadata = new Meta_DataBlock::Meta_ResourceShape ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 16:
            {
                Meta_DataBlock::Meta_ResourceSound * metadata = new Meta_DataBlock::Meta_ResourceSound ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 31:
            {
                Meta_DataBlock::Meta_ResourceSpine * metadata = new Meta_DataBlock::Meta_ResourceSpine ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 21:
            {
                Meta_DataBlock::Meta_ResourceVideo * metadata = new Meta_DataBlock::Meta_ResourceVideo ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        case 18:
            {
                Meta_DataBlock::Meta_ResourceWindow * metadata = new Meta_DataBlock::Meta_ResourceWindow ();
                metadata->parse( _buff, _size, _read, m_userData );
    
                includes_Meta_Resource.push_back(metadata);
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_FragmentShader::Meta_FragmentShader()
        : Metabuf::Metadata()
        , File_Compile_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_FragmentShader::getId() const
    {
        return 4;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_FragmentShader::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Name );
        this->read( _buff, _size, _read, this->Platform );
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_FragmentShader::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->File_Compile );
    
                this->File_Compile_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_FragmentShader::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_FragmentShader::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_FragmentShader::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_Include::Meta_Include()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_Include::getId() const
    {
        return 2;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Include::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Include::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Include::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Include::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Include::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_Material::Meta_Material()
        : Metabuf::Metadata()
        , Debug_successful(false)
        , AlphaBlend_Enable_successful(false)
        , BlendFactor_Dest_successful(false)
        , BlendFactor_Op_successful(false)
        , BlendFactor_Source_successful(false)
        , Program_Name_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_Material::getId() const
    {
        return 6;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Material::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Name );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Material::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 2:
            {
                this->read( _buff, _size, _read, this->Debug );
    
                this->Debug_successful = true;
    
            }break;
        case 6:
            {
                this->read( _buff, _size, _read, this->AlphaBlend_Enable );
    
                this->AlphaBlend_Enable_successful = true;
    
            }break;
        case 4:
            {
                this->read( _buff, _size, _read, this->BlendFactor_Dest );
    
                this->BlendFactor_Dest_successful = true;
    
            }break;
        case 5:
            {
                this->read( _buff, _size, _read, this->BlendFactor_Op );
    
                this->BlendFactor_Op_successful = true;
    
            }break;
        case 3:
            {
                this->read( _buff, _size, _read, this->BlendFactor_Source );
    
                this->BlendFactor_Source_successful = true;
    
            }break;
        case 8:
            {
                this->read( _buff, _size, _read, this->Program_Name );
    
                this->Program_Name_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Material::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 7:
            {
                includes_Meta_TextureStages.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Material::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 7:
            {
                Meta_DataBlock::Meta_Material::Meta_TextureStages & metadata = includes_Meta_TextureStages.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Material::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_Material::Meta_TextureStages::Meta_TextureStages()
        : Metabuf::Metadata()
        , AddressMode_U_successful(false)
        , AddressMode_V_successful(false)
        , Alpha_Arg1_successful(false)
        , Alpha_Arg2_successful(false)
        , Color_Arg1_successful(false)
        , Color_Arg2_successful(false)
        , TextureCoord_Index_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_Material::Meta_TextureStages::getId() const
    {
        return 7;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Stage );
        this->read( _buff, _size, _read, this->Alpha_Operator );
        this->read( _buff, _size, _read, this->Color_Operator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 2:
            {
                this->read( _buff, _size, _read, this->AddressMode_U );
    
                this->AddressMode_U_successful = true;
    
            }break;
        case 3:
            {
                this->read( _buff, _size, _read, this->AddressMode_V );
    
                this->AddressMode_V_successful = true;
    
            }break;
        case 8:
            {
                this->read( _buff, _size, _read, this->Alpha_Arg1 );
    
                this->Alpha_Arg1_successful = true;
    
            }break;
        case 9:
            {
                this->read( _buff, _size, _read, this->Alpha_Arg2 );
    
                this->Alpha_Arg2_successful = true;
    
            }break;
        case 5:
            {
                this->read( _buff, _size, _read, this->Color_Arg1 );
    
                this->Color_Arg1_successful = true;
    
            }break;
        case 6:
            {
                this->read( _buff, _size, _read, this->Color_Arg2 );
    
                this->Color_Arg2_successful = true;
    
            }break;
        case 10:
            {
                this->read( _buff, _size, _read, this->TextureCoord_Index );
    
                this->TextureCoord_Index_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Material::Meta_TextureStages::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Material::Meta_TextureStages::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_Program::Meta_Program()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_Program::getId() const
    {
        return 5;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Program::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Name );
        this->read( _buff, _size, _read, this->Platform );
        this->read( _buff, _size, _read, this->FragmentShader_Name );
        this->read( _buff, _size, _read, this->Sampler_Count );
        this->read( _buff, _size, _read, this->VertexShader_Name );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Program::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Program::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Program::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Program::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_VertexShader::Meta_VertexShader()
        : Metabuf::Metadata()
        , File_Compile_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_VertexShader::getId() const
    {
        return 3;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_VertexShader::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Name );
        this->read( _buff, _size, _read, this->Platform );
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_VertexShader::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->File_Compile );
    
                this->File_Compile_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_VertexShader::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_VertexShader::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_VertexShader::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_Resource::Meta_Resource()
        : Metabuf::Metadata()
        , Unique_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_Resource::getId() const
    {
        return 7;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Resource::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Name );
        this->read( _buff, _size, _read, this->Type );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Resource::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 3:
            {
                this->read( _buff, _size, _read, this->Unique );
    
                this->Unique_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Resource::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Resource::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_Resource::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceAnimation::Meta_ResourceAnimation()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceAnimation::getId() const
    {
        return 22;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceAnimation::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceAnimation::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceAnimation::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    
        switch( _includes )
        {
        case 4:
            {
                includes_Meta_Sequence.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceAnimation::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    
        switch( _includes )
        {
        case 4:
            {
                Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence & metadata = includes_Meta_Sequence.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceAnimation::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::Meta_Sequence()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::getId() const
    {
        return 4;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Delay );
        this->read( _buff, _size, _read, this->ResourceImageName );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceCal3dAnimation::Meta_ResourceCal3dAnimation()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceCal3dAnimation::getId() const
    {
        return 29;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dAnimation::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceCal3dMesh::Meta_ResourceCal3dMesh()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceCal3dMesh::getId() const
    {
        return 30;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dMesh::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dMesh::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dMesh::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dMesh::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dMesh::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceCal3dSkeleton::Meta_ResourceCal3dSkeleton()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceCal3dSkeleton::getId() const
    {
        return 28;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceCursorICO::Meta_ResourceCursorICO()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceCursorICO::getId() const
    {
        return 14;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCursorICO::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCursorICO::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCursorICO::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCursorICO::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCursorICO::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceCursorSystem::Meta_ResourceCursorSystem()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceCursorSystem::getId() const
    {
        return 13;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCursorSystem::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCursorSystem::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCursorSystem::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCursorSystem::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceCursorSystem::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceExternal::Meta_ResourceExternal()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceExternal::getId() const
    {
        return 32;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceExternal::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceExternal::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceExternal::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceExternal::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceExternal::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceFile::Meta_ResourceFile()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceFile::getId() const
    {
        return 15;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceFile::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceFile::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceFile::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceFile::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceFile::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceHIT::Meta_ResourceHIT()
        : Meta_Resource()
        , File_Codec_successful(false)
        , File_Converter_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceHIT::getId() const
    {
        return 23;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceHIT::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceHIT::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 5:
            {
                this->read( _buff, _size, _read, this->File_Codec );
    
                this->File_Codec_successful = true;
    
            }break;
        case 6:
            {
                this->read( _buff, _size, _read, this->File_Converter );
    
                this->File_Converter_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceHIT::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceHIT::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceHIT::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceImageData::Meta_ResourceImageData()
        : Meta_Resource()
        , File_Codec_successful(false)
        , File_NoExist_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceImageData::getId() const
    {
        return 8;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageData::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_MaxSize );
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageData::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 5:
            {
                this->read( _buff, _size, _read, this->File_Codec );
    
                this->File_Codec_successful = true;
    
            }break;
        case 7:
            {
                this->read( _buff, _size, _read, this->File_NoExist );
    
                this->File_NoExist_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageData::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageData::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageData::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceImageDefault::Meta_ResourceImageDefault()
        : Meta_Resource()
        , File_Alpha_successful(false)
        , File_Codec_successful(false)
        , File_Converter_successful(false)
        , File_NoExist_successful(false)
        , File_Offset_successful(false)
        , File_Premultiply_successful(false)
        , File_Size_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceImageDefault::getId() const
    {
        return 9;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageDefault::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_MaxSize );
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageDefault::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 7:
            {
                this->read( _buff, _size, _read, this->File_Alpha );
    
                this->File_Alpha_successful = true;
    
            }break;
        case 5:
            {
                this->read( _buff, _size, _read, this->File_Codec );
    
                this->File_Codec_successful = true;
    
            }break;
        case 6:
            {
                this->read( _buff, _size, _read, this->File_Converter );
    
                this->File_Converter_successful = true;
    
            }break;
        case 12:
            {
                this->read( _buff, _size, _read, this->File_NoExist );
    
                this->File_NoExist_successful = true;
    
            }break;
        case 11:
            {
                this->read( _buff, _size, _read, this->File_Offset );
    
                this->File_Offset_successful = true;
    
            }break;
        case 8:
            {
                this->read( _buff, _size, _read, this->File_Premultiply );
    
                this->File_Premultiply_successful = true;
    
            }break;
        case 10:
            {
                this->read( _buff, _size, _read, this->File_Size );
    
                this->File_Size_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageDefault::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageDefault::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageDefault::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceImageSolid::Meta_ResourceImageSolid()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceImageSolid::getId() const
    {
        return 25;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSolid::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->Color_Value );
        this->read( _buff, _size, _read, this->Size_Value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSolid::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSolid::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSolid::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSolid::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceImageSubstract::Meta_ResourceImageSubstract()
        : Meta_Resource()
        , Image_Alpha_successful(false)
        , Image_Offset_successful(false)
        , Image_Size_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceImageSubstract::getId() const
    {
        return 11;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSubstract::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->Image_MaxSize );
        this->read( _buff, _size, _read, this->Image_Name );
        this->read( _buff, _size, _read, this->Image_UV );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSubstract::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 6:
            {
                this->read( _buff, _size, _read, this->Image_Alpha );
    
                this->Image_Alpha_successful = true;
    
            }break;
        case 9:
            {
                this->read( _buff, _size, _read, this->Image_Offset );
    
                this->Image_Offset_successful = true;
    
            }break;
        case 8:
            {
                this->read( _buff, _size, _read, this->Image_Size );
    
                this->Image_Size_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSubstract::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSubstract::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSubstract::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::Meta_ResourceImageSubstractRGBAndAlpha()
        : Meta_Resource()
        , Image_Offset_successful(false)
        , Image_Size_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::getId() const
    {
        return 10;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->Image_MaxSize );
        this->read( _buff, _size, _read, this->Image_NameAlpha );
        this->read( _buff, _size, _read, this->Image_NameRGB );
        this->read( _buff, _size, _read, this->Image_UVAlpha );
        this->read( _buff, _size, _read, this->Image_UVRGB );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 10:
            {
                this->read( _buff, _size, _read, this->Image_Offset );
    
                this->Image_Offset_successful = true;
    
            }break;
        case 9:
            {
                this->read( _buff, _size, _read, this->Image_Size );
    
                this->Image_Size_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceInternalObject::Meta_ResourceInternalObject()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceInternalObject::getId() const
    {
        return 26;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceInternalObject::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->Internal_Group );
        this->read( _buff, _size, _read, this->Internal_Name );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceInternalObject::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceInternalObject::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceInternalObject::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceInternalObject::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceModel3D::Meta_ResourceModel3D()
        : Meta_Resource()
        , File_Converter_successful(false)
        , File_Dataflow_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceModel3D::getId() const
    {
        return 24;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceModel3D::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
        this->read( _buff, _size, _read, this->Image_Resource );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceModel3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 6:
            {
                this->read( _buff, _size, _read, this->File_Converter );
    
                this->File_Converter_successful = true;
    
            }break;
        case 5:
            {
                this->read( _buff, _size, _read, this->File_Dataflow );
    
                this->File_Dataflow_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceModel3D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceModel3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceModel3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceMovie::Meta_ResourceMovie()
        : Meta_Resource()
        , Anchor_Point_successful(false)
        , Bounds_Box_successful(false)
        , KeyFramesPackPath_Codec_successful(false)
        , KeyFramesPackPath_Converter_successful(false)
        , Loop_Segment_successful(false)
        , Offset_Point_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceMovie::getId() const
    {
        return 20;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->Duration_Value );
        this->read( _buff, _size, _read, this->FrameDuration_Value );
        this->read( _buff, _size, _read, this->Height_Value );
        this->read( _buff, _size, _read, this->KeyFramesPackPath_Path );
        this->read( _buff, _size, _read, this->Width_Value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 12:
            {
                this->read( _buff, _size, _read, this->Anchor_Point );
    
                this->Anchor_Point_successful = true;
    
            }break;
        case 14:
            {
                this->read( _buff, _size, _read, this->Bounds_Box );
    
                this->Bounds_Box_successful = true;
    
            }break;
        case 9:
            {
                this->read( _buff, _size, _read, this->KeyFramesPackPath_Codec );
    
                this->KeyFramesPackPath_Codec_successful = true;
    
            }break;
        case 10:
            {
                this->read( _buff, _size, _read, this->KeyFramesPackPath_Converter );
    
                this->KeyFramesPackPath_Converter_successful = true;
    
            }break;
        case 11:
            {
                this->read( _buff, _size, _read, this->Loop_Segment );
    
                this->Loop_Segment_successful = true;
    
            }break;
        case 13:
            {
                this->read( _buff, _size, _read, this->Offset_Point );
    
                this->Offset_Point_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    
        switch( _includes )
        {
        case 17:
            {
                includes_Meta_MovieCamera3D.reserve( _count );
            }break;
        case 15:
            {
                includes_Meta_MovieLayer2D.reserve( _count );
            }break;
        case 16:
            {
                includes_Meta_MovieLayer3D.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    
        switch( _includes )
        {
        case 17:
            {
                Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D & metadata = includes_Meta_MovieCamera3D.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 15:
            {
                Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D & metadata = includes_Meta_MovieLayer2D.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 16:
            {
                Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D & metadata = includes_Meta_MovieLayer3D.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::Meta_MovieCamera3D()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::getId() const
    {
        return 17;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->CameraAspect );
        this->read( _buff, _size, _read, this->CameraFOV );
        this->read( _buff, _size, _read, this->CameraInterest );
        this->read( _buff, _size, _read, this->CameraPosition );
        this->read( _buff, _size, _read, this->Height );
        this->read( _buff, _size, _read, this->Width );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::Meta_MovieLayer2D()
        : Metabuf::Metadata()
        , AnchorPoint_successful(false)
        , BlendingMode_successful(false)
        , Loop_successful(false)
        , Params_successful(false)
        , Parent_successful(false)
        , PlayCount_successful(false)
        , Polygon_successful(false)
        , Position_successful(false)
        , Rotation_successful(false)
        , Scale_successful(false)
        , Shape_successful(false)
        , StartInterval_successful(false)
        , Stretch_successful(false)
        , Switch_successful(false)
        , TimeRemap_successful(false)
        , Viewport_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::getId() const
    {
        return 15;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->In );
        this->read( _buff, _size, _read, this->Index );
        this->read( _buff, _size, _read, this->Name );
        this->read( _buff, _size, _read, this->Out );
        this->read( _buff, _size, _read, this->Source );
        this->read( _buff, _size, _read, this->Type );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 17:
            {
                this->read( _buff, _size, _read, this->AnchorPoint );
    
                this->AnchorPoint_successful = true;
    
            }break;
        case 9:
            {
                this->read( _buff, _size, _read, this->BlendingMode );
    
                this->BlendingMode_successful = true;
    
            }break;
        case 16:
            {
                this->read( _buff, _size, _read, this->Loop );
    
                this->Loop_successful = true;
    
            }break;
        case 22:
            {
                this->read( _buff, _size, _read, this->Params );
    
                this->Params_successful = true;
    
            }break;
        case 7:
            {
                this->read( _buff, _size, _read, this->Parent );
    
                this->Parent_successful = true;
    
            }break;
        case 13:
            {
                this->read( _buff, _size, _read, this->PlayCount );
    
                this->PlayCount_successful = true;
    
            }break;
        case 12:
            {
                this->read( _buff, _size, _read, this->Polygon );
    
                this->Polygon_successful = true;
    
            }break;
        case 18:
            {
                this->read( _buff, _size, _read, this->Position );
    
                this->Position_successful = true;
    
            }break;
        case 20:
            {
                this->read( _buff, _size, _read, this->Rotation );
    
                this->Rotation_successful = true;
    
            }break;
        case 19:
            {
                this->read( _buff, _size, _read, this->Scale );
    
                this->Scale_successful = true;
    
            }break;
        case 11:
            {
                this->read( _buff, _size, _read, this->Shape );
    
                this->Shape_successful = true;
    
            }break;
        case 8:
            {
                this->read( _buff, _size, _read, this->StartInterval );
    
                this->StartInterval_successful = true;
    
            }break;
        case 14:
            {
                this->read( _buff, _size, _read, this->Stretch );
    
                this->Stretch_successful = true;
    
            }break;
        case 15:
            {
                this->read( _buff, _size, _read, this->Switch );
    
                this->Switch_successful = true;
    
            }break;
        case 10:
            {
                this->read( _buff, _size, _read, this->TimeRemap );
    
                this->TimeRemap_successful = true;
    
            }break;
        case 21:
            {
                this->read( _buff, _size, _read, this->Viewport );
    
                this->Viewport_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::Meta_MovieLayer3D()
        : Metabuf::Metadata()
        , AnchorPoint_successful(false)
        , BlendingMode_successful(false)
        , Loop_successful(false)
        , Params_successful(false)
        , Parent_successful(false)
        , PlayCount_successful(false)
        , Polygon_successful(false)
        , Position_successful(false)
        , Rotation_successful(false)
        , Scale_successful(false)
        , Shape_successful(false)
        , StartInterval_successful(false)
        , Stretch_successful(false)
        , Switch_successful(false)
        , TimeRemap_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::getId() const
    {
        return 16;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->In );
        this->read( _buff, _size, _read, this->Index );
        this->read( _buff, _size, _read, this->Name );
        this->read( _buff, _size, _read, this->Out );
        this->read( _buff, _size, _read, this->Source );
        this->read( _buff, _size, _read, this->Type );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 17:
            {
                this->read( _buff, _size, _read, this->AnchorPoint );
    
                this->AnchorPoint_successful = true;
    
            }break;
        case 9:
            {
                this->read( _buff, _size, _read, this->BlendingMode );
    
                this->BlendingMode_successful = true;
    
            }break;
        case 16:
            {
                this->read( _buff, _size, _read, this->Loop );
    
                this->Loop_successful = true;
    
            }break;
        case 21:
            {
                this->read( _buff, _size, _read, this->Params );
    
                this->Params_successful = true;
    
            }break;
        case 7:
            {
                this->read( _buff, _size, _read, this->Parent );
    
                this->Parent_successful = true;
    
            }break;
        case 13:
            {
                this->read( _buff, _size, _read, this->PlayCount );
    
                this->PlayCount_successful = true;
    
            }break;
        case 12:
            {
                this->read( _buff, _size, _read, this->Polygon );
    
                this->Polygon_successful = true;
    
            }break;
        case 18:
            {
                this->read( _buff, _size, _read, this->Position );
    
                this->Position_successful = true;
    
            }break;
        case 20:
            {
                this->read( _buff, _size, _read, this->Rotation );
    
                this->Rotation_successful = true;
    
            }break;
        case 19:
            {
                this->read( _buff, _size, _read, this->Scale );
    
                this->Scale_successful = true;
    
            }break;
        case 11:
            {
                this->read( _buff, _size, _read, this->Shape );
    
                this->Shape_successful = true;
    
            }break;
        case 8:
            {
                this->read( _buff, _size, _read, this->StartInterval );
    
                this->StartInterval_successful = true;
    
            }break;
        case 14:
            {
                this->read( _buff, _size, _read, this->Stretch );
    
                this->Stretch_successful = true;
    
            }break;
        case 15:
            {
                this->read( _buff, _size, _read, this->Switch );
    
                this->Switch_successful = true;
    
            }break;
        case 10:
            {
                this->read( _buff, _size, _read, this->TimeRemap );
    
                this->TimeRemap_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceMovie2::Meta_ResourceMovie2()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceMovie2::getId() const
    {
        return 19;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie2::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie2::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie2::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie2::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMovie2::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceMusic::Meta_ResourceMusic()
        : Meta_Resource()
        , DefaultVolume_Value_successful(false)
        , File_Codec_successful(false)
        , File_Converter_successful(false)
        , File_External_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceMusic::getId() const
    {
        return 17;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMusic::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMusic::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 8:
            {
                this->read( _buff, _size, _read, this->DefaultVolume_Value );
    
                this->DefaultVolume_Value_successful = true;
    
            }break;
        case 5:
            {
                this->read( _buff, _size, _read, this->File_Codec );
    
                this->File_Codec_successful = true;
    
            }break;
        case 6:
            {
                this->read( _buff, _size, _read, this->File_Converter );
    
                this->File_Converter_successful = true;
    
            }break;
        case 7:
            {
                this->read( _buff, _size, _read, this->File_External );
    
                this->File_External_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMusic::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMusic::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceMusic::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceParticle::Meta_ResourceParticle()
        : Meta_Resource()
        , File_Converter_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceParticle::getId() const
    {
        return 12;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceParticle::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->AtlasCount_Value );
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceParticle::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 5:
            {
                this->read( _buff, _size, _read, this->File_Converter );
    
                this->File_Converter_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceParticle::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    
        switch( _includes )
        {
        case 7:
            {
                includes_Meta_Atlas.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceParticle::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    
        switch( _includes )
        {
        case 7:
            {
                Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas & metadata = includes_Meta_Atlas.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceParticle::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::Meta_Atlas()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::getId() const
    {
        return 7;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Index );
        this->read( _buff, _size, _read, this->ResourceName );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceShape::Meta_ResourceShape()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceShape::getId() const
    {
        return 27;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceShape::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->Polygon_Value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceShape::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceShape::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceShape::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceShape::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceSound::Meta_ResourceSound()
        : Meta_Resource()
        , DefaultVolume_Value_successful(false)
        , File_Codec_successful(false)
        , File_Converter_successful(false)
        , IsStreamable_Value_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceSound::getId() const
    {
        return 16;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSound::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSound::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 7:
            {
                this->read( _buff, _size, _read, this->DefaultVolume_Value );
    
                this->DefaultVolume_Value_successful = true;
    
            }break;
        case 5:
            {
                this->read( _buff, _size, _read, this->File_Codec );
    
                this->File_Codec_successful = true;
    
            }break;
        case 6:
            {
                this->read( _buff, _size, _read, this->File_Converter );
    
                this->File_Converter_successful = true;
    
            }break;
        case 8:
            {
                this->read( _buff, _size, _read, this->IsStreamable_Value );
    
                this->IsStreamable_Value_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSound::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSound::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSound::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceSpine::Meta_ResourceSpine()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceSpine::getId() const
    {
        return 31;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSpine::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->Atlas_Path );
        this->read( _buff, _size, _read, this->Skeleton_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSpine::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSpine::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    
        switch( _includes )
        {
        case 6:
            {
                includes_Meta_Image.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSpine::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    
        switch( _includes )
        {
        case 6:
            {
                Meta_DataBlock::Meta_ResourceSpine::Meta_Image & metadata = includes_Meta_Image.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSpine::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceSpine::Meta_Image::Meta_Image()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceSpine::Meta_Image::getId() const
    {
        return 6;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSpine::Meta_Image::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Name );
        this->read( _buff, _size, _read, this->Resource );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSpine::Meta_Image::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSpine::Meta_Image::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSpine::Meta_Image::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceSpine::Meta_Image::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceVideo::Meta_ResourceVideo()
        : Meta_Resource()
        , File_Alpha_successful(false)
        , File_Codec_successful(false)
        , File_Converter_successful(false)
        , File_Duration_successful(false)
        , File_FrameRate_successful(false)
        , File_NoSeek_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceVideo::getId() const
    {
        return 21;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceVideo::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->File_Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceVideo::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 5:
            {
                this->read( _buff, _size, _read, this->File_Alpha );
    
                this->File_Alpha_successful = true;
    
            }break;
        case 6:
            {
                this->read( _buff, _size, _read, this->File_Codec );
    
                this->File_Codec_successful = true;
    
            }break;
        case 7:
            {
                this->read( _buff, _size, _read, this->File_Converter );
    
                this->File_Converter_successful = true;
    
            }break;
        case 10:
            {
                this->read( _buff, _size, _read, this->File_Duration );
    
                this->File_Duration_successful = true;
    
            }break;
        case 9:
            {
                this->read( _buff, _size, _read, this->File_FrameRate );
    
                this->File_FrameRate_successful = true;
    
            }break;
        case 8:
            {
                this->read( _buff, _size, _read, this->File_NoSeek );
    
                this->File_NoSeek_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceVideo::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceVideo::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceVideo::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceWindow::Meta_ResourceWindow()
        : Meta_Resource()
        , WindowBackground_ResourceImageName_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceWindow::getId() const
    {
        return 18;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceWindow::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        Meta_DataBlock::Meta_Resource::_parseData( _buff, _size, _read );
    
        this->read( _buff, _size, _read, this->WindowBottom_Offset );
        this->read( _buff, _size, _read, this->WindowBottom_ResourceImageName );
        this->read( _buff, _size, _read, this->WindowLeft_Offset );
        this->read( _buff, _size, _read, this->WindowLeft_ResourceImageName );
        this->read( _buff, _size, _read, this->WindowLeftBottom_Offset );
        this->read( _buff, _size, _read, this->WindowLeftBottom_ResourceImageName );
        this->read( _buff, _size, _read, this->WindowLeftTop_Offset );
        this->read( _buff, _size, _read, this->WindowLeftTop_ResourceImageName );
        this->read( _buff, _size, _read, this->WindowRight_Offset );
        this->read( _buff, _size, _read, this->WindowRight_ResourceImageName );
        this->read( _buff, _size, _read, this->WindowRightBottom_Offset );
        this->read( _buff, _size, _read, this->WindowRightBottom_ResourceImageName );
        this->read( _buff, _size, _read, this->WindowRightTop_Offset );
        this->read( _buff, _size, _read, this->WindowRightTop_ResourceImageName );
        this->read( _buff, _size, _read, this->WindowTop_Offset );
        this->read( _buff, _size, _read, this->WindowTop_ResourceImageName );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceWindow::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id );
    
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->WindowBackground_ResourceImageName );
    
                this->WindowBackground_ResourceImageName_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceWindow::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceWindow::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_DataBlock::Meta_ResourceWindow::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators );
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_KeyFramesPack()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::getId() const
    {
        return 4;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->MaxIndex );
        this->read( _buff, _size, _read, this->Version );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
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
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 5:
            {
                Meta_KeyFramesPack::Meta_ImageShape & metadata = includes_Meta_ImageShape.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 6:
            {
                Meta_KeyFramesPack::Meta_KeyFrames2D & metadata = includes_Meta_KeyFrames2D.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 7:
            {
                Meta_KeyFramesPack::Meta_KeyFrames3D & metadata = includes_Meta_KeyFrames3D.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 4:
            {
                Meta_KeyFramesPack::Meta_Polygon & metadata = includes_Meta_Polygon.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 3:
            {
                Meta_KeyFramesPack::Meta_TimeRemap & metadata = includes_Meta_TimeRemap.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_ImageShape::Meta_ImageShape()
        : Metabuf::Metadata()
        , Count_successful(false)
        , ImageOffset_successful(false)
        , ImageSize_successful(false)
        , Immutable_successful(false)
        , Subtract_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_ImageShape::getId() const
    {
        return 5;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_ImageShape::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->ImageMaxSize );
        this->read( _buff, _size, _read, this->LayerIndex );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_ImageShape::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 7:
            {
                this->read( _buff, _size, _read, this->Count );
    
                this->Count_successful = true;
    
            }break;
        case 4:
            {
                this->read( _buff, _size, _read, this->ImageOffset );
    
                this->ImageOffset_successful = true;
    
            }break;
        case 3:
            {
                this->read( _buff, _size, _read, this->ImageSize );
    
                this->ImageSize_successful = true;
    
            }break;
        case 5:
            {
                this->read( _buff, _size, _read, this->Immutable );
    
                this->Immutable_successful = true;
    
            }break;
        case 6:
            {
                this->read( _buff, _size, _read, this->Subtract );
    
                this->Subtract_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_ImageShape::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 8:
            {
                includes_Meta_Shape.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_ImageShape::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 8:
            {
                Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape & metadata = includes_Meta_Shape.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_ImageShape::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::Meta_Shape()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::getId() const
    {
        return 8;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Polygon );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrames2D()
        : Metabuf::Metadata()
        , Count_successful(false)
        , Immutable_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_KeyFrames2D::getId() const
    {
        return 6;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->LayerIndex );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 3:
            {
                this->read( _buff, _size, _read, this->Count );
    
                this->Count_successful = true;
    
            }break;
        case 2:
            {
                this->read( _buff, _size, _read, this->Immutable );
    
                this->Immutable_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames2D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 4:
            {
                includes_Meta_KeyFrame2D.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 4:
            {
                Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & metadata = includes_Meta_KeyFrame2D.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames2D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::Meta_KeyFrame2D()
        : Metabuf::Metadata()
        , AnchorPoint_successful(false)
        , Count_successful(false)
        , Opacity_successful(false)
        , Position_successful(false)
        , Rotation_successful(false)
        , Scale_successful(false)
        , Volume_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::getId() const
    {
        return 4;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->AnchorPoint );
    
                this->AnchorPoint_successful = true;
    
            }break;
        case 7:
            {
                this->read( _buff, _size, _read, this->Count );
    
                this->Count_successful = true;
    
            }break;
        case 5:
            {
                this->read( _buff, _size, _read, this->Opacity );
    
                this->Opacity_successful = true;
    
            }break;
        case 2:
            {
                this->read( _buff, _size, _read, this->Position );
    
                this->Position_successful = true;
    
            }break;
        case 4:
            {
                this->read( _buff, _size, _read, this->Rotation );
    
                this->Rotation_successful = true;
    
            }break;
        case 3:
            {
                this->read( _buff, _size, _read, this->Scale );
    
                this->Scale_successful = true;
    
            }break;
        case 6:
            {
                this->read( _buff, _size, _read, this->Volume );
    
                this->Volume_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrames3D()
        : Metabuf::Metadata()
        , Count_successful(false)
        , Immutable_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_KeyFrames3D::getId() const
    {
        return 7;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->LayerIndex );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 3:
            {
                this->read( _buff, _size, _read, this->Count );
    
                this->Count_successful = true;
    
            }break;
        case 2:
            {
                this->read( _buff, _size, _read, this->Immutable );
    
                this->Immutable_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames3D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 4:
            {
                includes_Meta_KeyFrame3D.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 4:
            {
                Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D & metadata = includes_Meta_KeyFrame3D.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::Meta_KeyFrame3D()
        : Metabuf::Metadata()
        , AnchorPoint_successful(false)
        , Count_successful(false)
        , Opacity_successful(false)
        , Orientation_successful(false)
        , Position_successful(false)
        , Rotation_successful(false)
        , Scale_successful(false)
        , Volume_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::getId() const
    {
        return 4;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->AnchorPoint );
    
                this->AnchorPoint_successful = true;
    
            }break;
        case 8:
            {
                this->read( _buff, _size, _read, this->Count );
    
                this->Count_successful = true;
    
            }break;
        case 6:
            {
                this->read( _buff, _size, _read, this->Opacity );
    
                this->Opacity_successful = true;
    
            }break;
        case 5:
            {
                this->read( _buff, _size, _read, this->Orientation );
    
                this->Orientation_successful = true;
    
            }break;
        case 2:
            {
                this->read( _buff, _size, _read, this->Position );
    
                this->Position_successful = true;
    
            }break;
        case 4:
            {
                this->read( _buff, _size, _read, this->Rotation );
    
                this->Rotation_successful = true;
    
            }break;
        case 3:
            {
                this->read( _buff, _size, _read, this->Scale );
    
                this->Scale_successful = true;
    
            }break;
        case 7:
            {
                this->read( _buff, _size, _read, this->Volume );
    
                this->Volume_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_Polygon::Meta_Polygon()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_Polygon::getId() const
    {
        return 4;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_Polygon::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->LayerIndex );
        this->read( _buff, _size, _read, this->Value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_Polygon::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_Polygon::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_Polygon::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_Polygon::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_TimeRemap::Meta_TimeRemap()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_TimeRemap::getId() const
    {
        return 3;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_TimeRemap::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->LayerIndex );
        this->read( _buff, _size, _read, this->Time );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_TimeRemap::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_TimeRemap::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_TimeRemap::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_KeyFramesPack::Meta_TimeRemap::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Pak()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::getId() const
    {
        return 2;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
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
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 5:
            {
                Meta_Pak::Meta_Datas & metadata = includes_Meta_Datas.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 2:
            {
                Meta_Pak::Meta_Fonts & metadata = includes_Meta_Fonts.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 6:
            {
                Meta_Pak::Meta_Materials & metadata = includes_Meta_Materials.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 3:
            {
                Meta_Pak::Meta_Resources & metadata = includes_Meta_Resources.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 1:
            {
                Meta_Pak::Meta_Scripts & metadata = includes_Meta_Scripts.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        case 4:
            {
                Meta_Pak::Meta_Texts & metadata = includes_Meta_Texts.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Datas::Meta_Datas()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Datas::getId() const
    {
        return 5;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Datas::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Datas::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Datas::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 1:
            {
                includes_Meta_Data.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Datas::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Pak::Meta_Datas::Meta_Data & metadata = includes_Meta_Data.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Datas::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Datas::Meta_Data::Meta_Data()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Datas::Meta_Data::getId() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Datas::Meta_Data::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Name );
        this->read( _buff, _size, _read, this->Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Datas::Meta_Data::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Datas::Meta_Data::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Datas::Meta_Data::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Datas::Meta_Data::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Fonts::Meta_Fonts()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Fonts::getId() const
    {
        return 2;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Fonts::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Fonts::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Fonts::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Fonts::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Fonts::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Materials::Meta_Materials()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Materials::getId() const
    {
        return 6;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Materials::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Materials::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Materials::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 1:
            {
                includes_Meta_Material.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Materials::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Pak::Meta_Materials::Meta_Material & metadata = includes_Meta_Material.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Materials::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Materials::Meta_Material::Meta_Material()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Materials::Meta_Material::getId() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Materials::Meta_Material::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Materials::Meta_Material::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Materials::Meta_Material::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Materials::Meta_Material::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Materials::Meta_Material::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Resources::Meta_Resources()
        : Metabuf::Metadata()
        , Ignored_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Resources::getId() const
    {
        return 3;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Resources::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Resources::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->Ignored );
    
                this->Ignored_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Resources::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 2:
            {
                includes_Meta_Resource.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Resources::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_Pak::Meta_Resources::Meta_Resource & metadata = includes_Meta_Resource.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Resources::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Resources::Meta_Resource::Meta_Resource()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Resources::Meta_Resource::getId() const
    {
        return 2;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Resources::Meta_Resource::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Resources::Meta_Resource::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Resources::Meta_Resource::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Resources::Meta_Resource::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Resources::Meta_Resource::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Scripts::Meta_Scripts()
        : Metabuf::Metadata()
        , Finalizer_successful(false)
        , Initializer_successful(false)
        , Module_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Scripts::getId() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Scripts::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Scripts::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->Finalizer );
    
                this->Finalizer_successful = true;
    
            }break;
        case 3:
            {
                this->read( _buff, _size, _read, this->Initializer );
    
                this->Initializer_successful = true;
    
            }break;
        case 2:
            {
                this->read( _buff, _size, _read, this->Module );
    
                this->Module_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Scripts::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Scripts::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Scripts::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Texts::Meta_Texts()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Texts::getId() const
    {
        return 4;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Texts::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Texts::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Texts::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 1:
            {
                includes_Meta_Text.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Texts::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Pak::Meta_Texts::Meta_Text & metadata = includes_Meta_Text.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Texts::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Texts::Meta_Text::Meta_Text()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Texts::Meta_Text::getId() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Texts::Meta_Text::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Path );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Texts::Meta_Text::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Texts::Meta_Text::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Texts::Meta_Text::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Pak::Meta_Texts::Meta_Text::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Texts::Meta_Texts()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Texts::getId() const
    {
        return 3;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Texts::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Texts::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    }
    
    //////////////////////////////////////////////////////////////////////////
    void Meta_Texts::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 1:
            {
                includes_Meta_Text.reserve( _count );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Texts::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Texts::Meta_Text & metadata = includes_Meta_Text.emplace_back();
    
                metadata.parse( _buff, _size, _read, m_userData );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Texts::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Texts::Meta_Text::Meta_Text()
        : Metabuf::Metadata()
        , CharOffset_successful(false)
        , Font_successful(false)
        , LineOffset_successful(false)
        , Value_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Texts::Meta_Text::getId() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Texts::Meta_Text::_parseData( const unsigned char * _buff, size_t _size, size_t & _read )
    {
        this->read( _buff, _size, _read, this->Key );
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Texts::Meta_Text::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->CharOffset );
    
                this->CharOffset_successful = true;
    
            }break;
        case 3:
            {
                this->read( _buff, _size, _read, this->Font );
    
                this->Font_successful = true;
    
            }break;
        case 5:
            {
                this->read( _buff, _size, _read, this->LineOffset );
    
                this->LineOffset_successful = true;
    
            }break;
        case 2:
            {
                this->read( _buff, _size, _read, this->Value );
    
                this->Value_successful = true;
    
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Texts::Meta_Text::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Texts::Meta_Text::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    }
    //////////////////////////////////////////////////////////////////////////
    void Meta_Texts::Meta_Text::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    }
}
