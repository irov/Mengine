#	include "ScriptImplement.h"

#	include "Player.h"
#	include "Scene.h"

#	include "Sprite.h"

#	include "VisitorFindNode.h"

namespace Menge
{
	DEF_SCRIPT_FUNCTION( playSprite, void, ( const std::string & _sprite ) )
	{
		Player *player = Keeper<Player>::hostage();

		Scene *scene = player->getScene();

		Sprite *sprite = findNode<Sprite>(scene, _sprite);

		if( sprite )
		{
			sprite->play();
		}
	}
}