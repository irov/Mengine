#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/SettingInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class SettingsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SettingsService" );

    public:
        virtual bool loadSetting( const ConstString & _name, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool saveSetting( const ConstString & _name, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool unloadSetting( const ConstString & _name ) = 0;

    public:
        virtual bool loadSettings( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool saveSettings( const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual const SettingInterfacePtr & getSetting( const ConstString & _name ) const = 0;
        virtual SettingInterfacePtr removeSetting( const ConstString & _name ) = 0;

    public:
        typedef Lambda<void( const ConstString &, const SettingInterfacePtr & )> LambdaSettings;
        virtual void foreachSettings( const LambdaSettings & _lambda ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SETTINGS_SERVICE()\
    ((Mengine::SettingsServiceInterface *)SERVICE_GET(Mengine::SettingsServiceInterface))
//////////////////////////////////////////////////////////////////////////