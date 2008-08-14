import Menge
from PoolEntity import PoolEntity

class PoolNode( PoolEntity ):
	
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self, nodeCode, listener ):
		PoolEntity.__init__( self, None, listener )
		self.nodeCode = nodeCode
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
		
	
	pass