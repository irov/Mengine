#	pragma once

#	include "Scriptable.h"

namespace Menge
{
	class PlayerScriptable
		: public Scriptable
	{
		MENGE_SCRIPTABLE_DEFINE();
	public:
		PlayerScriptable();	

	public:
		void setChapter( const char * _name );
		void gotoScene( const char * _name );
	};
}