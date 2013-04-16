#   include "Metacode.h"

namespace Metacode
{
    bool readHeader( const unsigned char * _buff, size_t _size, size_t & _read, size_t & _readVersion, size_t & _needVersion, void * _userData )
    {
       Metabuf::ArchiveReader ar(_buff, _size, _read, _userData);

       unsigned int head;
       ar.read( head );

       if( head != 3133062829 )
       {
           return false;
       }

       unsigned int version;
       ar.read( version );

       _readVersion = version;
       _needVersion = 8;

       if( version != 8 )
       {
           return false;
       }

       return true;
    }

    bool readStrings( const unsigned char * _buff, size_t _size, size_t & _read, size_t & _stringCount, void * _userData )
    {
       Metabuf::ArchiveReader ar(_buff, _size, _read, _userData);

       unsigned int count;
       ar.read( count );

       _stringCount = count;

       return true;
    }

    const char * readString( const unsigned char * _buff, size_t _size, size_t & _read, size_t & _stringSize, void * _userData )
    {
       Metabuf::ArchiveReader ar(_buff, _size, _read, _userData);

       unsigned int size;
       ar.readSize( size );

       const char * value = ar.current_buff<const char *>();
       ar.skip( size );

       _stringSize = size;

       return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        switch( _includes )
        {
        case 2:
            {
                includes_Meta_Resource.reserve( _count );
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        switch( _generators )
        {
        case 14:
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
        case 8:
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
        case 7:
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
        case 4:
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
        case 21:
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
        case 5:
            {
                Meta_DataBlock::Meta_ResourceFont * metadata = new Meta_DataBlock::Meta_ResourceFont ();
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
                Meta_DataBlock::Meta_ResourceGlyph * metadata = new Meta_DataBlock::Meta_ResourceGlyph ();
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
        case 18:
            {
                Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha * metadata = new Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha ();
                if( metadata->parse( _buff, _size, _read, m_userData ) == false )
                {
                    delete metadata;
    
                    return false;
                }
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 3:
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
        case 19:
            {
                Meta_DataBlock::Meta_ResourceImageMultiplyRGBAndAlpha * metadata = new Meta_DataBlock::Meta_ResourceImageMultiplyRGBAndAlpha ();
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
        case 20:
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
        case 17:
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
        case 12:
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
        case 9:
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
        case 13:
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
        case 11:
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
    bool Meta_DataBlock::Meta_Resource::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_Resource::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Resource::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Resource::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::_preparationIncludes( unsigned int _includes, unsigned int _count )
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
    bool Meta_DataBlock::Meta_ResourceAnimation::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 3:
            {
                Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_Sequence.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorICO::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourceCursorICO::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorICO::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorICO::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceExternal::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->External_Name ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceExternal::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceExternal::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceExternal::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFont::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 8:
            {
                if( this->read( _buff, _size, _read, this->Color_Value ) == false )
                {
                    return false;
                }
    
                this->Color_Value_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->Image_Codec ) == false )
                {
                    return false;
                }
    
                this->Image_Codec_successful = true;
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Image_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->OutlineImage_Codec ) == false )
                {
                    return false;
                }
    
                this->OutlineImage_Codec_successful = true;
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->OutlineImage_Path ) == false )
                {
                    return false;
                }
    
                this->OutlineImage_Path_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->ResourceGlyph_Name ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFont::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFont::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFont::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceGlyph::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->GlyphPath_Path ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceGlyph::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceGlyph::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceGlyph::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceHIT::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourceHIT::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceHIT::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceHIT::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 7:
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
                if( this->read( _buff, _size, _read, this->File_CodecAlpha ) == false )
                {
                    return false;
                }
    
                this->File_CodecAlpha_successful = true;
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->File_CodecRGB ) == false )
                {
                    return false;
                }
    
                this->File_CodecRGB_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->File_PathAlpha ) == false )
                {
                    return false;
                }
    
                this->File_PathAlpha_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->File_PathRGB ) == false )
                {
                    return false;
                }
    
                this->File_PathRGB_successful = true;
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->File_UV ) == false )
                {
                    return false;
                }
    
                this->File_UV_successful = true;
    
                return true;
            }break;
        case 9:
            {
                if( this->read( _buff, _size, _read, this->File_WrapX ) == false )
                {
                    return false;
                }
    
                this->File_WrapX_successful = true;
    
                return true;
            }break;
        case 10:
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
    bool Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageDefault::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 6:
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
                if( this->read( _buff, _size, _read, this->File_Rotate ) == false )
                {
                    return false;
                }
    
                this->File_Rotate_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->File_UV ) == false )
                {
                    return false;
                }
    
                this->File_UV_successful = true;
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->File_WrapX ) == false )
                {
                    return false;
                }
    
                this->File_WrapX_successful = true;
    
                return true;
            }break;
        case 9:
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
    bool Meta_DataBlock::Meta_ResourceImageDefault::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageDefault::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageDefault::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageMultiplyRGBAndAlpha::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 7:
            {
                if( this->read( _buff, _size, _read, this->FileAlpha_Codec ) == false )
                {
                    return false;
                }
    
                this->FileAlpha_Codec_successful = true;
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->FileAlpha_Path ) == false )
                {
                    return false;
                }
    
                this->FileAlpha_Path_successful = true;
    
                return true;
            }break;
        case 8:
            {
                if( this->read( _buff, _size, _read, this->FileAlpha_UV ) == false )
                {
                    return false;
                }
    
                this->FileAlpha_UV_successful = true;
    
                return true;
            }break;
        case 4:
            {
                if( this->read( _buff, _size, _read, this->FileRGB_Codec ) == false )
                {
                    return false;
                }
    
                this->FileRGB_Codec_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->FileRGB_Path ) == false )
                {
                    return false;
                }
    
                this->FileRGB_Path_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->FileRGB_UV ) == false )
                {
                    return false;
                }
    
                this->FileRGB_UV_successful = true;
    
                return true;
            }break;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageMultiplyRGBAndAlpha::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageMultiplyRGBAndAlpha::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageMultiplyRGBAndAlpha::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSolid::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourceImageSolid::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSolid::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSolid::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSubstract::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                if( this->read( _buff, _size, _read, this->Image_Name ) == false )
                {
                    return false;
                }
    
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
    bool Meta_DataBlock::Meta_ResourceImageSubstract::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSubstract::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSubstract::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceInternalObject::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourceInternalObject::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceInternalObject::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceInternalObject::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 4:
            {
                if( this->read( _buff, _size, _read, this->Duration_Value ) == false )
                {
                    return false;
                }
    
                this->Duration_Value_successful = true;
    
                return true;
            }break;
        case 3:
            {
                if( this->read( _buff, _size, _read, this->FrameDuration_Value ) == false )
                {
                    return false;
                }
    
                this->FrameDuration_Value_successful = true;
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->Height_Value ) == false )
                {
                    return false;
                }
    
                this->Height_Value_successful = true;
    
                return true;
            }break;
        case 7:
            {
                if( this->read( _buff, _size, _read, this->KeyFramesPackPath_Path ) == false )
                {
                    return false;
                }
    
                this->KeyFramesPackPath_Path_successful = true;
    
                return true;
            }break;
        case 5:
            {
                if( this->read( _buff, _size, _read, this->Width_Value ) == false )
                {
                    return false;
                }
    
                this->Width_Value_successful = true;
    
                return true;
            }break;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 10:
            {
                includes_Meta_MovieCamera3D.reserve( _count );
                return true;
                break;
            }
        case 8:
            {
                includes_Meta_MovieLayer2D.reserve( _count );
                return true;
                break;
            }
        case 9:
            {
                includes_Meta_MovieLayer3D.reserve( _count );
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 10:
            {
                Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_MovieCamera3D.push_back(metadata);
                return true;
                break;
            }
        case 8:
            {
                Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_MovieLayer2D.push_back(metadata);
                return true;
                break;
            }
        case 9:
            {
                Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_MovieLayer3D.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        switch( _id )
        {
        case 9:
            {
                if( this->read( _buff, _size, _read, this->BlendingMode ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->In ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 5:
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
        case 7:
            {
                if( this->read( _buff, _size, _read, this->Out ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Parent ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 10:
            {
                if( this->read( _buff, _size, _read, this->PlayCount ) == false )
                {
                    return false;
                }
    
                this->PlayCount_successful = true;
    
                return true;
            }break;
        case 3:
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
    
                return true;
            }break;
        case 4:
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
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        switch( _id )
        {
        case 9:
            {
                if( this->read( _buff, _size, _read, this->BlendingMode ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 6:
            {
                if( this->read( _buff, _size, _read, this->In ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 5:
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
        case 7:
            {
                if( this->read( _buff, _size, _read, this->Out ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 2:
            {
                if( this->read( _buff, _size, _read, this->Parent ) == false )
                {
                    return false;
                }
    
                return true;
            }break;
        case 3:
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
    
                return true;
            }break;
        case 4:
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
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourcePlaylist::_preparationIncludes( unsigned int _includes, unsigned int _count )
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
    bool Meta_DataBlock::Meta_ResourcePlaylist::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
    
        switch( _includes )
        {
        case 5:
            {
                Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_Tracks.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_preparationIncludes( unsigned int _includes, unsigned int _count )
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
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_Track.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceSound::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourceSound::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceSound::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceSound::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceVideo::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourceVideo::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceVideo::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceVideo::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceWindow::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_DataBlock::Meta_ResourceWindow::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceWindow::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceWindow::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_Font::_preparationIncludes( unsigned int _includes, unsigned int _count )
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
    bool Meta_Font::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_Font::Meta_Char metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_Char.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_Font::Meta_Char::_preparationIncludes( unsigned int _includes, unsigned int _count )
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
    bool Meta_Font::Meta_Char::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        switch( _includes )
        {
        case 5:
            {
                Meta_Font::Meta_Char::Meta_Kerning metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_Kerning.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::Meta_Kerning::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_Font::Meta_Char::Meta_Kerning::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::Meta_Kerning::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::Meta_Kerning::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_KeyFramesPack::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        switch( _includes )
        {
        case 2:
            {
                includes_Meta_KeyFrames2D.reserve( _count );
                return true;
                break;
            }
        case 3:
            {
                includes_Meta_KeyFrames3D.reserve( _count );
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_KeyFramesPack::Meta_KeyFrames2D metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_KeyFrames2D.push_back(metadata);
                return true;
                break;
            }
        case 3:
            {
                Meta_KeyFramesPack::Meta_KeyFrames3D metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_KeyFrames3D.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_preparationIncludes( unsigned int _includes, unsigned int _count )
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
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        switch( _includes )
        {
        case 4:
            {
                Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_KeyFrame2D.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        switch( _includes )
        {
        case 2:
            {
                includes_Meta_KeyFrame3D.reserve( _count );
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_KeyFrame3D.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        switch( _includes )
        {
        case 2:
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
        case 3:
            {
                includes_Meta_Texts.reserve( _count );
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_Pak::Meta_Resources metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_Resources.push_back(metadata);
                return true;
                break;
            }
        case 1:
            {
                Meta_Pak::Meta_Scripts metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_Scripts.push_back(metadata);
                return true;
                break;
            }
        case 3:
            {
                Meta_Pak::Meta_Texts metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_Texts.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::_preparationIncludes( unsigned int _includes, unsigned int _count )
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
    bool Meta_Pak::Meta_Resources::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Pak::Meta_Resources::Meta_Resource metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
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
    bool Meta_Pak::Meta_Resources::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::Meta_Resource::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_Pak::Meta_Resources::Meta_Resource::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::Meta_Resource::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::Meta_Resource::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Scripts::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_Pak::Meta_Scripts::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Scripts::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Scripts::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::_preparationIncludes( unsigned int _includes, unsigned int _count )
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
    bool Meta_Pak::Meta_Texts::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Pak::Meta_Texts::Meta_Text metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_Text.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::Meta_Text::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_Pak::Meta_Texts::Meta_Text::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::Meta_Text::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::Meta_Text::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_id;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::_preparationIncludes( unsigned int _includes, unsigned int _count )
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
    bool Meta_Texts::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Texts::Meta_Text metadata;
                if( metadata.parse( _buff, _size, _read, m_userData ) == false )
                {
                    return false;
                }
    
                includes_Meta_Text.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::Meta_Text::_parseArguments( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _id )
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
    bool Meta_Texts::Meta_Text::_preparationIncludes( unsigned int _includes, unsigned int _count )
    {
        (void)_includes;
        (void)_count;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::Meta_Text::_parseIncludes( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _includes )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_includes;
    
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::Meta_Text::_parseGenerators( const unsigned char * _buff, size_t _size, size_t & _read, unsigned int _generators )
    {
        (void)_buff;
        (void)_size;
        (void)_read;
        (void)_generators;
    
        return false;
    }
}
