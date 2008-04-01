#coding=UTF-8
import Menge
import Const
import PhysXml
import Framework
from PoolNode import PoolNode
from Functor import Functor

# ------------------------------------------------------------------------------
# Class: Scene
# Description:
# - 
# ------------------------------------------------------------------------------
class Scene( Menge.Scene ):
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		
		self.GG = Menge.createEntity( "Pepelaz", Menge.vec2f( 500, 400 ), Menge.vec2f( 1, 0 ) )
		self.GG.setScene( self )
		
		self.box = []
		for i in range( 10 ):
			box = Menge.createEntityFromXml( "SolidBody", PhysXml.box, Menge.vec2f( 100 + i * 50, 200 ), Menge.vec2f( 1, 0 ) )
			box.setScene( self )
			self.box.append( box )
			
			pass
		
		self.pool = []
		
		cloudXml = '''
		<Node Name = "Cloud" Type = "Emitter">
			<Resource Name = "Particles"/>
			<Emitter Name = "SnakeClouds"/>
		</Node>
		'''
		self.poolCloud = PoolNode( cloudXml, self )
		self.pool.append( self.poolCloud )
		
		meteorXml = '''
		<Node Name = "Meteor" Type = "Emitter">
			<Resource Name = "Particles"/>
			<Emitter Name = "Meteor"/>
		</Node>
		'''
		self.poolMeteor = PoolNode( meteorXml, self )
		self.pool.append( self.poolMeteor )
		
		sparcleXml = '''
		<Node Name = "Sparcle" Type = "Emitter">
			<Resource Name = "Particles"/>
			<Emitter Name = "HookSparcles"/>
		</Node>
		'''
		self.poolSparcle = PoolNode( sparcleXml, self )
		self.pool.append( self.poolSparcle )
		
		crushXml = '''
		<Node Name = "Sparcle" Type = "Emitter">
			<Resource Name = "Particles"/>
			<Emitter Name = "Crush"/>
		</Node>
		'''
		
		self.poolCrush = PoolNode( crushXml, self )
		self.pool.append( self.poolCrush )
		
		self.subScenes = {}
		
		self.ptcRain = None
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLoader
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLoader( self ):
		self.layerAppend( "Front", self.GG )
		
		self.ptcRain = self.getNode( "DustRain" )
		
		for box in self.box:
			self.layerAppend( "Front", box )
			#box.applyConstantForce( 0, 500, 0, 0 )
			pass
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: onEmitterEnd
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onEmitterEnd( self, emitter ):
		#print "!!!"
		for pool in self.pool:
			if emitter in pool.getInstance():
				#print "???"
				pool.delete( emitter )
				break
				pass
		
		emitter.deactivate()
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onNewPoolNode
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onNewPoolNode( self, pool, node ):
		if pool == self.poolCloud or pool == self.poolMeteor:
			self.layerAppend( "BackParticles", node )
		elif pool == self.poolSparcle or pool == self.poolCrush:
			self.layerAppend( "Front", node )
		
		node.setListener( self )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onHandleMouseMove
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onHandleMouseMove( self, x, y, wheel ):
		#print self.poolMeteor.getCount( True )
		self.GG.updateAim()
		
		return False
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onActivate
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onActivate( self ):
		self.playParticles()
		
		self.ptcRain.setLooped( True )
		
		for i in range( 1, len(self.box) ):
			Menge.createHingeJoint( self.box[i], self.box[i-1], Menge.vec2f( 0, 0 ), Menge.vec2f( 0, 0 ), False )
			Menge.createHingeJoint( self.box[i-1], self.box[i], Menge.vec2f( 0, 0 ), Menge.vec2f( 0, 0 ), False )
			pass
		
		Menge.setCamera2DTarget( self.GG )
		Menge.enableCamera2DFollowing( True, 0.01 )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onHandleKeyEvent
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onHandleKeyEvent( self, key, char, isDown ):
		
		return False
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: updateHUD
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def updateHUD( self ):
		self.subScenes["HUD"].setHealth( self.GG.getHealth() )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: addSubScene
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def addSubScene( self, name, scene ):
		self.subScenes[name] = scene
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: playClouds
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def playClouds( self, delay ):
		width, height = Framework.getVec2f( self.getLayerSize( "Back" ) )
		x = Menge.randint( 0, width )
		y = Menge.randint( 0, height )
		
		#cloud = self.poolCloud.new( Menge.vec2f( x, y ), Menge.vec2f( 1, 0 ) )
		#cloud.activate()
		#cloud.play()
		
		Menge.schedule( delay, Functor( self.playClouds, delay ) )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: playClouds
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def playMeteor( self, delay ):
		width, height = Framework.getVec2f( self.getLayerSize( "Back" ) )
		x = Menge.randint( 0, width )
		y = 0
		
		meteor = self.poolMeteor.new( Menge.vec2f( x, y ), Menge.vec2f( 1, 0 ) )
		meteor.activate()
		meteor.play()
		
		Menge.schedule( delay, Functor( self.playMeteor, delay ) )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: playParticles
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def playParticles( self ):
		#self.playClouds( 5000 )
		#self.playMeteor( 5000 )
		#self.ptcRain.play()
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: playSparcle
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def playSparcle( self, x, y ):
		sparcle = self.poolSparcle.new( Menge.vec2f( x, y ), Menge.vec2f( 1, 0 ) )
		sparcle.activate()
		sparcle.play()
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: playCrush
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def playCrush( self, x, y ):
		crush = self.poolCrush.new( Menge.vec2f( x, y ), Menge.vec2f( 1, 0 ) )
		crush.activate()
		crush.play()
		
		pass