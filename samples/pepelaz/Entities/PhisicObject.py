#coding=UTF-8
import Menge

# ------------------------------------------------------------------------------
# Class: PhisicObject
# Description:
# - 
# ------------------------------------------------------------------------------
class PhisicObject( Menge.Entity ):
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		Menge.Entity.__init__( self )
		
		self.scene = None
		
		self.imagePassive = None
		
		self.cbCollide = None
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setScene
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setScene( self, scene ):
		self.scene = scene
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLoader
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLoader( self ):
		self.imagePassive = self.getChildren( "Passive", True )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setShape
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setScene( self, scene ):
		self.scene = scene
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setShape
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setShape( self, passive ):
		if passive != None:
			self.imagePassive.setImageResource( passive )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setCbCollide
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCbCollide( self, fn ):
		self.cbCollide = fn
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onCollide
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onCollide( self, activeObj, passiveObj, x, y, nx, ny ):
		if self.cbCollide == None: return
		
		self.cbCollide( activeObj, passiveObj, x, y, nx, ny )
		pass