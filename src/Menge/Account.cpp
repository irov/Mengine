
#	include "Account.h"
#	include "LogEngine.h"
#	include "pybind/pybind.hpp"
#	include "Utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Account::Account( const String& _name )
		: m_name( _name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Account::~Account()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Account::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::addSetting( const String& _setting, PyObject* _applyFunc )
	{
		TSettingsMap::iterator it = m_settings.find( _setting );
		if( it == m_settings.end() )
		{
			m_settings[_setting] = std::make_pair( "", _applyFunc );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::changeSetting( const String& _setting, const String& _value )
	{
		TSettingsMap::iterator it = m_settings.find( _setting );
		if( it != m_settings.end() )
		{
			pybind::call( it->second.second, "" );
			it->second.first = _value;
		}
		else
		{
			MENGE_LOG( "Error: setting '%s' does not exist. Can't change",
				_setting.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Account::getSetting( const String& _setting )
	{
		TSettingsMap::iterator it = m_settings.find( _setting );
		if( it != m_settings.end() )
		{
			return it->second.first;
		}
		else
		{
			MENGE_LOG( "Error: setting '%s' does not exist. Can't get",
				_setting.c_str() );
		}
		return emptyString();
	}
	//////////////////////////////////////////////////////////////////////////
	void Account::load()
	{

	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge