import Menge

class Scene( Menge.Scene ):
	def __init__( self ):
		self.camera_x = 512
		self.camera_y = 384
		
		self.blow = None
		self.blow = Menge.createEntity("Blow", Menge.vec2f( 200, 200 ), Menge.vec2f( 1,0 ) )

		self.blowForce = 0
		self.blowTick = 0
		self.blowing = False


		Menge.setEnoughBlow( 60 )
		Menge.setBlow( True )
		Menge.setBlowCallback( self.onBlow )
		pass
		
	def onLoader( self ):
		self.layerAppend( "Front", self.blow )
		pass

	def onBlow( self, blow ):
		print blow
		if self.blowing == True:
			self.blowForce += ( blow + 1 )
			self.blowTick += 1
		else:
			self.blowForce = blow
			self.blowTick = 1
			self.blowing = True
			Menge.schedule( 50, self.onBlowForce )

	def onBlowForce( self ):
		blowForceMiddle = self.blowForce * self.blowForce / ( self.blowTick  * 4 )
		blowForceNow = -5000 * blowForceMiddle
		self.blow.applyImpulse( 0, blowForceNow, 0, 0 );
		self.blowing = False
		print self.blowForce, self.blowTick
		pass

	def onActivate( self ):
		self.blow.activate()
		pass

	def onUpdate( self, timing ):
		pass

	def onHandleMouseButtonEvent( self, button, isDown ):
		if button == 1 and isDown == True:
			self.blow.applyImpulse( 0, -20000, 0, 0 );
			print Menge.getBlow()
			
		return False
		pass

	def onHandleKeyEvent( self, key, char, isDown  ):
		print key
		return False
		pass
		
	pass
	pass
