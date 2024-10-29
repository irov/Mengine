#include "Win32FindPython3Plugin.h"

#include "Interface/SocketSystemInterface.h"

#include "Kernel/PluginHelper.h"
#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( Win32FindPython3Service );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32FindPython3, Mengine::Win32FindPython3Plugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FindPython3Plugin::Win32FindPython3Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FindPython3Plugin::~Win32FindPython3Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FindPython3Plugin::_initializePlugin()
    {
        SERVICE_CREATE( Win32FindPython3Service, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FindPython3Plugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( Win32FindPython3Service );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FindPython3Plugin::_destroyPlugin()
    {
        SERVICE_DESTROY( Win32FindPython3Service );
    }
    //////////////////////////////////////////////////////////////////////////
}