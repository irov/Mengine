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
	, m_isPlaying(false)
	, m_total_delay(0.0f)
	, m_delay(1000)
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

		//m_dialogFont = Holder<RenderEngine>::hostage()->loadFont(m_fontFilename);

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

		//Holder<RenderEngine>::hostage()->releaseRenderFont(m_dialogFont);
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::loader(TiXmlElement * _xml)
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE("ResourceMessageStorage", "File", m_resourceName);
			XML_CHECK_VALUE_NODE("Font", "File", m_fontFilename);
		}

		m_messageSpots.push_back(0);
		m_messageSpots.push_back(1);
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::_render( const mt::mat3f &rwm, const Viewport & _viewPort )
	{
		mt::vec2f pos(0,0);
		for each( const std::string & line in m_renderLines )
		{
			//Holder<RenderEngine>::hostage()
			//	->renderText( pos, m_dialogFont, line );

			//pos.y += m_dialogFont->getHeight();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::_debugRender()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const Message *	TextField::start()
	{
		m_isPlaying = true;
		m_currentMessageSpot = m_messageSpots.begin();

		const Message *	message = m_resourceMessageStore->getMessage( *m_currentMessageSpot );

		return message;
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::_update( size_t _timing )
	{
		if(m_isPlaying == false) return;

		m_total_delay += _timing;

		while(m_total_delay >= m_delay)
		{
			m_total_delay -= m_delay;
			getNextMessage();
		}
	}
	//////////////////////////////////////////////////////////////////////////	
	const Message * TextField::getNextMessage()
	{
		if (++m_currentMessageSpot == m_messageSpots.end())
		{
			m_isPlaying = false;
			return NULL;
		}

		const Message * message = m_resourceMessageStore->getMessage( *m_currentMessageSpot );

		_createFormattedMessage(message->message, m_dialogFont, message->width);

		return message;
	}
	//////////////////////////////////////////////////////////////////////////
	bool	TextField::isPlaying() const
	{
		return m_isPlaying;
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::_createFormattedMessage(const std::string& _text, RenderFontInterface* _font, float _width)
	{
		m_renderLines.clear();

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
				//width += _font->getCharWidth( symbol );
			}

			pixelFontSize += width;

			if(pixelFontSize < _width)
			{
				temp_string += " ";
				temp_string += *it;
			}
			else
			{
				m_renderLines.push_back(temp_string);
				temp_string.clear();
				temp_string += *it;
				pixelFontSize = 0.0f;
			}
		}

		m_renderLines.push_back(temp_string);
	}
}