import compiler
import os
import sys
import os.path
import shutil
import string

def compile_copy(srcdir,destdir):

    os.mkdir(destdir)
    
    for root, dirs, files in os.walk(srcdir):
        for file in files:
            
            fulldest = os.path.join(destdir, file)
            fullpath = os.path.join(root, file)
                
            if file.endswith('.py'):
                compiler.compileFile(fullpath)
                
                pycext = os.path.splitext(file)[0]+".pyc";
                fullpath = os.path.join(root,pycext)
                fulldest = os.path.join(destdir,pycext)
                
            shutil.copyfile(fullpath,fulldest)
              
def main():
    
    compile_copy(".","..\Dir")
    
main()