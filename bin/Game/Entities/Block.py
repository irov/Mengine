import Menge


class Block( Menge.Entity ):
	def __init__( self ):
		Menge.Entity.__init__( self )

		params = [ { "ImageMap": { "Name": "output_cng" }  } ]

		print "01", self
		print "01", self.addChildren
		image = Menge.createNode( "Sprite", params )
		print "02", self
		print "01", self.addChildren

		print image

		image.setName( "TestP" )

		self.addChildren( image )

		print "2"

		params = [ { 'Point': { 'Value': '-50;-50' } } ,{ 'Point': { 'Value': ' 50;-50' } } ,{ 'Point': { 'Value': ' 50; 50' } } ,{ 'Point': { 'Value': '-50; 50' } } ]

		hotspot = Menge.createNode( "HotSpot", params )

		print "2", hotspot, hotspot.setMouseLeftClickEvent
		print "3", self
		print "3", dir(self)
		print "4", self.addChildren

#		hotspot.setMouseLeftClickEvent( None )

		print "2"
		print "34", self

		print "34", hotspot

		self.addChildren( hotspot )

		print "2"

		pass
	pass

	def test( self ):
		print "Wazzzaaapp!!!"
		pass