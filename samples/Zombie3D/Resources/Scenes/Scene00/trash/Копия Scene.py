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
		pass

	def onUpdate( self, timing ):
		'''
		self.zombieActor.update(timing)

		self.FreeCam.update(timing)

		if self.isMouseDown == True:
			self.FreeCam.set2Direction( Menge.getMouseX(), Menge.getMouseY() )
		pass
		'''

		pass

	def onHandleMouseButtonEvent( self, botton, isDown ):
		if isDown == True:
			print "left pressed"
			self.FreeCam.set2Direction( Menge.getMouseX(), Menge.getMouseY() )
			self.isMouseDown = True
		else:
			print "left unpressed"
			self.isMouseDown = False
		return False
		pass

	def onHandleKeyEvent( self, key, char, isDown  ):
		print key


		if key == 2:
			self.player.playAnimation("Idle00")

		if key == 17:
			self.FreeCam.forward(2)

		if key == 30:
			self.FreeCam.left(-2)

		if key == 32:
			self.FreeCam.left(2)

		if key == 31:
			self.FreeCam.forward(-2)

		if key == 23:
			self.zombieActor.step( Menge.vec3f( 0, 0, -1 ) )

		if key == 37:
			self.zombieActor.step( Menge.vec3f( 0, 0, 1 ) )

		if key == 36:
			self.zombieActor.step( Menge.vec3f( -1, 0, 0 ) )

		if key == 38:
			self.zombieActor.step( Menge.vec3f( 1, 0, 0 ) )
		
		#if isDown == False:
		#	self.zombieActor.stop()
		#	return False

		if key == 200:
			if isDown == True:
				self.moving = True
				self.player.stopAnimation("Idle00")
				self.player.playAnimation("Walk")
				self.player.setMoveSpeed( Menge.vec3f( 0.0, 0.0, 1.0 ) )
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
