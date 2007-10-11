import Menge


class Block( Menge.Entity ):
	def __init__( self ):
		Menge.Entity.__init__( self )
		pass
		
	def onLoader( self ):
		hotspot = self.getChildren( "HS_01", True )
		hotspot.setInputHandler( self )
		
		self.animation = self.getChildren( "AN_01", True )
		self.animation.setAnimationListener( self )
		pass
			
	def onHandleMouseButtonEvent( self, button, isDwon ):
		print button, isDwon
		self.animation.play()
		return True
		pass
		
	#def onAnimationEnd( self ):
		#print "Bla bla bla..."
	#	pass
		
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