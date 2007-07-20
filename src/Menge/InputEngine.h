#	pragma once

#	include "Math/vec3.h"

#	include <string>
#	include <map>

#	include "Interface/InputSystemInterface.h"

#	include "Holder.h"

namespace Menge
{
	class ScriptEngine;

	class InputHandler;

	class InputEngine
	{
	public:
		InputEngine( InputSystemInterface * _interface );
		~InputEngine();

	public:
		void update();

	public:
		bool isKey(unsigned int index,int key_state)const;
		bool isAnyKeyDown()const;
		bool getChar(char *ch,int key_state)const;

		void regHandle( InputHandler * _handle );


		void setPosition( float _x, float _y, float _whell );
		void setSensitivity( float _sensitivity );
		void setRange ( const mt::vec3f &_minRange, const mt::vec3f & _maxRange );

		const mt::vec3f & getPosition() const;
		const mt::vec3f & getDelta() const;
		//mouse

		bool isMove()const;
		bool isAnyButtonDown()const;
		bool isButton(int but,int but_state)const;

	protected:
		InputSystemInterface * m_interface;
		InputHandler * m_handle;
	};
}