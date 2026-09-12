#include "FontEffectViewerExampleFileDialog.h"

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSelectFilePath( const Char * _title, const Char * _currentPath, Char * const _path, size_t _capacity )
    {
        MENGINE_UNUSED( _title );
        MENGINE_UNUSED( _currentPath );
        MENGINE_UNUSED( _path );
        MENGINE_UNUSED( _capacity );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSelectSaveFilePath( const Char * _title, const Char * _currentPath, const Char * _defaultName, Char * const _path, size_t _capacity )
    {
        MENGINE_UNUSED( _title );
        MENGINE_UNUSED( _currentPath );
        MENGINE_UNUSED( _defaultName );
        MENGINE_UNUSED( _path );
        MENGINE_UNUSED( _capacity );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
