import Menge

import Match3

class Scene( Menge.Scene ):
	def __init__( self ):
		self.match3 = Match3.Match3();
		pass

	def activate( self ):
		print "scene Match3 - activate!"

		self.match3.initLevel( self)
		pass

	pass