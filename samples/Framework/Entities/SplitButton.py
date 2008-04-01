import Menge


# ------------------------------------------------------------------------------
# Class: SplitButton
# Description:
# - 
# ------------------------------------------------------------------------------
class SplitButton( Menge.Entity ):
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		Menge.Entity.__init__( self )
		
		self.posX = 0
		self.posY = 0
		
		self.cbClickOn = None		#funkciya kotoraya vypolnyaetsya pri najatii na knopku
		self.cbClickOff = None
		
		self.scene = None			#scena v kotoroy nah knopka
		
		#spisok peremennyh kartinok dlya raznyh sostoyaniy
		self.imagePassiveOn = None	
		self.imageActiveOn = None
		self.imagePressedOn = None
		self.imageNoActiveOn = None
		self.imagePassiveOff = None	
		self.imageActiveOff = None
		self.imagePressedOff = None
		self.imageNoActiveOff = None
			
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
		self.imagePassiveOff = self.getChildren( "ButtonPassiveOff", True )
		self.imageActiveOff = self.getChildren( "ButtonActiveOff", True )
		self.imagePressedOff = self.getChildren( "ButtonPressedOff", True )
		self.imageNoActiveOff = self.getChildren( "ButtonNoActiveOff", True )
		self.imagePassiveOn = self.getChildren( "ButtonPassiveOn", True )
		self.imageActiveOn = self.getChildren( "ButtonActiveOn", True )
		self.imagePressedOn = self.getChildren( "ButtonPressedOn", True )
		self.imageNoActiveOn = self.getChildren( "ButtonNoActiveOn", True )
		

		self.hotSpot = self.getChildren( "ButtonHotSpot", True )
		self.hotSpot.setHotspotListener( self )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: selectSprite
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def selectSprite( self, sprite ):
		self.imagePassiveOff.hide( True )
		self.imageActiveOff.hide( True )
		self.imagePressedOff.hide( True )
		self.imageNoActiveOff.hide( True )
		self.imagePassiveOn.hide( True )
		self.imageActiveOn.hide( True )
		self.imagePressedOn.hide( True )
		self.imageNoActiveOn.hide( True )
		
		if sprite == "passiveOff":
			self.imagePassiveOff.hide( False )
			print "PASSIVEoff"
		if sprite == "activeOff":
			self.imageActiveOff.hide( False )
		if sprite == "pressedOff":
			self.imagePressedOff.hide( False )
		if sprite == "noactiveOff":
			self.imageNoActiveOff.hide( False )
		if sprite == "passiveOn":
			self.imagePassiveOn.hide( False )
			print "PASSIVEon"
		if sprite == "activeOn":
			self.imageActiveOn.hide( False )
		if sprite == "pressedOn":
			self.imagePressedOn.hide( False )
		if sprite == "noactiveOn":
			self.imageNoActiveOn.hide( False )
			
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: setSprites
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setSprites( self, passiveOff, activeOff, pressedOff, noactiveOff, passiveOn, activeOn, pressedOn, noactiveOn ):
			self.imagePassiveOff.setImageResource( passiveOff )
			self.imageActiveOff.setImageResource( activeOff )
			self.imagePressedOff.setImageResource( pressedOff )
			if noactiveOff != None:
				self.imageNoActiveOff.setImageResource( noactiveOff )
			self.imagePassiveOn.setImageResource( passiveOn )
			self.imageActiveOn.setImageResource( activeOn )
			self.imagePressedOn.setImageResource( pressedOn )
			if noactiveOn != None:
				self.imageNoActiveOn.setImageResource( noactiveOn )
		
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
					self.cbClickOff()
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