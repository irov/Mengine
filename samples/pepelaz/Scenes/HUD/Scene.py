#coding=UTF-8
import Menge

# ----------------------------------------------------------------------------
# Metod: Scene
# Description:
# - 
# ----------------------------------------------------------------------------
class Scene( Menge.Scene ):
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		
		self.imageHealth = None
		
		self.scene = None
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLoader
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLoader( self ):
		self.imageHealth = self.getNode( "HealthIndicator" )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: setHealth
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def setHealth( self, health ):
		self.imageHealth.setPercentVisibility( Menge.vec2f( 0, 1 - health / 100 ), Menge.vec2f( 0, 0 ) )
		
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: onSubScene
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onSubScene( self, scene ):
		self.scene = scene
		scene.addSubScene( "HUD", self )
		pass