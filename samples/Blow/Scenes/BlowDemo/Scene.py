import Menge

class Scene( Menge.Scene ):
	def __init__( self ):
		self.camera_x = 512
		self.camera_y = 384

		Menge.setBlow( True )
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
