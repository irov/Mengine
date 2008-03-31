import Menge

class Scene( Menge.Scene ):
	def __init__( self ):
		self.camera_x = 512
		self.camera_y = 384

		pass

	def onLoader( self ):
		self.player = self.getNode("Pers")
		self.moving = False
		pass

	def onActivate( self ):
		self.player.playAnimation("Idle00")


		self.FreeCam = Menge.FFCamera3D()
		self.FreeCam.activate()
		self.FreeCam.setActor(self.player)

		pass

	def onUpdate( self, timing ):
		self.FreeCam.update(timing)
		pass

	def onHandleMouseButtonEvent( self, botton, isDown ):
		if isDown == True:
			print "left pressed"
			self.isMouseDown = True
		else:
			print "left unpressed"
			self.isMouseDown = False
		return False
		pass

	def onHandleKeyEvent( self, key, char, isDown  ):
		print key

		if key == 16:
			self.FreeCam.yaw(0.5)

		if key == 18:
			self.FreeCam.yaw(-0.5)

		if key == 44:
			self.FreeCam.pitch(0.5)

		if key == 45:
			self.FreeCam.pitch(-0.5)

		if key == 19:
			self.FreeCam.zoom(-0.1)

		if key == 20:
			self.FreeCam.zoom(0.1)


		if key == 2:
			self.player.playAnimation("Idle00")

		if key == 200:
			if isDown == True:
				self.moving = True
				self.player.stopAnimation("Idle00")
				self.player.playAnimation("Walk")
				self.player.setMoveSpeed( Menge.vec3f( 0.0, 0.0, 5.0 ) )
			else:
				self.moving = False
				self.player.stopAnimation("Walk")
				self.player.playAnimation("Idle00")
				self.player.setMoveSpeed( Menge.vec3f( 0.0, 0.0, 0.0 ) )
			pass

		if key == 203:
			if isDown == True:
				self.turning = True
				self.player.stopAnimation("Idle00")
				self.player.playAnimation("Turn")
				self.player.setYawSpeed( 0.1 )
			else:
				self.turning = False
				self.player.playAnimation("Idle00")
				self.player.stopAnimation("Turn")
				self.player.setYawSpeed( 0.0 )

		if key == 205:
			if isDown == True:
				self.turning = True
				self.player.stopAnimation("Idle00")
				self.player.playAnimation("Turn")
				self.player.setYawSpeed( -0.1 )
			else:
				self.turning = False
				self.player.playAnimation("Idle00")
				self.player.stopAnimation("Turn")
				self.player.setYawSpeed( 0.0 )

		return False
		pass
		
	pass
	pass
