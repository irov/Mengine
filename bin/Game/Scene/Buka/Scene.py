import Menge
import Block

from Menge import vec2f

def SceneBuka_activate( scene ):
	en = Menge.createEntity( "Block", vec2f(200,200), vec2f(1,0) )
	en.activate();

	scene.layerAppend( "Main", en )
	pass