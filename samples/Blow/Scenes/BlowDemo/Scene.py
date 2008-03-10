import Menge

class Scene( Menge.Scene ):
	def __init__( self ):
		self.camera_x = 512
		self.camera_y = 384
		
		self.blow = None
		self.blow = Menge.createEntity("Blow", Menge.vec2f( 200, 200 ), Menge.vec2f( 1,0 ) )

		Menge.setEnoughBlow( 70 )
		Menge.setBlow( True )
		Menge.setBlowCallback( self.onBlow )
		pass
		
	def onLoader( self ):
		
		self.layerAppend( "Front", self.blow )
		pass

	def onBlow( self, blow ):
		print blow
		pass

	def onActivate( self ):
		self.blow.activate()
		pass

	def onUpdate( self, timing ):
		pass

	def onHandleMouseButtonEvent( self, button, isDown ):
		if button == 1 and isDown == True:
			self.blow.applyForce( 100, 100, 0, 0 );
			print Menge.getBlow()
			
		return False
		pass

	def onHandleKeyEvent( self, key, char, isDown  ):
		print key
		return False
		pass
		
	pass
	pass
