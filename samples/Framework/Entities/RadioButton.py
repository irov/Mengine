#coding=UTF-8
import Menge
import Button

# ------------------------------------------------------------------------------
# Class: RadioButton
# Description:
# - 
# ------------------------------------------------------------------------------
class RadioButton( Button.Button ):
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		Button.Button.__init__( self )
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
	# Metod: onHandleMouseButtonEvent
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onHandleMouseButtonEvent( self, hotSpot, button, isDown ):
		if isDown == True:
			#self.soundClick.stop()
			#self.soundClick.play()
			self.imagePassive.hide( True )
			self.imageActive.hide( True )
			self.imagePressed.hide( False )
			
			if self.cbPress != None:
				self.cbPress( self.index )
			
			self.isPushed = True
			pass
		else:
			if self.isNoActive == False:
				self.imagePassive.hide( True )
				self.imageActive.hide( False )
				self.imagePressed.hide( True )
				pass
				
			if self.cbClick != None and self.isPushed == True:
				self.cbClick( self.index )
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
		#self.soundDirect.play()
		self.imagePassive.hide( True )
		self.imageActive.hide( False )
		self.imagePressed.hide( True )
		
		if self.cbEnter != None:
			self.cbEnter( self.index )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLeave
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLeave( self, hotSpot ):	
		self.imagePassive.hide( False )
		self.imageActive.hide( True )
		self.imagePressed.hide( True )
		self.isPushed = False
		
		if self.cbLeave != None:
			self.cbLeave( self.index )
		pass