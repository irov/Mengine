import Menge
import Block

import Pole

from Menge import vec2f

pole = Pole.Pole();

def SceneBuka_activate( scene ):
	en = Menge.createEntity( "Block", vec2f(200,200), vec2f(1,0) )
	en.activate();

	scene.layerAppend( "Main", en )

	pole.loadLevel( scene, "Game/Levels/01.txt" )

	pass