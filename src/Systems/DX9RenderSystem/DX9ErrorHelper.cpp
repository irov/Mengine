#include "DX9ErrorHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9ErrorHelper::DX9ErrorHelper( const Char * _file, uint32_t _line, const Char * _method )
        : m_file( _file )
        , m_line( _line )
        , m_method( _method )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9ErrorHelper::operator == ( HRESULT _hr )
    {
        if( _hr == S_OK )
        {
            return false;
        }

        LOGGER_ERROR( "file '%s' line %d DX call '%s' get error '%p'"
            , m_file
            , m_line
            , m_method
            , _hr
        );

        return true;
    }
}