#	include "DialogManager.h"

#	include "FileEngine.h"
//////////////////////////////////////////////////////////////////////////
using namespace Menge;
//////////////////////////////////////////////////////////////////////////
DialogManager::DialogManager()
{
	Holder<DialogManager>::keep(this);
	m_localId = -1;
	m_localWidthText = -1;
	m_localText.reserve(1000);
	m_localSoundname.reserve(1000);
}
//////////////////////////////////////////////////////////////////////////
DialogManager::~DialogManager()
{
	for(TMapMessageSpot::iterator it = m_messageSpots.begin(); it != m_messageSpots.end(); ++it)
	{
		//	it->second->release();
		delete it->second;
	}
}
//////////////////////////////////////////////////////////////////////////
void	DialogManager::loadMessagesList(const std::string& _xml )
{
	TiXmlDocument * document = Holder<FileEngine>::hostage()
		->loadXml( _xml );

	XML_FOR_EACH_DOCUMENT( document )
	{
		XML_CHECK_NODE_FOR_EACH("Messages")
		{
			XML_CHECK_VALUE_NODE("Message","id",m_localId);
			XML_CHECK_VALUE_NODE("Message","text",m_localText);
			XML_CHECK_VALUE_NODE("Message","sound",m_localSoundname);
			XML_CHECK_VALUE_NODE("Message","width",m_localWidthText);

			m_messageSpots.insert(
				std::make_pair(m_localId,
					new MessageSpot(m_localId,m_localText,m_localSoundname,m_localWidthText)
				)
			);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
MessageSpot*	DialogManager::getMessageSpot(int id)
{
	TMapMessageSpot::iterator it = m_messageSpots.find(id);

	if(it == m_messageSpots.end())
	{
		return NULL;
	}

	return it->second;
}
//////////////////////////////////////////////////////////////////////////