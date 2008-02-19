import Menge

class Scene( Menge.Scene ):
	def __init__( self ):
		self.camera_x = 512
		self.camera_y = 384

		pass

	def onActivate( self ):
		print "asdf"

		self.node = Menge.createNodeFromXml(
			'<Node Name = "Water_01" Type = "Sprite">'
			'<ImageMap Name = "BlueActive" />'
			'<Transformation Value = "1;0;0;1;0;0;" />'
			'</Node>'
			)

		self.node.activate()
		self.layerAppend( "Water", self.node )
		
		self.node1 = self.getNode("Water_01")
	
		self.node1.setLocalPosition( Menge.vec2f( 400,200 ) )

		position = self.node1.getLocalPosition()
		print Menge.getVec2fX(position), Menge.getVec2fY(position)

		
		self.node1.hide( False )
		
		block = Menge.createEntity("Block",Menge.vec2f( 100, 100 ), Menge.vec2f( 1, 0 ) )

		posX = Menge.getVec2fX(block.getWorldPosition())
		posY = Menge.getVec2fY(block.getWorldPosition())
		
		x = 0
		y = -100
		
		self.testTextField = self.getNode( "Arial2112" )
		self.testTextField.setLocalPosition( Menge.vec2f( 200,100 ) )
		self.testTextField.setText("Gloria");

		self.emitterTest = self.getNode( "EmitterTest1" );

		self.emitterTest1 = self.getNode( "EmitterTestRush" );


		self.bubble = Menge.createNodeFromXml(
                        '<Node Type = "Emitter">'
			' <Resource Name = "ResourceEmitterTest1"/>'
			' <Emitter Name = "Meduse1"/>'
			'</Node>'
			);

		self.bubble.activate();

		self.layerAppend( "Water", self.bubble )

		self.bubble1 = Menge.createNodeFromXml(
                        '<Node Type = "Emitter">'
			' <Resource Name = "ResourceEmitterTest1"/>'
			' <Emitter Name = "Meduse1"/>'
			'</Node>'
			);

		self.bubble1.activate();

		self.layerAppend( "Water", self.bubble1 )

		block.activate();
		
		self.layerAppend( "Deep", block )

		#self.levelActor = Menge.createActor("Level1", "Level");
		#self.levelActor.setParentRoot( Menge.vec3f( 0,0,0 ), Menge.quatf(1,0,0,0) );

		#self.entityLevel = self.getEntity( "Level" )
		#self.levelActor.attachEntity( self.entityLevel );

		self.barrelActor = Menge.createActor("Barrel1", "Barrel")
		self.barrelActor.setParentRoot( Menge.vec3f( 0,0,0 ), Menge.quatf(1,0,0,0) );

		self.entityBarrel = self.getEntity( "Barrel" )
		self.barrelActor.attachEntity( self.entityBarrel );

		self.zombieActor = Menge.createActor("Zombie1", "Zombie");
		self.zombieActor.setParentRoot( Menge.vec3f( 10,10,40 ), Menge.quatf(1,0,0,0) );

		self.entityZombie = self.getEntity( "Zombie" )
		self.zombieActor.attachEntity( self.entityZombie );
		#self.zombieActor.setScale(0.010);
		self.zombieActor.yaw(180);

		self.levelPhysicBody = self.getRigidBody("LevelPhysicBody");

		self.barrelPhysicBody = self.getRigidBody("BarrelPhysicBody");
		self.barrelPhysicBody.setPosition( Menge.vec3f( 10,20,5 ) );

		#self.levelActor.addChild(self.barrelActor);

		self.barrelActor.activate();
		#self.levelActor.activate();
		self.zombieActor.activate();

		#self.actorAppend(self.levelActor);
		self.actorAppend(self.barrelActor);
		self.actorAppend(self.zombieActor);

		'''self.camera = self.getCamera("MainCamera");
		self.pos = self.zombieActor.getWorldPosition();

		self.camPos = Menge.vec3f( Menge.getVec3fX(self.pos), Menge.getVec3fY(self.pos) + 40, Menge.getVec3fZ(self.pos) );

		self.camera.setPosition( self.camPos );

		self.camera.lookAt( Menge.vec3f( Menge.getVec3fX(self.pos)-15, Menge.getVec3fY(self.pos), Menge.getVec3fZ(self.pos) ) );

		self.camera.yaw(-90);'''

		pass

	def onUpdate( self, timing ):

		self.barrelActor.setLocalPosition( self.barrelPhysicBody.getPosition() )
		self.barrelActor.setLocalOrient( self.barrelPhysicBody.getOrientation() )

		self.camera = self.getCamera("MainCamera");
		self.pos = self.zombieActor.getWorldPosition();

		self.camPos = Menge.vec3f( Menge.getVec3fX(self.pos), Menge.getVec3fY(self.pos) + 40, Menge.getVec3fZ(self.pos) );

		self.camera.setPosition( self.camPos );

		self.camera.lookAt( Menge.vec3f( Menge.getVec3fX(self.pos)-15, Menge.getVec3fY(self.pos), Menge.getVec3fZ(self.pos) ) );

		self.camera.yaw(-90);

		Menge.setCamera2DPosition( self.camera_x, self.camera_y );
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
	
		if isDown == False:
			self.zombieActor.stop();
			return False

		if key == 23:
			self.zombieActor.step( Menge.vec3f( 0, 0, -1 ) );
			#self.zombieActor.left();

		if key == 37:
			self.zombieActor.step( Menge.vec3f( 0, 0, 1 ) );

		if key == 36:
			self.zombieActor.step( Menge.vec3f( -1, 0, 0 ) );

		if key == 38:
			self.zombieActor.step( Menge.vec3f( 1, 0, 0 ) );
	
		return False
		pass
		
	pass
	pass
