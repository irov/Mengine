import Menge

class Grid( Menge.Entity ):
	def __init__( self ):
		Menge.Entity.__init__( self )
		pass

	def onActivate( self ):
		self.image = self.getChildren( "gridImage" )
		pass

	def setImageIndex( self, frame ):
		print self.image, frame
		self.image.setImageIndex( frame )
		pass
	pass
