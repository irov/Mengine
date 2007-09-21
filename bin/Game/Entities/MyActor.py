import Menge

class MyActor( Menge.Entity ):
        
        def __init__( self ):
                Menge.Entity.__init__( self )
         
                self.actor = Menge.createNodeFromXml("""
                        <Node Name = "TestActor" Type = "Actor2D" >
			<Animation>
				<ImageMap Name = "BallRed" />
				<Animation Name = "BallRedCNG" />
				<Transformation Value = "1;0;0;1;0;0" /> 
			</Animation>
			<MovementParams>
				<MaxSpeed Value = "30.0" />
				<Acceleration Value = "20.0" />
				<RotateSpeed Value = "2.0" /> 
			</MovementParams>
                        </Node>
                        """
                        )

                self.addChildren( self.actor )

                pass

        def onHandleMouseButtonEvent( self, button, isDown ):
                print button, isDown
                pass
        pass
