#	include "GameScriptable.h"

#	include "ScriptEngine.h"

MENGE_SCRIPTABLE_IMPLEMENT( GameScriptable );

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	GameScriptable::GameScriptable()
	{
		ScriptEngine * scriptEngine = 
			Holder<ScriptEngine>::hostage();

		scriptEngine->regModule()
			[
				scriptEngine
				->regClass<GameScriptable>("GameScripting")
				.def( "setChapter", &GameScriptable::setChapter )
				.def( "gotoScene", &GameScriptable::gotoScene )
			];

		scriptEngine->refObject( "Game", this );
	}
	//////////////////////////////////////////////////////////////////////////
	void GameScriptable::setChapter( const char * _name )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void GameScriptable::gotoScene( const char * _name )
	{

	}
}