#include "ResourceDebuggerPlugin.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/PluginHelper.h"

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
        ResourceUselessCompileCheckerPtr resourceUselessCompileChecker = Helper::makeFactorableUnique<ResourceUselessCompileChecker>( MENGINE_DOCUMENT_FACTORABLE );

        if( resourceUselessCompileChecker->initialize() == false )
        {
            return false;
        }

        m_resourceUselessCompileChecker = resourceUselessCompileChecker;

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
    //////////////////////////////////////////////////////////////////////////
}