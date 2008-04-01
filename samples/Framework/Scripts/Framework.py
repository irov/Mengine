import Menge

# ----------------------------------------------------------------------------
# Metod: setupHotspot
# Description:
# - 
# ----------------------------------------------------------------------------
def setupHotspot( hotspot, points ):
	for point in points:
		hotspot.addPoint( point )
		pass
	pass

# ----------------------------------------------------------------------------
# Metod: tanf
# Description:
# - 
# ----------------------------------------------------------------------------
def tanf( value ):
	return Menge.sinf( value ) / Menge.cosf( value )
	pass

# ----------------------------------------------------------------------------
# Metod: getVec2f
# Description:
# - 
# ----------------------------------------------------------------------------
def getVec2f( value ):
	x = Menge.getVec2fX( value )
	y = Menge.getVec2fY( value )
	return x, y
	pass

# ----------------------------------------------------------------------------
# Metod: calculateDistance
# Description:
# - 
# ----------------------------------------------------------------------------
def calculateDistance( x1, y1, x2, y2 ):
	return Menge.sqrtf( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) )
	pass

	
