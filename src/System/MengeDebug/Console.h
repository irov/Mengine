#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/LogSystemInterface.h"
#	include "Interface/ApplicationInterface.h"

#	include <list>
#	include <set>

#	include "math/vec2.h"

namespace Menge
{
	class RenderImageInterface;

	class Console 
		: public ConsoleInterface, public OutStreamInterface
	{
	public:
		Console();
		~Console();

	public:
		bool inititalize( LogSystemInterface* _logSystemInterface );
		void render();
		void changeSettings();
		void proccessInput( unsigned int _key, unsigned int _char, bool _isDown );

	private:

		void addMessageToHistory_( const String& _message );
		void addMessageToConsole_( const String& _message );
		void proccessInput_( unsigned int _key, unsigned int _char );

		void write( const void* _data, std::streamsize _count ) override;
		void write( const String& _str ) override;
		void write( int _num ) override;
		void write( float _num ) override;
		void flush() override;

		typedef std::list<Menge::String> TStringList;

		TStringList m_text;
		TStringList m_commandHistory;
		TStringList::iterator m_currentHistory;

		typedef std::set<Menge::String> TStringSet;
		
		TStringSet m_commandSet;

		Menge::String m_inputString;

		bool m_isEnabled;
		float m_inputTextPos;
		int m_maxLines;
		int m_cursorPos;

		void show_();

		RenderImageInterface * m_background;
		mt::vec2f m_renderVertices[4];
	};
}