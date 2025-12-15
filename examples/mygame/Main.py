class Main(object):
    def __init__(self):
        self.node = None
        pass
        
    def onCreate(self, node):
        self.node = node
        pass

    def onDestroy(self):
        self.node = None
        pass

    def onPreparation(self):
        def __pipe(id, index):
            if index == 5000:
                return -1;
        
            return 1.0
            pass
            
        def __update(id, index, delay):
            surface = Mengine.createSurface("SurfaceSolidColor")
            
            r = Mengine.range_randf(0, 1)
            g = Mengine.range_randf(0, 1)
            b = Mengine.range_randf(0, 1)
            a = Mengine.range_randf(0, 1)
            
            surface.setSolidColor((r,g,b, a))
            
            w = Mengine.range_randf(20, 200)
            h = Mengine.range_randf(20, 200)
            surface.setSolidSize((w,h))
        
            shape = Mengine.createNode("ShapeQuadFixed")
            shape.setSurface(surface)
            
            Resolution = Mengine.getContentResolution()
            ResolutionWidth = Resolution.getWidth()
            ResolutionHeight = Resolution.getHeight()
            x = Mengine.range_rand(0, ResolutionWidth)
            y = Mengine.range_rand(0, ResolutionHeight)
            
            shape.setLocalPosition((x, y))
            
            self.node.addChild(shape)
            pass
            
        def __event(id, isComplete):
            pass

        id = Mengine.pipe(__pipe, __update, __event)
        pass

    def onActivate(self):
        pass

    def onDeactivate(self):
        pass
    pass