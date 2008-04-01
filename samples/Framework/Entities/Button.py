import Menge
from Functor import Functor
import Framework


# ------------------------------------------------------------------------------
# Class: Button
# Description:
# - 
# ------------------------------------------------------------------------------
class Button( Menge.Entity ):
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		Menge.Entity.__init__( self )
		
		self.posX = 0
		self.posY = 0
		
		self.cbClick = None		#funkciya kotoraya vypolnyaetsya pri najatii na knopku
		self.cbPress = None
		self.cbEnter = None
		self.cbLeave = None
		
		self.scene = None			#scena v kotoroy nah knopka
		
		#spisok peremennyh kartinok dlya raz
		self.imagePassive = None	
		self.imageActive = None
		self.imagePressed = None
		self.imageAttention = None
		
		#self.soundDirect = None
		#self.soundClick = None
			
		self.hotSpot = None
		
		self.isNoActive = False
		self.isPushed = False
		
		self.showAtt = False
		
		self.mouseInHotspot = False
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onActivate
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onActivate( self ):
		self.imagePassive.hide( False )
		self.imageActive.hide( True )
		self.imagePressed.hide( True )
		self.imageAttention.hide( True )
		pass
	
	
	# ----------------------------------------------------------------------------
	# Metod: setAttention
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def showAttention( self, show ):
		
		if show != None:
			self.showAtt = show
		
		if self.isNoActive: return False
		
		#print self.showAtt
		if self.showAtt == False: return False	
			
		self.imagePassive.hide( True )
		self.imageAttention.hide( False )
		Menge.schedule( 200, self.hideAttention )
			
		#Menge.schedule( Menge.randint( 20000, 40000 ), self.showAttention ) 
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: hideAttention
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def hideAttention( self ):
		self.imagePassive.hide( False )
		self.imageAttention.hide( True )
		Menge.schedule( 200, Functor( self.showAttention, None ) )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLoader
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLoader( self ):
		self.imagePassive = self.getChildren( "ButtonPassive", True )
		self.imageActive = self.getChildren( "ButtonActive", True )
		self.imagePressed = self.getChildren( "ButtonPressed", True )
		self.imageAttention = self.getChildren( "ButtonAttention", True )
		
		#self.soundDirect = self.getChildren( "ButtonSoundDirect", True )
		#self.soundClick = self.getChildren( "ButtonSoundClick", True )
		

		self.hotSpot = self.getChildren( "ButtonHotSpot", True )
		self.hotSpot.setHotspotListener( self )
		self.hotSpot.enableGlobalMouseEvent( True )
		
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: setSprites
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setSprites( self, passive, active, pressed, attention ):
		if passive != None:
			self.imagePassive.setImageResource( passive )
		if active != None:
			self.imageActive.setImageResource( active )
		if pressed != None:
			self.imagePressed.setImageResource( pressed )
		if attention != None:
			self.imageAttention.setImageResource( attention )
			#self.showAttention()
			pass
			
		#self.addChildren( self.imagePassive )
		#self.addChildren( self.imageActive )
		#self.addChildren( self.imagePressed )
		
			pass
		
	# ----------------------------------------------------------------------------
	# Metod: onEnter
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onEnter( self, hotSpot ):
		self.mouseInHotspot = True
		#self.soundDirect.play()
		if self.isPushed == True:
			self.selectSprite( "pressed" )
		else:
			self.selectSprite( "active" )
		
		if self.cbEnter != None:
			self.cbEnter()
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLeave
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLeave( self, hotSpot ):	
		self.selectSprite( "passive" )
		#self.isPushed = False
		
		self.mouseInHotspot = False
		
		if self.cbLeave != None:
			self.cbLeave()
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
				self.cbPress()
			
			self.isPushed = True
			pass
		else:
			if self.isNoActive == False:
				self.selectSprite( "passive" )
				pass
				
			if self.cbClick != None and self.isPushed == True:
				self.cbClick()
			self.isPushed = False
			
			pass
		return False
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onHandleMouseButtonGlobalEvent
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onGlobalHandleMouseButtonEvent( self, hotSpot, button, isDown ):
		if self.mouseInHotspot == False:
			if isDown == False and self.isPushed == True:
				self.isPushed = False
				
		return False
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setCbClick
	# Description:
	# - ustanavlivaet funkciyu kotoraya budet vypolnyatsya pri najatii knopki mishi
	# ----------------------------------------------------------------------------
	def setCbClick( self, fn ):
		self.cbClick = fn
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: setCbEnter
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCbEnter( self, fn ):
		self.cbEnter = fn
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setCbLeave
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCbLeave( self, fn ):
		self.cbLeave = fn
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setCbPress
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCbPress( self, fn ):
		self.cbPress = fn
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
	# Metod: selectSprite
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def selectSprite( self, sprite ):
		self.imagePassive.hide( True )
		self.imageActive.hide( True )
		self.imagePressed.hide( True )
		self.imageAttention.hide( True )
		
		if sprite == "passive":
			self.imagePassive.hide( False )
		elif sprite == "active":
			self.imageActive.hide( False )
		elif sprite == "pressed":
			self.imagePressed.hide( False )
		elif sprite == "attention":
			self.imageAttention.hide( False )
		
	pass
	
	# ----------------------------------------------------------------------------
	# Metod: setupHotspot
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setupHotspot( self, points ):
		Framework.setupHotspot( self.hotSpot, points )
		pass