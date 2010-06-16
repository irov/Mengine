#	pragma once

namespace Menge
{
	class ScriptNodeInterface
	{
	public:
		virtual bool activate() = 0;
		virtual void deactivate() = 0;

			.def( "isActivate", &Node::isActivate )
			.def( "enable", &Node::enable )
			.def( "disable", &Node::disable )
			.def( "isEnable", &Node::isEnable )
			.def( "setUpdatable", &Node::setUpdatable )
			.def( "isUpdatable", &Node::isUpdatable )
			.def( "addChildren", &Node::addChildren )
			.def( "addChildrenFront", &Node::addChildrenFront )
			.def( "removeChildren", &Node::removeChildren )
			.def( "getChildren", &Node::getChildren )
			.def( "isChildren", &Node::isChildren )
			.def( "updatable", &Node::updatable )
			.def( "update", &Node::update )
			.def( "getParent", &Node::getParent )
			.def( "setListener", &Node::setListener )
			.def( "getListener", &Node::getListener )

			//.def( "getWorldPosition", &Node::getWorldPosition )
			//.def( "getWorldDirection", &Node::getWorldDirection )
			//.def( "setAlpha", &Node::setAlpha )
			.def( "getScreenPosition", &Node::getScreenPosition )
			.def( "setLocalColor", &Node::setLocalColor )
			.def( "setLocalAlpha", &Node::setLocalAlpha )
			.def( "getWorldColor", &Node::getWorldColor )
			.def( "getLocalColor", &Node::getLocalColor )

			.def( "localColorToCb", &Node::localColorToCb )
			.def( "localAlphaToCb", &Node::localAlphaToCb )
			.def( "localColorToStop", &Node::localColorToStop )

			.def( "moveToCb", &Node::moveToCb )
			.def( "moveToStop", &Node::moveToStop )

			.def( "angleToCb", &Node::angleToCb )
			.def( "angleToStop", &Node::angleToStop )
			.def( "scaleToCb", &Node::scaleToCb )
			.def( "scaleToStop", &Node::scaleToStop )

			.def( "accMoveToCb", &Node::accMoveToCb )
			.def( "accAngleToCb", &Node::accAngleToCb )
	};
	class ScriptSystem
	{

	};
}