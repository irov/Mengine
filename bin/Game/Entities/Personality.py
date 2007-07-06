import Menge
import gc

def init():
	Menge.setCurrentScene("Buka")

	return True
	pass

def update( timing ):
	pass

def fini():
	gc.collect();
	pass