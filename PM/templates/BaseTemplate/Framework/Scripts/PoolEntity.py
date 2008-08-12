# ------------------------------------------------------------------------------
# Module: 
# Creating: 10.11.2007 12:53
# Description:
# - 
# ------------------------------------------------------------------------------
import Menge

# ----------------------------------------------------------------------------
# Metod: PoolEntity
# Description:
# - 
# ----------------------------------------------------------------------------
class PoolEntity:
	
	# ----------------------------------------------------------------------------
	# Metod: __init__
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def __init__( self, typeName, listener ):
		self.instance = []
		self.free = []
		self.typeName = typeName
		self.listener = listener
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: new
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def new( self, position, direction, xml = None ):
		en = None
		
		if len( self.free ) == 0:
			if xml == None:
				en = Menge.createEntity( self.typeName, position, direction )
			else:
				en = Menge.createEntityFromXml( self.typeName, xml, position, direction )
			if self.listener:
				self.listener.onNewPoolEntity( self, en )
		else:
			en = self.free.pop(0)
			
			en.setLocalPosition( position )
			en.setLocalDirection( direction )

		self.instance.append(en)
		return en
		pass
		
	# ----------------------------------------------------------------------------
	# Metod: delete
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def delete( self, en ):
		self.free.append( en )
		self.instance.remove( en )
		pass		
	
	# ----------------------------------------------------------------------------
	# Metod: getType
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getType( self ):
		return self.typeName
		pass

	# ----------------------------------------------------------------------------
	# Metod: clear
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def clear( self ):
		for entity in self.instance:
			self.free.append( entity )

		self.instance = []
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: foreach
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def foreach( self, functor ):
		for entity in self.instance:
			functor( entity )
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getCount
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getCount( self, withFree = False ):
		if withFree == True:
			return len(self.instance) + len(self.free)
		else:
			return len(self.instance)
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getInstance
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getInstance( self ):
		return self.instance
		pass
	
	# ----------------------------------------------------------------------------
	# Metod: getFree
	# Description:
	# - 
	# ----------------------------------------------------------------------------
	def getFree( self ):
		return self.free
		pass
		
	pass