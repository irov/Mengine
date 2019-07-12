#include "ResourceTexturepacker.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Plugins/JSONPlugin/JSONInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceTexturepacker::ResourceTexturepacker()
        : m_atlasWidth( 0 )
        , m_atlasHeight( 0 )
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
    const ResourceImagePtr & ResourceTexturepacker::getFrame( const ConstString & _name ) const
    {
        const ResourceImagePtr & image = m_hashtableFrames.find( _name );

        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorResourceImages & ResourceTexturepacker::getFrames() const
    {
        return m_frames;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceTexturepacker::_compile()
    {
        const ResourceImagePtr & resourceImage = RESOURCE_SERVICE()
            ->getResource( m_resourceImageName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, false, "'%s' category '%s' group '%s' invalid get image resource '%s'"
            , this->getName().c_str()
            , this->getFileGroup()->getName().c_str()
            , this->getGroupName().c_str()
            , m_resourceImageName.c_str()
        );

        m_resourceImage = resourceImage;

        const ResourcePtr & resourceJSON = RESOURCE_SERVICE()
            ->getResource( m_resourceJSONName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceJSON, false, "'%s' category '%s' group '%s' invalid get image resource '%s'"
            , this->getName().c_str()
            , this->getFileGroup()->getName().c_str()
            , this->getGroupName().c_str()
            , m_resourceJSONName.c_str()
        );

        m_resourceJSON = resourceJSON;

        bool atlasHasAlpha = m_resourceImage->hasAlpha();
        bool atlasIsPremultiply = m_resourceImage->isPremultiply();
        bool atlasIsPow2 = m_resourceImage->isPow2();
        const RenderTextureInterfacePtr & atlasTexture = m_resourceImage->getTexture();
        const RenderTextureInterfacePtr & atlasTextureAlpha = m_resourceImage->getTextureAlpha();

        UnknownResourceJSONInterface * resourceJsonInterface = m_resourceJSON->getUnknown();

        const jpp::object & root = resourceJsonInterface->getJSON();

        if( root == jpp::detail::invalid )
        {
            LOGGER_ERROR( "invalid json '%s'"
                , m_resourceJSONName.c_str()
            );

            return false;
        }

        jpp::object root_meta = root["meta"];

        jpp::object root_meta_size = root_meta["size"];

        uint32_t atlas_width = root_meta_size["w"];
        uint32_t atlas_height = root_meta_size["h"];

        uint32_t atlas_width_pow2 = Helper::getTexturePOW2( atlas_width );
        uint32_t atlas_height_pow2 = Helper::getTexturePOW2( atlas_height );

        m_atlasWidth = atlas_width_pow2;
        m_atlasHeight = atlas_height_pow2;

        float atlas_width_inv = 1.f / (float)atlas_width_pow2;
        float atlas_height_inv = 1.f / (float)atlas_height_pow2;

        m_atlasWidthInv = atlas_width_inv;
        m_atlasHeightInv = atlas_height_inv;

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

            if( trimmed == true )
            {
                MENGINE_ASSERTION_NOT_IMPLEMENTED();
            }

            jpp::object spriteSourceSize = value["spriteSourceSize"];

            //int32_t spriteSourceSize_x = spriteSourceSize["x"];
            //int32_t spriteSourceSize_y = spriteSourceSize["y"];
            //int32_t spriteSourceSize_w = spriteSourceSize["w"];
            //int32_t spriteSourceSize_h = spriteSourceSize["h"];

            jpp::object sourceSize = value["sourceSize"];

            int32_t sourceSize_w = sourceSize["w"];
            int32_t sourceSize_h = sourceSize["h"];

            ConstString c_name = Helper::stringizeString( name );

            ResourceImagePtr image = RESOURCE_SERVICE()
                ->generateResource( STRINGIZE_STRING_LOCAL( "ResourceImage" ), MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( image, false );

            image->setName( c_name );
            image->setTexture( atlasTexture );
            image->setTextureAlpha( atlasTextureAlpha );

            mt::vec2f size( (float)sourceSize_w, (float)sourceSize_h );
            image->setMaxSize( size );
            image->setSize( size );

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
        m_frames.clear();
    }
}