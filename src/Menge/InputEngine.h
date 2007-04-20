#	pragma once

#	include "SystemDLL.h"
#	include "Holder.h"

#	include "InputSystemInterface.h"

#	include "Math/vec3.h"

#	include <string>
#	include <map>

namespace Menge
{
	class InputManager;
	class ScriptEngine;

	class InputEngine
		: public SystemDLL<InputSystemInterface>
	{
	public:
		InputEngine(const std::string &_dllModule);
		~InputEngine();

	public:
		bool init();
		void update();

	public:
		bool isKey(unsigned int index,int key_state)const;
		bool isAnyKeyDown()const;
		bool getChar(char *ch,int key_state)const;


		void setPosition( float _x, float _y, float _whell );
		void setSensitivity( float _sensitivity );
		void setRange ( const mt::vec3f &_minRange, const mt::vec3f & _maxRange );

		const mt::vec3f & getPosition() const;
		const mt::vec3f & getDelta() const;
		//mouse

		bool isMove()const;
		bool isAnyButtonDown()const;
		bool isButton(int but,int but_state)const;
	};
}