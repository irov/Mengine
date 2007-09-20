import Menge

class MyActor( Menge.Entity ):
        
        def __init__( self ):
                Menge.Entity.__init__( self )

                self.actor = Menge.createNode( "Actor2D", 
			[
                                {
                                        "Animation":
                                        { 
                                                "ImageMap" :
                                                {
                                                        "Name" : "output" 
                                                },

                                                "Animation":
                                                {
                                                         "Name": "output_cngAnimation" 
                                                },
                                                "Transformation":
                                                {
                                                         "Value": "1;0;0;1;0;0" 
                                                }
                                        },

                                        'MovementParams':
                                        {
                                                'MaxSpeed':
                                                        {
                                                         'Value': '30.0' 
                                                        },

                                                'Acceleration':
                                                        {
                                                         'Value': '20.0' 
                                                        },
                                                'RotateSpeed':
                                                        {
                                                         'Value': '2.0' 
                                                        }
                                        }
                                }
                        ])

                self.addChildren( self.actor )
                pass

        def onHandleMouseButtonEvent( self, button, isDown ):
                print button, isDown
                pass
        pass
