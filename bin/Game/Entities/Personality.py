import Menge

def init():
	Menge.player().setChapter("Buba")
	Menge.player().gotoScene("Buka")

	b1 = Menge.createEntity( "Block", "Boba1" )
	b2 = Menge.createEntity( "Block", "Boba2" )

	b1.addChildren( b2 )

	Menge.addEntity( b1 )

	name = b1.getName()

	print name

	return True
	pass

def update( timing ):
	pass

def fini():
	pass