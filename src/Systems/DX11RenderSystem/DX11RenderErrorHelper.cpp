#include "DX11RenderErrorHelper.h"

#include "Kernel/Logger.h"

#include "Config/Switch.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * getDX11ErrorMessage( HRESULT _hr )
        {
            switch( _hr )
            {
                MENGINE_MESSAGE_CASE( S_OK, "Ok" );
                MENGINE_MESSAGE_CASE( S_FALSE, "S_FALSE" );

                MENGINE_MESSAGE_CASE( D3D11_ERROR_FILE_NOT_FOUND, "file not found" );
                MENGINE_MESSAGE_CASE( D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS, "There are too many unique instances of a particular type of state object" );
                MENGINE_MESSAGE_CASE( D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS, "There are too many unique instances of a particular type of view object" );
                MENGINE_MESSAGE_CASE( D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD, "The first call to ID3D11DeviceContext::Map after either ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per Resource was not D3D11_MAP_WRITE_DISCARD" );
                MENGINE_MESSAGE_CASE( E_FAIL, "Attempted to create a device with the debug layer enabled and the layer is not installed" );
                MENGINE_MESSAGE_CASE( E_INVALIDARG, "An invalid parameter was passed to the returning function" );
                MENGINE_MESSAGE_CASE( E_NOTIMPL, "The method call isn't implemented with the passed parameter combination" );

                MENGINE_MESSAGE_CASE( DXGI_ERROR_ACCESS_DENIED, "You tried to use a resource to which you did not have the required access privileges. This error is most typically caused when you write to a shared resource with read-only acces" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_ACCESS_LOST, "The desktop duplication interface is invalid. The desktop duplication interface typically becomes invalid when a different type of image is displayed on the desktop" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_ALREADY_EXISTS, "The desired element already exists. This is returned by DXGIDeclareAdapterRemovalSupport if it is not the first time that the function is called" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_CANNOT_PROTECT_CONTENT, "DXGI can't provide content protection on the swap chain. This error is typically caused by an older driver, or when you use a swap chain that is incompatible with content protection" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_DEVICE_HUNG, "The application's device failed due to badly formed commands sent by the application. This is an design-time issue that should be investigated and fixed" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_DEVICE_REMOVED, "The video card has been physically removed from the system, or a driver upgrade for the video card has occurred. Check DXGI_ERROR_DEVICE_REMOVED" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_DEVICE_RESET, "The device failed due to a badly formed command. This is a run-time issue; The application should destroy and recreate the device" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_DRIVER_INTERNAL_ERROR, "The driver encountered a problem and was put into the device removed state" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_FRAME_STATISTICS_DISJOINT, "An event (for example, a power cycle) interrupted the gathering of presentation statistics" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE, "The application attempted to acquire exclusive ownership of an output, but failed because some other application (or device within the application) already acquired ownership" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_INVALID_CALL, "The application provided invalid parameter data; this must be debugged and fixed before the application is released" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_MORE_DATA, "The buffer supplied by the application is not big enough to hold the requested data" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_NAME_ALREADY_EXISTS, "The supplied name of a resource in a call to IDXGIResource1::CreateSharedHandle is already associated with some other resource" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_NONEXCLUSIVE, "A global counter resource is in use, and the Direct3D device can't currently use the counter resource" );

                MENGINE_MESSAGE_CASE( DXGI_ERROR_NOT_CURRENTLY_AVAILABLE, "The resource or request is not currently available, but it might become available later" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_NOT_FOUND, "When calling IDXGIObject::GetPrivateData, the GUID passed in is not recognized as one previously passed to IDXGIObject::SetPrivateData or IDXGIObject::SetPrivateDataInterface. When calling IDXGIFactory::EnumAdapters or IDXGIAdapter::EnumOutputs, the enumerated ordinal is out of range" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED, "Reserved" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_REMOTE_OUTOFMEMORY, "The device has been removed during a remote session because the remote computer ran out of memory" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE, "The DXGI output (monitor) to which the swap chain content was restricted is now disconnected or changed" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_SDK_COMPONENT_MISSING, "The operation depends on an SDK component that is missing or mismatched" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_SESSION_DISCONNECTED, "The Remote Desktop Services session is currently disconnected" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_UNSUPPORTED, "The requested functionality is not supported by the device or the driver" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_WAIT_TIMEOUT, "The time-out interval elapsed before the next desktop frame was available" );
                MENGINE_MESSAGE_CASE( DXGI_ERROR_WAS_STILL_DRAWING, "The previous blit operation that is transferring information to or from this surface is incomplete" );

            default:
                return "Unknown error.";
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    DX11ErrorHelper::DX11ErrorHelper( const Char * _file, uint32_t _line, const Char * _method )
        : m_file( _file )
        , m_line( _line )
        , m_method( _method )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11ErrorHelper::operator == ( HRESULT _hr ) const
    {
        if( _hr == S_OK )
        {
            return false;
        }

        const Char * message = Helper::getDX11ErrorMessage( _hr );

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "dx11" ), LM_ERROR, LFILTER_NONE, LCOLOR_RED, m_file, m_line, LFLAG_SHORT | LFLAG_FUNCTIONSTAMP )("call '%s' get error: %s (hr:%x)"
            , m_method
            , message
            , (uint32_t)_hr
            );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}