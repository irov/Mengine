import Menge


class Block( Menge.Entity ):
	def __init__( self ):
		Menge.Entity.__init__( self )

		image = Menge.createNodeFromXml(
			'<Node Type = "Sprite">'
			'	<ImageMap Name = "BallRed"/>'
			'</Node>'
			)

		self.addChildren( image )

		params = [ { 'Point': { 'Value': '-50;-50' } } ,{ 'Point': { 'Value': ' 50;-50' } } ,{ 'Point': { 'Value': ' 50; 50' } } ,{ 'Point': { 'Value': '-50; 50' } } ]

		hotspot = Menge.createNodeFromXml(
			'<Node Type = "HotSpot">'
			'	<Point Value = "0;0" />'
			'	<Point Value = "100;0" />'
			'	<Point Value = "100;100" />'
			'	<Point Value = "0;100" />'
			'</Node>'
			)
		
		hotspot.setInputHandler( self )

		self.addChildren( hotspot )

		pass
	
	def onHandleMouseButtonEvent( self, button, isDwon ):
		print button, isDwon
		self.test()
		return True
		pass
		
	def onEnter( self ):
		print "enter"
		pass
		
	def onLeave( self ):
		print "leave"
		pass

	def test( self ):
		print "Wazzzaaapp!!!"
		pass
	pass