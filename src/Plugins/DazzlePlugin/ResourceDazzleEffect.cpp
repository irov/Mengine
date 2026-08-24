#include "ResourceDazzleEffect.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/Dataflow.h"
#include "Kernel/Logger.h"
#include "Kernel/PixelFormatHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool loadEmitterTextureRGBA( const ResourceImagePtr & _resource, Data * const _rgba, uint32_t * const _width, uint32_t * const _height,
            uint32_t * const _pitch, const DocumentInterfacePtr & _doc )
        {
            const ContentInterfacePtr & content = _resource->getContent();

            if( content == nullptr )
            {
                LOGGER_ERROR( "dazzle emitter texture resource '%s' group '%s' has no content", _resource->getName().c_str(), _resource->getGroupName().c_str() );

                return false;
            }

            const ConstString & codecType = Helper::findContentCodecType( content );

            if( codecType.empty() == true )
            {
                LOGGER_ERROR( "dazzle emitter texture resource '%s' file '%s' has no image codec", _resource->getName().c_str(), content->getFilePath().c_str() );

                return false;
            }

            InputStreamInterfacePtr stream = content->openInputStreamFile( false, false, _doc );

            if( stream == nullptr )
            {
                LOGGER_ERROR( "dazzle emitter texture resource '%s' file '%s' was not found", _resource->getName().c_str(), content->getFilePath().c_str() );

                return false;
            }

            ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
                ->createDecoder( codecType, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid image decoder '%s' for dazzle emitter texture resource '%s'", codecType.c_str(),
                _resource->getName().c_str() );

            if( decoder->prepareData( content, stream ) == false )
            {
                LOGGER_ERROR( "dazzle emitter texture resource '%s' decoder preparation failed", _resource->getName().c_str() );

                return false;
            }

            const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();
            const uint32_t width = dataInfo->width;
            const uint32_t height = dataInfo->height;
            const uint32_t pitch = width * 4U;

            _rgba->resize( (size_t)pitch * height );

            ImageDecoderData decoderData;
            decoderData.buffer = _rgba->data();
            decoderData.size = _rgba->size();
            decoderData.pitch = pitch;
            decoderData.format = PF_A8R8G8B8;

            if( decoder->decode( &decoderData ) == 0 )
            {
                LOGGER_ERROR( "dazzle emitter texture resource '%s' decode failed", _resource->getName().c_str() );

                return false;
            }

            *_width = width;
            *_height = height;
            *_pitch = pitch;

            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceDazzleEffect::ResourceDazzleEffect()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceDazzleEffect::~ResourceDazzleEffect()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceDazzleEffect::setResourceEmitterImage( const ResourceImagePtr & _resource )
    {
        m_resourceEmitterImage = _resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceDazzleEffect::getResourceEmitterImage() const
    {
        return m_resourceEmitterImage;
    }
    //////////////////////////////////////////////////////////////////////////
    const DataInterfacePtr & ResourceDazzleEffect::getData() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceDazzleEffect::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();
        const ConstString & resourceGroupName = this->getGroupName();

        const FilePath & filePath = content->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_ERROR( "dazzle effect resource '%s' group '%s' don`t set file path"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        DataflowContext context;
        context.filePath = filePath;
        context.groupName = resourceGroupName;

        DazzleDataInterfacePtr data = Helper::getDataflow( content, &context, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "resource '%s' group '%s' invalid compile data"
            , this->getName().c_str()
            , this->getGroupName().c_str()
        );

        const dz_effect_t * effect = data->getDazzleEffect();

        if( dz_effect_requires_emitter_texture( effect ) == DZ_TRUE )
        {
            if( m_resourceEmitterImage == nullptr )
            {
                LOGGER_ERROR( "dazzle effect resource '%s' group '%s' requires emitter texture resource", this->getName().c_str(), this->getGroupName().c_str() );

                return false;
            }

            uint32_t width;
            uint32_t height;
            uint32_t pitch;

            if( Detail::loadEmitterTextureRGBA( m_resourceEmitterImage, &m_emitterTextureBuffer, &width, &height, &pitch, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                Data().swap( m_emitterTextureBuffer );

                return false;
            }

            dz_shape_mask_source_t source;
            source.buffer = m_emitterTextureBuffer.data();
            source.pitch = pitch;
            source.width = width;
            source.height = height;
            source.channel_count = 4U;
            source.alpha_channel = 3U;
            source.alpha_threshold = 0U;

            dz_result_t result = data->setDazzleEmitterTexture( &source );

            if( result != DZ_SUCCESSFUL )
            {
                LOGGER_ERROR( "dazzle effect resource '%s' group '%s' invalid emitter texture resource '%s' result '%u'", this->getName().c_str(),
                    this->getGroupName().c_str(), m_resourceEmitterImage->getName().c_str(), (uint32_t)result );

                Data().swap( m_emitterTextureBuffer );

                return false;
            }
        }

        if( data->acquire() == false )
        {
            Data().swap( m_emitterTextureBuffer );

            return false;
        }

        m_data = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceDazzleEffect::_release()
    {
        m_data->release();
        m_data = nullptr;
        Data().swap( m_emitterTextureBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceDazzleEffect::_finalize()
    {
        m_resourceEmitterImage = nullptr;
        Data().swap( m_emitterTextureBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
}
