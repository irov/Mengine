#	pragma once
#	include <map>

#	include "MessageSpot.h"

namespace	Menge
{
	typedef std::map<int,MessageSpot*> TMapMessageSpot;

	class DialogManager
	{
	public:
		DialogManager();
		~DialogManager();
		void	loadMessagesList(const std::string& _filename);
		MessageSpot*	getMessageSpot(int id);
	private:
		TMapMessageSpot m_messageSpots;
	private:
		int	m_local_id;
		float m_local_width_text;
		std::string m_local_text;		
		std::string m_local_soundname;
	};
}