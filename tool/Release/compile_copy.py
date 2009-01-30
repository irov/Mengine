import compiler
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

#skipped extensions
bad_ext = [u'.py']  
#skipped files
bad_files = [u'thumbs.db',u'thumbs.db']
#skipped folders
bad_dirs = [u'thumbnails',u'.svn']
#files with bad extension, but needed. Example - log file
good_files = []
#file to copy
copy_files = []

atlas_width = 2048
atlas_height = 2048

# jpg quality, in percent
jpg_quality = 95
alpha_min = 0 # alpha <= alpha_min

allowed_type = ['ResourceImageDefault'] #,'ResourceImageSet','ResourceImageCell']

lang_pack = ""

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

def copytonewfolder(src, DestGameDir):

    SourceGameDir = getGameDirectory(src)
    
    FullPath = getGamePathFromAppXML(src)
    GamePath = os.path.dirname(FullPath)
        
    FullGamePath = os.path.join(SourceGameDir,FullPath)
    FullGameDir = os.path.join(SourceGameDir,GamePath)
    
    IconPath = getIconPathFromGameINI(FullGamePath,"Icon")
    if(IconPath != []): 
        print "IconPath: ",IconPath
        IconName = os.path.basename(IconPath)
        bad_files.append(IconName.lower())
        
    IconSmallPath = getIconPathFromGameINI(FullGamePath,"IconSmall")
    if(IconSmallPath != []): 
        print "IconSmallPath: ",IconSmallPath
        IconName = os.path.basename(IconSmallPath)
        bad_files.append(IconName.lower())
        
    Icon48Path = getIconPathFromGameINI(FullGamePath,"Icon48")
    if(Icon48Path != []): 
        print "Icon48Path: ",Icon48Path
        IconName = os.path.basename(Icon48Path)
        bad_files.append(IconName.lower())
        
    Icon64Path = getIconPathFromGameINI(FullGamePath,"Icon64")
    if(Icon64Path != []): 
        print "Icon64Path: ",Icon64Path
        IconName = os.path.basename(Icon64Path)
        bad_files.append(IconName.lower())
    
    SrcGameExe = os.path.normpath(os.path.join(SourceGameDir,"Bin/Game.exe"))
    DstGameExe = os.path.normpath(os.path.join(DestGameDir,"Bin/Game.exe"))
        
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
   
    exe = 'tool.exe %(srcgame)s %(destgame)s' % \
                {'srcgame' : appfile_path, \
                'destgame' : folder_path, \
                }
            
    subprocess.call(exe)
        
    copytonewfolder(appfile_path,folder_path)
    
main()
