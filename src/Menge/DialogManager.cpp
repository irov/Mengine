#	include "DialogManager.h"
#	include "XmlParser.h"
#	include "Keeper.h"
#	include "FileEngine.h"


namespace Menge
{
	DialogManager::DialogManager()
	{
		Keeper<DialogManager>::keep(this);
	}

	DialogManager::~DialogManager()
	{
		for(TMapMessageSpot::iterator it = m_messageSpots.begin(); it != m_messageSpots.end(); ++it)
		{
			//	it->second->release();
			delete it->second;
		}
	}

	void	DialogManager::loadMessagesList(const std::string& _filename)
	{
		std::string playListFileName;

		int id = -1;
		std::string text;
		std::string sound;
		float width;

		XML_PARSE_FILE_EX(_filename)
		{
			XML_CHECK_NODE_FOR_EACH("Messages")
			{
				XML_CHECK_VALUE_NODE("Message","id",id);
				XML_CHECK_VALUE_NODE("Message","text",text);
				XML_CHECK_VALUE_NODE("Message","sound",sound);
				XML_CHECK_VALUE_NODE("Message","width",width);

				m_messageSpots.insert(
					std::make_pair(id,
					new MessageSpot(id,text,sound,width)
					)
					);
			}
		}
	}

	MessageSpot*	DialogManager::getMessageSpot(int id)
	{
		TMapMessageSpot::iterator it = m_messageSpots.find(id);

		if(it == m_messageSpots.end())
		{
			return NULL;
		}

		return it->second;
	}
};