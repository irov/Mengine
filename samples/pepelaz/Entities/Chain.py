#coding=UTF-8
import Menge
import Framework

# ------------------------------------------------------------------------------
# Class: Chain
# Description:
# - 
# ------------------------------------------------------------------------------
class Chain():
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self, scene = None ):
		self.scene = scene
		#self.resource = resource
		#self.physObject = physObject
		#self.quantity = quantity
		
		self.chain = []
		
		self.dx = 0
		self.dy = 0
		self.quantity = 0
		
		self.joints = []
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: createChain
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def createChain( self, xml, quantity, x, y, dx, dy, reel = False ):
		self.dx = dx
		self.dy = dy
		self.quantity = quantity
		
		for i in range( quantity ):
			if reel == True:
				position = Menge.vec2f( x + dx*(i%2), y + dy*(i%2) )
			else:
				position = Menge.vec2f( x + dx*i, y + dy*i )
			chain = Menge.createEntityFromXml( "SolidBody", xml, position, Menge.vec2f( 1, 0 ) )
			chain.activate()
			self.scene.layerAppend( "Front", chain )
			self.chain.append( chain )
			if i > 0:
				joint = Menge.createHingeJoint( self.chain[i-1], self.chain[i], Menge.vec2f( 0, 0 ), Menge.vec2f( 0, 0 ), False )
				self.joints.append( joint )
				pass
			pass
		
		#Menge.createHingeJoint( self, self.chain[0], Menge.vec2f( 0, 0 ), False )
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getFirstSection
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getFirstSection( self ):
		if self.chain == []: return None
		else: return self.chain[0]
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getFirstSection
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getLastSection( self ):
		if self.chain == []: return None
		else: return self.chain[-1]
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getWholeChain
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getWholeChain( self ):
		if self.chain == []: return None
		else: return self.chain
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
	# Metod: getPixelLength
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getPixelLength( self ):
		return Framework.calculateDistance( 0, 0, self.dx, self.dy ) * self.quantity
		
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: destroy
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def destroy( self ):
		for joint in self.joints:
			Menge.destroyJoint( joint )
		pass
		