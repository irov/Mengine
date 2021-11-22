#pragma once

#include "Interface/SettingsServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"

namespace Mengine
{
    class SettingsService
        : public ServiceBase<SettingsServiceInterface>
    {
    public:
        SettingsService();
        ~SettingsService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool loadSetting( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc ) override;
        bool unloadSetting( const ConstString & _name ) override;

    public:
        bool loadSettings( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc ) override;

    public:
        const SettingInterfacePtr & getSetting( const ConstString & _name ) const override;
        SettingInterfacePtr removeSetting( const ConstString & _name ) override;

    protected:
        void foreachSettings( const LambdaSettings & _lambda ) const override;

    protected:
        typedef Hashtable<ConstString, SettingInterfacePtr> HashtableSettings;
        HashtableSettings m_settings;
    };
};
