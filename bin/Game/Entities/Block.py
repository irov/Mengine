import Menge


class Block( Menge.Entity ):
	def __init__( self ):
		Menge.Entity.__init__( self )

		params = [ { "ImageMap": { "Name": "output_cng" }  } ]

		image = Menge.createNode( "Sprite", params )

		image.setName( "TestP" )

		self.addChildren( image )

		params = [ { 'Point': { 'Value': '-50;-50' } } ,{ 'Point': { 'Value': ' 50;-50' } } ,{ 'Point': { 'Value': ' 50; 50' } } ,{ 'Point': { 'Value': '-50; 50' } } ]

		hotspot = Menge.createNode( "HotSpot", params )

		self.addChildren( hotspot )

		pass
	pass

	def test( self ):
		print "Wazzzaaapp!!!"
		pass
