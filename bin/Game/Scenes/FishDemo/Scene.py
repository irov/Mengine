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

		#self.atg = self.getNode( "TestAnimationGroup1" )
		#self.atg.play("Test1")
		#self.atg.play("Test2")
		
		x = 0
		y = -100

		#if x > posX:
		#	block.rotateTo( 3000, Menge.vec2f( x, y ) )
		#else:
		#	mirrorX = posX + ( posX - x )
		#	mirrorY = posY + ( posY - y )
		#	block.rotateTo( 3000, Menge.vec2f( mirrorX, mirrorY ) )
		
		#block.moveTo( 30000, Menge.vec2f( 1000,200 ), False )
		
		#block.rotateTo( 3000, Menge.vec2f( 0,0 ) )

		#block.setRotate( 3.14/3.0  )

		#block.setLocalDirection(Menge.vec2f( 0,-1 ))


		#self.testSound = self.getNode( "SoundEmitterTest" )
		#self.testSound.setSoundResource("ResourceSoundTest")
		#self.testSound.play()
		#self.testSound.setVolume(1.0)

		#Menge.soundSetVolume( 0.1 )

		#self.testSound1 = self.getNode( "SoundEmitterTest1" )
		#self.testSound1.setSoundResource("ResourceSoundTest1")
		#self.testSound1.play()
		#self.testSound1.setVolume(1.0)


		#Menge.musicPlayList("ResourcePlayListTest1");
		#Menge.musicSetVolume( 0.3 )
		#Menge.musicSetVolume( 0.2 )

		#Menge.musicPlayList("ResourcePlayListTest1");

		#Menge.stopMusic();
		
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

		self.delta = -2.75

		self.a = self.delta;
		self.b = self.delta;
		self.c = self.delta;
		self.d = self.delta;

		self.xoffset = 40;
		self.zoffset = 0;
		self.yoffset = 50;
		
		self.levelActor = Menge.createActor("Level1", "Level");
		self.levelActor.setParentRoot( Menge.vec3f( 0,0,0 ), Menge.quatf(1,0,0,0) );

		self.entityLevel = self.getEntity( "Level" )
		self.levelActor.attachEntity( self.entityLevel );

		#self.barrelActor = Menge.createActor("Barrel1", "Barrel")
		#self.barrelActor.setParentRoot( Menge.vec3f( 0,0,0 ), Menge.quatf(1,0,0,0) );

		#self.entityBarrel = self.getEntity( "Barrel" )
		#self.barrelActor.attachEntity( self.entityBarrel );

		self.zombieActor = Menge.createActor("Zombie1", "Zombie");
		self.zombieActor.setParentRoot( Menge.vec3f( 10,0,40 ), Menge.quatf(1,0,0,0) );

		self.entityZombie = self.getEntity( "Zombie" )
		self.zombieActor.attachEntity( self.entityZombie );
		self.zombieActor.setScale(10);
		self.zombieActor.yaw(180);

		self.levelPhysicBody = self.getRigidBody("LevelPhysicBody");
		self.levelActor.setPhysicBody(self.levelPhysicBody);

		self.levelActor.activate();
		#self.barrelActor.activate();
		self.zombieActor.activate();

		self.actorAppend(self.levelActor);
		#self.actorAppend(self.barrelActor);
		self.actorAppend(self.zombieActor);

		self.camera = self.getCamera("MainCamera");
		self.pos = self.zombieActor.getWorldPosition();

		self.camPos = Menge.vec3f( Menge.getVec3fX(self.pos), Menge.getVec3fY(self.pos) + 40, Menge.getVec3fZ(self.pos) );

		self.camera.setPosition( self.camPos );

		self.camera.lookAt( Menge.vec3f( Menge.getVec3fX(self.pos)-15, Menge.getVec3fY(self.pos), Menge.getVec3fZ(self.pos) ) );

		self.camera.yaw(-90);


		pass

	def onUpdate( self, timing ):
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
			self.emitterTest.play( False );
		else:
			print "left unpressed"
			self.isMouseDown = False
			#self.emitterTest.play( False );
		return False
		pass

	def onHandleKeyEvent( self, key, char, isDown  ):
		print key
	
		if isDown == False:
			self.zombieActor.stop();
			return False

		if key == 205:
			shoted = Menge.createShot("shot", Menge.vec2f( 0, 0 ), Menge.vec2f( 500, 500 ) );
			self.node.addChildren( shoted );

			#self.node.flip(True);
			#self.node.deactivate();
			#self.a = self.a + self.delta;
			#self.node.setPercentVisibility( Menge.vec2f( 0, self.a ), Menge.vec2f( 0, 0 ));

		if key == 203:	
			self.node.flip(False);
			#self.c = self.c + self.delta;
			#self.node.setPercentVisibility( Menge.vec2f( 0, 0 ), Menge.vec2f( self.c, 0 ));
		
		if key == 208:	
			self.node.setPercentVisibility( Menge.vec2f( self.a, 0 ), Menge.vec2f( 0, 0 ));
		
		if key == 32:
			print "32"
			#Menge.setCamera2DDirection( 1, 0)
			#self.testTextField.setMaxLen(80);
			#self.testTextField.setText("Zater        yannyi\ntyjgffgfh gfhfghgfh trytry\nattol\ndfgdfgdfgdfgdfg\ndfg");
			#self.bubble.play();
			#self.emitterTest.play();
			self.ROBOT.play("Walk");


		if key == 49:
			print "1"
			self.emitterTest.stop();

		if key == 50:
			print "2"
			self.emitterTest.setLooped( 0 );

		if key == 51:
			print "3"
			self.emitterTest.setLooped( 1 );

		if key == 52:
			print "4"
			self.emitterTest.setLooped( 2 );

		if key == 53:
			print "5"
			self.emitterTest.pause();

		if key == 119:
			self.xoffset = self.xoffset - 10;

		if key == 100:
			self.zoffset = self.zoffset - 10;

		if key == 115:
			self.xoffset = self.xoffset + 10;

		if key == 97:
			self.zoffset = self.zoffset + 10;

		if key == 120:
			self.yoffset = self.yoffset - 5;

	
		if key == 23:
			self.zombieActor.step( Menge.vec3f( 0, 0, -1 ) );

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
