#include "ResourceDebuggerPlugin.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ResourceDebugger, Mengine::ResourceDebuggerPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceDebuggerPlugin::ResourceDebuggerPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceDebuggerPlugin::~ResourceDebuggerPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceDebuggerPlugin::_initialize()
    {
        m_resourceUselessCompileChecker = new FactorableUnique<ResourceUselessCompileChecker>();

        if( m_resourceUselessCompileChecker->initialize() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceDebuggerPlugin::_finalize()
    {
        if( m_resourceUselessCompileChecker != nullptr )
        {
            m_resourceUselessCompileChecker->finalize();
            m_resourceUselessCompileChecker = nullptr;
        }
    }
}