import Menge

import Block

from Pole import Pole

from MyActor import MyActor

from Menge import vec2f

class Scene( Menge.Scene ):
    def __init__( self ):
        self.pole = Pole()
        pass

    def onActivate( self ):
        Menge.setCamera2DPosition( 0, 0 );
        Menge.setCamera3DPosition( 300, 300, 0 );

        en = Menge.createEntity( "MyActor", vec2f(200,200), vec2f(1,0) )
        en.activate();
        self.layerAppend( "Main", en )
        pass

    def onHandleMouseButtonEvent( self, button, isDown ):
        pass

    def onHandleKeyEvent( self, key, isDown ):
        if key == 67:
                    print "preved"
        pass
    
    def onUpdate( self, timing ):

        pass
   
    pass
