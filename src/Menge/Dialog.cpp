#	include "Dialog.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "XmlParser.h"
#	include "DialogManager.h"
#	include "ObjectImplement.h"
#	include "MessageSpot.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Dialog)
//////////////////////////////////////////////////////////////////////////
bool	Dialog::_compile()
{
	m_dialogFont = Holder<RenderEngine>::hostage()->loadFont(m_fontFilename);
	m_soundSource = 0;
	m_fileData = 0;
	m_isUpdate = false;
	return true;
}
//////////////////////////////////////////////////////////////////////////
void	Dialog::loader(TiXmlElement * _xml)
{
	int id = -1;
	MessageSpot* elem = NULL;

	XML_FOR_EACH_TREE(_xml)
	{
		XML_CHECK_NODE_FOR_EACH("Messages")
		{
			XML_CHECK_VALUE_NODE("Message","id",id);

			elem = Holder<DialogManager>::hostage()->getMessageSpot(id);

			m_messageSpots.push_back(elem);
		}
		XML_CHECK_VALUE_NODE("Font", "File", m_fontFilename);
	}

	NodeImpl::loader(_xml);
}
//////////////////////////////////////////////////////////////////////////
void	Dialog::_release()
{
	Holder<RenderEngine>::hostage()->releaseRenderFont(m_dialogFont);
	m_dialogFont = 0;
}
//////////////////////////////////////////////////////////////////////////
void	Dialog::loadCurrentMessageSpot()
{
	MessageSpot * it = *m_currentMessageSpot;

	std::string text = it->getText();

	assert(text.empty() == false);

	createFormattedMessage(text, m_dialogFont, it->getWidth());

	const std::string& soundName = it->getSoundName();

	if(soundName.empty() == false)
	{
/*		if(	Holder<SoundEngine>::hostage()->addSoundNode(
				m_soundSource,	m_fileData,	soundName,0, true) == false
			)
		{
			assert(!"Sound for dialog is not loaded");
		}
		m_soundSource->play();
		*/
	}
	else
	{	//означает что нету звука для данного мессаджа.
		m_soundSource = NULL;
	}

	m_isUpdate = true;
}
//////////////////////////////////////////////////////////////////////////
void	Dialog::start()
{
	m_lines.clear();

	m_isUpdate = false;

	m_currentMessageSpot = m_messageSpots.begin();

	loadCurrentMessageSpot();
}
//////////////////////////////////////////////////////////////////////////
void	Dialog::nextMessageSpot()
{
	Holder<SoundEngine>::hostage()->deleteSound(m_soundSource);
	Holder<FileEngine>::hostage()->closeFile(m_fileData);
	m_soundSource = NULL;
	m_fileData = NULL;

	m_lines.clear();

	if (++m_currentMessageSpot == m_messageSpots.end())
	{
		m_isUpdate = false;
//		release();
		return;
	}

	loadCurrentMessageSpot();
}
//////////////////////////////////////////////////////////////////////////
void	Dialog::_update(float _timing)
{
	if (!m_isUpdate)
	{
		return;
	}

	if (m_lines.empty() == false)
	{
		mt::vec2f pos(0,0);
		for (TListLine::iterator it = m_lines.begin(); it != m_lines.end(); ++it)
		{
			Holder<RenderEngine>::hostage()->renderText(pos,m_dialogFont,*it);
			pos.y+=m_dialogFont->getHeight();
		}
	}

	if(m_soundSource != NULL)
	{
		//m_soundSource->process();

		if(m_soundSource->isPlaying() == false)
		{
			nextMessageSpot();
			return;
		}
	}
	else
	{

		//ээ, как то выставляет время проигрыша мессаги без звука?
	}
}
//////////////////////////////////////////////////////////////////////////
void	Dialog::createFormattedMessage(const std::string& _text, RenderFontInterface* _font, float _width)
{
	std::list<std::string> words;

	size_t endpos = 0;

	std::string::size_type pos = _text.find_first_not_of(" ");
	
	if(pos != std::string::npos)
	{
		endpos = _text.find_first_of(" ",pos+1);
	}

	while((pos < endpos) && (pos != std::string::npos) && (endpos != std::string::npos))
	{
		words.push_back(
			_text.substr(pos,endpos-pos)
			);

		pos = _text.find_first_not_of(" ",endpos+1);
		endpos = _text.find_first_of(" ",pos+1);
	}

	if(endpos == std::string::npos)
	{
		words.push_back(
			_text.substr(pos,_text.size())
			);
	}

	float pixelFontSize = 0;

	std::string	temp_string = *words.begin();

	for (TListLine::iterator it = ++words.begin(); it != words.end(); ++it)
	{
		std::string& str = *it;

		float width = 0;

		for (std::string::iterator symbol = str.begin(),
			sym_end = str.end(); symbol != sym_end; ++symbol)
		{
			width+=_font->getCharWidth(*symbol);
		}

		pixelFontSize += width;

		if(pixelFontSize < _width)
		{
			temp_string += " ";
			temp_string += *it;
		}
		else
		{
			m_lines.push_back(temp_string);
			temp_string.clear();
			temp_string += *it;
			pixelFontSize = 0.0f;
		}
	}

	m_lines.push_back(temp_string);
}
