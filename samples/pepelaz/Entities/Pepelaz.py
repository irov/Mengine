#coding=UTF-8
import Menge
from PhisicObject import PhisicObject
from Chain import Chain
import Framework
import PhysXml
import Keys

# ------------------------------------------------------------------------------
# Class: Pepelaz
# Description:
# - 
# ------------------------------------------------------------------------------
class Pepelaz( PhisicObject ):
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self ):
		PhisicObject.__init__( self )
		
		self.chain = []
		self.hook = []
		
		self.imageScrewUp = None
		self.imageScrewDown = None
		#self.imageLeftEngine = None
		#self.imageRightEngine = None
		
		self.hotSpot = None
		
		self.hookObject = [ None, None, None, None ]
		self.hookReady = [ False, False, False, False ]
		self.hookJoint = [ None, None, None, None ]
		
		self.currentChain = 0
		
		self.moveForce = 0.03
		self.shootForce = 0.2
		
		self.moveForceVector = [ 0, 0 ]
		
		self.hookLenght = 0
		
		self.imageAim = None
		
		self.health = 100
		self.minVelocityToDamage = 100
		
		self.joints = []
		
		self.ptcLeftFire = None
		self.ptcRightFire = None
		self.ptcSmoke = None
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onLoader
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onLoader( self ):
		self.imageScrewUp = self.getChildren( "ScrewUp", True )
		self.imageScrewDown = self.getChildren( "ScrewDown", True )
		self.imageScrewDown.hide( True )
		
		self.ptcLeftFire = self.getChildren( "LeftFire", True )
		self.ptcRightFire = self.getChildren( "RightFire", True )
		self.ptcSmoke = self.getChildren( "Smoke", True )
		
		self.imageAim = self.getChildren( "Aim", True )
		
		#self.imageLeftEngine = self.getChildren( "LeftEngine", True )
		#self.imageRightEngine = self.getChildren( "RightEngine", True )
		
		self.hotSpot = self.getChildren( "HotSpot", True )
		self.hotSpot.setHotspotListener( self )
		self.hotSpot.enableGlobalKeyEvent( True )
		self.hotSpot.enableGlobalMouseEvent( True )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onActivate
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onActivate( self ):
		
		selfX, selfY = Framework.getVec2f( self.getLocalPosition() )
		
		self.aim = Menge.createEntityFromXml( "SolidBody", PhysXml.pepelaz["NullObject"], Menge.vec2f( selfX, selfY ), Menge.vec2f( 1, 0 ) )
		self.aim.activate()
		self.scene.layerAppend( "AimLayer", self.aim )
		Menge.createHingeJoint( self, self.aim, Menge.vec2f( 0, 0 ), Menge.vec2f( 0, 0 ), False )
		self.aim.addChildren( self.imageAim )
		self.aim.addChildren( self.ptcSmoke )
		
		for i in range( 4 ):
			chain = Chain()
			chain.setScene( self.scene )
			chain.createChain( PhysXml.pepelaz["Chain"], 10, selfX, selfY + 30, 0, 10, True )
			joint = Menge.createHingeJoint( self, chain.getFirstSection(), Menge.vec2f( 0, 0 ), Menge.vec2f( 0, 0 ), False )
			self.joints.append( joint )
			
			lastSection = chain.getLastSection()
			chainX, chainY = Framework.getVec2f( lastSection.getLocalPosition() )
			hook = Menge.createEntityFromXml( "SolidBody", PhysXml.pepelaz["Hook"], Menge.vec2f( chainX, chainY + 20 ), Menge.vec2f( 1, 0 ) )
			hook.setCbCollide( self.onHookCollide )
			hook.activate()
			self.scene.layerAppend( "Front", hook )
			joint = Menge.createHingeJoint( lastSection, hook, Menge.vec2f( 0, 0 ), Menge.vec2f( 0, 0 ), False )
			self.joints.append( joint )
			
			self.chain.append( chain )
			self.hook.append( hook )
			pass
			
		self.hookLength = 30 #self.chain.getPixelLength()
		
		self.leftChassis = Menge.createEntityFromXml( "SolidBody", PhysXml.pepelaz["Chassis"], Menge.vec2f( selfX - 30, selfY ), Menge.vec2f( 1, 0 ) )
		self.leftChassis.setShape( "PplLeftChassis" )
		self.leftChassis.activate()
		joint = Menge.createHingeJoint( self, self.leftChassis, Menge.vec2f( -25, 0 ), Menge.vec2f( 0, 0 ), False )
		self.joints.append( joint )
		self.scene.layerAppend( "Front", self.leftChassis )
		
		self.rightChassis = Menge.createEntityFromXml( "SolidBody", PhysXml.pepelaz["Chassis"], Menge.vec2f( selfX + 30, selfY ), Menge.vec2f( 1, 0 ) )
		self.rightChassis.setShape( "PplRightChassis" )
		self.rightChassis.activate()
		joint = Menge.createHingeJoint( self, self.rightChassis, Menge.vec2f( 25, 0 ), Menge.vec2f( 0, 0 ), False )
		self.joints.append( joint )
		self.scene.layerAppend( "Front", self.rightChassis )
		
		self.leftEngine = Menge.createEntityFromXml( "SolidBody", PhysXml.pepelaz["Engine"], Menge.vec2f( selfX - 25, selfY + 40 ), Menge.vec2f( 1, 0 ) )
		self.leftEngine.setShape( "PplLeftEngine" )
		self.leftEngine.activate()
		joint = Menge.createHingeJoint( self, self.leftEngine, Menge.vec2f( 0, 0 ), Menge.vec2f( 0, 0 ), False )
		self.joints.append( joint )
		self.scene.layerAppend( "Front", self.leftEngine )
		self.leftEngine.addChildren( self.ptcLeftFire )
		self.leftEngine.removeChildren( self.leftEngine.imagePassive )
		self.leftEngine.addChildren( self.leftEngine.imagePassive )
		self.ptcLeftFire.setLooped( True )
		
		self.rightEngine = Menge.createEntityFromXml( "SolidBody", PhysXml.pepelaz["Engine"], Menge.vec2f( selfX + 25, selfY + 38 ), Menge.vec2f( 1, 0 ) )
		self.rightEngine.setShape( "PplRightEngine" )
		self.rightEngine.activate()
		joint = Menge.createHingeJoint( self, self.rightEngine, Menge.vec2f( 0, 0 ), Menge.vec2f( 0, 0 ), False )
		self.joints.append( joint )
		self.scene.layerAppend( "Front", self.rightEngine )
		self.rightEngine.addChildren( self.ptcRightFire )
		self.rightEngine.removeChildren( self.rightEngine.imagePassive )
		self.rightEngine.addChildren( self.rightEngine.imagePassive )
		self.ptcRightFire.setLooped( True )
		
		self.placeForce = Menge.createEntityFromXml( "SolidBody", PhysXml.pepelaz["NullObject"], Menge.vec2f( selfX, selfY - 10 ), Menge.vec2f( 1, 0 ) )
		self.placeForce.activate()
		joint = Menge.createHingeJoint( self.placeForce, self, Menge.vec2f( 0, 0 ), Menge.vec2f( -0.5, 0.5 ), False )
		self.joints.append( joint )
		self.scene.layerAppend( "Front", self.placeForce )
		
		self.ptcSmoke.setLooped( True )
		
		self.updateAim()
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onGlobalHandleKeyEvent
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onGlobalHandleKeyEvent( self, hotSpot, key, char, isDown ):
		if self.isDead() == True: return False
		
		if key == Keys.VK_A:
			if isDown == True:
				self.moveForceVector[0] = -self.moveForce
			else:
				self.moveForceVector[0] = 0
		elif key == Keys.VK_D:
			if isDown == True:
				self.moveForceVector[0] = self.moveForce
			else:
				self.moveForceVector[0] = 0
		elif key == Keys.VK_W:
			if isDown == True:
				self.runScrew( 1 )
				self.moveForceVector[1] = -self.moveForce
			else:
				self.runScrew( 0 )
				self.moveForceVector[1] = 0
			pass
		elif key == Keys.VK_S:
			if isDown == True:
				self.runScrew( 2 )
				self.moveForceVector[1] = self.moveForce
			else:
				self.runScrew( 0 )
				self.moveForceVector[1] = 0
			pass
		
		#print self.moveForceVector
		self.runEngines( self.moveForceVector )
		self.placeForce.applyConstantForce( self.moveForceVector[0], self.moveForceVector[1], 0, 0 )
		
		return False
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onGlobalHandleMouseButtonEvent
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onGlobalHandleMouseButtonEvent( self, hotSpot, button, isDown ):
		if self.isDead() == True: return False
		if isDown == False: return False
		
		if button == 0: 
		#if self.hookObject[self.currentChain] == None:
			aimX, aimY = Framework.getVec2f( self.imageAim.getWorldPosition() )
			self.hookShoot( aimX, aimY, self.shootForce ) 
			self.hookReady[self.currentChain] = True
			pass
		elif button == 1:
		#else:
			for i in range( 4 ):
				self.hookReady[i] = False
				if self.hookJoint[i] != None:
					Menge.destroyJoint( self.hookJoint[i] )
					self.hookJoint[i] = None
					self.hookObject[i] = None
					pass
			pass
		
		return False
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getHookLength
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getHookLength( self ):
		return self.hookLength
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onHookCollide
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onHookCollide( self, hook, passiveObj, x, y, nx, ny ):
		for chain in self.chain:
			if passiveObj in chain.getWholeChain(): return
		
		#self.hook.removeConstantForce()
		index = self.hook.index( hook )
		if self.hookReady[index] == False: return
		if self.hookObject[index] != None: return
		
		#print "!!!"
		self.hookObject[index] = passiveObj
		self.hookReady[index] = False
		self.hookJoint[index] = Menge.createHingeJoint( self.hook[index], passiveObj, Menge.vec2f( 0, 0 ), Menge.vec2f( 0, 0 ), False )
		
		hookX, hookY = Framework.getVec2f( self.hook[index].getLocalPosition() )
		self.scene.playSparcle( x, y )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: runScrew
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def runScrew( self, screwIndex ):
		self.imageScrewDown.stop()
		self.imageScrewUp.stop()
		self.ptcSmoke.stop()
		
		if screwIndex == 1:
			self.imageScrewDown.hide( True )
			self.imageScrewUp.hide( False )
			self.imageScrewUp.play()
			self.ptcSmoke.play()
			pass
		elif screwIndex == 2:
			self.imageScrewUp.hide( True )
			self.imageScrewDown.hide( False )
			self.imageScrewDown.play()
			self.ptcSmoke.play()
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: runEngines
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def runEngines( self, rotation ):
		
		#print rotation
		if rotation == [ 0, 0 ]: 
			self.ptcLeftFire.stop()
			self.ptcRightFire.stop()
			return
		
		#length = Framework.calculateDistance( 0, 0, rotation[0], rotation[1] )
		#cosA = float( rotation[0] ) / length
		#angle = Menge.acosf( cosA )
		#if rotation[1] < 0: angle *= -1
		#angle += 1.57
		
		#selfX, selfY = Framework.getVec2f( self.getLocalPosition() )
		#length = Framework.calculateDistance( 0, 0, rotation[0], rotation[1] )
		#vX = float( rotation[0] ) / length
		#vY = float( rotation[1] ) / length
		#print rotation[0]
		
		e1X, e1Y = Framework.getVec2f( self.leftEngine.getLocalPosition() )
		e2X, e2Y = Framework.getVec2f( self.rightEngine.getLocalPosition() )
		
		if rotation[0] == 0:
			e1X += 1
			e2X += 1
			pass
		elif rotation[0] > 0:
			e1Y += 1
			e1X += 1
			e2Y += 1
			e2X += 1
			pass
		elif rotation[0] < 0:
			e1Y -= 1
			e1X += 1
			e2Y -= 1
			e2X += 1
			pass
		
		#self.leftEngine.setDirection( Menge.vec2f( vX, vY ) )
		#self.rightEngine.setDirection( Menge.vec2f( vX, vY ) )
		self.leftEngine.rotateTo( 200, Menge.vec2f( e1X, e1Y ) )
		self.rightEngine.rotateTo( 200, Menge.vec2f( e2X, e2Y ) )
		#self.leftEngine.setAngle( angle )
		#self.rightEngine.setAngle( angle )
		#self.imageRightEngine.setLocalDirection( Menge.vec2f( -dx/length, dy/length ) )
		
		self.ptcLeftFire.play()
		self.ptcRightFire.play()
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: hookShoot
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def hookShoot( self, x, y, force ):
		self.currentChain = (self.currentChain+1) % 4
		
		if self.hookObject[self.currentChain] != None: return
		
		hookX, hookY = Framework.getVec2f( self.hook[self.currentChain].getLocalPosition() )
		length = Framework.calculateDistance( x, y, hookX, hookY )
		forceVectorX = float( x - hookX ) / length * force
		forceVectorY = float( y - hookY ) / length * force
		
		#print forceVectorX, forceVectorY
		self.hook[self.currentChain].applyForce( forceVectorX, forceVectorY, 0 , 0 )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: onCollide
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def onCollide( self, collideObj, passiveObj, x, y, nx, ny ):
		if self.isDead() == True: return
		
		velocityXY = Framework.getVec2f( self.getLinearVelocity() )
		velocity = Framework.calculateDistance( 0, 0, velocityXY[0], velocityXY[1] )
		if velocity < self.minVelocityToDamage: return
		
		self.health -= float( velocity ) / self.minVelocityToDamage
		#print self.health
		if self.health < 0:
			self.health = 0
			self.kill()
			pass
		#print self.health 
		
		self.scene.updateHUD()
		
		selfX, selfY = Framework.getVec2f( self.getLocalPosition() )
		self.scene.playCrush( x, y )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getHealth
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getHealth( self ):
		return self.health
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: updateAim
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def updateAim( self ):
		
		selfX, selfY = Framework.getVec2f( self.getScreenPosition() )
		mouseX, mouseY = Menge.getMouseX(), Menge.getMouseY()
		
		vecLength = Framework.calculateDistance( selfX, selfY, mouseX, mouseY )
		if vecLength < self.hookLength:
			aimX, aimY = mouseX - selfX, mouseY - selfY 
		else:
			aimX, aimY = float(mouseX - selfX) * self.hookLength / vecLength, float(mouseY - selfY) * self.hookLength / vecLength
		
		self.imageAim.setLocalPosition( Menge.vec2f( aimX, aimY ) )	
			
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: kill
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def kill( self ):
		self.moveForceVector = [ 0, 0 ]
		self.runEngines( self.moveForceVector )
		self.runScrew( 0 )
		self.placeForce.removeConstantForce()
		
		self.chain.destroy()
		for joint in self.joints:
			Menge.destroyJoint( joint )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: isDead
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def isDead( self ):
		if self.health == 0: return True
		else: return False
		pass