#coding=UTF-8
import Menge
import Button

# ------------------------------------------------------------------------------
# Class: SplitButton
# Description:
# - 
# ------------------------------------------------------------------------------
class SplitButton( Button.Button ):
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		Button.Button.__init__( self )
		
		self.posX = 0
		self.posY = 0
		
		self.cbClickOn = None		#funkciya kotoraya vypolnyaetsya pri najatii na knopku
		self.cbClickOff = None
		
		self.scene = None			#scena v kotoroy nah knopka
		
		#spisok peremennyh kartinok dlya raznyh sostoyaniy
			
		self.hotSpot = None
		
		self.isPushed = False
		
		self.isNoActive = False
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onActivate
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onActivate( self ):
		if self.isPushed == False:
			self.selectSprite( "passiveOff" )
		else:
			#print "Activate"
			self.selectSprite( "passiveOn" )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLoader
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLoader( self ):
		self.sprites = {
			  "passiveOff": self.getChildren( "ButtonPassiveOff", True )
			, "activeOff": self.getChildren( "ButtonActiveOff", True )
			, "pressedOff": self.getChildren( "ButtonPressedOff", True )
			, "attentionOn": self.getChildren( "ButtonNoActiveOff", True )
			, "passiveOn": self.getChildren( "ButtonPassiveOn", True )
			, "activeOn": self.getChildren( "ButtonActiveOn", True )
			, "pressedOn": self.getChildren( "ButtonPressedOn", True )
			, "attentionOn": self.getChildren( "ButtonNoActiveOn", True )
			}
		

		self.hotSpot = self.getChildren( "ButtonHotSpot", True )
		self.hotSpot.setListener( self )
		
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: setSprites
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setSprites( self, passiveOff, activeOff, pressedOff, noactiveOff, passiveOn, activeOn, pressedOn, noactiveOn ):
			self.sprites["passiveOff"].setImageResource( passiveOff )
			self.sprites["activeOff"].setImageResource( activeOff )
			self.sprites["pressedOff"].setImageResource( pressedOff )
			if noactiveOff != None:
				self.sprites["attentionOff"].setImageResource( noactiveOff )
			self.sprites["passiveOn"].setImageResource( passiveOn )
			self.sprites["activeOn"].setImageResource( activeOn )
			self.sprites["pressedOn"].setImageResource( pressedOn )
			if noactiveOn != None:
				self.sprites["attentionOn"].setImageResource( noactiveOn )
		
			pass
		
	# ----------------------------------------------------------------------------
	# Metod: onEnter
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onEnter( self, hotSpot ):
		if self.isPushed == False:
			self.selectSprite( "activeOff" )
		else:
			self.selectSprite( "activeOn" )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLeave
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLeave( self, hotSpot ):
		#print "LEAVE"
		if self.isNoActive == False:		
			if self.isPushed == False:
				self.selectSprite( "passiveOff" )
			else:
				self.selectSprite( "passiveOn" )
				#print "Leave"
			pass
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: onHandleMouseButtonEvent
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onHandleMouseButtonEvent( self, hotSpot, button, isDown ):
		if isDown == True:
			if self.isPushed == False:
				self.selectSprite( "pressedOff" )
			else:
				self.selectSprite( "pressedOn" )
			pass
		else:
			if self.isPushed == False:
				self.selectSprite( "activeOn" )
				self.isPushed = True
				if self.cbClickOff != None:
					self.cbClickOff( )
				pass
			else:
				self.selectSprite( "activeOff" )
				self.isPushed = False
				if self.cbClickOn != None:
					self.cbClickOn()
				pass
			
			pass
		return True
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setCbClick
	# Description:
	# - ustanavlivaet funkciyu kotoraya budet vypolnyatsya pri najatii knopki mishi
	# ----------------------------------------------------------------------------
	def setCbClickOn( self, fn ):
		self.cbClickOn = fn
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: setCbClickOn
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCbClickOff( self, fn ):
		self.cbClickOff = fn
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
	# Metod: getPush
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getCheck( self ):
		return self.isPushed
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setCheck
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCheck( self, check ):
		if check == True:
			self.selectSprite( "passiveOn" )
		else:
			self.selectSprite( "passiveOff" )
		
		self.isPushed = check
		pass

		
	pass