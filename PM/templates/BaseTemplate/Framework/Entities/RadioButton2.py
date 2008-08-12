#coding=UTF-8
import Menge
import Button2

# ------------------------------------------------------------------------------
# Class: RadioButton
# Description:
# - 
# ------------------------------------------------------------------------------
class RadioButton2( Button2.Button2 ):
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		Button2.Button2.__init__( self )
		self.index = -1
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setIndex
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setIndex( self, index ):
		self.index = index
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setIndex
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getIndex( self ):
		return self.index
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onHandleMouseButtonEvent
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onHandleMouseButtonEvent( self, hotSpot, button, isDown ):
		if isDown == True:
			#self.soundClick.stop()
			#self.soundClick.play()
			self.selectSprite( "pressed" )
			
			if self.cbPress != None:
				self.cbPress( self, self.index )
			
			self.isPushed = True
			pass
		else:
			if self.isNoActive == False:
				self.selectSprite( "active" )
				pass
			
			if self.cbClick != None and self.isPushed == True:
				self.cbClick( self, self.index )
			self.isPushed = False
			
			pass
		return False
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onEnter
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onEnter( self, hotSpot ):
		self.mouseInHotspot = True
		#self.soundDirect.play()
		self.selectSprite( "active" )
		
		if self.cbEnter != None:
			self.cbEnter( self, self.index )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLeave
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLeave( self, hotSpot ):	
		self.mouseInHotspot = False	

		self.selectSprite( "passive" )
		self.isPushed = False
		
		if self.cbLeave != None:
			self.cbLeave( self, self.index )
		pass