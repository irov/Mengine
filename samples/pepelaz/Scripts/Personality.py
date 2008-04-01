import Menge
#from Functor import Functor

# ----------------------------------------------------------------------------
# Metod: init
# Description:
# - 
# ----------------------------------------------------------------------------
def init():
	Menge.setCurrentScene( "TestScene", False )
	
	
	return True
	pass


# ----------------------------------------------------------------------------
# Metod: update
# Description:
# - 
# ----------------------------------------------------------------------------
def update( timing ):
	pass

# ----------------------------------------------------------------------------
# Metod: fini
# Description:
# - 
# ----------------------------------------------------------------------------
def fini():
	pass


# ----------------------------------------------------------------------------
# Metod: onHandleKeyEvent
# Description:
# - 
# ----------------------------------------------------------------------------
def onHandleKeyEvent( key, char, isDown ):
	if key == 1:
		#return True
		pass
	
	#print key
	#print "onHandleKeyEvent", key, isDown
	#Menge.setCurrentScene("Menu")
	return False
	pass

# ----------------------------------------------------------------------------
# Metod: onHandleMouseButtonEvent
# Description:
# - 
# ----------------------------------------------------------------------------
def onHandleMouseButtonEvent( button, isDown ):
	#print "onHandleMouseButtonEvent", button, isDown
	#Menge.setCurrentScene("Menu")
	return False
	pass

# ----------------------------------------------------------------------------
# Metod: onHandleMouseMove
# Description:
# - 
# ----------------------------------------------------------------------------
def onHandleMouseMove( x, y, z ):
	#print "onHandleMouseMove", x, y, z
	return False
	pass



