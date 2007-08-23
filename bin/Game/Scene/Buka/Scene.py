import Menge
import Block

import Pole

from Menge import vec2f

pole = Pole.Pole();

def SceneBuka_activate( scene ):
	en = Menge.createEntity( "Block", vec2f(200,200), vec2f(1,0) )
	en.activate();

	print en.test();
	print hasattr( en, "test" )
	
	scene.layerAppend( "Main", en )

#	print "1"
	pole.loadLevel( scene, "Game/Levels/01.txt" )
#	print "222"

	pass
