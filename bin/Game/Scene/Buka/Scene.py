import Menge
import Block

import Pole

from Menge import vec2f

#pole = Pole.Pole();

def SceneBuka_activate( scene ):
	en = Menge.createEntity( "Block", vec2f(200,200), vec2f(1,0) )

	print "en 1 ->", en.refcount();	

	en.activate();

	print "en 2 ->", en.refcount();

	scene.layerAppend( "Main", en )

	print "en 3 ->", en.refcount();

	print scene.refcount();
#	pole.loadLevel( scene, "Game/Levels/01.txt" )

	pass
