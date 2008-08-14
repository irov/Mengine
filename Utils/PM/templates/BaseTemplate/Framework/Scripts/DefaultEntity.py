#cpdong=UTF-8
import Menge

# ------------------------------------------------------------------------------
# Class: DefaultEntity
# Description:
# - 
# ------------------------------------------------------------------------------
class DefaultEntity( Menge.Entity ):
	# ----------------------------------------------------------------------------
	# Method: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		Menge.Entity.__init__( self )
		
		self.cbMoveEnd = None
		self.cbRotateEnd = None
		
		pass
	
	# ----------------------------------------------------------------------------
	# Method: setCbMoveEnd
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCbMoveEnd( self, fn ):
		self.cbMoveEnd = fn
		pass
	
	# ----------------------------------------------------------------------------
	# Method: onRotateEnd
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCbRotateEnd( self, fn ):
		self.cbRotateEnd = fn
		pass
	
	# ----------------------------------------------------------------------------
	# Method: onMoveEnd
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onMoveEnd( self ):
		if self.cbMoveEnd != None:
			self.cbMoveEnd( self )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Method: onRotateEnd
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onRotateEnd( self ):
		if self.cbRotateEnd != None:
			self.cbRotateEnd( self )
		
		pass
	
	pass