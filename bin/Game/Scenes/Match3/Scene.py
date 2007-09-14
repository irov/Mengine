import Menge

import MatchPole

class Scene( Menge.Scene ):
	def __init__( self ):
		self.pole = MatchPole.MatchPole();
		pass

	def onActivate( self ):
		print "scene Match3 - activate!"

		self.pole.initLevel( self )
		pass

	pass