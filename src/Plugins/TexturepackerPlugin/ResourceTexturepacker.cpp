#include "ResourceTexturepacker.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionNotImplemented.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void* my_jpp_malloc(size_t _size)
    {
        return Helper::allocateMemory(_size, "ResourceImageTexturepacker");
    }
    //////////////////////////////////////////////////////////////////////////
    static void my_jpp_free( void * _ptr )
    {
        Helper::freeMemory(_ptr, "ResourceImageTexturepacker");
    }
    //////////////////////////////////////////////////////////////////////////
    static void my_jpp_error( int32_t _line, int32_t _column, int32_t _position, const char * _source, const char * _text, void * _ud )
    {
        MENGINE_UNUSED( _ud );

        LOGGER_ERROR( "jpp error: %s\nline: %d\n column: %d\nposition: %d\nsource: %s\n"
            , _text
            , _line
            , _column
            , _position
            , _source
        );
    }
    //////////////////////////////////////////////////////////////////////////
    struct my_json_load_data
    {
        const uint8_t * buffer;
        size_t carriage;
        size_t capacity;
    };
    //////////////////////////////////////////////////////////////////////////
    static size_t my_jpp_load_callback( void* _buffer, size_t _buflen, void* _data )
    {
        my_json_load_data* jd = static_cast<my_json_load_data *>(_data);
        
        if( _buflen > jd->capacity - jd->carriage )
        {
            _buflen = jd->capacity - jd->carriage;
        }

        if( _buflen > 0 )
        {
            const uint8_t * jd_buffer = jd->buffer + jd->carriage;
            ::memcpy( _buffer, jd_buffer, _buflen );
            jd->carriage += _buflen;
            
            return _buflen;
        }
        else 
        {
            return 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceTexturepacker::ResourceTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceTexturepacker::~ResourceTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceTexturepacker::setResourceImageName(const ConstString& _resourceImageName)
    {
        m_resourceImageName = _resourceImageName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString& ResourceTexturepacker::getResourceImageName() const
    {
        return m_resourceImageName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr& ResourceTexturepacker::getFrame(const ConstString& _name) const
    {
        const ResourceImagePtr& image = m_frames.find(_name);

        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceTexturepacker::_compile()
    {
        const ResourceImagePtr& resourceImage = RESOURCE_SERVICE()
            ->getResource( m_resourceImageName );

        if( resourceImage == nullptr )
        {
            LOGGER_ERROR( "'%s' category '%s' group '%s' invalid get image resource '%s'"
                , this->getName().c_str()
                , this->getFileGroup()->getName().c_str()
                , this->getGroupName().c_str()
                , m_resourceImageName.c_str()
            );

            return false;
        }

        m_resourceImage = resourceImage;

        bool atlasHasAlpha = m_resourceImage->hasAlpha();
        bool atlasIsPremultiply = m_resourceImage->isPremultiply();
        bool atlasIsPow2 = m_resourceImage->isPow2();
        const RenderTextureInterfacePtr& atlasTexture = m_resourceImage->getTexture();
        const RenderTextureInterfacePtr& atlasTextureAlpha = m_resourceImage->getTextureAlpha();


        const FileGroupInterfacePtr& fileGroup = this->getFileGroup();
        const FilePath& filePath = this->getFilePath();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile(fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION);

        if (stream == nullptr)
        {
            return false;
        }

        MemoryInterfacePtr memory = Helper::createMemoryStream(stream, MENGINE_DOCUMENT_FUNCTION, __FILE__, __LINE__);

        if (memory == nullptr)
        {
            return false;
        }

        const void* memory_buffer = memory->getBuffer();
        size_t memory_size = memory->getSize();

        my_json_load_data jd;
        jd.buffer = static_cast<const uint8_t *>(memory_buffer);
        jd.carriage = 0;
        jd.capacity = memory_size;

        jpp::object root = jpp::load( &my_jpp_load_callback, &my_jpp_malloc, &my_jpp_free, &my_jpp_error, &jd );

        if( root == jpp::detail::invalid )
        {
            LOGGER_ERROR("invalid load json '%s'"
                , filePath.c_str()
            );

            return false;
        }

        jpp::object root_meta = root["meta"];

        jpp::object root_meta_size = root_meta["size"];

        uint32_t atlas_width = root_meta_size["w"];
        uint32_t atlas_height = root_meta_size["h"];

        float atlas_width_1inv = 1.f / (float)atlas_width;
        float atlas_height_1inv = 1.f / (float)atlas_height;

        jpp::object root_frames = root["frames"];

        for( auto&& [name, value] : root_frames )
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

            int32_t sourceSize_x = sourceSize["x"];
            int32_t sourceSize_y = sourceSize["y"];

            ConstString c_name = Helper::stringizeString( name );

            ResourceImagePtr image = RESOURCE_SERVICE()
                ->generateResource( STRINGIZE_STRING_LOCAL( "ResourceImage" ), MENGINE_DOCUMENT_FUNCTION );

            image->setTexture( atlasTexture );
            image->setTextureAlpha( atlasTextureAlpha );

            mt::vec2f size( (float)sourceSize_x, (float)sourceSize_y );
            image->setMaxSize( size );
            image->setSize( size );

            mt::vec4f uv_mask( (float)frame_x, (float)frame_y, (float)frame_x + frame_w, (float)frame_y + frame_h );
            uv_mask *= mt::vec2f( atlas_width_1inv, atlas_height_1inv );
            mt::uv4f uv( uv_mask );
            image->setUVImage( uv );
            image->setUVAlpha( uv );

            image->setUVImageRotate( rotated );
            image->setUVAlphaRotate( rotated );

            image->setAlpha( atlasHasAlpha );
            image->setPremultiply( atlasIsPremultiply );
            image->setPow2( atlasIsPow2 );

            m_frames.insert( c_name, image );
        }        

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceTexturepacker::_release()
    {
        //Empty
    }
}