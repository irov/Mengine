import Menge

class Scene( Menge.Scene ):
	def onActivate( self ):
		self.camera_x = 512
		self.camera_y = 384

		Menge.setCamera2DPosition( self.camera_x, self.camera_y );
		pass

	def onUpdate( self, timing ):
		Menge.setCamera2DPosition( self.camera_x, self.camera_y );

		self.camera_x += timing * 25
		pass
	pass