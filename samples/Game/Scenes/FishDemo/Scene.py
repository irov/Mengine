import Menge

class Scene( Menge.Scene ):
	def __init__( self ):
		self.camera_x = 512
		self.camera_y = 384

		pass

	def onActivate( self ):
		self.layer3d = self.getNode("Level3D");
		
		self.levelPhysicBody = self.layer3d.getRigidBody("LevelPhysicBody");

		self.zombieCap = self.layer3d.getController("Cap");

		self.zombieActor = Menge.createActor("Barrel1", "Barrel")
		self.zombieActor.setPosition( Menge.vec3f( 0,20,38 ) );
		self.zombieActor.setOrient( Menge.quatf(1,0,0,0) );
		self.zombieActor.setController(self.zombieCap)

		self.zombieActor.activate()

		self.zombieCap.setPosition(Menge.vec3f( 0,20,38 ))

		self.FreeCam = Menge.FFCamera3D()
		self.FreeCam.activate()
		self.FreeCam.setActor(self.zombieActor)

		self.isMouseDown = False

		pass

	def onUpdate( self, timing ):
		self.zombieActor.update(timing)

		self.FreeCam.update(timing)

		if self.isMouseDown == True:
			self.FreeCam.set2Direction( Menge.getMouseX(), Menge.getMouseY() )
		pass

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
			#self.FreeCam.forward(2)

		if key == 37:
			self.zombieActor.step( Menge.vec3f( 0, 0, 1 ) )
			#self.FreeCam.forward(-2)

		if key == 36:
			self.zombieActor.step( Menge.vec3f( -1, 0, 0 ) )
			#self.FreeCam.left(-2)

		if key == 38:
			self.zombieActor.step( Menge.vec3f( 1, 0, 0 ) )
			#self.FreeCam.left(2)
		
		if isDown == False:
			self.zombieActor.stop()
			return False

		return False
		pass
		
	pass
	pass
