import Menge

class Scene( Menge.Scene ):
	def __init__( self ):
		self.camera_x = 512
		self.camera_y = 384

		Menge.setEnoughBlow( 70 )
		Menge.setBlow( True )
		Menge.setBlowCallback( self.onBlow )
		pass

	def onBlow( self, blow ):
		print blow
		pass

	def onActivate( self ):
		pass

	def onUpdate( self, timing ):
		pass

	def onHandleMouseButtonEvent( self, button, isDown ):
		if button == 1 and isDown == True:
			print Menge.getBlow()
			
		return False
		pass

	def onHandleKeyEvent( self, key, char, isDown  ):
		print key
		return False
		pass
		
	pass
	pass
