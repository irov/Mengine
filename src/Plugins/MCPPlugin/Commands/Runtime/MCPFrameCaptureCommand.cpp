#include "MCPFrameCaptureCommand.h"

#include "../../MCPMemoryOutputStream.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPFrameCaptureCommand::MCPFrameCaptureCommand( MCPCommandHostInterface * _host )
        : m_host( _host )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPFrameCaptureCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        const String & mode = m_host->getMCPMode();

        if( mode == "headless_logic" )
        {
            _response->errorCode = "unsupported";
            _response->errorMessage = "screenshots are unavailable in headless_logic mode";

            return EMCPCommandStatus::FAILURE;
        }

        if( m_pendingCapture.requestId != 0 )
        {
            _response->errorMessage = "another frame capture is already pending";

            return EMCPCommandStatus::FAILURE;
        }

        const Resolution & resolution = APPLICATION_SERVICE()->getCurrentWindowResolution();
        uint32_t width = resolution.getWidth();
        uint32_t height = resolution.getHeight();

        if( width == 0 || height == 0 )
        {
            _response->errorCode = "unsupported";
            _response->errorMessage = "the render window has no capture resolution";

            return EMCPCommandStatus::FAILURE;
        }

        DocumentInterfacePtr document = MENGINE_DOCUMENT_FACTORABLE;
        RenderTargetInterfacePtr target = RENDER_SYSTEM()->createRenderTargetTexture( width, height, PF_A8R8G8B8, document );

        if( target == nullptr )
        {
            _response->errorCode = "unsupported";
            _response->errorMessage = "the active render backend cannot create a capture target";

            return EMCPCommandStatus::FAILURE;
        }

        m_pendingCapture.requestId = _request.requestId;
        m_pendingCapture.width = width;
        m_pendingCapture.height = height;
        m_pendingCapture.armedGeneration = m_host->getMCPUpdateGeneration();
        m_pendingCapture.includeAlpha = _request.params.get( "includeAlpha", false );
        m_pendingCapture.target = target;
        m_pendingCapture.previousTarget = PLAYER_SERVICE()->getRenderTarget();
        PLAYER_SERVICE()->setRenderTarget( target );

        return EMCPCommandStatus::DEFERRED;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFrameCaptureCommand::update()
    {
        if( m_pendingCapture.requestId == 0 )
        {
            return;
        }

        if( m_host->isMCPConnected() == false )
        {
            this->reset_();

            return;
        }

        uint64_t generation = m_host->getMCPUpdateGeneration();

        if( generation <= m_pendingCapture.armedGeneration )
        {
            return;
        }

        uint32_t requestId = m_pendingCapture.requestId;
        PLAYER_SERVICE()->setRenderTarget( m_pendingCapture.previousTarget );

        Data png;
        String error;
        bool encoded = this->encodeFrame_( &png, &error );

        if( encoded == false )
        {
            const Char * errorMessage = error.c_str();
            m_host->sendMCPCommandError( requestId, "execution_error", errorMessage );
            m_pendingCapture = PendingCapture();

            return;
        }

        size_t pngSizeValue = png.size();
        uint32_t pngSize = (uint32_t)pngSizeValue;
        jpp::object attachment = jpp::make_object();
        attachment.set( "size", pngSize );
        attachment.set( "mimeType", "image/png" );

        jpp::object result = jpp::make_object();
        result.set( "width", m_pendingCapture.width );
        result.set( "height", m_pendingCapture.height );
        result.set( "attachment", attachment );

        m_host->sendMCPCommandResponse( requestId, result );
        m_host->sendMCPCommandBinary( requestId, png );
        m_pendingCapture = PendingCapture();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFrameCaptureCommand::cancel( uint32_t _requestId )
    {
        if( m_pendingCapture.requestId != _requestId )
        {
            return;
        }

        this->reset_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFrameCaptureCommand::disconnect()
    {
        this->reset_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFrameCaptureCommand::encodeFrame_( Data * const _png, String * const _error )
    {
        uint32_t hwWidth = m_pendingCapture.target->getHWWidth();
        uint32_t hwHeight = m_pendingCapture.target->getHWHeight();
        size_t pitch = (size_t)hwWidth * 4;
        Data pixels;
        pixels.resize( pitch * hwHeight );
        uint8_t * pixelsBuffer = pixels.data();
        bool read = m_pendingCapture.target->getData( pixelsBuffer, pitch );

        if( read == false )
        {
            *_error = "render backend failed to read the captured frame";

            return false;
        }

        if( m_pendingCapture.includeAlpha == false )
        {
            for( size_t offset = 3; offset < pixels.size(); offset += 4 )
            {
                pixels[offset] = 255;
            }
        }

        ConstString codecType = STRINGIZE_STRING_LOCAL( "pngImage" );
        DocumentInterfacePtr encoderDocument = MENGINE_DOCUMENT_FACTORABLE;
        ImageEncoderInterfacePtr encoder = CODEC_SERVICE()->createEncoder( codecType, encoderDocument );

        if( encoder == nullptr )
        {
            *_error = "PNG encoder is unavailable in this Development build";

            return false;
        }

        DocumentInterfacePtr streamDocument = MENGINE_DOCUMENT_FACTORABLE;
        MCPMemoryOutputStreamPtr stream = Helper::makeFactorableUnique<MCPMemoryOutputStream>( streamDocument );
        bool initialized = encoder->initialize( nullptr, stream );

        if( initialized == false )
        {
            *_error = "failed to initialize PNG encoder";

            return false;
        }

        ImageEncoderData encoderData;
        encoderData.buffer = pixelsBuffer;
        encoderData.size = pixels.size();
        encoderData.pitch = pitch;

        ImageCodecDataInfo codecInfo;
        codecInfo.width = m_pendingCapture.width;
        codecInfo.height = m_pendingCapture.height;
        codecInfo.format = PF_A8R8G8B8;

        size_t encoded = encoder->encode( &encoderData, &codecInfo );
        encoder->finalize();

        const Data & data = stream->getData();

        if( encoded == 0 || data.empty() == true )
        {
            *_error = "PNG frame encoding failed";

            return false;
        }

        *_png = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFrameCaptureCommand::reset_()
    {
        if( m_pendingCapture.requestId == 0 )
        {
            return;
        }

        PLAYER_SERVICE()->setRenderTarget( m_pendingCapture.previousTarget );
        m_pendingCapture = PendingCapture();
    }
    //////////////////////////////////////////////////////////////////////////
}
