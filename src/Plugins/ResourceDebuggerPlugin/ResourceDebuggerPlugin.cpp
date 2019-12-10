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
    bool ResourceDebuggerPlugin::_initializePlugin()
    {
        m_resourceUselessCompileChecker = Helper::makeFactorableUnique<ResourceUselessCompileChecker>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_resourceUselessCompileChecker->initialize() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceDebuggerPlugin::_finalizePlugin()
    {
        if( m_resourceUselessCompileChecker != nullptr )
        {
            m_resourceUselessCompileChecker->finalize();
            m_resourceUselessCompileChecker = nullptr;
        }
    }
}