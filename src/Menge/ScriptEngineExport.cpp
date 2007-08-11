#	include "ScriptEngine.h"

#	include "Player.h"
#	include "Scene.h"

#	include "Sprite.h"
#	include "TextField.h"

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

		//Scene *scene = player->getScene();

		//TextField *dlg = findNode<TextField>(scene, _dialog);

		//if( dlg )
		{
			//dlg->start();
		}
	}

	//void ScriptEngine::export_function()
	//{
	//	m_luaBoost.reg_function( "playSprite", &playSprite );
	//	m_luaBoost.reg_function( "playDialog", &playDialog );
	//}
}