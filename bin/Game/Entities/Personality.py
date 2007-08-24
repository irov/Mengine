import Menge

def init():
	#Menge.setCurrentScene("Match3")
	Menge.setCurrentScene("Buka")

	return True
	pass

def update( timing ):
	pass

def fini():
	pass


def onHandleKeyEvent( key, isDown ):
	print "onHandleKeyEvent", key, isDown
	pass

def onHandleMouseButtonEvent( button, isDown ):
	print "onHandleMouseButtonEvent", button, isDown
	pass

def onHandleMouseMove( x, y, z ):
	print "onHandleMouseMove", x, y, z
	pass

