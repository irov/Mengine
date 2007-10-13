import Menge

def init():
	#Menge.setCurrentScene("Match3")
	#Menge.setCurrentScene("Buka")
	#Menge.setCurrentScene("FishDemo")	
	#Menge.setCurrentScene("FishDemo")
	Menge.schedule( 1000, setScene )
	return True
	pass

def setScene():
	Menge.setCurrentScene("FishDemo")
	Menge.schedule( 1000, setTest )
	pass

def setTest():
	print "bla bla"
	pass

def update( timing ):
	pass

def fini():
	pass


def onHandleKeyEvent( key, isDown ):
#	print "onHandleKeyEvent", key, isDown
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

