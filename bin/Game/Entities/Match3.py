import Menge

class Match3:

	def __init__( self ):
		self.pole_data = []
		self.pole_width = 14
		self.pole_height = 5
		self.pole_count = self.pole_width * self.pole_height

		self.imageResourceMap = [ 'BallRed', 'BallGreen', 'BallBlue', 'BallYellow', 'BallVialet' ]

		print "__init__ !!", self.pole_count
		pass

	def initLevel( self, scene ):
		print "initLevel !!", self.pole_count
		for i in xrange( self.pole_count ):
			x = 100 + (i % self.pole_width) * 48
			y = 100 + (i / self.pole_width) * 48 * 2 + ( (i % self.pole_width) % 2 ) * 48

			print "x, y ", x, y
			en = Menge.createEntity( "Ball", Menge.vec2f(x,y), Menge.vec2f(1,0) )
			
			imageResourceID = Menge.randint( 0 , 4 )		
				
			en.init( self.imageResourceMap[ imageResourceID ] )
			en.activate()

			scene.layerAppend( "Main", en )

			self.pole_data.append( en )
			pass		

		pass
	pass