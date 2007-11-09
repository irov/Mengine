import Menge

class Scene( Menge.Scene ):
	def __init__( self ):
		self.camera_x = 512
		self.camera_y = 384

		#Menge.setCamera2DPosition( self.camera_x, self.camera_y );
		pass

	def onActivate( self ):

		self.node = Menge.createNodeFromXml(
			'<Node Name = "Water_01" Type = "Sprite">'
			'<ImageMap Name = "Water_01" />'
			'<Transformation Value = "1;0;0;1;0;0" />'
			'</Node> <!-- Sprite -->'
			)

		self.node.activate()
		self.layerAppend( "Water", self.node )
		
		self.node1 = self.getNode("Water_01")
	
		self.node1.setLocalPosition( Menge.vec2f( 500,400 ) )

		position = self.node1.getLocalPosition()
		print Menge.getVec2fX(position), Menge.getVec2fY(position)

		
		self.node1.hide( False )
		
		block = Menge.createEntity("Block",Menge.vec2f( 100, 100 ), Menge.vec2f( 1, 0 ) )

		posX = Menge.getVec2fX(block.getWorldPosition())
		posY = Menge.getVec2fY(block.getWorldPosition())


		self.atg = self.getNode( "TestAnimationGroup1" )
		self.atg.play("Test1")
		self.atg.play("Test2")
		
		x = 0
		y = -100

		if x > posX:
			block.rotateTo( 3000, Menge.vec2f( x, y ) )
		else:
			mirrorX = posX + ( posX - x )
			mirrorY = posY + ( posY - y )
			block.rotateTo( 3000, Menge.vec2f( mirrorX, mirrorY ) )
		
		#block.moveTo( 30000, Menge.vec2f( 1000,200 ), False )
		
		#block.rotateTo( 3000, Menge.vec2f( 0,0 ) )

		block.setRotate( 3.14/3.0  )

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


		Menge.musicPlayList("ResourcePlayListTest1");
		#Menge.musicSetVolume( 0.3 )
		#Menge.musicSetVolume( 0.2 )
		#Menge.musicPlayList("ResourcePlayListTest1");
		
		#self.testTextField = self.getNode( "SoundEmitterTest" )
		#self.testTextField.setLocalPosition( Menge.vec2f( 100,1016:30 06.11.20070 ) )
		
		block.activate();
		
		self.layerAppend( "Deep", block )
	
		pass

	def onUpdate( self, timing ):
		Menge.setCamera2DPosition( self.camera_x, self.camera_y );

		self.camera_x += timing * 0
		pass
	pass
