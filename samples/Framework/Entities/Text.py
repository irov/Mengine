import Menge

# ------------------------------------------------------------------------------
# Class: Text
# Description:
# - 
# ------------------------------------------------------------------------------
class Text( Menge.Entity ):
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		Menge.Entity( self )
		
		self.text = ""
		self.imageText = None
		
		self.cbMoveEnd = None
		self.cbColorEnd = None
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLoader
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLoader( self ):
		self.imageText = self.getChildren( "Data", True )
		self.imageText.setListener( self )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onActivate
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onActivate( self ):
		self.imageText.activate()
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setText
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setText( self, text ):
		self.text = text
		
		self.imageText.setText( text )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setCbMoveEnd
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCbMoveEnd( self, fn ):
		self.cbMoveEnd = fn
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setCbColorEnd
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCbColorEnd( self, fn ):
		self.cbColorEnd = fn
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onMoveEnd
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onMoveEnd( self ):
		if self.cbMoveEnd != None:
			self.cbMoveEnd( self )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getColor
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getColor( self ):
		return self.imageText.getColor()
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setColor
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setColor( self, color ):
		self.imageText.setColor( color )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setOutlineColor
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setOutlineColor( self, color ):
		self.imageText.setOutlineColor( color )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setAlpha
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setAlpha( self, alpha ):
		self.imageText.setAlpha( alpha )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: alphaTo
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def alphaTo( self, alpha, time ):
		self.imageText.alphaTo( alpha, time )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onColorEnd
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onColorEnd( self, node ):
		if self.cbColorEnd != None:
			self.cbColorEnd( self )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setHeight
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setHeight( self, height ):
		self.imageText.setHeight( height )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getHeight
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getHeight( self ):
		self.imageText.getHeight()
		pass
	
	pass