import Menge

import Grid

class Pole:
	def __init__( self ):
		self.size_x = 0
		self.size_y = 0
		self.buffer = []
		self.pole_img = []
		pass

	def loadLevel( self, scene, file ):
		
		f = open( file )
		try:
			size = f.readline();

			sz = size.split(' ');
			
			self.size_x = int(sz[0]);
			self.size_y = int(sz[1]);
			self.img_size_x = self.size_x - 1
			self.img_size_y = self.size_y - 1
			self.pole_img_count = self.img_size_x * self.img_size_y
			
			for line in f:
				line = line.rstrip( '\n' );
				line = line.rstrip( '\r' );   
				data = line.split(' ')
				for v in data:
					self.buffer.append( int( v ) );
					pass
				pass
			
		finally:
			f.close()
		pass

	
		for index in xrange( 0, self.pole_img_count ):
			x = int( index % self.img_size_x ) * 54 + 200
			y = int( index / self.img_size_x ) * 54 + 200
			en = Menge.createEntity( "Grid", Menge.vec2f(x,y), Menge.vec2f(1,0) )
			en.activate();

			scene.layerAppend( "Main", en )
			self.pole_img.append( en )
			pass 

		self.refreshImgPoleAll()
		pass


	def refreshImgPole( self, i, j ):
		frame = 0;
		frame += self.isPoleDataWall( i + 0, j + 0 ) * 8;
		frame += self.isPoleDataWall( i + 1, j + 0 ) * 4;
		frame += self.isPoleDataWall( i + 1, j + 1 ) * 2;
		frame += self.isPoleDataWall( i + 0, j + 1 ) * 1;

		self.setImgFrame( i, j, frame )
		pass



	def refreshImgPoleAll( self ):
		w = self.img_size_x
	
		for index in xrange( 0, self.pole_img_count ):
			i = index % w
			j = index / w
			self.refreshImgPole( i, j )
			pass
		pass


	def setImgFrame( self, i, j, frame ):
		index = j * self.img_size_x + i;
		self.pole_img[ index ].setImageIndex( frame )
		pass


	def isPoleDataWall( self, i, j ):
		w = self.size_x
		h = self.size_y
		
		if i < 0 or i >= w or j < 0 or j >= h :
			return 0
			pass
		
		if self.buffer[ w * j + i ] == 1 :
			return 1
			pass

		return 0
		pass

	pass