#   include "Metacode.h"

namespace Metacode
{
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::_preparationIncludes( size_t _includes, size_t _count )
    {
        switch( _includes )
        {
        case 1:
            {
                includes_Meta_FileStore.reserve( _count );
                return true;
                break;
            }
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
    bool Meta_DataBlock::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_DataBlock::Meta_FileStore metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_FileStore.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        switch( _generators )
        {
        case 14:
            {
                Meta_DataBlock::Meta_ResourceAnimation * metadata = new Meta_DataBlock::Meta_ResourceAnimation ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 8:
            {
                Meta_DataBlock::Meta_ResourceCursorICO * metadata = new Meta_DataBlock::Meta_ResourceCursorICO ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 7:
            {
                Meta_DataBlock::Meta_ResourceCursorSystem * metadata = new Meta_DataBlock::Meta_ResourceCursorSystem ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 4:
            {
                Meta_DataBlock::Meta_ResourceEmitterContainer * metadata = new Meta_DataBlock::Meta_ResourceEmitterContainer ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 5:
            {
                Meta_DataBlock::Meta_ResourceFont * metadata = new Meta_DataBlock::Meta_ResourceFont ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 6:
            {
                Meta_DataBlock::Meta_ResourceGlyph * metadata = new Meta_DataBlock::Meta_ResourceGlyph ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 17:
            {
                Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha * metadata = new Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 3:
            {
                Meta_DataBlock::Meta_ResourceImageDefault * metadata = new Meta_DataBlock::Meta_ResourceImageDefault ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 15:
            {
                Meta_DataBlock::Meta_ResourceImageSolid * metadata = new Meta_DataBlock::Meta_ResourceImageSolid ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 16:
            {
                Meta_DataBlock::Meta_ResourceInternalObject * metadata = new Meta_DataBlock::Meta_ResourceInternalObject ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 12:
            {
                Meta_DataBlock::Meta_ResourceMovie * metadata = new Meta_DataBlock::Meta_ResourceMovie ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 10:
            {
                Meta_DataBlock::Meta_ResourcePlaylist * metadata = new Meta_DataBlock::Meta_ResourcePlaylist ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 9:
            {
                Meta_DataBlock::Meta_ResourceSound * metadata = new Meta_DataBlock::Meta_ResourceSound ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 13:
            {
                Meta_DataBlock::Meta_ResourceVideo * metadata = new Meta_DataBlock::Meta_ResourceVideo ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        case 11:
            {
                Meta_DataBlock::Meta_ResourceWindow * metadata = new Meta_DataBlock::Meta_ResourceWindow ();
                metadata->parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_FileStore::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->Count );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_FileStore::_preparationIncludes( size_t _includes, size_t _count )
    {
        switch( _includes )
        {
        case 2:
            {
                includes_Meta_File.reserve( _count );
                return true;
                break;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_FileStore::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_DataBlock::Meta_FileStore::Meta_File metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_File.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_FileStore::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_FileStore::Meta_File::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 3:
            {
                this->read( _buff, _size, _read, this->ID );
                this->ID_successful = true;
                return true;
                break;
            }
        case 1:
            {
                this->read( _buff, _size, _read, this->Index );
                return true;
                break;
            }
        case 2:
            {
                this->read( _buff, _size, _read, this->Path );
                this->Path_successful = true;
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_FileStore::Meta_File::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_FileStore::Meta_File::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_FileStore::Meta_File::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Resource::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->Name );
                return true;
                break;
            }
        case 2:
            {
                this->read( _buff, _size, _read, this->Type );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Resource::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Resource::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_Resource::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_DataBlock::Meta_ResourceAnimation::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
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
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_Sequence.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 2:
            {
                this->read( _buff, _size, _read, this->Delay );
                return true;
                break;
            }
        case 1:
            {
                this->read( _buff, _size, _read, this->ResourceImageName );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceAnimation::Meta_Sequence::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorICO::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                this->read( _buff, _size, _read, this->File_Path );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorICO::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorICO::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorICO::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                this->read( _buff, _size, _read, this->File_Path );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceCursorSystem::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                this->read( _buff, _size, _read, this->File_Path );
                return true;
                break;
            }
        case 4:
            {
                this->read( _buff, _size, _read, this->Folder_Path );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceEmitterContainer::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFont::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 8:
            {
                this->read( _buff, _size, _read, this->Color_Value );
                this->Color_Value_successful = true;
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->Image_Codec );
                this->Image_Codec_successful = true;
                return true;
                break;
            }
        case 4:
            {
                this->read( _buff, _size, _read, this->Image_Path );
                return true;
                break;
            }
        case 7:
            {
                this->read( _buff, _size, _read, this->OutlineImage_Codec );
                this->OutlineImage_Codec_successful = true;
                return true;
                break;
            }
        case 6:
            {
                this->read( _buff, _size, _read, this->OutlineImage_Path );
                this->OutlineImage_Path_successful = true;
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->ResourceGlyph_Name );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFont::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFont::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceFont::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceGlyph::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                this->read( _buff, _size, _read, this->GlyphPath_Path );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceGlyph::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceGlyph::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceGlyph::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 7:
            {
                this->read( _buff, _size, _read, this->File_Alpha );
                this->File_Alpha_successful = true;
                return true;
                break;
            }
        case 4:
            {
                this->read( _buff, _size, _read, this->File_CodecAlpha );
                this->File_CodecAlpha_successful = true;
                return true;
                break;
            }
        case 6:
            {
                this->read( _buff, _size, _read, this->File_CodecRGB );
                this->File_CodecRGB_successful = true;
                return true;
                break;
            }
        case 8:
            {
                this->read( _buff, _size, _read, this->File_MaxSize );
                this->File_MaxSize_successful = true;
                return true;
                break;
            }
        case 11:
            {
                this->read( _buff, _size, _read, this->File_Offset );
                this->File_Offset_successful = true;
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->File_PathAlpha );
                this->File_PathAlpha_successful = true;
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->File_PathRGB );
                this->File_PathRGB_successful = true;
                return true;
                break;
            }
        case 9:
            {
                this->read( _buff, _size, _read, this->File_Size );
                this->File_Size_successful = true;
                return true;
                break;
            }
        case 10:
            {
                this->read( _buff, _size, _read, this->File_UV );
                this->File_UV_successful = true;
                return true;
                break;
            }
        case 12:
            {
                this->read( _buff, _size, _read, this->File_WrapX );
                this->File_WrapX_successful = true;
                return true;
                break;
            }
        case 13:
            {
                this->read( _buff, _size, _read, this->File_WrapY );
                this->File_WrapY_successful = true;
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageDefault::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 8:
            {
                this->read( _buff, _size, _read, this->File_Alpha );
                this->File_Alpha_successful = true;
                return true;
                break;
            }
        case 4:
            {
                this->read( _buff, _size, _read, this->File_Codec );
                this->File_Codec_successful = true;
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->File_MaxSize );
                this->File_MaxSize_successful = true;
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->File_Path );
                return true;
                break;
            }
        case 6:
            {
                this->read( _buff, _size, _read, this->File_Size );
                this->File_Size_successful = true;
                return true;
                break;
            }
        case 7:
            {
                this->read( _buff, _size, _read, this->File_UV );
                this->File_UV_successful = true;
                return true;
                break;
            }
        case 9:
            {
                this->read( _buff, _size, _read, this->File_WrapX );
                this->File_WrapX_successful = true;
                return true;
                break;
            }
        case 10:
            {
                this->read( _buff, _size, _read, this->File_WrapY );
                this->File_WrapY_successful = true;
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageDefault::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageDefault::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageDefault::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSolid::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->Color_Value );
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->Size_Value );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSolid::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSolid::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceImageSolid::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceInternalObject::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->Internal_Group );
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->Internal_Name );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceInternalObject::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceInternalObject::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceInternalObject::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->Duration_Value );
                this->Duration_Value_successful = true;
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->FrameDuration_Value );
                this->FrameDuration_Value_successful = true;
                return true;
                break;
            }
        case 6:
            {
                this->read( _buff, _size, _read, this->Height_Value );
                this->Height_Value_successful = true;
                return true;
                break;
            }
        case 7:
            {
                this->read( _buff, _size, _read, this->KeyFramesPackPath_Path );
                this->KeyFramesPackPath_Path_successful = true;
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->Width_Value );
                this->Width_Value_successful = true;
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_DataBlock::Meta_ResourceMovie::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
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
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_MovieCamera3D.push_back(metadata);
                return true;
                break;
            }
        case 8:
            {
                Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_MovieLayer2D.push_back(metadata);
                return true;
                break;
            }
        case 9:
            {
                Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_MovieLayer3D.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->CameraAspect );
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->CameraFOV );
                return true;
                break;
            }
        case 2:
            {
                this->read( _buff, _size, _read, this->CameraInterest );
                return true;
                break;
            }
        case 1:
            {
                this->read( _buff, _size, _read, this->CameraPosition );
                return true;
                break;
            }
        case 6:
            {
                this->read( _buff, _size, _read, this->Height );
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->Width );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 9:
            {
                this->read( _buff, _size, _read, this->BlendingMode );
                return true;
                break;
            }
        case 6:
            {
                this->read( _buff, _size, _read, this->In );
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->Index );
                return true;
                break;
            }
        case 1:
            {
                this->read( _buff, _size, _read, this->Name );
                return true;
                break;
            }
        case 7:
            {
                this->read( _buff, _size, _read, this->Out );
                return true;
                break;
            }
        case 2:
            {
                this->read( _buff, _size, _read, this->Parent );
                return true;
                break;
            }
        case 10:
            {
                this->read( _buff, _size, _read, this->PlayCount );
                this->PlayCount_successful = true;
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->Source );
                return true;
                break;
            }
        case 8:
            {
                this->read( _buff, _size, _read, this->StartInterval );
                return true;
                break;
            }
        case 4:
            {
                this->read( _buff, _size, _read, this->Type );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 9:
            {
                this->read( _buff, _size, _read, this->BlendingMode );
                return true;
                break;
            }
        case 6:
            {
                this->read( _buff, _size, _read, this->In );
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->Index );
                return true;
                break;
            }
        case 1:
            {
                this->read( _buff, _size, _read, this->Name );
                return true;
                break;
            }
        case 7:
            {
                this->read( _buff, _size, _read, this->Out );
                return true;
                break;
            }
        case 2:
            {
                this->read( _buff, _size, _read, this->Parent );
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->Source );
                return true;
                break;
            }
        case 8:
            {
                this->read( _buff, _size, _read, this->StartInterval );
                return true;
                break;
            }
        case 4:
            {
                this->read( _buff, _size, _read, this->Type );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                this->read( _buff, _size, _read, this->Loop_Value );
                this->Loop_Value_successful = true;
                return true;
                break;
            }
        case 4:
            {
                this->read( _buff, _size, _read, this->Shuffle_Value );
                this->Shuffle_Value_successful = true;
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_DataBlock::Meta_ResourcePlaylist::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
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
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_Tracks.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
    
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_Track.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 2:
            {
                this->read( _buff, _size, _read, this->Codec );
                this->Codec_successful = true;
                return true;
                break;
            }
        case 1:
            {
                this->read( _buff, _size, _read, this->File );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceSound::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 6:
            {
                this->read( _buff, _size, _read, this->DefaultVolume_Value );
                this->DefaultVolume_Value_successful = true;
                return true;
                break;
            }
        case 4:
            {
                this->read( _buff, _size, _read, this->File_Codec );
                this->File_Codec_successful = true;
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->File_Converter );
                this->File_Converter_successful = true;
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->File_Path );
                return true;
                break;
            }
        case 7:
            {
                this->read( _buff, _size, _read, this->IsStreamable_Value );
                this->IsStreamable_Value_successful = true;
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceSound::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceSound::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceSound::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceVideo::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->File_Alpha );
                this->File_Alpha_successful = true;
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->File_Codec );
                this->File_Codec_successful = true;
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->File_Path );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceVideo::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceVideo::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceVideo::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceWindow::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        if( Meta_DataBlock::Meta_Resource::_parseArguments( _buff, _size, _read, _id ) == true )
        {
            return true;
        }
    
        switch( _id )
        {
        case 3:
            {
                this->read( _buff, _size, _read, this->WindowBackground_ResourceImageName );
                this->WindowBackground_ResourceImageName_successful = true;
                return true;
                break;
            }
        case 17:
            {
                this->read( _buff, _size, _read, this->WindowBottom_Offset );
                return true;
                break;
            }
        case 9:
            {
                this->read( _buff, _size, _read, this->WindowBottom_ResourceImageName );
                return true;
                break;
            }
        case 19:
            {
                this->read( _buff, _size, _read, this->WindowLeft_Offset );
                return true;
                break;
            }
        case 11:
            {
                this->read( _buff, _size, _read, this->WindowLeft_ResourceImageName );
                return true;
                break;
            }
        case 18:
            {
                this->read( _buff, _size, _read, this->WindowLeftBottom_Offset );
                return true;
                break;
            }
        case 10:
            {
                this->read( _buff, _size, _read, this->WindowLeftBottom_ResourceImageName );
                return true;
                break;
            }
        case 12:
            {
                this->read( _buff, _size, _read, this->WindowLeftTop_Offset );
                return true;
                break;
            }
        case 4:
            {
                this->read( _buff, _size, _read, this->WindowLeftTop_ResourceImageName );
                return true;
                break;
            }
        case 15:
            {
                this->read( _buff, _size, _read, this->WindowRight_Offset );
                return true;
                break;
            }
        case 7:
            {
                this->read( _buff, _size, _read, this->WindowRight_ResourceImageName );
                return true;
                break;
            }
        case 16:
            {
                this->read( _buff, _size, _read, this->WindowRightBottom_Offset );
                return true;
                break;
            }
        case 8:
            {
                this->read( _buff, _size, _read, this->WindowRightBottom_ResourceImageName );
                return true;
                break;
            }
        case 14:
            {
                this->read( _buff, _size, _read, this->WindowRightTop_Offset );
                return true;
                break;
            }
        case 6:
            {
                this->read( _buff, _size, _read, this->WindowRightTop_ResourceImageName );
                return true;
                break;
            }
        case 13:
            {
                this->read( _buff, _size, _read, this->WindowTop_Offset );
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->WindowTop_ResourceImageName );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceWindow::_preparationIncludes( size_t _includes, size_t _count )
    {
        if( Meta_DataBlock::Meta_Resource::_preparationIncludes( _includes, _count ) == true )
        {
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceWindow::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        if( Meta_DataBlock::Meta_Resource::_parseIncludes( _buff, _size, _read, _includes ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_DataBlock::Meta_ResourceWindow::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        if( Meta_DataBlock::Meta_Resource::_parseGenerators( _buff, _size, _read, _generators ) == true )
        {
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->height );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_Font::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_Font::Meta_Char metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_Char.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->code );
                return true;
                break;
            }
        case 2:
            {
                this->read( _buff, _size, _read, this->offset );
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->rect );
                return true;
                break;
            }
        case 1:
            {
                this->read( _buff, _size, _read, this->width );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_Font::Meta_Char::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 5:
            {
                Meta_Font::Meta_Char::Meta_Kerning metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_Kerning.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::Meta_Kerning::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->advance );
                return true;
                break;
            }
        case 2:
            {
                this->read( _buff, _size, _read, this->id );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::Meta_Kerning::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::Meta_Kerning::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Font::Meta_Char::Meta_Kerning::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->MaxIndex );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_KeyFramesPack::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_KeyFramesPack::Meta_KeyFrames2D metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_KeyFrames2D.push_back(metadata);
                return true;
                break;
            }
        case 3:
            {
                Meta_KeyFramesPack::Meta_KeyFrames3D metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_KeyFrames3D.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 3:
            {
                this->read( _buff, _size, _read, this->Count );
                this->Count_successful = true;
                return true;
                break;
            }
        case 2:
            {
                this->read( _buff, _size, _read, this->Immutable );
                this->Immutable_successful = true;
                return true;
                break;
            }
        case 1:
            {
                this->read( _buff, _size, _read, this->LayerIndex );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 4:
            {
                Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_KeyFrame2D.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->AnchorPoint );
                this->AnchorPoint_successful = true;
                return true;
                break;
            }
        case 7:
            {
                this->read( _buff, _size, _read, this->Count );
                this->Count_successful = true;
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->Opacity );
                this->Opacity_successful = true;
                return true;
                break;
            }
        case 2:
            {
                this->read( _buff, _size, _read, this->Position );
                this->Position_successful = true;
                return true;
                break;
            }
        case 4:
            {
                this->read( _buff, _size, _read, this->Rotation );
                this->Rotation_successful = true;
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->Scale );
                this->Scale_successful = true;
                return true;
                break;
            }
        case 6:
            {
                this->read( _buff, _size, _read, this->Volume );
                this->Volume_successful = true;
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->LayerIndex );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_KeyFrame3D.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->AnchorPoint );
                this->AnchorPoint_successful = true;
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->Opacity );
                this->Opacity_successful = true;
                return true;
                break;
            }
        case 2:
            {
                this->read( _buff, _size, _read, this->Position );
                this->Position_successful = true;
                return true;
                break;
            }
        case 4:
            {
                this->read( _buff, _size, _read, this->Rotation );
                this->Rotation_successful = true;
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->Scale );
                this->Scale_successful = true;
                return true;
                break;
            }
        case 6:
            {
                this->read( _buff, _size, _read, this->Volume );
                this->Volume_successful = true;
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_Pak::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 2:
            {
                Meta_Pak::Meta_Resources metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_Resources.push_back(metadata);
                return true;
                break;
            }
        case 1:
            {
                Meta_Pak::Meta_Scripts metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_Scripts.push_back(metadata);
                return true;
                break;
            }
        case 3:
            {
                Meta_Pak::Meta_Texts metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_Texts.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_Pak::Meta_Resources::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Pak::Meta_Resources::Meta_Resource metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_Resource.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::Meta_Resource::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->Path );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::Meta_Resource::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::Meta_Resource::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Resources::Meta_Resource::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Scripts::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->Path );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Scripts::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Scripts::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Scripts::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_Pak::Meta_Texts::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Pak::Meta_Texts::Meta_Text metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_Text.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::Meta_Text::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 1:
            {
                this->read( _buff, _size, _read, this->Path );
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::Meta_Text::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::Meta_Text::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Pak::Meta_Texts::Meta_Text::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::_preparationIncludes( size_t _includes, size_t _count )
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
    bool Meta_Texts::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        switch( _includes )
        {
        case 1:
            {
                Meta_Texts::Meta_Text metadata;
                metadata.parse( _buff, _size, _read );
    
                includes_Meta_Text.push_back(metadata);
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::Meta_Text::_parseArguments( const char * _buff, size_t _size, size_t & _read, size_t _id )
    {
        switch( _id )
        {
        case 4:
            {
                this->read( _buff, _size, _read, this->CharOffset );
                this->CharOffset_successful = true;
                return true;
                break;
            }
        case 3:
            {
                this->read( _buff, _size, _read, this->Font );
                this->Font_successful = true;
                return true;
                break;
            }
        case 1:
            {
                this->read( _buff, _size, _read, this->Key );
                return true;
                break;
            }
        case 5:
            {
                this->read( _buff, _size, _read, this->LineOffset );
                this->LineOffset_successful = true;
                return true;
                break;
            }
        case 2:
            {
                this->read( _buff, _size, _read, this->Value );
                this->Value_successful = true;
                return true;
                break;
            }
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::Meta_Text::_preparationIncludes( size_t _includes, size_t _count )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::Meta_Text::_parseIncludes( const char * _buff, size_t _size, size_t & _read, size_t _includes )
    {
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    bool Meta_Texts::Meta_Text::_parseGenerators( const char * _buff, size_t _size, size_t & _read, size_t _generators )
    {
        return false;
    }
    
}
