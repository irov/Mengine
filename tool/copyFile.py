import os
import os.path
import shutil
import py_compile

def copyfile(src, dst):
    destdir = os.path.dirname(dst)
    
    if not os.path.exists(destdir):
        os.makedirs(destdir)
        
    shutil.copy2(src, dst)

    pass

def callback(src, dst):
    sourcePath = os.path.normpath("..\\"+src)
    destPath = os.path.normpath(dst)
    copyfile(sourcePath, destPath)
    pass
# end