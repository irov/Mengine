#	pragma once

#	pragma once

#	include "Node.h"

#	include "InputHandler.h"
#	include "MousePickerTrap.h"

#	include "Math/polygon.h"
#	include "Node.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/LogSystemInterface.h"

#	 include <list>

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

		void write( const void* _data, std::streamsize _count ) override;
		void write( const String& _str ) override;
		void write( int _num ) override;
		void write( float _num ) override;
		void flush() override;

		std::list<Menge::String> m_text;
		Menge::String m_inputString;

		bool m_isEnabled;
		float m_inputTextPos;

		RenderImageInterface * m_background;
		mt::vec2f m_renderVertices[4];
	};
}