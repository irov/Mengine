#	pragma once
#	include <map>

#	include "MessageSpot.h"

namespace	Menge
{
	typedef std::map<int,MessageSpot*> TMapMessageSpot;

	/*	Паттерн Holder.
		Контейнер сообщений - MessageSpot	*/
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
		/*	Пришлось вынести в область видимости класса, потому что иногда падало
			во вложенных блоках при парсинге xml.	
			Добавил префикс local.	*/
		int	m_local_id;
		float m_local_width_text;
		std::string m_local_text;		
		std::string m_local_soundname;
	};
}