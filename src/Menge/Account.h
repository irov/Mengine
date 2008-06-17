#	pragma once

#	include "Config/Typedef.h"
#	include "GameSettings.h"

namespace Menge
{
	class Account
	{
	public:
		Account();
		~Account();

	protected:
		String m_name;
		GameSettings m_settings;		
	};
}	// namespace Menge