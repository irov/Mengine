#include "ResourceTexturepacker.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ContentInterface.h"

#include "Plugins/JSONPlugin/JSONInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/FileStreamHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceTexturepacker::ResourceTexturepacker()
        : m_atlasWidth( 0 )
        , m_atlasHeight( 0 )
        , m_atlasWidthInv( 0.f )
        , m_atlasHeightInv( 0.f )
        , m_needStripFrameNameExtension( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceTexturepacker::~ResourceTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceTexturepacker::getAtlasWidth() const
    {
        return m_atlasWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceTexturepacker::getAtlasHeight() const
    {
        return m_atlasHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceTexturepacker::getAtlasWidthInv() const
    {
        return m_atlasWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceTexturepacker::getAtlasHeightInv() const
    {
        return m_atlasHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceTexturepacker::setResourceJSONName( const ConstString & _resourceJSONName )
    {
        m_resourceJSONName = _resourceJSONName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceTexturepacker::getResourceJSONName() const
    {
        return m_resourceJSONName;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceTexturepacker::setResourceImageName( const ConstString & _resourceImageName )
    {
        m_resourceImageName = _resourceImageName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceTexturepacker::getResourceImageName() const
    {
        return m_resourceImageName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceTexturepacker::getAtlasImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceTexturepacker::findFrame( const ConstString & _name, ResourceImagePtr * const _resourceImage ) const
    {
        const ResourceImagePtr & image = m_hashtableFrames.find( _name );

        if( image == nullptr )
        {
            return false;
        }

        if( _resourceImage != nullptr )
        {
            *_resourceImage = image;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorResourceImages & ResourceTexturepacker::getFrames() const
    {
        return m_frames;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceTexturepacker::setStripFrameNameExtension( bool _value )
    {
        m_needStripFrameNameExtension = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceTexturepacker::getStripFrameNameExtension() const
    {
        return m_needStripFrameNameExtension;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceTexturepacker::_compile()
    {
        ResourceBankInterface * resourceBank = this->getResourceBank();

        if( m_resourceImageName.empty() == false )
        {
            const ResourceImagePtr & resourceImage = resourceBank->getResource( m_resourceImageName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, false, "'%s' group '%s' invalid get image resource '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , m_resourceImageName.c_str()
            );

            m_resourceImage = resourceImage;
        }

        const ResourcePtr & resourceJSON = resourceBank->getResource( m_resourceJSONName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceJSON, false, "'%s' group '%s' invalid get image resource '%s'"
            , this->getName().c_str()
            , this->getGroupName().c_str()
            , m_resourceJSONName.c_str()
        );

        m_resourceJSON = resourceJSON;

        UnknownResourceJSONInterface * unknownResourceJSON = m_resourceJSON->getUnknown();

        const JSONStorageInterfacePtr & storage = unknownResourceJSON->getJSONStorage();

        const jpp::object & root = storage->getJSON();

        MENGINE_ASSERTION_RETURN( !(root == jpp::detail::invalid), false, "invalid json '%s'"
            , m_resourceJSONName.c_str()
        );

        const ConstString & locale = this->getLocale();
        const ConstString & groupName = this->getGroupName();

        if( m_resourceImageName.empty() == true )
        {
            jpp::object root_meta = root["meta"];

            const Char * root_meta_image = root_meta["image"];

            ResourceImagePtr resource = resourceBank->createResource( locale, groupName, ConstString::none(), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), false, nullptr, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, false );

            const ContentInterface * json_content = m_resourceJSON->getContent();
            const FileGroupInterfacePtr & fileGroup = json_content->getFileGroup();
            const FilePath & filePath = json_content->getFilePath();

            ContentInterface * resource_content = resource->getContent();

            FilePath newFilePath = Helper::replaceFileSpec( filePath, root_meta_image );

            resource_content->setFileGroup( fileGroup );
            resource_content->setFilePath( newFilePath );

            const ConstString & codecType = CODEC_SERVICE()
                ->findCodecType( newFilePath );

            resource_content->setCodecType( codecType );

            jpp::object root_meta_size = root_meta["size"];
            mt::vec2f atlasSize;
            if( root_meta_size.invalid() == false )
            {
                atlasSize.x = root_meta_size["w"];
                atlasSize.y = root_meta_size["h"];
            }
            else
            {
                InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, newFilePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

                ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
                    ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_MEMORY_PANIC( decoder, false );

                if( decoder->prepareData( stream ) == false )
                {
                    return false;
                }

                const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

                atlasSize.x = (float)dataInfo->width;
                atlasSize.y = (float)dataInfo->height;
            }

            resource->setMaxSize( atlasSize );
            resource->setSize( atlasSize );

            if( resource->compile() == false )
            {
                return false;
            }

            m_resourceImage = resource;
        }

        const mt::vec2f & resourceImageMaxsize = m_resourceImage->getMaxSize();

        uint32_t atlas_width = (uint32_t)(resourceImageMaxsize.x + 0.5f);
        uint32_t atlas_height = (uint32_t)(resourceImageMaxsize.y + 0.5f);

        uint32_t atlas_width_pow2 = Helper::getTexturePOW2( atlas_width );
        uint32_t atlas_height_pow2 = Helper::getTexturePOW2( atlas_height );

        m_atlasWidth = atlas_width_pow2;
        m_atlasHeight = atlas_height_pow2;

        float atlas_width_inv = 1.f / (float)atlas_width_pow2;
        float atlas_height_inv = 1.f / (float)atlas_height_pow2;

        m_atlasWidthInv = atlas_width_inv;
        m_atlasHeightInv = atlas_height_inv;

        bool atlasHasAlpha = m_resourceImage->hasAlpha();
        bool atlasIsPremultiply = m_resourceImage->isPremultiply();
        bool atlasIsPow2 = m_resourceImage->isPow2();
        const RenderTextureInterfacePtr & atlasTexture = m_resourceImage->getTexture();
        const RenderTextureInterfacePtr & atlasTextureAlpha = m_resourceImage->getTextureAlpha();

        jpp::object root_frames = root["frames"];

        for( auto && [name, value] : root_frames )
        {
            jpp::object frame = value["frame"];

            int32_t frame_x = frame["x"];
            int32_t frame_y = frame["y"];
            int32_t frame_w = frame["w"];
            int32_t frame_h = frame["h"];

            bool rotated = value["rotated"];
            bool trimmed = value["trimmed"];

            jpp::object sourceSize = value["sourceSize"];

            int32_t sourceSize_w = sourceSize["w"];
            int32_t sourceSize_h = sourceSize["h"];

            ConstString c_name;

            if( m_needStripFrameNameExtension == false )
            {
                c_name = Helper::stringizeString( name );
            }
            else
            {
                const Char * ext_delimiter = MENGINE_STRRCHR( name, '.' );

                if( ext_delimiter == nullptr )
                {
                    c_name = Helper::stringizeString( name );
                }
                else if( MENGINE_STRCMP( ext_delimiter, ".png" ) != 0 && MENGINE_STRCMP( ext_delimiter, ".jpg" ) != 0 )
                {
                    c_name = Helper::stringizeString( name );
                }
                else
                {
                    size_t size_full_name = ext_delimiter - name;
                    c_name = Helper::stringizeStringSize( name, (ConstStringHolder::size_type)size_full_name );
                }
            }

            ResourceImagePtr image = resourceBank->createResource( locale, groupName, ConstString::none(), STRINGIZE_STRING_LOCAL( "ResourceImage" ), false, nullptr, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( image, false );

            image->setName( c_name );
            image->setTexture( atlasTexture );
            image->setTextureAlpha( atlasTextureAlpha );

            mt::vec2f maxSize( (float)sourceSize_w, (float)sourceSize_h );
            image->setMaxSize( maxSize );

            if( trimmed == true )
            {
                jpp::object spriteSourceSize = value["spriteSourceSize"];

                int32_t spriteSourceSize_x = spriteSourceSize["x"];
                int32_t spriteSourceSize_y = spriteSourceSize["y"];
                int32_t spriteSourceSize_w = spriteSourceSize["w"];
                int32_t spriteSourceSize_h = spriteSourceSize["h"];

                if( sourceSize_h > spriteSourceSize_h )
                {
                    mt::vec2f offset( (float)spriteSourceSize_x, (float)sourceSize_h - spriteSourceSize_h - spriteSourceSize_y );
                    image->setOffset( offset );
                }
                else
                {
                    mt::vec2f offset( (float)spriteSourceSize_x, (float)spriteSourceSize_y );
                    image->setOffset( offset );
                }

                mt::vec2f size( (float)spriteSourceSize_w, (float)spriteSourceSize_h );
                image->setSize( size );
            }
            else
            {
                image->setSize( maxSize );
            }

            mt::vec4f uv_mask( (float)frame_x, (float)frame_y, (float)(frame_x + frame_w), (float)(frame_y + frame_h) );
            uv_mask.x *= atlas_width_inv;
            uv_mask.y *= atlas_height_inv;
            uv_mask.z *= atlas_width_inv;
            uv_mask.w *= atlas_height_inv;

            mt::uv4f uv;
            mt::uv4_from_mask( uv, uv_mask );

            image->setUVImage( uv );
            image->setUVAlpha( uv );
            image->setUVTextureImage( uv );
            image->setUVTextureAlpha( uv );

            image->setUVImageRotate( rotated );
            image->setUVAlphaRotate( rotated );

            image->setAlpha( atlasHasAlpha );
            image->setPremultiply( atlasIsPremultiply );
            image->setPow2( atlasIsPow2 );

            if( image->compile() == false )
            {
                return false;
            }

            m_hashtableFrames.emplace( c_name, image );
            m_frames.push_back( image );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceTexturepacker::_release()
    {
        if( m_resourceJSON != nullptr )
        {
            m_resourceJSON->release();
            m_resourceJSON = nullptr;
        }

        if( m_resourceImage != nullptr )
        {
            m_resourceImage->release();
            m_resourceImage = nullptr;
        }

        m_hashtableFrames.clear();

        for( const ResourceImagePtr & frame : m_frames )
        {
            frame->release();
        }

        m_frames.clear();
    }
}