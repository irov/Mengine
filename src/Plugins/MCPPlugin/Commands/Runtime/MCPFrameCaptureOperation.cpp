#include "MCPFrameCaptureOperation.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/PlayerServiceInterface.h"

#include "../../MCPMemoryOutputStream.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const int32_t MCP_FRAME_CAPTURE_PNG_COMPRESSION_LEVEL = 1;
    //////////////////////////////////////////////////////////////////////////
    MCPFrameCaptureOperation::MCPFrameCaptureOperation( MCPFrameCaptureCommand * _command, MCPCommandHostInterface * _host, const RenderTargetInterfacePtr & _target, bool _ownTarget, uint32_t _width, uint32_t _height, bool _includeAlpha )
        : m_command( _command )
        , m_host( _host )
        , m_target( _target )
        , m_ownTarget( _ownTarget )
        , m_width( _width )
        , m_height( _height )
        , m_includeAlpha( _includeAlpha )
    {
        m_armedFrame = m_host->getMCPRenderFrame();
    }
    //////////////////////////////////////////////////////////////////////////
    MCPFrameCaptureOperation::~MCPFrameCaptureOperation()
    {
        this->restoreTarget_();
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPFrameCaptureOperation::update( MCPCommandResponse * const _response )
    {
        uint64_t renderFrame = m_host->getMCPRenderFrame();

        if( renderFrame <= m_armedFrame )
        {
            return EMCPCommandStatus::DEFERRED;
        }

        this->restoreTarget_();

        String error;
        if( this->encodeFrame_( &_response->attachment, &error ) == false )
        {
            _response->errorMessage = error;

            return EMCPCommandStatus::FAILURE;
        }

        _response->result.set( "width", m_width );
        _response->result.set( "height", m_height );
        _response->result.set( "mimeType", "image/png" );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFrameCaptureOperation::cancel()
    {
        this->restoreTarget_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFrameCaptureOperation::restoreTarget_()
    {
        if( m_command != nullptr )
        {
            m_command->endCapture();
            m_command = nullptr;
        }

        if( m_ownTarget == false )
        {
            return;
        }

        m_ownTarget = false;

        PLAYER_SERVICE()
            ->setRenderTarget( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFrameCaptureOperation::encodeFrame_( Data * const _png, String * const _error ) const
    {
        uint32_t hwWidth = m_target->getHWWidth();
        uint32_t hwHeight = m_target->getHWHeight();
        size_t pixelChannels = 4;
        size_t pitch = (size_t)hwWidth * pixelChannels;

        Data pixels;
        pixels.resize( pitch * hwHeight );
        uint8_t * pixelsBuffer = pixels.data();

        if( m_target->getData( pixelsBuffer, pitch ) == false )
        {
            *_error = "render backend failed to read the captured frame";

            return false;
        }

        if( m_includeAlpha == false )
        {
            size_t alphaChannel = pixelChannels - 1;
            uint8_t opaqueAlpha = 255;

            for( size_t offset = alphaChannel; offset < pixels.size(); offset += pixelChannels )
            {
                pixels[offset] = opaqueAlpha;
            }
        }

        ImageEncoderInterfacePtr encoder = CODEC_SERVICE()
            ->createEncoder( STRINGIZE_STRING_LOCAL( "pngImage" ), MENGINE_DOCUMENT_FACTORABLE );

        if( encoder == nullptr )
        {
            *_error = "PNG encoder is unavailable in this Development build";

            return false;
        }

        MCPMemoryOutputStreamPtr stream = Helper::makeFactorableUnique<MCPMemoryOutputStream>( MENGINE_DOCUMENT_FACTORABLE );

        if( encoder->initialize( nullptr, stream ) == false )
        {
            *_error = "failed to initialize PNG encoder";

            return false;
        }

        ImageEncoderData encoderData;
        encoderData.buffer = pixelsBuffer;
        encoderData.size = pixels.size();
        encoderData.pitch = pitch;

        ImageCodecDataInfo codecInfo;
        codecInfo.width = m_width;
        codecInfo.height = m_height;
        codecInfo.format = PF_A8R8G8B8;
        codecInfo.compressionLevel = MCP_FRAME_CAPTURE_PNG_COMPRESSION_LEVEL;

        size_t encoded = encoder->encode( &encoderData, &codecInfo );
        encoder->finalize();

        const Data & data = stream->getData();

        if( encoded == 0 )
        {
            *_error = "PNG frame encoding failed";

            return false;
        }

        if( data.empty() == true )
        {
            *_error = "PNG frame encoding failed";

            return false;
        }

        *_png = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
