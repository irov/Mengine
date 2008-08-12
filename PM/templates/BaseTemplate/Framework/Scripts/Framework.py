#coding=UTF-8
import Menge
from Functor import Functor

# ----------------------------------------------------------------------------
# CONSTANTS
# ----------------------------------------------------------------------------
M_PI  = 3.1415926535897932384626433832795
M_PI2 = 1.5707963267948966192313216916398
M_PI4 = 0.78539816339744830961566084581988
M_3PI4 = 2.3561944901923449288469825374596
M_PI8 = 0.39269908169872415480783042290994

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

	
# ----------------------------------------------------------------------------
# Metod: testHotSpot
# Description:
# - 
# ----------------------------------------------------------------------------
def testHotSpots( point, hotspots ):
	for hotspot in hotspots:
		if point.testHotSpot( hotspot ):
			return True
	return False
	pass

# ----------------------------------------------------------------------------
# Metod: setSoundResource
# Description:
# - 
# ----------------------------------------------------------------------------
def playSound( node, resource = None, rangeVal = None, stop = True ):
	if stop == False and node.isPlaying() == True: return
	
	if resource != None:
		number = ""
		if rangeVal != None:
			number += str(Menge.randint( rangeVal[0], rangeVal[1]+1 ) )
		node.setSoundResource( resource + number )
		pass
		
	if stop == True:	
		node.stop()
	node.play()
	pass


# ----------------------------------------------------------------------------
# Metod: rotateVec2f
# Description:
# - 
# ----------------------------------------------------------------------------
def rotateVec2f( vec, angle ):
	cosa = Menge.cosf( angle )
	sina = Menge.sinf( angle )
	x, y = getVec2f( vec )
	dx = x * cosa - y * sina
	dy = x * sina + y * cosa
	return Menge.vec2f( dx, dy )
	pass

# ----------------------------------------------------------------------------
# Method: getVectorsAngle
# Description:
# - param: 0 - вернуть угол, 1 - вернуть косинус, 2 - вурнуть синус, 3 - модуль угла
# ----------------------------------------------------------------------------
def getVectorsAngle( vec1, vec2, param = 0 ):
	x1, y1 = getVec2f( vec1 )
	x2, y2 = getVec2f( vec2 )
	
	length1 = Menge.sqrtf( x1*x1 + y1*y1 )
	length2 = Menge.sqrtf( x2*x2 + y2*y2 )
	
	dotProduct = x1 * x2 + y1 * y2
	vecProduct = x1*y2 - y1*x2
	
	#print x1, y1, x2, y2
	
	cos = float(dotProduct) / ( length1 * length2 )
	sin = float(vecProduct) / ( length1 * length2 )
	
	if param in [ 0, 3 ]:
		angle = Menge.acosf( cos )
		if sin < 0 and param == 0: angle *= -1
		return angle
	elif param == 1:
		return cos
	elif param == 2:
		return sin
	
	return None
	pass

# ----------------------------------------------------------------------------
# Method: loadScene
# Description:
# - 
# ----------------------------------------------------------------------------
def loadScene( globalModule, oldScene, newSceneName ):
	globalModule.currentSceneName = newSceneName
	globalModule.oldSceneName = oldScene.getName()
	Menge.setCurrentScene( "Loading", True )
	pass

# ----------------------------------------------------------------------------
# Method: convertTime
# Description:
# - flag: 0 - дописывать ноль перед одним порядком
# ----------------------------------------------------------------------------
def convertTime( time, flag = 0 ):
	parsTime = ""
	minutes = int(time / 60)
	seconds = time % 60
	if flag == 0 and minutes < 10:
		parsTime += "0"
	parsTime += str( minutes ) + ":"
	if flag == 0 and seconds < 10:
		parsTime += "0"
	parsTime += str( seconds )
	
	return parsTime
	pass

# ----------------------------------------------------------------------------
# Method: setRightAlign
# Description:
# - 
# ----------------------------------------------------------------------------
def setRightAlign( text ):
	length = Menge.getVec2fX( text.getLength() )
	posX, posY = getVec2f( text.getLocalPosition() )
	text.setLocalPosition( Menge.vec2f( posX-length, posY ) )
	pass

# ----------------------------------------------------------------------------
# Method: buttonBlink
# Description:
# - 
# ----------------------------------------------------------------------------
scheduleBlink = {}
def buttonBlink( button, time, blink, nowActive = True ):
	global scheduleBlink
	
	if blink == False:
		if button in scheduleBlink.keys():
			Menge.scheduleRemove( scheduleBlink[button] )
			#scheduleBlink = None
			del scheduleBlink[button]
			pass
		button.selectSprite( "passive" )
		return
		pass
	
	if nowActive == True:
		button.selectSprite( "active" )
	else:
		button.selectSprite( "passive" )
	
	scheduleBlink[button] = Menge.schedule( time, Functor( buttonBlink, button, time, True, not nowActive ) )
	pass

