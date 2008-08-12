import Menge
from Functor import Functor
import Framework
 

# ------------------------------------------------------------------------------
# Class: Button
# Description:
# - 
# ------------------------------------------------------------------------------
class Button2( Menge.Entity ):
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
		self.cbMoveEnd = None
		
		self.scene = None			#scena v kotoroy nah knopka
		
		#spisok peremennyh kartinok dlya raz
		#self.imagePassive = None	
		#self.imageActive = None
		#self.imagePressed = None
		#self.imageAttention = None
		
		#self.soundDirect = None
		#self.soundClick = None
			
		self.hotSpot = None
		
		self.isNoActive = False
		self.isPushed = False
		
		self.showAtt = False
		
		self.mouseInHotspot = False
		
		self.resource = {
			  "passive": False
			, "active": False
			, "pressed": False
			, "attention": False
			}
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onActivate
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onActivate( self ):
		self.selectSprite( "passive" )
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
			
		self.selectSprite( "attention" )
		Menge.schedule( 200, self.hideAttention )
			
		#Menge.schedule( Menge.randint( 20000, 40000 ), self.showAttention ) 
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: hideAttention
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def hideAttention( self ):
		self.selectSprite( "passive" )
		Menge.schedule( 200, Functor( self.showAttention, None ) )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLoader
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLoader( self ):
		self.sprites = {
			"passive": self.getChildren( "ButtonPassive", True ),
			"active": self.getChildren( "ButtonActive", True ),
			"pressed": self.getChildren( "ButtonPressed", True ),
			"attention": self.getChildren( "ButtonAttention", True )
			}

		self.hotSpot = self.getChildren( "ButtonHotSpot", True )
		self.hotSpot.setListener( self )
		self.hotSpot.enableGlobalMouseEvent( True )
		
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: setSprites
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setSprites( self, passive = None, active = None, pressed = None, attention = None):
		if passive != None:
			self.sprites["passive"].setImageResource( passive )
			self.resource["passive"] = True
			pass
		if active != None:
			self.sprites["active"].setImageResource( active )
			self.resource["active"] = True
			pass
		if pressed != None:
			self.sprites["pressed"].setImageResource( pressed )
			self.resource["pressed"] = True
			pass
		if attention != None:
			self.sprites["attention"].setImageResource( attention )
			self.resource["attention"] = True
			pass
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
			self.cbEnter( self )
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
			self.cbLeave( self )
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: onHandleMouseButtonEvent
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onHandleMouseButtonEvent( self, hotSpot, button, isDown ):
		if button != 0: return False

		
		if isDown == True:
			#self.soundClick.stop()
			#self.soundClick.play()
			self.selectSprite( "pressed" )
			
			if self.cbPress != None:
				self.cbPress( self )
			
			self.isPushed = True
			pass
		else:
			if self.isNoActive == False:
				self.selectSprite( "active" )
				pass
				
			if self.cbClick != None and self.isPushed == True:
				self.cbClick( self )
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
		if button != 0: return False
		
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
	# Method: setCbMoveEnd
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCbMoveEnd( self, fn ):
		self.cbMoveEnd = fn
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
	def selectSprite( self, name ):
		if self.resource[name] == False: return
		
		for sprite in self.sprites.values():
			sprite.hide( True )
		
		self.sprites[name].hide( False )
		
		pass
		
	pass
	
	# ----------------------------------------------------------------------------
	# Metod: setupHotspot
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setupHotspot( self, points ):
		Framework.setupHotspot( self.hotSpot, points )
		pass
	
	# ----------------------------------------------------------------------------
	# Method: setupHotSpotFromPicture
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setupHotSpotFromImage( self, name, left = 0, top = 0, right = 0, bottom = 0 ):
		width, height = Framework.getVec2f( self.sprites[name].getImageSize() )
		if self.sprites[name].getCenterAlign() == True:
			center = 1
		else:
			center = 0
		
		points = []
		points.append( Menge.vec2f( 0 - width/2*center - left, 0 - height/2*center - top) )
		points.append( Menge.vec2f( width - width/2*center + right, 0 - height/2*center - top ) )
		points.append( Menge.vec2f( width - width/2*center + right, height - height/2*center + bottom ) )
		points.append( Menge.vec2f( 0 - width/2*center - left, height - height/2*center + bottom ) )
		
		
		self.setupHotspot( points )
		pass
	
	# ----------------------------------------------------------------------------
	# Method: forEachSprite
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def forEachSprite( self, functor ):
		for sprite in self.sprites.values():
			functor( sprite )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Method: blockHotSpot
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def blockHotSpot( self, block ):
		if block == True:
			self.hotSpot.disable()
		else:
			self.hotSpot.enable()
		
		pass
	
	# ----------------------------------------------------------------------------
	# Method: hotSpotBlocked
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def hotSpotBlocked( self ):
		return not self.hotSpot.isEnable()
		pass
	
	# ----------------------------------------------------------------------------
	# Method: getNode
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getNode( self, name ):
		return self.sprites[name]
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