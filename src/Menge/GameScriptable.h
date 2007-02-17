#	pragma once

#	include "Scriptable.h"

namespace Menge
{
	class GameScriptable
		: public Scriptable
	{
		MENGE_SCRIPTABLE_DEFINE();
	public:
		GameScriptable();	

	public:
		void setChapter( const char * _name );
		void gotoScene( const char * _name );
	};
}