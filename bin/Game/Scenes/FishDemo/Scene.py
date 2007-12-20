import Menge

class Scene( Menge.Scene ):
	def __init__( self ):
		self.camera_x = 512
		self.camera_y = 384

		pass

	def onActivate( self ):

		self.node = Menge.createNodeFromXml(
			'<Node Name = "Water_01" Type = "Sprite">'
			'<ImageMap Name = "BlueActive" />'
			'<Transformation Value = "1;0;0;1;0;0" />'
			'</Node> <!-- Sprite -->'
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

		#self.node.setImageIndex(2);

		#self.node.setPercentVisibility( Menge.vec2f( 0.3, 0 ), Menge.vec2f( 0.1, 0 ));
	
		pass

	def onUpdate( self, timing ):
		Menge.setCamera2DPosition( self.camera_x, self.camera_y );

		self.camera_x -= timing * 0

		#self.a = self.a + timing * 0.00005

		#self.node.setPercentVisibility( Menge.vec2f( 1 - self.a, 0 ), Menge.vec2f( 0, 0 ));

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

	def onHandleKeyEvent( self, key, isDown ):
	
		if isDown == False:
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
			self.testTextField.setText("Zateryannyi attol");
			#self.bubble.play();
			#self.emitterTest.play();

		if key == 54:
			print "54"
			self.emitterTest.pause();

		if key == 2:
			print "2"
			#self.bubble.stop();
			#self.emitterTest1.play();
			#self.emitterTest.stop();

		if key == 3:
			print "3"
			self.emitterTest.setLooped( True );

		if key == 4:
			print "4"
			self.emitterTest.setLooped( False );

		return False
		pass
		
	pass
	pass
