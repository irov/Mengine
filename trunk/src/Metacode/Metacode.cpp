#   include "Metacode.h"

namespace Metacode
{
    //////////////////////////////////////////////////////////////////////////
    static bool readHeader2( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _readVersion, uint32_t & _needVersion )
    {
        stdex::memory_reader ar(_buff, _size, _read);

        uint32_t head;
        ar.readPOD( head );

        if( head != 3133062829u )
        {
            return false;
        }

        uint32_t version;
        ar.readPOD( version );

        _readVersion = version;
        _needVersion = 74;

        if( version != 74 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool readHeader( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _readVersion, uint32_t & _needVersion )
    {
        try
        {
            bool successful = readHeader2( _buff, _size, _read, _readVersion, _needVersion );

            return successful;
        }
        catch( const stdex::memory_reader_exception & )
        {
        }

        return false;
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
        try
        {
            bool successful = readStrings2( _buff, _size, _read, _stringCount );

            return successful;
        }
        catch( const stdex::memory_reader_exception & )
        {
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    static const char * readString2( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _stringSize )
    {
        stdex::memory_reader ar(_buff, _size, _read);

        uint32_t size;
        ar.readSize( size );

        const char * value = ar.current_buff<char>();
        ar.skip( size );

        _stringSize = size;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * readString( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t & _stringSize )
    {
        try
        {
            const char * value = readString2( _buff, _size, _read, _stringSize );

            return value;
        }
        catch( const stdex::memory_reader_exception & )
        {
        }

        return nullptr;
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
    bool Meta_DataBlock::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Name ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 2:
            {
                includes_Meta_Include.reserve( _count );
                return true;
                break;
            }
        case 3:
            {
                includes_Meta_Resource.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_DataBlock::Meta_Include & metadata = includes_Meta_Include.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        switch( _generators )
        {
        case 19:
            {
                Meta_DataBlock::Meta_ResourceAnimation * metadata = new Meta_DataBlock::Meta_ResourceAnimation ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 26:
            {
                Meta_DataBlock::Meta_ResourceCal3dAnimation * metadata = new Meta_DataBlock::Meta_ResourceCal3dAnimation ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 27:
            {
                Meta_DataBlock::Meta_ResourceCal3dMesh * metadata = new Meta_DataBlock::Meta_ResourceCal3dMesh ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 25:
            {
                Meta_DataBlock::Meta_ResourceCal3dSkeleton * metadata = new Meta_DataBlock::Meta_ResourceCal3dSkeleton ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 12:
            {
                Meta_DataBlock::Meta_ResourceCursorICO * metadata = new Meta_DataBlock::Meta_ResourceCursorICO ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 11:
            {
                Meta_DataBlock::Meta_ResourceCursorSystem * metadata = new Meta_DataBlock::Meta_ResourceCursorSystem ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 9:
            {
                Meta_DataBlock::Meta_ResourceEmitter * metadata = new Meta_DataBlock::Meta_ResourceEmitter ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 8:
            {
                Meta_DataBlock::Meta_ResourceEmitterContainer * metadata = new Meta_DataBlock::Meta_ResourceEmitterContainer ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 28:
            {
                Meta_DataBlock::Meta_ResourceExternal * metadata = new Meta_DataBlock::Meta_ResourceExternal ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 13:
            {
                Meta_DataBlock::Meta_ResourceFile * metadata = new Meta_DataBlock::Meta_ResourceFile ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 20:
            {
                Meta_DataBlock::Meta_ResourceHIT * metadata = new Meta_DataBlock::Meta_ResourceHIT ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 4:
            {
                Meta_DataBlock::Meta_ResourceImageData * metadata = new Meta_DataBlock::Meta_ResourceImageData ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 5:
            {
                Meta_DataBlock::Meta_ResourceImageDefault * metadata = new Meta_DataBlock::Meta_ResourceImageDefault ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 22:
            {
                Meta_DataBlock::Meta_ResourceImageSolid * metadata = new Meta_DataBlock::Meta_ResourceImageSolid ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 7:
            {
                Meta_DataBlock::Meta_ResourceImageSubstract * metadata = new Meta_DataBlock::Meta_ResourceImageSubstract ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 6:
            {
                Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha * metadata = new Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 23:
            {
                Meta_DataBlock::Meta_ResourceInternalObject * metadata = new Meta_DataBlock::Meta_ResourceInternalObject ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 21:
            {
                Meta_DataBlock::Meta_ResourceModel3D * metadata = new Meta_DataBlock::Meta_ResourceModel3D ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 17:
            {
                Meta_DataBlock::Meta_ResourceMovie * metadata = new Meta_DataBlock::Meta_ResourceMovie ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 10:
            {
                Meta_DataBlock::Meta_ResourceParticle * metadata = new Meta_DataBlock::Meta_ResourceParticle ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 15:
            {
                Meta_DataBlock::Meta_ResourcePlaylist * metadata = new Meta_DataBlock::Meta_ResourcePlaylist ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 24:
            {
                Meta_DataBlock::Meta_ResourceShape * metadata = new Meta_DataBlock::Meta_ResourceShape ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 14:
            {
                Meta_DataBlock::Meta_ResourceSound * metadata = new Meta_DataBlock::Meta_ResourceSound ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 18:
            {
                Meta_DataBlock::Meta_ResourceVideo * metadata = new Meta_DataBlock::Meta_ResourceVideo ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        case 16:
            {
                Meta_DataBlock::Meta_ResourceWindow * metadata = new Meta_DataBlock::Meta_ResourceWindow ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
    
                return true;
                break;
            }
        }
    
        return false;
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
    bool Meta_DataBlock::Meta_Include::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Include::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Include::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Include::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_Resource::Meta_Resource()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_Resource::getId() const
    {
        return 3;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Resource::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Name ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Type ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Resource::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Resource::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Resource::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceAnimation::Meta_ResourceAnimation()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceAnimation::getId() const
    {
        return 19;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 3:
            {
                includes_Meta_Sequence.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 3:
            {
                Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence & metadata = includes_Meta_Sequence.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::Meta_Sequence()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::getId() const
    {
        return 3;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Delay ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 1:
            {
                if( this->read( _buff, _size, _read, this->ResourceImageName ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceCal3dAnimation::Meta_ResourceCal3dAnimation()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceCal3dAnimation::getId() const
    {
        return 26;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dAnimation::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dAnimation::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceCal3dMesh::Meta_ResourceCal3dMesh()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceCal3dMesh::getId() const
    {
        return 27;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dMesh::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dMesh::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dMesh::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dMesh::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceCal3dSkeleton::Meta_ResourceCal3dSkeleton()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceCal3dSkeleton::getId() const
    {
        return 25;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dSkeleton::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCal3dSkeleton::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceCursorICO::Meta_ResourceCursorICO()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceCursorICO::getId() const
    {
        return 12;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorICO::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorICO::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorICO::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorICO::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceCursorSystem::Meta_ResourceCursorSystem()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceCursorSystem::getId() const
    {
        return 11;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceEmitter::Meta_ResourceEmitter()
        : Meta_Resource()
        , Offset_Value_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceEmitter::getId() const
    {
        return 9;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitter::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Container_Name ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Emitter_Name ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->EmitterRelative_Value ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->Offset_Value ) == false )
                {
                    return false;
                }
    
                this->Offset_Value_successful = true;
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitter::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitter::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitter::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceEmitterContainer::Meta_ResourceEmitterContainer()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceEmitterContainer::getId() const
    {
        return 8;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Folder_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceExternal::Meta_ResourceExternal()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceExternal::getId() const
    {
        return 28;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceExternal::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceExternal::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceExternal::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceExternal::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceFile::Meta_ResourceFile()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceFile::getId() const
    {
        return 13;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFile::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFile::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFile::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFile::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
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
        return 20;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceHIT::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 4:
            {
                if( this->read( _buff, _size, _read, this->File_Codec ) == false )
                {
                    return false;
                }
    
                this->File_Codec_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->File_Converter ) == false )
                {
                    return false;
                }
    
                this->File_Converter_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceHIT::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceHIT::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceHIT::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceImageData::Meta_ResourceImageData()
        : Meta_Resource()
        , File_Codec_successful(false)
        , File_MaxSize_successful(false)
        , File_NoExist_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceImageData::getId() const
    {
        return 4;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageData::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 4:
            {
                if( this->read( _buff, _size, _read, this->File_Codec ) == false )
                {
                    return false;
                }
    
                this->File_Codec_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->File_MaxSize ) == false )
                {
                    return false;
                }
    
                this->File_MaxSize_successful = true;
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->File_NoExist ) == false )
                {
                    return false;
                }
    
                this->File_NoExist_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageData::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageData::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageData::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceImageDefault::Meta_ResourceImageDefault()
        : Meta_Resource()
        , File_Alpha_successful(false)
        , File_Codec_successful(false)
        , File_MaxSize_successful(false)
        , File_NoExist_successful(false)
        , File_Offset_successful(false)
        , File_Size_successful(false)
        , File_WrapX_successful(false)
        , File_WrapY_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceImageDefault::getId() const
    {
        return 5;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageDefault::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 5:
            {
                if( this->read( _buff, _size, _read, this->File_Alpha ) == false )
                {
                    return false;
                }
    
                this->File_Alpha_successful = true;
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->File_Codec ) == false )
                {
                    return false;
                }
    
                this->File_Codec_successful = true;
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->File_MaxSize ) == false )
                {
                    return false;
                }
    
                this->File_MaxSize_successful = true;
    
                return true;
            }break;
        case 11:
            {
                if( this->read( _buff, _size, _read, this->File_NoExist ) == false )
                {
                    return false;
                }
    
                this->File_NoExist_successful = true;
    
                return true;
            }break;
        case 10:
            {
                if( this->read( _buff, _size, _read, this->File_Offset ) == false )
                {
                    return false;
                }
    
                this->File_Offset_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 9:
            {
                if( this->read( _buff, _size, _read, this->File_Size ) == false )
                {
                    return false;
                }
    
                this->File_Size_successful = true;
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->File_WrapX ) == false )
                {
                    return false;
                }
    
                this->File_WrapX_successful = true;
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->File_WrapY ) == false )
                {
                    return false;
                }
    
                this->File_WrapY_successful = true;
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageDefault::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageDefault::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageDefault::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceImageSolid::Meta_ResourceImageSolid()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceImageSolid::getId() const
    {
        return 22;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSolid::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Color_Value ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Size_Value ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSolid::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSolid::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSolid::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceImageSubstract::Meta_ResourceImageSubstract()
        : Meta_Resource()
        , Image_Alpha_successful(false)
        , Image_Offset_successful(false)
        , Image_Rotate_successful(false)
        , Image_Size_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceImageSubstract::getId() const
    {
        return 7;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSubstract::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 6:
            {
                if( this->read( _buff, _size, _read, this->Image_Alpha ) == false )
                {
                    return false;
                }
    
                this->Image_Alpha_successful = true;
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->Image_MaxSize ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Image_Name ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 9:
            {
                if( this->read( _buff, _size, _read, this->Image_Offset ) == false )
                {
                    return false;
                }
    
                this->Image_Offset_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->Image_Rotate ) == false )
                {
                    return false;
                }
    
                this->Image_Rotate_successful = true;
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->Image_Size ) == false )
                {
                    return false;
                }
    
                this->Image_Size_successful = true;
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Image_UV ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSubstract::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSubstract::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSubstract::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::Meta_ResourceImageSubstractRGBAndAlpha()
        : Meta_Resource()
        , Image_Offset_successful(false)
        , Image_Size_successful(false)
        , Image_UVAlphaRotate_successful(false)
        , Image_UVRGBRotate_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::getId() const
    {
        return 6;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 9:
            {
                if( this->read( _buff, _size, _read, this->Image_MaxSize ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->Image_NameAlpha ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Image_NameRGB ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 11:
            {
                if( this->read( _buff, _size, _read, this->Image_Offset ) == false )
                {
                    return false;
                }
    
                this->Image_Offset_successful = true;
    
                return true;
            }break;
        case 10:
            {
                if( this->read( _buff, _size, _read, this->Image_Size ) == false )
                {
                    return false;
                }
    
                this->Image_Size_successful = true;
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->Image_UVAlpha ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->Image_UVAlphaRotate ) == false )
                {
                    return false;
                }
    
                this->Image_UVAlphaRotate_successful = true;
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Image_UVRGB ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->Image_UVRGBRotate ) == false )
                {
                    return false;
                }
    
                this->Image_UVRGBRotate_successful = true;
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceInternalObject::Meta_ResourceInternalObject()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceInternalObject::getId() const
    {
        return 23;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceInternalObject::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Internal_Group ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Internal_Name ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceInternalObject::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceInternalObject::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceInternalObject::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
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
        return 21;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceModel3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 5:
            {
                if( this->read( _buff, _size, _read, this->File_Converter ) == false )
                {
                    return false;
                }
    
                this->File_Converter_successful = true;
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->File_Dataflow ) == false )
                {
                    return false;
                }
    
                this->File_Dataflow_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->Image_Resource ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceModel3D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceModel3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceModel3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceMovie::Meta_ResourceMovie()
        : Meta_Resource()
        , Anchor_Point_successful(false)
        , Bounds_Box_successful(false)
        , KeyFramesPackPath_Codec_successful(false)
        , KeyFramesPackPath_Converter_successful(false)
        , Loop_Segment_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceMovie::getId() const
    {
        return 17;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 11:
            {
                if( this->read( _buff, _size, _read, this->Anchor_Point ) == false )
                {
                    return false;
                }
    
                this->Anchor_Point_successful = true;
    
                return true;
            }break;
        case 12:
            {
                if( this->read( _buff, _size, _read, this->Bounds_Box ) == false )
                {
                    return false;
                }
    
                this->Bounds_Box_successful = true;
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Duration_Value ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->FrameDuration_Value ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->Height_Value ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->KeyFramesPackPath_Codec ) == false )
                {
                    return false;
                }
    
                this->KeyFramesPackPath_Codec_successful = true;
    
                return true;
            }break;
        case 9:
            {
                if( this->read( _buff, _size, _read, this->KeyFramesPackPath_Converter ) == false )
                {
                    return false;
                }
    
                this->KeyFramesPackPath_Converter_successful = true;
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->KeyFramesPackPath_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 10:
            {
                if( this->read( _buff, _size, _read, this->Loop_Segment ) == false )
                {
                    return false;
                }
    
                this->Loop_Segment_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->Width_Value ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 15:
            {
                includes_Meta_MovieCamera3D.reserve( _count );
                return true;
                break;
            }
        case 13:
            {
                includes_Meta_MovieLayer2D.reserve( _count );
                return true;
                break;
            }
        case 14:
            {
                includes_Meta_MovieLayer3D.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 15:
            {
                Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D & metadata = includes_Meta_MovieCamera3D.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        case 13:
            {
                Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D & metadata = includes_Meta_MovieLayer2D.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        case 14:
            {
                Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D & metadata = includes_Meta_MovieLayer3D.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::Meta_MovieCamera3D()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::getId() const
    {
        return 15;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 4:
            {
                if( this->read( _buff, _size, _read, this->CameraAspect ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->CameraFOV ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->CameraInterest ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 1:
            {
                if( this->read( _buff, _size, _read, this->CameraPosition ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->Height ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->Width ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::Meta_MovieLayer2D()
        : Metabuf::Metadata()
        , BlendingMode_successful(false)
        , Params_successful(false)
        , Parent_successful(false)
        , PlayCount_successful(false)
        , Polygon_successful(false)
        , Position_successful(false)
        , Shape_successful(false)
        , StartInterval_successful(false)
        , Stretch_successful(false)
        , Switch_successful(false)
        , TimeRemap_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::getId() const
    {
        return 13;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 9:
            {
                if( this->read( _buff, _size, _read, this->BlendingMode ) == false )
                {
                    return false;
                }
    
                this->BlendingMode_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->In ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Index ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Name ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->Out ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 17:
            {
                if( this->read( _buff, _size, _read, this->Params ) == false )
                {
                    return false;
                }
    
                this->Params_successful = true;
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->Parent ) == false )
                {
                    return false;
                }
    
                this->Parent_successful = true;
    
                return true;
            }break;
        case 13:
            {
                if( this->read( _buff, _size, _read, this->PlayCount ) == false )
                {
                    return false;
                }
    
                this->PlayCount_successful = true;
    
                return true;
            }break;
        case 12:
            {
                if( this->read( _buff, _size, _read, this->Polygon ) == false )
                {
                    return false;
                }
    
                this->Polygon_successful = true;
    
                return true;
            }break;
        case 16:
            {
                if( this->read( _buff, _size, _read, this->Position ) == false )
                {
                    return false;
                }
    
                this->Position_successful = true;
    
                return true;
            }break;
        case 11:
            {
                if( this->read( _buff, _size, _read, this->Shape ) == false )
                {
                    return false;
                }
    
                this->Shape_successful = true;
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Source ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->StartInterval ) == false )
                {
                    return false;
                }
    
                this->StartInterval_successful = true;
    
                return true;
            }break;
        case 14:
            {
                if( this->read( _buff, _size, _read, this->Stretch ) == false )
                {
                    return false;
                }
    
                this->Stretch_successful = true;
    
                return true;
            }break;
        case 15:
            {
                if( this->read( _buff, _size, _read, this->Switch ) == false )
                {
                    return false;
                }
    
                this->Switch_successful = true;
    
                return true;
            }break;
        case 10:
            {
                if( this->read( _buff, _size, _read, this->TimeRemap ) == false )
                {
                    return false;
                }
    
                this->TimeRemap_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Type ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::Meta_MovieLayer3D()
        : Metabuf::Metadata()
        , BlendingMode_successful(false)
        , Params_successful(false)
        , Parent_successful(false)
        , PlayCount_successful(false)
        , Polygon_successful(false)
        , Position_successful(false)
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
        return 14;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 9:
            {
                if( this->read( _buff, _size, _read, this->BlendingMode ) == false )
                {
                    return false;
                }
    
                this->BlendingMode_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->In ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Index ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Name ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->Out ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 17:
            {
                if( this->read( _buff, _size, _read, this->Params ) == false )
                {
                    return false;
                }
    
                this->Params_successful = true;
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->Parent ) == false )
                {
                    return false;
                }
    
                this->Parent_successful = true;
    
                return true;
            }break;
        case 13:
            {
                if( this->read( _buff, _size, _read, this->PlayCount ) == false )
                {
                    return false;
                }
    
                this->PlayCount_successful = true;
    
                return true;
            }break;
        case 12:
            {
                if( this->read( _buff, _size, _read, this->Polygon ) == false )
                {
                    return false;
                }
    
                this->Polygon_successful = true;
    
                return true;
            }break;
        case 16:
            {
                if( this->read( _buff, _size, _read, this->Position ) == false )
                {
                    return false;
                }
    
                this->Position_successful = true;
    
                return true;
            }break;
        case 11:
            {
                if( this->read( _buff, _size, _read, this->Shape ) == false )
                {
                    return false;
                }
    
                this->Shape_successful = true;
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Source ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->StartInterval ) == false )
                {
                    return false;
                }
    
                this->StartInterval_successful = true;
    
                return true;
            }break;
        case 14:
            {
                if( this->read( _buff, _size, _read, this->Stretch ) == false )
                {
                    return false;
                }
    
                this->Stretch_successful = true;
    
                return true;
            }break;
        case 15:
            {
                if( this->read( _buff, _size, _read, this->Switch ) == false )
                {
                    return false;
                }
    
                this->Switch_successful = true;
    
                return true;
            }break;
        case 10:
            {
                if( this->read( _buff, _size, _read, this->TimeRemap ) == false )
                {
                    return false;
                }
    
                this->TimeRemap_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Type ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
        return 10;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceParticle::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 5:
            {
                if( this->read( _buff, _size, _read, this->AtlasCount_Value ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->File_Converter ) == false )
                {
                    return false;
                }
    
                this->File_Converter_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceParticle::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 6:
            {
                includes_Meta_Atlas.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceParticle::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 6:
            {
                Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas & metadata = includes_Meta_Atlas.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceParticle::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::Meta_Atlas()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::getId() const
    {
        return 6;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Index ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->ResourceName ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourcePlaylist::Meta_ResourcePlaylist()
        : Meta_Resource()
        , Loop_Value_successful(false)
        , Shuffle_Value_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourcePlaylist::getId() const
    {
        return 15;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Loop_Value ) == false )
                {
                    return false;
                }
    
                this->Loop_Value_successful = true;
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Shuffle_Value ) == false )
                {
                    return false;
                }
    
                this->Shuffle_Value_successful = true;
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 5:
            {
                includes_Meta_Tracks.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 5:
            {
                Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks & metadata = includes_Meta_Tracks.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Tracks()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::getId() const
    {
        return 5;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 1:
            {
                includes_Meta_Track.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track & metadata = includes_Meta_Track.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::Meta_Track()
        : Metabuf::Metadata()
        , Codec_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::getId() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Codec ) == false )
                {
                    return false;
                }
    
                this->Codec_successful = true;
    
                return true;
            }break;
        case 1:
            {
                if( this->read( _buff, _size, _read, this->File ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_DataBlock::Meta_ResourceShape::Meta_ResourceShape()
        : Meta_Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_DataBlock::Meta_ResourceShape::getId() const
    {
        return 24;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceShape::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Polygon_Value ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceShape::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceShape::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceShape::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
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
        return 14;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceSound::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 6:
            {
                if( this->read( _buff, _size, _read, this->DefaultVolume_Value ) == false )
                {
                    return false;
                }
    
                this->DefaultVolume_Value_successful = true;
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->File_Codec ) == false )
                {
                    return false;
                }
    
                this->File_Codec_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->File_Converter ) == false )
                {
                    return false;
                }
    
                this->File_Converter_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->IsStreamable_Value ) == false )
                {
                    return false;
                }
    
                this->IsStreamable_Value_successful = true;
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceSound::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceSound::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceSound::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
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
        return 18;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceVideo::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 4:
            {
                if( this->read( _buff, _size, _read, this->File_Alpha ) == false )
                {
                    return false;
                }
    
                this->File_Alpha_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->File_Codec ) == false )
                {
                    return false;
                }
    
                this->File_Codec_successful = true;
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->File_Converter ) == false )
                {
                    return false;
                }
    
                this->File_Converter_successful = true;
    
                return true;
            }break;
        case 9:
            {
                if( this->read( _buff, _size, _read, this->File_Duration ) == false )
                {
                    return false;
                }
    
                this->File_Duration_successful = true;
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->File_FrameRate ) == false )
                {
                    return false;
                }
    
                this->File_FrameRate_successful = true;
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->File_NoSeek ) == false )
                {
                    return false;
                }
    
                this->File_NoSeek_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceVideo::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceVideo::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceVideo::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
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
        return 16;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceWindow::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->WindowBackground_ResourceImageName ) == false )
                {
                    return false;
                }
    
                this->WindowBackground_ResourceImageName_successful = true;
    
                return true;
            }break;
        case 17:
            {
                if( this->read( _buff, _size, _read, this->WindowBottom_Offset ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 9:
            {
                if( this->read( _buff, _size, _read, this->WindowBottom_ResourceImageName ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 19:
            {
                if( this->read( _buff, _size, _read, this->WindowLeft_Offset ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 11:
            {
                if( this->read( _buff, _size, _read, this->WindowLeft_ResourceImageName ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 18:
            {
                if( this->read( _buff, _size, _read, this->WindowLeftBottom_Offset ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 10:
            {
                if( this->read( _buff, _size, _read, this->WindowLeftBottom_ResourceImageName ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 12:
            {
                if( this->read( _buff, _size, _read, this->WindowLeftTop_Offset ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->WindowLeftTop_ResourceImageName ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 15:
            {
                if( this->read( _buff, _size, _read, this->WindowRight_Offset ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->WindowRight_ResourceImageName ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 16:
            {
                if( this->read( _buff, _size, _read, this->WindowRightBottom_Offset ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->WindowRightBottom_ResourceImageName ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 14:
            {
                if( this->read( _buff, _size, _read, this->WindowRightTop_Offset ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->WindowRightTop_ResourceImageName ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 13:
            {
                if( this->read( _buff, _size, _read, this->WindowTop_Offset ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->WindowTop_ResourceImageName ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceWindow::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceWindow::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceWindow::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Font::Meta_Font()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Font::getId() const
    {
        return 3;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->height ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 2:
            {
                includes_Meta_Char.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_Font::Meta_Char & metadata = includes_Meta_Char.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Font::Meta_Char::Meta_Char()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Font::Meta_Char::getId() const
    {
        return 2;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 4:
            {
                if( this->read( _buff, _size, _read, this->code ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->offset ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->rect ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 1:
            {
                if( this->read( _buff, _size, _read, this->width ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 5:
            {
                includes_Meta_Kerning.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 5:
            {
                Meta_Font::Meta_Char::Meta_Kerning & metadata = includes_Meta_Kerning.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Font::Meta_Char::Meta_Kerning::Meta_Kerning()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Font::Meta_Char::Meta_Kerning::getId() const
    {
        return 5;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::Meta_Kerning::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->advance ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->id ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::Meta_Kerning::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::Meta_Kerning::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::Meta_Kerning::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_KeyFramesPack()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::getId() const
    {
        return 5;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->MaxIndex ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 4:
            {
                includes_Meta_ImageShape.reserve( _count );
                return true;
                break;
            }
        case 5:
            {
                includes_Meta_KeyFrames2D.reserve( _count );
                return true;
                break;
            }
        case 6:
            {
                includes_Meta_KeyFrames3D.reserve( _count );
                return true;
                break;
            }
        case 3:
            {
                includes_Meta_Polygon.reserve( _count );
                return true;
                break;
            }
        case 2:
            {
                includes_Meta_TimeRemap.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 4:
            {
                Meta_KeyFramesPack::Meta_ImageShape & metadata = includes_Meta_ImageShape.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        case 5:
            {
                Meta_KeyFramesPack::Meta_KeyFrames2D & metadata = includes_Meta_KeyFrames2D.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        case 6:
            {
                Meta_KeyFramesPack::Meta_KeyFrames3D & metadata = includes_Meta_KeyFrames3D.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        case 3:
            {
                Meta_KeyFramesPack::Meta_Polygon & metadata = includes_Meta_Polygon.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        case 2:
            {
                Meta_KeyFramesPack::Meta_TimeRemap & metadata = includes_Meta_TimeRemap.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_ImageShape::Meta_ImageShape()
        : Metabuf::Metadata()
        , Count_successful(false)
        , Immutable_successful(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_ImageShape::getId() const
    {
        return 4;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_ImageShape::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 5:
            {
                if( this->read( _buff, _size, _read, this->Count ) == false )
                {
                    return false;
                }
    
                this->Count_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Height ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Immutable ) == false )
                {
                    return false;
                }
    
                this->Immutable_successful = true;
    
                return true;
            }break;
        case 1:
            {
                if( this->read( _buff, _size, _read, this->LayerIndex ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Width ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_ImageShape::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 6:
            {
                includes_Meta_Shape.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_ImageShape::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 6:
            {
                Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape & metadata = includes_Meta_Shape.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_ImageShape::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::Meta_Shape()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::getId() const
    {
        return 6;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Polygon ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
        return 5;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Count ) == false )
                {
                    return false;
                }
    
                this->Count_successful = true;
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Immutable ) == false )
                {
                    return false;
                }
    
                this->Immutable_successful = true;
    
                return true;
            }break;
        case 1:
            {
                if( this->read( _buff, _size, _read, this->LayerIndex ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 4:
            {
                includes_Meta_KeyFrame2D.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 4:
            {
                Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & metadata = includes_Meta_KeyFrame2D.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->AnchorPoint ) == false )
                {
                    return false;
                }
    
                this->AnchorPoint_successful = true;
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->Count ) == false )
                {
                    return false;
                }
    
                this->Count_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->Opacity ) == false )
                {
                    return false;
                }
    
                this->Opacity_successful = true;
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Position ) == false )
                {
                    return false;
                }
    
                this->Position_successful = true;
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Rotation ) == false )
                {
                    return false;
                }
    
                this->Rotation_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Scale ) == false )
                {
                    return false;
                }
    
                this->Scale_successful = true;
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->Volume ) == false )
                {
                    return false;
                }
    
                this->Volume_successful = true;
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
        return 6;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Count ) == false )
                {
                    return false;
                }
    
                this->Count_successful = true;
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Immutable ) == false )
                {
                    return false;
                }
    
                this->Immutable_successful = true;
    
                return true;
            }break;
        case 1:
            {
                if( this->read( _buff, _size, _read, this->LayerIndex ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 4:
            {
                includes_Meta_KeyFrame3D.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 4:
            {
                Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D & metadata = includes_Meta_KeyFrame3D.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->AnchorPoint ) == false )
                {
                    return false;
                }
    
                this->AnchorPoint_successful = true;
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->Count ) == false )
                {
                    return false;
                }
    
                this->Count_successful = true;
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->Opacity ) == false )
                {
                    return false;
                }
    
                this->Opacity_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->Orientation ) == false )
                {
                    return false;
                }
    
                this->Orientation_successful = true;
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Position ) == false )
                {
                    return false;
                }
    
                this->Position_successful = true;
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Rotation ) == false )
                {
                    return false;
                }
    
                this->Rotation_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Scale ) == false )
                {
                    return false;
                }
    
                this->Scale_successful = true;
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->Volume ) == false )
                {
                    return false;
                }
    
                this->Volume_successful = true;
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_Polygon::Meta_Polygon()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_Polygon::getId() const
    {
        return 3;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_Polygon::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->LayerIndex ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Value ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_Polygon::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_Polygon::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_Polygon::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_KeyFramesPack::Meta_TimeRemap::Meta_TimeRemap()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_KeyFramesPack::Meta_TimeRemap::getId() const
    {
        return 2;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_TimeRemap::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->LayerIndex ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Time ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_TimeRemap::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_TimeRemap::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_TimeRemap::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
    bool Meta_Pak::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 5:
            {
                includes_Meta_Datas.reserve( _count );
                return true;
                break;
            }
        case 2:
            {
                includes_Meta_Fonts.reserve( _count );
                return true;
                break;
            }
        case 3:
            {
                includes_Meta_Resources.reserve( _count );
                return true;
                break;
            }
        case 1:
            {
                includes_Meta_Scripts.reserve( _count );
                return true;
                break;
            }
        case 4:
            {
                includes_Meta_Texts.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 5:
            {
                Meta_Pak::Meta_Datas & metadata = includes_Meta_Datas.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        case 2:
            {
                Meta_Pak::Meta_Fonts & metadata = includes_Meta_Fonts.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        case 3:
            {
                Meta_Pak::Meta_Resources & metadata = includes_Meta_Resources.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        case 1:
            {
                Meta_Pak::Meta_Scripts & metadata = includes_Meta_Scripts.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        case 4:
            {
                Meta_Pak::Meta_Texts & metadata = includes_Meta_Texts.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
    bool Meta_Pak::Meta_Datas::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Datas::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 1:
            {
                includes_Meta_Data.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Datas::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Pak::Meta_Datas::Meta_Data & metadata = includes_Meta_Data.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Datas::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
    bool Meta_Pak::Meta_Datas::Meta_Data::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Name ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Datas::Meta_Data::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Datas::Meta_Data::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Datas::Meta_Data::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
    bool Meta_Pak::Meta_Fonts::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Fonts::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Fonts::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Fonts::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Resources::Meta_Resources()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Resources::getId() const
    {
        return 3;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 1:
            {
                includes_Meta_Resource.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Pak::Meta_Resources::Meta_Resource & metadata = includes_Meta_Resource.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Resources::Meta_Resource::Meta_Resource()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Pak::Meta_Resources::Meta_Resource::getId() const
    {
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::Meta_Resource::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::Meta_Resource::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::Meta_Resource::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::Meta_Resource::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Pak::Meta_Scripts::Meta_Scripts()
        : Metabuf::Metadata()
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
    bool Meta_Pak::Meta_Scripts::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Initializer ) == false )
                {
                    return false;
                }
    
                this->Initializer_successful = true;
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Module ) == false )
                {
                    return false;
                }
    
                this->Module_successful = true;
    
                return true;
            }break;
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Scripts::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Scripts::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Scripts::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
    bool Meta_Pak::Meta_Texts::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 1:
            {
                includes_Meta_Text.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Pak::Meta_Texts::Meta_Text & metadata = includes_Meta_Text.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
    bool Meta_Pak::Meta_Texts::Meta_Text::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::Meta_Text::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::Meta_Text::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::Meta_Text::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Meta_Texts::Meta_Texts()
        : Metabuf::Metadata()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Meta_Texts::getId() const
    {
        return 4;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        switch( _includes )
        {
        case 1:
            {
                includes_Meta_Text.reserve( _count );
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Texts::Meta_Text & metadata = includes_Meta_Text.emplace_back();
    
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                return true;
                break;
            }
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
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
    bool Meta_Texts::Meta_Text::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _id )
    {
        switch( _id )
        {
        case 4:
            {
                if( this->read( _buff, _size, _read, this->CharOffset ) == false )
                {
                    return false;
                }
    
                this->CharOffset_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Font ) == false )
                {
                    return false;
                }
    
                this->Font_successful = true;
    
                return true;
            }break;
        case 1:
            {
                if( this->read( _buff, _size, _read, this->Key ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->LineOffset ) == false )
                {
                    return false;
                }
    
                this->LineOffset_successful = true;
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Value ) == false )
                {
                    return false;
                }
    
                this->Value_successful = true;
    
                return true;
            }break;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::Meta_Text::_preparationIncludes( uint32_t _includes, uint32_t _count )
    {
        (void)_includes;
        (void)_count;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::Meta_Text::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::Meta_Text::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, uint32_t _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
    
        return false;
    }
}
