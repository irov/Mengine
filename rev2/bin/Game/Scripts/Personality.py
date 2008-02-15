import Menge

def init():
	#Menge.setCurrentScene("Match3")
	#Menge.setCurrentScene("Buka")
	#Menge.setCurrentScene("FishDemo")	
	Menge.setCurrentScene("FishDemo", False)
	return True
	pass


def update( timing ):
	pass

def fini():
	pass


def onHandleKeyEvent( key, char, isDown ):
	return False
	pass

def onHandleMouseButtonEvent( button, isDown ):
#	print "onHandleMouseButtonEvent", button, isDown
	return False
	pass

def onHandleMouseMove( x, y, z ):
#	print "onHandleMouseMove", x, y, z
	return False
	pass

