#	include "ScriptEngine.h"

#	include "lua_boost/lua_boost.h"

#	include "Player.h"
#	include "Scene.h"

#	include "Sprite.h"
#	include "Dialog.h"

#	include "VisitorFindNode.h"

namespace Menge
{
	void playSprite( const char * _sprite ) 
	{
		//Player *player = Holder<Player>::hostage();

		//Scene *scene = player->getScene();

		//Sprite *sprite = findNode<Sprite>(scene, _sprite);

		//if( sprite )
		//{
		//	sprite->play();
		//}
	}

	void playDialog( const char * _dialog )
	{
		Player *player = Holder<Player>::hostage();

		Scene *scene = player->getScene();

		Dialog *dlg = findNode<Dialog>(scene, _dialog);

		if( dlg )
		{
			dlg->start();
		}
	}

	//void ScriptEngine::export_function()
	//{
	//	m_luaBoost.reg_function( "playSprite", &playSprite );
	//	m_luaBoost.reg_function( "playDialog", &playDialog );
	//}
}