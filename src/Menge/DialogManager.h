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
		MessageSpot*	getMessageSpot(int _id);
	private:
		TMapMessageSpot m_messageSpots;
	private:
		int	m_localId;
		float m_localWidthText;
		std::string m_localText;		
		std::string m_localSoundname;
	};
}