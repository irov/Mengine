#	include "ScriptDeclarationDefine.h"

#	include "Player.h"

namespace Menge
{

	namespace ScriptPlayer
	{
		Player * player()
		{
			return Holder<Player>::hostage();
		}
	}

	REGISTER_SCRIPT_CLASS( Menge, Player )
	{
		boost::python::class_<Player>("Player")
			.def( "setChapter", &Player::setChapter )
			.def( "gotoScene", &Player::gotoScene );

		boost::python::def( "player", 
			&ScriptPlayer::player, 
			boost::python::return_value_policy<boost::python::reference_existing_object>() );
	}
}