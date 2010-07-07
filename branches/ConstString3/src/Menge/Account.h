#	pragma once

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

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
		Account( const String& _folder );
		~Account();

	public:
		//const String& getName() const;
		const ConstString& getFolder() const;

		void addSetting( const String& _setting, const String& _defaultValue, PyObject* _applyFunc );
		void changeSetting( const String& _setting, const String& _value );
		const String& getSetting( const String& _setting );

		void apply();

		void load();
		void save();

	public:
		void loader( BinParser * _parser ) override;

	protected:
		void _loaded() override;

	protected:
		//String m_name;
		ConstString m_folder;

		typedef std::pair<String, PyObject*> TPairSettings;
		typedef std::map<String, TPairSettings> TMapSettings;
		TMapSettings m_settings;
	};
}	// namespace Menge
