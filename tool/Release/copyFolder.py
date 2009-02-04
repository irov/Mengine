import os
import os.path
import shutil
import py_compile

#skipped extensions
bad_ext = [u'.py', u'.pyc']  
#skipped files
bad_files = [u'thumbs.db',u'thumbs.db']
#skipped folders
bad_dirs = [u'thumbnails',u'.svn']

def copytree(src, dst):
    #compileall.compile_dir(src, rx=re.compile('/[.]svn'), force=True)
    print src
    print dst
    
    if not os.path.exists(dst):
        os.makedirs(dst)
        
    names = os.listdir(src)

    for name in names:
        
        if (name.lower() in bad_dirs) or (name.lower() in bad_files):
            continue
        
        ext = os.path.splitext(name)[1]
        
        if(ext==".py"):
            srcname = os.path.join(src, name)
            dstname = os.path.join(dst, name) + "o"
            py_compile.compile(srcname, dstname)
            os.rename(dstname, dstname)
        
        if (ext in bad_ext):
            continue
        
        srcname = os.path.join(src, name)
        
        if (srcname.lower() in bad_files):
            continue
                
        dstname = os.path.join(dst, name)
           
        if os.path.isdir(srcname):
            copytree(srcname, dstname)
        else:
            shutil.copy2(srcname, dstname)
    
    pass

def callback(src, dst):
    sourcePath = os.path.normpath("..\\"+src)
    destPath = os.path.normpath(dst)
    copytree(sourcePath, destPath)
    pass
# end