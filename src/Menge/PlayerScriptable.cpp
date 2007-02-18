#	include "PlayerScriptable.h"

#	include "ScriptEngine.h"

#	include "Player.h"

MENGE_SCRIPTABLE_IMPLEMENT( PlayerScriptable );

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PlayerScriptable::PlayerScriptable()
	{
		ScriptEngine * scriptEngine = 
			Holder<ScriptEngine>::hostage();

		scriptEngine->regModule()
			[
				scriptEngine
				->regClass<PlayerScriptable>("PlayerScriptable")
				.def( "setChapter", &PlayerScriptable::setChapter )
				.def( "gotoScene", &PlayerScriptable::gotoScene )
			];

		scriptEngine->refObject( "Player", this );
	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerScriptable::setChapter( const char * _name )
	{
		Player * player = 
			Holder<Player>::hostage();

		player->setChapter( _name );

	}
	//////////////////////////////////////////////////////////////////////////
	void PlayerScriptable::gotoScene( const char * _name )
	{
		Player * player = 
			Holder<Player>::hostage();

		player->gotoScene( _name );
	}
}