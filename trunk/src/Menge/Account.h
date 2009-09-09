#	pragma once

#	include "Config/Typedef.h"

#	include <map>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

class XmlElement;

namespace Menge
{

	class Account
	{
		typedef std::map< String, std::pair<String, PyObject*> > TSettingsMap;
	public:
		Account( const String& _name );
		~Account();

		const String& getName() const;

		void addSetting( const String& _setting, const String& _defaultValue, PyObject* _applyFunc );
		void changeSetting( const String& _setting, const String& _value );
		const String& getSetting( const String& _setting );

		void apply();

		void load();
		void loader_( XmlElement* _xml );

		void save();

	protected:
		String m_name;
		TSettingsMap m_settings;
	};
}	// namespace Menge
