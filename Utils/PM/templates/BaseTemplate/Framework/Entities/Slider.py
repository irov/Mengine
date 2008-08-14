import Menge
import Framework

# ------------------------------------------------------------------------------
# Class: Slider
# Description:
# - 
# ------------------------------------------------------------------------------
class Slider( Menge.Entity ):
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		Menge.Entity.__init__( self )
		
		self.posX = 0
		self.posY = 0
		
		self.minPos = 0
		self.maxPos = 0
		
		self.imageBckGround = None
		self.btnHead = Menge.createEntity( "Button2", Menge.vec2f( 0, 0 ), Menge.vec2f( 1, 0 ) )
		
		self.hotSpot = None
		
		self.isCatched = False
		
		self.cbMove = None
		
		self.dispX = 0
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: onActivate
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onActivate( self ):
		self.getSelfPosition()
		pass
	
	# ----------------------------------------------------------------------------
	# Method: getSelfPosition
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getSelfPosition( self ):
		self.posX, self.posY = Framework.getVec2f( self.getScreenPosition() )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setSprites
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setSprites( self, background, passive, active, pressed ):
		if background != None:
			self.imageBckGround.setImageResource( background )
		
		self.btnHead.setSprites( passive, active, pressed, None )
		
		self.dispX = Menge.getVec2fX( self.btnHead.getNode("passive").getImageSize() ) / 2
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: onLoader
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLoader( self ):
		self.imageBckGround = self.getChildren( "Background", True )
		
		self.hotSpot = self.getChildren( "HotSpot", True )
		self.hotSpot.setListener( self )
		#self.btnHead.hotSpot.setListener( self )
		self.hotSpot.enableGlobalMouseEvent( True )
		
		self.addChildren( self.btnHead )
		
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: onHandleMouseButtonEvent
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onHandleMouseButtonEvent( self, hotSpot, button, isDown ):
		#print "TRUE"
		if isDown == True:
			self.getSelfPosition()
			
			self.isCatched = True
			self.btnHead.setLocalPosition( Menge.vec2f( Menge.getMouseX() - self.posX - self.dispX, 0 ) )
			self.imageBckGround.setPercentVisibility( Menge.vec2f( 0, 0 ), Menge.vec2f( 1 - self.getValue(), 0 ) )
			pos = self.getPos()
			if pos < self.minPos:
				self.btnHead.setLocalPosition( Menge.vec2f( self.minPos, 0 ) )
			if pos > self.maxPos:
				self.btnHead.setLocalPosition( Menge.vec2f( self.maxPos, 0 ) )
			if self.cbMove != None:
				self.cbMove()
			pass
		else:
			self.isCatched = False
			
		return False
		pass
	
	

	# ----------------------------------------------------------------------------
	# Metod: onLeave
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLeave( self, hotSpot ):
		#self.isCatched = False
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setRange
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setRange( self, minpos, maxpos ):
		self.minPos = minpos
		self.maxPos = maxpos
		self.btnHead.setLocalPosition( Menge.vec2f( self.maxPos, 0 ) )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getPos
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getPos( self ):
		coords = self.btnHead.getLocalPosition()
		return Menge.getVec2fX( coords )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getValue
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getValue( self ):
		return ( self.getPos() - self.minPos ) / ( self.maxPos - self.minPos )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setValue
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setValue( self, value ):
		pos = ( self.maxPos - self.minPos ) * value
		self.btnHead.setLocalPosition( Menge.vec2f( pos, 0 ) )
		
		self.imageBckGround.setPercentVisibility( Menge.vec2f( 0, 0 ), Menge.vec2f( 1 - self.getValue(), 0 ) )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setCbMove
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCbMove( self, fn ):
		self.cbMove = fn
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onGlobalHandleMouseMove
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onGlobalHandleMouseMove( self, hotSpot, x, y, whell ):
		if self.isCatched == False: return False
			
		self.btnHead.selectSprite( "pressed" )
		self.btnHead.hotSpot.deactivate()
		self.btnHead.setLocalPosition( Menge.vec2f( Menge.getMouseX() - self.posX-self.dispX, 0 ) )
		pos = self.getPos()
		if pos < self.minPos:
			self.btnHead.setLocalPosition( Menge.vec2f( self.minPos, 0 ) )
		if pos > self.maxPos:
			self.btnHead.setLocalPosition( Menge.vec2f( self.maxPos, 0 ) )
		if self.cbMove != None:
			self.cbMove()
		
		self.imageBckGround.setPercentVisibility( Menge.vec2f( 0, 0 ), Menge.vec2f( 1 - self.getValue(), 0 ) )
		
		return False
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onGlobalHandleMouseButtonEvent
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onGlobalHandleMouseButtonEvent(	self, hotSpot, button, isDown ):
		if isDown == False:
			self.isCatched = False
			self.btnHead.selectSprite( "passive" )
			self.btnHead.hotSpot.activate()
			pass
		
		return False
		pass
	
	# ----------------------------------------------------------------------------
	# Method: setupHotSpot
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setupHotspot( self, pointsBack, pointsHead ):
		Framework.setupHotspot( self.hotSpot, pointsBack )
		
		if pointsHead == None: return
		#if pointsHead in ( 0, 1, 2, 3 ):
		self.setupHotspotFromImage( None, pointsHead )
		#else:
			#self.btnHead.setupHotspot( pointsHead )
		pass
	
	# ----------------------------------------------------------------------------
	# Method: setupHotSpotFromImage
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setupHotspotFromImage( self, spriteBack, spriteHead ):
		self.btnHead.setupHotSpotFromImage( spriteHead )
		pass
		
