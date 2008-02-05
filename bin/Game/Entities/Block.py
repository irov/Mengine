import Menge


class Block( Menge.Entity ):
	def __init__( self ):
		Menge.Entity.__init__( self )
		pass
		
	def onLoader( self ):
		print "!@!"
		hotspot = self.getChildren( "HS_01", True )
		hotspot.setHotspotListener( self )
		
		self.animation = self.getChildren( "AN_01", True )
		self.animation.setAnimationListener( self )
		pass
			
	def onHandleMouseButtonEvent( self, key, isDown ):
		print button, isDwon
		self.animation.play()
		self.moveTo( 3000, Menge.vec2f( 1000,200 ), False )
		return True
		pass
		
	def onMoveEnd( self ):
		print "onMoveEnd..."
		#self.animation.stop()
		print "Blo blu ble..."
		pass
		
	def onAnimationEnd( self, animation ):
		print "onAnimationEnd...", animation.getName()
		self.moveTo( 4000, Menge.vec2f( 100,200 ), False )
		
		
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