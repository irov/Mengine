#	pragma once

#   include "Interface/AccountInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "Config/Typedef.h"

#   include "Core/ServantBase.h"
#	include "Core/ConstString.h"

#   include "stdex/stl_map.h"

#	include "pybind/object.hpp"

namespace Menge
{
	class Account
        : public ServantBase<AccountInterface>
	{		
	public:
		Account();
		~Account();

	public:
		bool initialize( const WString & _name, const FilePath & _folder, uint32_t _projectVersion );
        
	public:
		const WString & getID() const override;
		const String & getUID() const override;
        const FilePath & getFolder() const override;

    public:
		bool addSetting( const ConstString & _setting, const WString& _defaultValue, const pybind::object & _applyFunc ) override;
		bool changeSetting( const ConstString & _setting, const WString& _value ) override;
		WString getSetting( const ConstString & _setting ) const override;
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

		WString m_name;
		String m_uid;
		uint32_t m_projectVersion;

		FilePath m_folder;
        FilePath m_settingsPath;
        		
        struct Setting
        {
            WString value;
            pybind::object cb;
        };

		typedef stdex::map<ConstString, Setting> TMapSettings;
		TMapSettings m_settings;
	};

	typedef stdex::intrusive_ptr<Account> AccountPtr;
}	// namespace Menge
