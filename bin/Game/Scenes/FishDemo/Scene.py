import Menge

class Scene( Menge.Scene ):
	def onActivate( self ):
		self.camera_x = 512
		self.camera_y = 384

		Menge.setCamera2DPosition( self.camera_x, self.camera_y );

		self.node = Menge.createNodeFromXml(
			'<Node Name = "Water_01" Type = "Sprite">'
			'<ImageMap Name = "Water_01" />'
			'<Transformation Value = "1;0;0;1;0;0" />'
			'</Node> <!-- Sprite -->'
			)

		self.node.activate()
		self.layerAppend( "Water", self.node )
		
		self.node1 = self.getNode("Water_01")
	
		position = self.node1.getLocalPosition()
		print position
		
		self.node1.hide( True )
		
		block = Menge.createEntity("Block",Menge.vec2f( 0,0 ), Menge.vec2f( 1, 0 ) )
		
		block.activate();
		
		self.layerAppend( "Deep", block )
				
		#self.boo = 
		#print self.boo
		pass

	def onUpdate( self, timing ):
		Menge.setCamera2DPosition( self.camera_x, self.camera_y );

		self.camera_x += timing * 25
		pass
	pass