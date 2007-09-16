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
        en = Menge.createEntity( "Block", vec2f(200,200), vec2f(1,0) )
        en.activate();
        self.layerAppend( "Main", en )
        self.pole.loadLevel( self, "Game/Levels/01.txt" )

        en1 = Menge.createEntity( "MyActor", vec2f(200,200), vec2f(1,0) )

        self.layerAppend( "Main", en1 )
        pass

    def onHandleMouseButtonEvent( self, button, isDown ):
        pass

    def onHandleKeyEvent( self, key, isDown ):
        if key == 2:
            pass
        pass
    def onUpdate( self, f ):
        pass
    
    pass
