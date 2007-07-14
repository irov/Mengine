import Menge


class Block( Menge.Entity ):
	def __init__( self ):
		Menge.Entity.__init__( self )

		image = Menge.createNode( "Sprite", 
			[ { "ImageMap": { "Name": "output_cng" }  } ]
			)

		image.setName( "TestP" )

		self.addChildren( image )

		params = [ { 'Point': { 'Value': '-50;-50' } } ,{ 'Point': { 'Value': ' 50;-50' } } ,{ 'Point': { 'Value': ' 50; 50' } } ,{ 'Point': { 'Value': '-50; 50' } } ]

		hotspot = Menge.createNode( "HotSpot", params )

		hotspot.setMouseLeftClickEvent( self.test )


		self.addChildren( hotspot )

		pass
	pass

	def test( self ):
		print "Wazzzaaapp!!!"
		pass