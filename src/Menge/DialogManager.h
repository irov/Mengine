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
	};
};