import os
import sys
import os.path
import shutil
import string
import compileall
import py_compile
import re
import xml.dom.minidom 
import pprint
import subprocess
import struct
import ConfigParser
import codecs
from Tkinter import *
import tkFileDialog, tkMessageBox
from xml.dom import *

def getGameDirectory(src):
    return os.path.dirname(os.path.dirname(src))

def getIconPathFromGameINI(src,IconType):
   
    thefile=codecs.open(src,'r','utf-8')
    lines = thefile.readlines()
    thefile.close()
    
    for line in lines:
        value = line.split()
        if value != []:
            if value[0] == IconType:
                return value[2]
                
    return []

def getGamePathFromAppXML(src):
    dom = xml.dom.minidom.parse(src)
    app_tag = dom.getElementsByTagName("Application")[0]
    
    iconresource = app_tag.getElementsByTagName("Game")
    
    if(iconresource==[]):
        return []
    
    Path = iconresource[0].getAttribute("File")
    
    dom.unlink()
    
    return Path

def copytree(src, dst):
    #compileall.compile_dir(src, rx=re.compile('/[.]svn'), force=True)
    
    names = os.listdir(src)

    for name in names:
        ext = os.path.splitext(name)[1]
        
        if(ext==".py"):
            srcname = os.path.join(src, name)
            #srcname = os.path.normpath(srcname)
            dstname = os.path.join(dst, name) + "o"
            print dstname
            py_compile.compile(srcname, dstname)
            os.rename(dstname, dstname)
             
        srcname = os.path.join(src, name)
        srcname = os.path.normpath(srcname)
                           
        if os.path.isdir(srcname):
            dstname = os.path.join(dst, name)
            copytree(srcname, dstname)
   
    pass

def copytonewfolder(src, DestGameDir):

    SourceGameDir = getGameDirectory(src)
    
    FullPath = getGamePathFromAppXML(src)
    GamePath = os.path.dirname(FullPath)
        
    FullGamePath = os.path.join(SourceGameDir,FullPath)
    FullGameDir = os.path.join(SourceGameDir,GamePath)
    
    IconPath = getIconPathFromGameINI(FullGamePath,"Icon")
    IconSmallPath = getIconPathFromGameINI(FullGamePath,"IconSmall")
    Icon48Path = getIconPathFromGameINI(FullGamePath,"Icon48")
    Icon64Path = getIconPathFromGameINI(FullGamePath,"Icon64")

    SrcGameExe = os.path.normpath(os.path.join(SourceGameDir,"Bin/Game.exe"))
    DstGameExe = os.path.normpath(os.path.join(DestGameDir,"Bin/Game.exe"))
        
    copytree(SourceGameDir,DestGameDir)
    
    exe = 'upx.exe -d %(dstgame)s' % { 'dstgame' : DstGameExe }
            
    subprocess.call(exe)
        
    if(IconPath != []):
        IconPath = os.path.normpath(os.path.join(FullGameDir,IconPath))
            
        exe = 'reshacker.exe -addoverwrite %(destgame)s,%(destgame)s,%(iconpath)s,icongroup,100,' % \
                {'srcgame' : SrcGameExe, \
                 'destgame' : DstGameExe, \
                 'iconpath' : IconPath, \
                }
        
        subprocess.call(exe)

        
    if(IconSmallPath!=[]):
        IconSmallPath = os.path.normpath(os.path.join(FullGameDir,IconSmallPath))
                        
        exe = 'reshacker.exe -addoverwrite %(destgame)s,%(destgame)s,%(iconpath)s,icongroup,101,' % \
                {'srcgame' : SrcGameExe, \
                'destgame' : DstGameExe, \
                'iconpath' : IconSmallPath, \
                }
            
        subprocess.call(exe)

    if(Icon48Path!=[]):
        Icon48Path = os.path.normpath(os.path.join(FullGameDir,Icon48Path))
                        
        exe = 'reshacker.exe -addoverwrite %(destgame)s,%(destgame)s,%(iconpath)s,icongroup,102,' % \
                {'srcgame' : SrcGameExe, \
                'destgame' : DstGameExe, \
                'iconpath' : Icon48Path, \
                }
            
        subprocess.call(exe)
    
    if(Icon64Path!=[]):
        Icon64Path = os.path.normpath(os.path.join(FullGameDir,Icon64Path))
                        
        exe = 'reshacker.exe -addoverwrite %(destgame)s,%(destgame)s,%(iconpath)s,icongroup,103,' % \
                {'srcgame' : SrcGameExe, \
                'destgame' : DstGameExe, \
                'iconpath' : Icon64Path, \
                }
            
        subprocess.call(exe)
            
    exe = 'upx.exe -9 %(dstgame)s' % {'dstgame' : DstGameExe}
            
    subprocess.call(exe)
    
    print "Done!"
    
def main():
    
    master = Tk()
    master.withdraw()
        
    appfile_path = tkFileDialog.askopenfilename(title="Open file",\
                                             filetypes=[("application file",".xml"),\
                                                        ("All files",".*")])
 
    if appfile_path != "":
        folder_path = tkFileDialog.asksaveasfilename(initialfile='OutputDir')
    else:
        print "you didn't open anything!"
        return
   
    folder_path = folder_path + "\\"
    
    exe = 'tool.exe %(srcgame)s %(destgame)s' % \
                {'srcgame' : appfile_path, \
                'destgame' : folder_path, \
                }
            
    subprocess.call(exe)
        
    copytonewfolder(appfile_path,folder_path)
    
main()