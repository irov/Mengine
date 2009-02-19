#	pragma once

#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/LogSystemInterface.h"

#	include "math/vec2.h"

#	include <list>

namespace Menge
{
	class RenderImageInterface;

	class Console 
		: public OutStreamInterface
	{
	public:
		Console();
		~Console();

	public:
		bool inititalize( LogSystemInterface* _logSystemInterface );
		void show();
		void render();
		void changeSettings();
		void onKeyEvent( unsigned int _key, unsigned int _char, bool _isDown );

	private:

		void addMessage_( const String& _message );

		void write( const void* _data, std::streamsize _count ) override;
		void write( const String& _str ) override;
		void write( int _num ) override;
		void write( float _num ) override;
		void flush() override;

		typedef std::list<Menge::String> TStringList;
		TStringList m_text;
		TStringList m_commandHistory;
		TStringList::iterator m_currentHistory;

		Menge::String m_inputString;

		bool m_isEnabled;
		float m_inputTextPos;
		int m_maxLines;

		RenderImageInterface * m_background;
		mt::vec2f m_renderVertices[4];
	};
}