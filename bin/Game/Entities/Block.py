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

		print "params %d" % ( len(params) )

		hotspot = Menge.createNode( "HotSpot", params )


		self.addChildren( hotspot )

		pass
	pass