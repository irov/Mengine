#pragma once

#include "Interface/AccountInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Account
        : public AccountInterface
    {
        DECLARE_FACTORABLE( Account );

    public:
        Account();
        ~Account() override;

    public:
        bool initialize( const ConstString & _id, const ArchivatorInterfacePtr & _archivator, const FileGroupInterfacePtr & _fileGroup, const FilePath & _folderPath, uint32_t _projectVersion );
        void finalize();

    public:
        const ConstString & getAccountId() const override;
        const FilePath & getFolderName() const override;

    public:
        void setUID( const AccountUID & _uid ) override;
        const AccountUID & getUID() const override;

    public:
        bool addSetting( const ConstString & _setting, const Char * _defaultValue, const AccountSettingProviderInterfacePtr & _provider ) override;
        bool changeSetting( const ConstString & _setting, const Char * _value ) override;
        bool getSetting( const ConstString & _setting, const Char ** const _value ) const override;
        bool hasSetting( const ConstString & _setting, const Char ** const _value ) const override;

    public:
        void apply() override;

    public:
        bool load() override;
        bool save() override;

    protected:
        ConfigInterfacePtr getLoadSettingConfig_() const;

    public:
        InputStreamInterfacePtr openReadBinaryFile( const FilePath & _filePath ) override;
        void closeReadBinaryFile( const InputStreamInterfacePtr & _stream ) override;

        OutputStreamInterfacePtr openWriteBinaryFile( const FilePath & _filePath ) override;
        void closeWriteBinaryFile( const OutputStreamInterfacePtr & _stream ) override;

    public:
        MemoryInterfacePtr loadBinaryFile( const FilePath & _filepath ) override;
        bool writeBinaryFile( const FilePath & _filepath, const void * _data, size_t _size ) override;
        bool hasBinaryFile( const FilePath & _filepath ) const override;

    protected:
        ArchivatorInterfacePtr m_archivator;
        FileGroupInterfacePtr m_fileGroup;

        ConstString m_accountId;
        AccountUID m_uid;
        uint32_t m_projectVersion;

        FilePath m_folderName;

        ContentInterfacePtr m_settingsJSONContent;
        ContentInterfacePtr m_settingsINIContent;        

        struct Setting
        {
            String value;
            AccountSettingProviderInterfacePtr provider;
        };

        typedef Map<ConstString, Setting> MapSettings;
        MapSettings m_settings;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Account, AccountInterface> AccountPtr;
    //////////////////////////////////////////////////////////////////////////
}
