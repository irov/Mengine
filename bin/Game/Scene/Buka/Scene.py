import Menge
import Block

import Pole

from Menge import vec2f

pole = Pole.Pole();

def SceneBuka_activate( scene ):
	en = Menge.createEntity( "Block", vec2f(200,200), vec2f(1,0) )
	
	print "1"

	en.activate();

	print "1"

	scene.layerAppend( "Main", en )

	print "1"

	pole.loadLevel( scene, "Game/Levels/01.txt" )

	print "1"

	pass
