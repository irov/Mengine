#	pragma once

#   include "Interface/AccountInterface.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

#   include "stdex/binary_vector.h"

#	include "pybind/types.hpp"

namespace Menge
{
    class ServiceProviderInterface;

    class LogServiceInterface;
    class FileServiceInterface;

	class Account
        : public AccountInterface
	{		
	public:
		Account( ServiceProviderInterface * _serviceProvider, const WString & _name, size_t _projectVersion );
		~Account();
        
    public:
        void setFolder( const FilePath & _folder );

	public:
		const WString & getName() const override;
        const FilePath & getFolder() const override;

    public:
		void addSetting( const ConstString & _setting, const WString& _defaultValue, PyObject* _applyFunc ) override;
		void changeSetting( const ConstString & _setting, const WString& _value ) override;
		const WString& getSetting( const ConstString & _setting ) const override;
        bool hasSetting( const ConstString & _setting ) const override;

	public:
		void apply() override;

		bool load() override;
		bool save() override;

    public:
        bool loadBinaryFile( const FilePath & _filename, TBlobject & _data ) override;
        bool writeBinaryFile( const FilePath & _filename, const TBlobject & _data ) override;
	
	protected:
        ServiceProviderInterface * m_serviceProvider;

		WString m_name;
		size_t m_projectVersion;

        FilePath m_folder;
        FilePath m_settingsPath;
        		
        struct Setting
        {
            WString value;
            PyObject * cb;
        };

		typedef stdex::binary_vector<ConstString, Setting> TMapSettings;
		TMapSettings m_settings;
	};
}	// namespace Menge
