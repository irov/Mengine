#coding=UTF-8
import Menge
import Button2
import Framework

# ------------------------------------------------------------------------------
# Class: TextButton
# Description:
# - 
# ------------------------------------------------------------------------------
class TextButton( Button2.Button2 ):
	# ----------------------------------------------------------------------------
	# Method: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		Button2.Button2.__init__( self )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Method: onLoader
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLoader( self ):
		self.text = {
			"Title": self.getChildren( "Title", True )
			}
		
		self.hotSpot = self.getChildren( "HotSpot", True )
		self.hotSpot.setListener( self )
		self.hotSpot.enableGlobalMouseEvent( True )
		
		self.setColors( None )
		pass
	
	# ----------------------------------------------------------------------------
	# Method: setColors
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setColors( self, passive, active = None, pressed = None, attention = None ):
		self.color = {
			  "passive": passive
			, "active": active
			, "pressed": pressed
			, "attention": attention	
			}
		
		pass
	
	# ----------------------------------------------------------------------------
	# Method: selectColor
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def selectColor( self, name ):
		if self.color[name] != None:
			self.text["Title"].setColor( self.color[name] )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Method: selectSprite
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def selectSprite( self, name ):
		self.selectColor( name )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Method: setupHotSpotFromPicture
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setupHotSpotFromImage( self, left = 0, top = 0, right = 0, bottom = 0 ):
		width, height = Framework.getVec2f( self.text["Title"].getLength() )
		'''
		if self.text["Title"].getCenterAlign() == True:
			center = 1
		else:
			center = 0
		'''
		center = 0
		
		points = []
		points.append( Menge.vec2f( 0 - width/2*center - left, 0 - height/2*center - top) )
		points.append( Menge.vec2f( width - width/2*center + right, 0 - height/2*center - top ) )
		points.append( Menge.vec2f( width - width/2*center + right, height - height/2*center + bottom ) )
		points.append( Menge.vec2f( 0 - width/2*center - left, height - height/2*center + bottom ) )
		
		
		self.setupHotspot( points )
		pass
	
	# ----------------------------------------------------------------------------
	# Method: setCaption
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setCaption( self, text ):
		self.text["Title"].setText( text )
		
		pass
	
	pass