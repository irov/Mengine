import Menge


class Ball( Menge.Entity ):
	def __init__( self ):
		pass

	def init( self, imageResource ):

		self.image = Menge.createNode( "Sprite", 
			[ { "ImageMap": { "Name": imageResource }  } ]
			)

		self.addChildren( self.image )

		params = [ 
			{ 'Point': { 'Value': '-24;-24' } } ,
			{ 'Point': { 'Value': ' 24;-24' } } ,
			{ 'Point': { 'Value': ' 24; 24' } } ,
			{ 'Point': { 'Value': '-24; 24' } } ]

		hotspot = Menge.createNode( "HotSpot", params )

#		hotspot.setMouseLeftClickEvent( self.onMouseLeftClick )

		self.addChildren( hotspot )
		pass

	def onMouseLeftClick( self ):
		pass


	pass