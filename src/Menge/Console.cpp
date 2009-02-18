#	include "Console.h"
#	include "Holder.h"
#	include "RenderEngine.h"
#	include "Application.h"
#	include "ResourceImageDynamic.h"
#	include "ResourceManager.h"
#	include "ScriptEngine.h"
#	include "Utils.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Console::Console()
		: m_inputString( "" )
		, m_isEnabled( false )
		, m_background( 0 )
		, m_inputTextPos( 180 )
		, m_maxLines( 7 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Console::~Console()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Console::inititalize( LogSystemInterface* _logSystemInterface )
	{
		m_inputString.reserve( 1024 );

		m_commandHistory.push_back( Utils::emptyString() );

		m_currentHistory = m_commandHistory.begin();

		return _logSystemInterface->registerLogger( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::write( const void* _data, std::streamsize _count )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::write( const String& _str )
	{
		if( m_text.size() > m_maxLines )
		{
			m_text.pop_front();
		}

		m_text.push_back( _str );
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
	void Console::onKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( m_isEnabled == false )
		{
			return;
		}

		if( _key == 56 || _key == 184 ||_key == 42
			|| _key == 54 || _key == 41 || _key == 203
			|| _key == 205 ) // Shift or ~
		{
			return;
		}

		if( _isDown && _key == 14 ) //Backspace
		{
			if( !m_inputString.empty() )
			{
				m_inputString.erase( m_inputString.end() - 1);
			}
		}
		else if(_isDown && _key == 200)	//Up arrow
		{
			if( ++m_currentHistory == m_commandHistory.end() )
			{
				m_currentHistory = m_commandHistory.begin();
			}

			m_inputString = *m_currentHistory;
		}
		else if(_isDown && _key == 208) // Down arrow
		{
			m_inputString = *m_currentHistory;

			if( m_currentHistory == m_commandHistory.begin() )
			{				
				m_currentHistory = m_commandHistory.end();
			}

			m_currentHistory--;
		}
		else if( _key == 28 && _isDown ) // Enter
		{
			Holder<ScriptEngine>::hostage()->exec( m_inputString );

			m_commandHistory.push_back( m_inputString );

			m_inputString = "";
		}
		else if( _key != 28 && _isDown)
		{
				m_inputString += _char;
		}
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

			pos.y +=20;
		}

		Holder<RenderEngine>::hostage()->renderText(
				m_inputString, pos, 0xFF000000 );
	
		Holder<RenderEngine>::hostage()
			->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Console::show()
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
		float height = 200;

		m_renderVertices[0] = mt::vec2f( 0.0f, 0.0f );
		m_renderVertices[1] = mt::vec2f( width, 0.0f );
		m_renderVertices[2] = mt::vec2f( width, height );
		m_renderVertices[3] = mt::vec2f( 0.0f, height );
	}
	//////////////////////////////////////////////////////////////////////////
}
