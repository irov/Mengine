#import sys

#sys.path.insert(0,"Game/Entities" )

import Menge
import BlockImpl

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

		hotspot.setMouseLeftClickEvent( self.test )


		self.addChildren( hotspot )

		self.doo = BlockImpl.Doo()
		
		self.doo.foo()

		pass
	pass

	def test( self ):
		print "Wazzzaaapp!!!"
		pass