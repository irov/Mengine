import Menge
import Block

from Pole import Pole

from Menge import vec2f

class Scene( Menge.Scene ):
	def __init__( self ):
		self.pole = Pole()
		pass

	def onActivate( self ):
		en = Menge.createEntity( "Block", vec2f(200,200), vec2f(1,0) )
		en.activate();
		self.layerAppend( "Main", en )
		self.pole.loadLevel( self, "Game/Levels/01.txt" )
		pass
	pass