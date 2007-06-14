#	include "DialogManager.h"
#	include "XmlParser.h"
#	include "Holder.h"
#	include "FileEngine.h"

using namespace Menge;

DialogManager::DialogManager()
{
	Holder<DialogManager>::keep(this);
	m_local_id = -1;
	m_local_width_text = -1;
	m_local_text.reserve(1000);
	m_local_soundname.reserve(1000);
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
	XML_PARSE_FILE_EX(_filename)
	{
		XML_CHECK_NODE_FOR_EACH("Messages")
		{
			XML_CHECK_VALUE_NODE("Message","id",m_local_id);
			XML_CHECK_VALUE_NODE("Message","text",m_local_text);
			XML_CHECK_VALUE_NODE("Message","sound",m_local_soundname);
			XML_CHECK_VALUE_NODE("Message","width",m_local_width_text);

			m_messageSpots.insert(
				std::make_pair(m_local_id,
					new MessageSpot(m_local_id,m_local_text,m_local_soundname,m_local_width_text)
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