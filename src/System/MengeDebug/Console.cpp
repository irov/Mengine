#	include "Console.h"

#	include "Interface/FileSystemInterface.h"
#	include "Menge/Application.h"
#	include "Menge/Holder.h"
#	include "Menge/RenderEngine.h"
#	include "Menge/Application.h"
#	include "Menge/ResourceImageDynamic.h"
#	include "Menge/ResourceManager.h"
#	include "Menge/ScriptEngine.h"
#	include "Menge/Utils.h"
#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Console::Console()
		: m_inputString( "" )
		, m_isEnabled( false )
		, m_background( 0 )
		, m_inputTextPos( 180 )
		, m_maxLines( 7 )
		, m_cursorPos( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Console::~Console()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Console::inititalize( LogSystemInterface* _logSystemInterface )
	{
		bool result = _logSystemInterface->registerLogger( this );

		if( result == false )
		{
			return false;
		}

		m_inputString.reserve( 1024 );

		m_commandHistory.push_back( Utils::emptyString() );

		m_currentHistory = m_commandHistory.begin();

		Holder<ScriptEngine>::hostage()->exec( "import Menge;" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::proccessInput( unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( _key == 41 && _isDown )
		{
			show_();
		}

		if( m_isEnabled == false )
		{
			return;
		}

		if( _key == 29 || _key == 157 || _key == 56 || _key == 184 ||_key == 42
			|| _key == 54 || _key == 41)
		{
			return;
		}

		if (!_isDown)
		{
			return;
		}

		proccessInput_( _key, _char );
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::proccessInput_( unsigned int _key, unsigned int _char )
	{
		switch ( _key )
		{
			case 15://tab
										
					break;

			case 199:
					m_cursorPos = 0;
					break;

			case 207:
					m_cursorPos = m_inputString.length();					
					break;

			case 203:
					if( --m_cursorPos < 0 )
					{
						m_cursorPos = 0;
					}				
					break;

			case 205:
					if( ++m_cursorPos >= m_inputString.length() )
					{
						m_cursorPos = m_inputString.length();
					}
					break;

			case 211:
					if( !m_inputString.empty() && m_cursorPos < m_inputString.length() )
					{
						m_inputString.replace( m_cursorPos,1,Utils::emptyString()  );
					}
					break;

			case 14:
					if( !m_inputString.empty() && m_cursorPos > 0 )
					{
						m_inputString.replace( m_cursorPos - 1,1,Utils::emptyString()  );
						m_cursorPos--;
					}
					break;

			case 200:
					if( ++m_currentHistory == m_commandHistory.end() )
					{
						m_currentHistory = m_commandHistory.begin();
					}

					m_inputString = *m_currentHistory;
					m_cursorPos = m_inputString.length();

					break;

			case 208:
					m_inputString = *m_currentHistory;
					m_cursorPos = m_inputString.length();

					if( m_currentHistory == m_commandHistory.begin() )
					{				
						m_currentHistory = m_commandHistory.end();
					}

					m_currentHistory--;
					break;

			case 28:
					Holder<ScriptEngine>::hostage()->exec( m_inputString );

					addMessageToConsole_( m_inputString );
					addMessageToHistory_( m_inputString );
					
					m_inputString = "";

					m_cursorPos = 0;

					break;

			default:
					m_inputString.insert( m_cursorPos, 1, _char );
					m_cursorPos++;
					//m_inputString += _char;
					//m_cursorPos = m_inputString.length();
					break;
					
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::addMessageToConsole_( const String& _message )
	{
		if( m_text.size() > m_maxLines )
		{
			m_text.pop_front();
		}

		m_text.push_back( _message );
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::addMessageToHistory_( const String& _message )
	{
		TStringSet::iterator it = m_commandSet.find( _message );

		if( it != m_commandSet.end() )
		{
			return;
		}

		m_commandSet.insert( m_inputString );
		m_commandHistory.push_back( m_inputString );
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::render()
	{
		if( m_isEnabled == false )
		{
			return;
		}

		Holder<RenderEngine>::hostage()
		->setRenderArea( mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		Holder<RenderEngine>::hostage()->renderImage( m_renderVertices,
										mt::vec4f( 0.0f, 0.0f, 1.0f, 1.0f ),
										0x8FFFFFFF, m_background 
										);

		mt::vec2f pos(0,20);

		for( TStringList::iterator it = m_text.begin();
			it != m_text.end(); 
			it++)
		{
			const String & line = *it;

			Holder<RenderEngine>::hostage()->renderText(
				line, pos, 0xFF000000 );

			pos.y += 20;
		}

		Holder<RenderEngine>::hostage()->renderText(
				m_inputString, pos, 0xFF000000 );
	
		Holder<RenderEngine>::hostage()
			->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::show_()
	{
		m_isEnabled = !m_isEnabled;

		if( m_isEnabled == true )
		{
			changeSettings();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::changeSettings()
	{
		ResourceImageDynamic * resource 
				= Holder<ResourceManager>::hostage()->
					getResourceT<ResourceImageDynamic>("WhitePixel");

		m_background = resource->getImage(0);

		Resolution resolution = Holder<Application>::hostage()->getCurrentResolution();

		float width = resolution.getWidth();
		float height = 0.3 * resolution.getHeight();

		m_renderVertices[0] = mt::vec2f( 0.0f, 0.0f );
		m_renderVertices[1] = mt::vec2f( width, 0.0f );
		m_renderVertices[2] = mt::vec2f( width, height );
		m_renderVertices[3] = mt::vec2f( 0.0f, height );
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::write( const String& _str )
	{
		addMessageToConsole_( _str );
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::write( const void* _data, std::streamsize _count )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::write( int _num )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::write( float _num )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::flush()
	{
	}
	//////////////////////////////////////////////////////////////////////////
}