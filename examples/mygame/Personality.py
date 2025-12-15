from Main import Main

def onPreparation(isDebug):
    Mengine.setCursorMode(True)

    return True
    pass

def onRun():
    Scene = Mengine.createScene("Main", Main)

    Mengine.setCurrentScene(Scene, False, True, None)
    pass

def onFinalize():
    pass