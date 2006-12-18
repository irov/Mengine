#	include "ScriptImplement.h"

#	include "Player.h"
#	include "Scene.h"

#	include "Dialog.h"

#	include "VisitorFindNode.h"

namespace Menge
{
	DEF_SCRIPT_FUNCTION( playDialog, void, ( const std::string & _dialog ) )
	{
		Player *player = Keeper<Player>::hostage();

		Scene *scene = player->getScene();

		Dialog *dlg = findNode<Dialog>(scene, _dialog);

		if( dlg )
		{
			dlg->start();
		}
	}

}