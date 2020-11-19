#include "DX9ErrorHelper.h"

#include "Kernel/Logger.h"

#define MENGINE_CHEK_ERR(hrchk, strOut)\
        case hrchk:\
             return strOut

namespace Mengine
{
    namespace Helper
    {
        const Char * getDX9ErrorMessage( HRESULT _hr )
        {
            switch( _hr )
            {
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)

                MENGINE_CHEK_ERR( D3D_OK, "Ok" );
                MENGINE_CHEK_ERR( D3DERR_WRONGTEXTUREFORMAT, "Wrong texture format" );
                MENGINE_CHEK_ERR( D3DERR_UNSUPPORTEDCOLOROPERATION, "Unsupported color operation" );
                MENGINE_CHEK_ERR( D3DERR_UNSUPPORTEDCOLORARG, "Unsupported color arg" );
                MENGINE_CHEK_ERR( D3DERR_UNSUPPORTEDALPHAOPERATION, "Unsupported alpha operation" );
                MENGINE_CHEK_ERR( D3DERR_UNSUPPORTEDALPHAARG, "Unsupported alpha arg" );
                MENGINE_CHEK_ERR( D3DERR_TOOMANYOPERATIONS, "Too many operations" );
                MENGINE_CHEK_ERR( D3DERR_CONFLICTINGTEXTUREFILTER, "Conflicting texture filter" );
                MENGINE_CHEK_ERR( D3DERR_UNSUPPORTEDFACTORVALUE, "Unsupported factor value" );
                MENGINE_CHEK_ERR( D3DERR_CONFLICTINGRENDERSTATE, "Conflicting render state" );
                MENGINE_CHEK_ERR( D3DERR_UNSUPPORTEDTEXTUREFILTER, "Unsupported texture filter" );
                MENGINE_CHEK_ERR( D3DERR_CONFLICTINGTEXTUREPALETTE, "Conflicting texture palette" );
                MENGINE_CHEK_ERR( D3DERR_DRIVERINTERNALERROR, "Driver internal error" );
                MENGINE_CHEK_ERR( D3DERR_NOTFOUND, "Not found" );
                MENGINE_CHEK_ERR( D3DERR_MOREDATA, "More data" );
                MENGINE_CHEK_ERR( D3DERR_DEVICELOST, "Device lost" );
                MENGINE_CHEK_ERR( D3DERR_DEVICENOTRESET, "Device not reset" );
                MENGINE_CHEK_ERR( D3DERR_NOTAVAILABLE, "Not available" );
                MENGINE_CHEK_ERR( D3DERR_OUTOFVIDEOMEMORY, "Out of video memory" );
                MENGINE_CHEK_ERR( D3DERR_INVALIDDEVICE, "Invalid device" );
                MENGINE_CHEK_ERR( D3DERR_INVALIDCALL, "Invalid call" );
                MENGINE_CHEK_ERR( D3DERR_DRIVERINVALIDCALL, "Driver invalid call" );
                MENGINE_CHEK_ERR( D3DERR_WASSTILLDRAWING, "Was Still Drawing" );
                MENGINE_CHEK_ERR( D3DOK_NOAUTOGEN, "The call succeeded but there won't be any mipmaps generated" );

                MENGINE_CHEK_ERR( D3DERR_DEVICEREMOVED, "Hardware device was removed" );
                MENGINE_CHEK_ERR( S_NOT_RESIDENT, "Resource not resident in memory" );
                MENGINE_CHEK_ERR( S_RESIDENT_IN_SHARED_MEMORY, "Resource resident in shared memory" );
                MENGINE_CHEK_ERR( S_PRESENT_MODE_CHANGED, "Desktop display mode has changed" );
                MENGINE_CHEK_ERR( S_PRESENT_OCCLUDED, "Client window is occluded (minimized or other fullscreen)" );
                MENGINE_CHEK_ERR( D3DERR_DEVICEHUNG, "Hardware adapter reset by OS" );
                                
                MENGINE_CHEK_ERR( D3DERR_UNSUPPORTEDOVERLAY, "Overlay is not supported" );
                MENGINE_CHEK_ERR( D3DERR_UNSUPPORTEDOVERLAYFORMAT, "Overlay format is not supported" );
                MENGINE_CHEK_ERR( D3DERR_CANNOTPROTECTCONTENT, "Contect protection not available" );
                MENGINE_CHEK_ERR( D3DERR_UNSUPPORTEDCRYPTO, "Unsupported cryptographic system" );
                MENGINE_CHEK_ERR( D3DERR_PRESENT_STATISTICS_DISJOINT, "Presentation statistics are disjoint" );
#endif

            default:
                return "Unknown error.";
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    DX9ErrorHelper::DX9ErrorHelper( const Char * _file, uint32_t _line, const Char * _method )
        : m_file( _file )
        , m_line( _line )
        , m_method( _method )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9ErrorHelper::operator == ( HRESULT _hr ) const
    {
        if( _hr == S_OK )
        {
            return false;
        }

        const Char * message = Helper::getDX9ErrorMessage( _hr );

        LOGGER_VERBOSE_LEVEL( ConstString::none(), LM_ERROR, LCOLOR_RED, nullptr, 0 )("[DX9] file '%s' line %u call '%s' get error: %s (hr:%x)"
            , m_file
            , m_line
            , m_method
            , message
            , (uint32_t)_hr
            );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}