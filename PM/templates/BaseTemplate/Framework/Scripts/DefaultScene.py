#coding=UTF-8
import Menge

# ------------------------------------------------------------------------------
# Class: Scene
# Description:
# - 
# ------------------------------------------------------------------------------
class Scene( Menge.Scene ):
	# ----------------------------------------------------------------------------
	# Method: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		self.scene = None
		self.subScenes = {}
		
		pass
	
	# ----------------------------------------------------------------------------
	# Method: onSubScene
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onSubScene( self, scene ):
		self.scene = scene
		scene.addSubScene( self.getName(), self )
		pass
	
	# ----------------------------------------------------------------------------
	# Method: addSubScene
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def addSubScene( self, name, scene ):
		self.subScenes[name] = scene
		pass
