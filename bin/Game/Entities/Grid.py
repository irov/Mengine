import Menge

class Grid( Menge.Entity ):
	def __init__( self ):
		Menge.Entity.__init__( self )

		self.image = Menge.createNode( "Sprite", 
			[ {
                                "ImageMap" :
                                {
                                        "Name" : "grid"
                                }
                                }
                          ]
			)	
		
		self.addChildren( self.image )
		pass

	def setImageIndex( self, frame ):
		print self.image, frame
		self.image.setImageIndex( frame )
		pass
	pass
