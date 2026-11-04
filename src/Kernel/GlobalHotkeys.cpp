#include "GlobalHotkeys.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GlobalHotkeys::GlobalHotkeys()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GlobalHotkeys::~GlobalHotkeys()
    {
        MENGINE_ASSERTION_FATAL( m_hotkeys.empty() == true, "hotkeys not removed" );
    }
    //////////////////////////////////////////////////////////////////////////
    void GlobalHotkeys::addHotkey( EKeyCode _code, const Helper::LambdaInputKeyEvent & _event, const DocumentInterfacePtr & _doc )
    {
        UniqueId id = Helper::addGlobalKeyHandler( _code, true, _event, _doc );

        m_hotkeys.emplace_back( id );
    }
    //////////////////////////////////////////////////////////////////////////
    void GlobalHotkeys::removeHotkeys()
    {
        for( UniqueId id : m_hotkeys )
        {
            Helper::removeGlobalHandler( id );
        }

        m_hotkeys.clear();
    }
    //////////////////////////////////////////////////////////////////////////
}
