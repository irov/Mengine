#pragma once

#include "Interface/AccountInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

#include "Config/String.h"
#include "Config/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Account
        : public AccountInterface
        , public Factorable
    {
    public:
        Account();
        ~Account() override;

    public:
        bool initialize( const ConstString & _id, const ArchivatorInterfacePtr & _archivator, const FileGroupInterfacePtr & _fileGroup, const FilePath & _folder, uint32_t _projectVersion );

    public:
        const ConstString & getID() const override;
        const FilePath & getFolder() const override;

    public:
        void setUID( const AccountUID & _uid ) override;
        const AccountUID & getUID() const override;


    public:
        bool addSetting( const ConstString & _setting, const Char * _defaultValue, const AccountSettingProviderInterfacePtr & _provider ) override;
        bool changeSetting( const ConstString & _setting, const Char * _value ) override;
        bool getSetting( const ConstString & _setting, Char * _value ) const override;
        bool hasSetting( const ConstString & _setting ) const override;

    public:
        void apply() override;

        bool load() override;
        bool save() override;

    public:
        InputStreamInterfacePtr openReadBinaryFile( const FilePath & _filepath ) override;
        OutputStreamInterfacePtr openWriteBinaryFile( const FilePath & _filepath ) override;

    public:
        MemoryInterfacePtr loadBinaryFile( const FilePath & _filepath ) override;
        bool writeBinaryFile( const FilePath & _filepath, const void * _data, size_t _size ) override;
        bool hasBinaryFile( const FilePath & _filepath ) const override;

    protected:
        ArchivatorInterfacePtr m_archivator;
        FileGroupInterfacePtr m_fileGroup;

        ConstString m_id;
        AccountUID m_uid;
        uint32_t m_projectVersion;

        FilePath m_folder;
        FilePath m_settingsPath;

        struct Setting
        {
            String value;
            AccountSettingProviderInterfacePtr provider;
        };

        typedef Map<ConstString, Setting> MapSettings;
        MapSettings m_settings;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Account> AccountPtr;
    //////////////////////////////////////////////////////////////////////////
}
