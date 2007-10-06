import Menge


class Block( Menge.Entity ):
	def __init__( self ):
		Menge.Entity.__init__( self )
		pass
		
	def onActivate( self ):
		hotspot = self.getChildren( "HS_01", True )
		hotspot.setInputHandler( self )
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