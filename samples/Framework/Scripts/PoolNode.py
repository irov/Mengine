import Menge

class PoolNode:
	
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self, nodeCode, listener ):
		self.instance = []
		self.free = []
		self.nodeCode = nodeCode
		self.listener = listener
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: new
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def new( self, position, direction ):
		node = None
		
		if len( self.free ) == 0:
			node = Menge.createNodeFromXml( self.nodeCode )
			if self.listener:
				self.listener.onNewPoolNode( self, node )
		else:
			node = self.free.pop(0)
			
		node.setLocalPosition( position )
		node.setLocalDirection( direction )

		self.instance.append(node)
		return node
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: delete
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def delete( self, node ):
		self.free.append( node )
		self.instance.remove( node )
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: foreach
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def foreach( self, functor ):
		for node in self.instance:
			functor( node )
		pass	
		
	# ----------------------------------------------------------------------------
	# Metod: clear
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def clear( self ):
		for node in self.instance:
			self.free.append( node )

		self.instance = []
		pass
	
	pass