#	pragma once

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

#	include <map>

#	include "pybind/types.hpp"

namespace Menge
{
	class Account
	{		
	public:
		Account( const WString & _name );
		~Account();

	public:
		const WString& getName() const;

		void addSetting( const WString& _setting, const WString& _defaultValue, PyObject* _applyFunc );
		void changeSetting( const WString& _setting, const WString& _value );
		const WString& getSetting( const WString& _setting ) const;

	public:
		void apply();

		bool load();
		void save();
	
	protected:
		WString m_name;

		typedef std::pair<WString, PyObject*> TPairSettings;
		typedef std::map<WString, TPairSettings> TMapSettings;
		TMapSettings m_settings;
	};
}	// namespace Menge
