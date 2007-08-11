#	include "TextField.h"

#	include "FileEngine.h"

#	include "RenderEngine.h"

#	include "SoundEngine.h"

#	include "ObjectImplement.h"

#	include "ResourceMessageStorage.h"

#	include "ResourceManager.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(TextField)
	//////////////////////////////////////////////////////////////////////////
	TextField::TextField()
	: m_dialogFont(0)
	, m_isPaused(false)
	, m_nextDialog(false)
	{}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::_activate()
	{
		m_resourceMessageStore = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceMessageStorage>( m_resourceName );

		if( m_resourceMessageStore == 0 )
		{
			return false;
		}

		m_dialogFont = Holder<RenderEngine>::hostage()->loadFont(m_fontFilename);

		if( m_dialogFont == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::_deactivate()
	{
		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceMessageStore );

		Holder<RenderEngine>::hostage()->releaseRenderFont(m_dialogFont);
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::loader(TiXmlElement * _xml)
	{
		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE("ResourceMessageStorage", "File", m_resourceName);
			XML_CHECK_VALUE_NODE("Font", "File", m_fontFilename);
		}

		m_messageSpots.push_back(0);
		m_messageSpots.push_back(1);

		NodeCore::loader(_xml);
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::_render( const mt::mat3f &rwm, const Viewport & _viewPort )
	{
		mt::vec2f pos(0,0);
		for each( const std::string & line in m_lines )
		{
			Holder<RenderEngine>::hostage()
				->renderText( pos, m_dialogFont, line );

			pos.y += m_dialogFont->getHeight();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::_debugRender()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::start()
	{
		m_currentMessageSpot = m_messageSpots.begin();
		m_nextDialog = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::_update(float _timing)
	{
		if (!m_isPaused)
		{
			return;
		}

		if(m_nextDialog)
		{
			m_nextDialog = false;

			if (++m_currentMessageSpot == m_messageSpots.end())
			{
				return;
			}

			Message * message = m_resourceMessageStore->getMessage( *m_currentMessageSpot );

			createFormattedMessage(message->message, m_dialogFont, message->width);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::createFormattedMessage(const std::string& _text, RenderFontInterface* _font, float _width)
	{
		m_lines.clear();

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

		for( TListLine::iterator it = ++words.begin(); it != words.end(); ++it)
		{
			std::string & str = *it;

			float width = 0;

			for each( char symbol in str )
			{
				width += _font->getCharWidth( symbol );
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
}