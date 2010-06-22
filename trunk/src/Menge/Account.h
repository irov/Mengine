#	pragma once

#	include "Config/Typedef.h"

#	include "Loadable.h"

#	include <map>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class Account
		: public Loadable
	{		
	public:
		Account( /*const String& _name,*/ const String& _folder );
		~Account();

	public:
		//const String& getName() const;
		const String& getFolder() const;

		void addSetting( const String& _setting, const String& _defaultValue, PyObject* _applyFunc );
		void changeSetting( const String& _setting, const String& _value );
		const String& getSetting( const String& _setting );

		void addSettingU( const String& _setting, const String& _defaultValue, PyObject* _applyFunc );
		void changeSettingU( const String& _setting, const String& _value );
		const String& getSettingU( const String& _setting );

		void apply();

		void load();
		void save();

	public:
		void loader( XmlElement* _xml ) override;
		void parser( BinParser * _parser ) override;

	protected:
		//String m_name;
		String m_folder;

		typedef std::map< String, std::pair<String, PyObject*> > TSettingsMap;
		TSettingsMap m_settings;
		TSettingsMap m_settingsU;
	};
}	// namespace Menge
