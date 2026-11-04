#include "DevelopmentHotkeyReloadLocaleSceneChange.h"

#include "Interface/ApplicationInterface.h"

#include "Kernel/NotificationHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevelopmentHotkeyReloadLocaleSceneChange::DevelopmentHotkeyReloadLocaleSceneChange()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevelopmentHotkeyReloadLocaleSceneChange::~DevelopmentHotkeyReloadLocaleSceneChange()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentHotkeyReloadLocaleSceneChange::onSceneChange( const ScenePtr & _scene, bool _enable, bool _remove, bool _error )
    {
        MENGINE_UNUSED( _enable );
        MENGINE_UNUSED( _remove );
        MENGINE_UNUSED( _error );

        if( _scene != nullptr )
        {
            return;
        }

        const ConstString & locale = APPLICATION_SERVICE()
            ->getLocale();

        NOTIFICATION_NOTIFY( NOTIFICATOR_RELOAD_LOCALE_PREPARE );
        NOTIFICATION_NOTIFY( NOTIFICATOR_RELOAD_LOCALE );
        NOTIFICATION_NOTIFY( NOTIFICATOR_RELOAD_LOCALE_POST );

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE_PREPARE, locale, locale );
        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE, locale, locale );
        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE_POST, locale, locale );
    }
    //////////////////////////////////////////////////////////////////////////
}
