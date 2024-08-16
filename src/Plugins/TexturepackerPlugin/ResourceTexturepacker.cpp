#include "ResourceTexturepacker.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/PrototypeServiceInterface.h"

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
#include "Kernel/ResourceImageSubstract.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceTexturepacker::ResourceTexturepacker()
        : m_atlasWidth( 0 )
        , m_atlasHeight( 0 )
        , m_atlasWidthInv( 0.f )
        , m_atlasHeightInv( 0.f )
        , m_extraResourceImage( false )
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
    void ResourceTexturepacker::setResourceJSON( const ResourcePtr & _resourceJSON )
    {
        m_resourceJSON = _resourceJSON;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & ResourceTexturepacker::getResourceJSON() const
    {
        return m_resourceJSON;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceTexturepacker::setResourceImage( const ResourceImagePtr & _resourceImage )
    {
        if( _resourceImage == nullptr )
        {
            m_resourceImage = nullptr;
            m_extraResourceImage = false;

            return;
        }

        MENGINE_ASSERTION_FATAL( m_extraResourceImage == false, "already set resource image" );

        m_resourceImage = _resourceImage;

        if( m_resourceImage != nullptr )
        {
            m_extraResourceImage = true;
        }
        else
        {
            m_extraResourceImage = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceTexturepacker::getResourceImage() const
    {
        return m_resourceImage;
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
    bool ResourceTexturepacker::foreachFrames( const LambdaFrames & _lambdaFrames ) const
    {
        for( const FrameDesc & desc : m_frames )
        {
            const ResourceImagePtr & resourceImage = desc.resourceImage;

            if( _lambdaFrames( resourceImage ) == false )
            {
                return false;
            }
        }

        return true;
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
        m_resourceImage->compile();

        for( const FrameDesc & desc : m_frames )
        {
            const ResourceImagePtr & resourceImage = desc.resourceImage;

            resourceImage->compile();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceTexturepacker::_release()
    {     
        for( const FrameDesc & desc : m_frames )
        {
            const ResourceImagePtr & resourceImage = desc.resourceImage;

            resourceImage->release();
        }

        m_resourceImage->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceTexturepacker::_initialize()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceJSON );

        if( m_resourceJSON->compile() == false )
        {
            return false;
        }

        bool successful = this->initializeFrames_();

        m_resourceJSON->release();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceTexturepacker::_finalize()
    {
        ResourceBankInterface * resourceBank = this->getResourceBank();

        m_hashtableFrames.clear();

        for( const FrameDesc & desc : m_frames )
        {
            const ResourceImagePtr & resourceImage = desc.resourceImage;

            resourceBank->removeResource( resourceImage );
        }

        m_frames.clear();

        if( m_resourceImage != nullptr )
        {
            if( m_extraResourceImage == false )
            {
                resourceBank->removeResource( m_resourceImage );
            }
        }

        m_resourceJSON = nullptr;
        m_resourceImage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceTexturepacker::initializeFrames_()
    {
        ResourceBankInterface * resourceBank = this->getResourceBank();

        UnknownResourceJSONInterface * unknownResourceJSON = m_resourceJSON->getUnknown();

        const jpp::object & j = unknownResourceJSON->getJSON();

        MENGINE_ASSERTION_FATAL( j != jpp::detail::invalid, "invalid json '%s'"
            , m_resourceJSON->getName().c_str()
        );

        const VectorConstString & locales = this->getLocales();
        const ConstString & groupName = this->getGroupName();

        if( m_resourceImage == nullptr )
        {
            jpp::object j_meta = j["meta"];

            const Char * j_meta_image = j_meta["image"];

            ResourceCook cook;
            cook.locales = locales;
            cook.type = STRINGIZE_STRING_LOCAL( "ResourceImageDefault" );

            ResourceImagePtr resourceImage = resourceBank->createResource( cook, nullptr, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceImage );

            const ContentInterfacePtr & json_content = m_resourceJSON->getContent();
            const FileGroupInterfacePtr & fileGroup = json_content->getFileGroup();
            const FilePath & filePath = json_content->getFilePath();

            FilePath newFilePath = Helper::replaceFileSpec( filePath, j_meta_image );

            ContentInterfacePtr resource_content = Helper::makeFileContent( fileGroup, newFilePath, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( resource_content );

            const ConstString & codecType = CODEC_SERVICE()
                ->findCodecType( newFilePath );

            resource_content->setCodecType( codecType );

            resourceImage->setContent( resource_content );

            jpp::object j_meta_size = j_meta["size"];
            mt::vec2f atlasSize;
            if( j_meta_size.invalid() == false )
            {
                atlasSize.x = j_meta_size["w"];
                atlasSize.y = j_meta_size["h"];
            }
            else
            {
                InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, newFilePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_FATAL( stream->size() != 0, "empty stream '%s' codec '%s'"
                    , Helper::getFileGroupFullPath( fileGroup, newFilePath )
                    , codecType.c_str()
                );

                ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
                    ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_MEMORY_PANIC( decoder );

                if( decoder->prepareData( stream ) == false )
                {
                    return false;
                }

                const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

                atlasSize.x = (float)dataInfo->width;
                atlasSize.y = (float)dataInfo->height;
            }

            resourceImage->setMaxSize( atlasSize );
            resourceImage->setSize( atlasSize );

            if( resourceImage->initialize() == false )
            {
                LOGGER_ERROR( "resource texturepacker '%s' sub resource '%s' type '%s' invalid initialize"
                    , this->getName().c_str()
                    , resourceImage->getName().c_str()
                    , resourceImage->getType().c_str()
                );

                return false;
            }

            m_resourceImage = resourceImage;
            m_extraResourceImage = false;
        }

        const mt::vec2f & resourceImageMaxsize = m_resourceImage->getMaxSize();

        uint32_t atlas_width = (uint32_t)(resourceImageMaxsize.x + 0.5f);
        uint32_t atlas_height = (uint32_t)(resourceImageMaxsize.y + 0.5f);

        uint32_t atlas_width_pow2 = Helper::getTexturePow2( atlas_width );
        uint32_t atlas_height_pow2 = Helper::getTexturePow2( atlas_height );

        m_atlasWidth = atlas_width_pow2;
        m_atlasHeight = atlas_height_pow2;

        float atlas_width_inv = 1.f / (float)atlas_width_pow2;
        float atlas_height_inv = 1.f / (float)atlas_height_pow2;

        m_atlasWidthInv = atlas_width_inv;
        m_atlasHeightInv = atlas_height_inv;

        bool atlasHasAlpha = m_resourceImage->hasAlpha();
        bool atlasIsPremultiply = m_resourceImage->isPremultiply();
        bool atlasIsPow2 = m_resourceImage->isPow2();

        jpp::object j_frames = j["frames"];

        for( auto && [name, value] : j_frames )
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
                const Char * ext_delimiter = StdString::strrchr( name, '.' );

                if( ext_delimiter == nullptr )
                {
                    c_name = Helper::stringizeString( name );
                }
                else if( StdString::strcmp( ext_delimiter, ".png" ) != 0 && StdString::strcmp( ext_delimiter, ".jpg" ) != 0 )
                {
                    c_name = Helper::stringizeString( name );
                }
                else
                {
                    size_t size_full_name = ext_delimiter - name;
                    c_name = Helper::stringizeStringSize( name, (ConstStringHolder::size_type)size_full_name );
                }
            }

            ResourceCook cook;
            cook.groupName = groupName;
            cook.type = STRINGIZE_STRING_LOCAL( "ResourceImageSubstract" );

            ResourceImageSubstractPtr resourceImage = resourceBank->createResource( cook, nullptr, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceImage );

            resourceImage->setName( c_name );
            resourceImage->setResourceImage( m_resourceImage );

            mt::vec2f maxSize( (float)sourceSize_w, (float)sourceSize_h );
            resourceImage->setMaxSize( maxSize );

            if( trimmed == true )
            {
                jpp::object spriteSourceSize = value["spriteSourceSize"];

                int32_t spriteSourceSize_x = spriteSourceSize["x"];
                int32_t spriteSourceSize_y = spriteSourceSize["y"];
                int32_t spriteSourceSize_w = spriteSourceSize["w"];
                int32_t spriteSourceSize_h = spriteSourceSize["h"];

                mt::vec2f offset( (float)spriteSourceSize_x, (float)spriteSourceSize_y );
                resourceImage->setOffset( offset );

                mt::vec2f size( (float)spriteSourceSize_w, (float)spriteSourceSize_h );
                resourceImage->setSize( size );
            }
            else
            {
                resourceImage->setSize( maxSize );
            }

            mt::vec4f uv_mask( (float)frame_x, (float)frame_y, (float)(frame_x + frame_w), (float)(frame_y + frame_h) );
            uv_mask.x *= atlas_width_inv;
            uv_mask.y *= atlas_height_inv;
            uv_mask.z *= atlas_width_inv;
            uv_mask.w *= atlas_height_inv;

            mt::uv4f uv;
            mt::uv4_from_mask( &uv, uv_mask );

            resourceImage->setUVImage( uv );
            resourceImage->setUVAlpha( uv );
            resourceImage->setUVTextureImage( uv );
            resourceImage->setUVTextureAlpha( uv );

            resourceImage->setUVImageRotate( rotated );
            resourceImage->setUVAlphaRotate( rotated );

            resourceImage->setAlpha( atlasHasAlpha );
            resourceImage->setPremultiply( atlasIsPremultiply );
            resourceImage->setPow2( atlasIsPow2 );

            if( resourceImage->initialize() == false )
            {
                LOGGER_ERROR( "resource texturepacker '%s' frame resource '%s' type '%s' invalid initialize"
                    , this->getName().c_str()
                    , resourceImage->getName().c_str()
                    , resourceImage->getType().c_str()
                );

                return false;
            }

            m_hashtableFrames.emplace( c_name, resourceImage );

            FrameDesc desc;
            desc.resourceImage = resourceImage;

            m_frames.push_back( desc );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
