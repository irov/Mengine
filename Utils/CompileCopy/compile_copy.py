import compiler
import os
import sys
import os.path
import shutil
import string
import compileall
import re

source = '.'
bad_ext = ['.py']
bad_files = ['thumbs.db']
bad_dirs = ['thumbnails', '.svn']
good_files = []

def copytree(src, dst):
    
    compileall.compile_dir(src, rx=re.compile('/[.]svn'), force=True)
    
    names = os.listdir(src)

    os.mkdir(dst)

    errors = []
    for name in names:
        if (name in bad_dirs) or (name in bad_files):
            continue
        ext = os.path.splitext(name)[1]
        
        if (ext in bad_ext) and not (name in good_files):
            continue
        
        srcname = os.path.join(src, name)
        dstname = os.path.join(dst, name)
        
        try:
            if os.path.isdir(srcname):
                copytree(srcname, dstname)
            else:
                shutil.copy2(srcname, dstname)
                
        except (IOError, os.error), why:
            errors.append((srcname, dstname, why))
    if errors:
        raise Error, errors

def main():

    copytree("Game","New1")

main()