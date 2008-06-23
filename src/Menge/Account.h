#	pragma once

#	include "Config/Typedef.h"
#	include "GameSettings.h"

namespace Menge
{
	class Account
	{
	public:
		Account( const String& _name, const String& _path );
		~Account();

		const String& getName() const;
	protected:
		String m_name;
		String m_path;
		GameSettings m_settings;		
	};
}	// namespace Menge