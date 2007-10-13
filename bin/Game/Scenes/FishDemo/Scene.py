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
		
		block = Menge.createEntity("Block",Menge.vec2f( 500, 100 ), Menge.vec2f( 1, 0 ) )
		
		block.activate();
		
		self.layerAppend( "Deep", block )
				
		#self.boo = 
		#print self.boo
		pass

	def onUpdate( self, timing ):
		Menge.setCamera2DPosition( self.camera_x, self.camera_y );

		self.camera_x += timing * 0.025
		pass
	pass