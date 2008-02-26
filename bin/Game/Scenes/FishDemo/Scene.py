import Menge

class Scene( Menge.Scene ):
	def __init__( self ):
		self.camera_x = 512
		self.camera_y = 384

		pass

	def onActivate( self ):
		block = Menge.createEntity("Block",Menge.vec2f( 100, 100 ), Menge.vec2f( 1, 0 ) )
		block.activate();
		self.layerAppend( "Deep", block )

		#and there 3d is starting...
		self.layer3d = self.getNode("Level3D");
		

		self.levelPhysicBody = self.layer3d.getRigidBody("LevelPhysicBody");

		self.barrelPhysicBody = self.layer3d.getRigidBody("BarrelPhysicBody");
		self.barrelPhysicBody.setPosition( Menge.vec3f( 10,20,5 ) );


		#self.light = self.layer3d.getNode("Light1")

		self.zombieCap = self.layer3d.getController("Cap");

		self.zombieActor = Menge.createActor("Barrel1", "Barrel")
		self.zombieActor.setPosition( Menge.vec3f( 0,25,0 ) );
		self.zombieActor.setOrient( Menge.quatf(1,0,0,0) );
		self.zombieActor.activate()
		self.zombieActor.setController(self.zombieCap)


		self.camera = self.getNode("NodeCamera").getCamera();

		self.pos = self.zombieActor.getPosition();

		self.camPos = Menge.vec3f( Menge.getVec3fX(self.pos), Menge.getVec3fY(self.pos) + 20, Menge.getVec3fZ(self.pos) );

		self.camera.setPosition( self.camPos );
		self.camera.lookAt( Menge.vec3f( Menge.getVec3fX(self.pos)-15, Menge.getVec3fY(self.pos), Menge.getVec3fZ(self.pos) ) )
		self.camera.yaw(-90);


		self.zombieActor.addChild(self.getNode("NodeCamera"))

		#self.FreeCam = Menge.FFCamera3D()
		#self.FreeCam.activate()

		#self.isMouseDown = False


		pass

	def onUpdate( self, timing ):
		#self.FreeCam.update()

		'''if self.isMouseDown == True:
			self.FreeCam.set2Direction( Menge.getMouseX(), Menge.getMouseY() )
		pass'''

		#self.light.translate( Menge.vec3f( 0,0.1,0) )

		self.zombieActor.update(timing)

		'''self.pos = self.zombieActor.getPosition();

		self.camera = self.getNode("NodeCamera").getCamera();

		self.camPos = Menge.vec3f( Menge.getVec3fX(self.pos)-45, Menge.getVec3fY(self.pos) + 40, Menge.getVec3fZ(self.pos) );

		self.camera.setPosition( self.camPos );

		self.camera.lookAt( Menge.vec3f( Menge.getVec3fX(self.pos), Menge.getVec3fY(self.pos), Menge.getVec3fZ(self.pos) ) );

		self.camera.yaw(-90);'''

		Menge.setCamera2DPosition( self.camera_x, self.camera_y );

		pass

	def onHandleMouseButtonEvent( self, botton, isDown ):
		if isDown == True:
			print "left pressed"
			#self.FreeCam.set2Direction( Menge.getMouseX(), Menge.getMouseY() )
			self.isMouseDown = True
		else:
			print "left unpressed"
			self.isMouseDown = False
		return False
		pass

	def onHandleKeyEvent( self, key, char, isDown  ):
		print key
	
		if isDown == False:
			self.zombieActor.stop()
			return False


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
	
		return False
		pass
		
	pass
	pass
