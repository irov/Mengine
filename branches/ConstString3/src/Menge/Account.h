#	pragma once

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

#	include <map>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class Account
	{		
	public:
		Account( const String& _folder );
		~Account();

	public:
		//const String& getName() const;
		const ConstString& getFolder() const;

		void addSetting( const String& _setting, const String& _defaultValue, PyObject* _applyFunc );
		void changeSetting( const String& _setting, const String& _value );
		const String& getSetting( const String& _setting );

		void addSettingU( const String& _setting, const String& _defaultValue, PyObject* _applyFunc );
		void changeSettingU( const String& _setting, const String& _value );
		const String& getSettingU( const String& _setting );

		void apply();

		void load();
		void save();
	
	protected:
		//String m_name;
		ConstString m_folder;

		typedef std::pair<String, PyObject*> TPairSettings;
		typedef std::map<String, TPairSettings> TMapSettings;
		TMapSettings m_settings;
		TMapSettings m_settingsU;
	};
}	// namespace Menge
