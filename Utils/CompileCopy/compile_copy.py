import compiler
import os
import sys
import os.path
import shutil
import string
import compileall
import re
import xml.dom.minidom 
import pprint
import subprocess
import struct
from Tkinter import *
import tkFileDialog, tkMessageBox
from xml.dom import *

#skipped extensions
bad_ext = ['.py']  
#skipped files
bad_files = ['thumbs.db','thumbs.db']
#skipped folders
bad_dirs = ['thumbnails','.svn']
#files with bad extension, but needed. Example - log file
good_files = []
#file to copy
copy_files = []

atlas_width = 1024
atlas_height = 1024

# jpg quality, in percent
jpg_quality = 95

allowed_type = ['ResourceImageDefault'] #,'ResourceImageSet','ResourceImageCell']

def getGameDirectory(src):
    return os.path.dirname(os.path.dirname(src))

def getIconPathFromGameXML(src, directory):
    dom = xml.dom.minidom.parse(src)
    app_tag = dom.getElementsByTagName("Application")[0]
    
    iconresource = app_tag.getElementsByTagName("Game")
    
    if(iconresource==[]):
        return []
    
    Path = iconresource[0].getAttribute("File")
    
    dom.unlink()
    
    GamePath = os.path.dirname(Path)
    
    FullPath = os.path.join(directory,Path)
    
    dom = xml.dom.minidom.parse(FullPath)
    app_tag = dom.getElementsByTagName("Game")[0]
    
    iconresource = app_tag.getElementsByTagName("Icon")
    
    if(iconresource==[]):
        return []
    
    Path = iconresource[0].getAttribute("Value")
    
    dom.unlink()
    
    return [Path,GamePath]

def getFileResourcesFromAppXML(src):
    dom = xml.dom.minidom.parse(src)

    app_tag = dom.getElementsByTagName("Application")[0]
    resources = app_tag.getElementsByTagName("Resource")

    resource_list = []
    
    for resource in resources:
        resource_list.append(resource.getAttribute("File"))
        
    #TODO kill unique resources
    
    return resource_list

def getResourcePaths(src):
    dom = xml.dom.minidom.parse(src)
    resources_tag = dom.getElementsByTagName("Resources")[0]
    resource = resources_tag.getElementsByTagName("Resource")
    
    if(resource == []):
        return []
                    
    path_resources = resource[0].getAttribute("Path");
    
    path_resources_src = os.path.join(os.path.dirname(src),path_resources)
    
    resource_list = []
    
    for node in resource[0].childNodes:
        if node.nodeType == node.ELEMENT_NODE:
            resource_list.append(os.path.join(path_resources_src,node.localName)+".resource")
    
    dom.unlink()
    
    return (resource_list,path_resources)
        
def copyfiles():
    for file in copy_files:
        src = os.path.basename(file)
        shutil.copy2(src,file)
        os.remove(src)
    
    del copy_files[:]
        
def copytree(src, dst):
    compileall.compile_dir(src, rx=re.compile('/[.]svn'), force=True)
    
    os.makedirs(dst)
    
    names = os.listdir(src)

    for name in names:
        
        if (name in bad_dirs) or (name.lower() in bad_files):
            continue
        
        ext = os.path.splitext(name)[1]
        
        if (ext in bad_ext) and not (name in good_files):
            continue
        
        srcname = os.path.join(src, name)
        srcname = os.path.normpath(srcname)
        srcname = srcname.lower()
        
        if (srcname in bad_files):
            continue
                
        dstname = os.path.join(dst, name)
        
        if os.path.isdir(srcname):
            copytree(srcname, dstname)
        else:
            shutil.copy2(srcname, dstname)
    
    pass

def make_input_for_atlas_make(resource_list):
    doc = xml.dom.minidom.Document()
    
    root = doc.createElement("resources")
    doc.appendChild(root)
    
    for resource in resource_list:
        location_element = doc.createElement("resource")
        location_element.setAttribute('value',os.path.normpath(resource))
        
        root.appendChild(location_element)
    
    thefile=open("input.xml","w")
    doc.writexml(thefile)
    thefile.close()
    doc.unlink()
    
    pass


def read_output_file(src, destdir, resourcepath):
    
    resources_list = []
    
    dom = xml.dom.minidom.parse(src)

    app_tag = dom.getElementsByTagName("datapack")[0]
    atlases = dom.getElementsByTagName("atlas")
        
    for atlas in atlases:
        val = atlas.getAttribute("value")
        copy_files.append(os.path.join(destdir,val))
        
    resources = dom.getElementsByTagName("resource")
    
    for resource in resources:
        val = resource.getAttribute("value")
        copy_files.append(os.path.join(destdir,os.path.join(resourcepath,val)))
        
    dom.unlink()        
    pass
    
def atlas(src,destdir):
    resourceList = getResourcePaths(src)
    
    if resourceList == []:
        return
  
    make_input_for_atlas_make(resourceList[0])
    
    gamedir = os.path.dirname(src)
    
    exe = 'AtlasMaker.exe input.xml %(width)i %(height)i %(gamedir)s %(jpgquality)i' % \
            {'width' : atlas_width, \
             'height' : atlas_height, \
             'gamedir' : gamedir, \
             'jpgquality' : jpg_quality \
            }
        
    subprocess.call(exe)
    
    read_output_file("output.xml", destdir, resourceList[1])
                              
    for resource in resourceList[0]:
        bad_files.append(resource)
        
        dom = xml.dom.minidom.parse(resource)
        resources = dom.getElementsByTagName("DataBlock")[0].getElementsByTagName("Resource")
        
        for resource in resources:
            type = resource.getAttribute("Type")
            
            if(type in allowed_type):
                files = resource.getElementsByTagName("File")
                if(files != []):
                    for file in files:
                        value = file.getAttribute("Path")
                        value = os.path.join(gamedir,value).lower()
                        bad_files.append(os.path.normpath(value))

        
        dom.unlink()

def copytonewfolder(src, DestGameDir):
    
    if os.path.exists(DestGameDir):
        print "Error: Output directory already exist!"
        return
    
    FileResources = getFileResourcesFromAppXML(src)
    
    if FileResources == []:
        print "Error: resources are empty in %s!" % ('src')
        return
    
    SourceGameDir = getGameDirectory(src)
    
    Paths = getIconPathFromGameXML(src, SourceGameDir)
    
    if(Paths != []):
        IconPath = Paths[0]
        GamePath = Paths[1]
    
        IconName = os.path.basename(IconPath)
        bad_files.append(IconName.lower())

    os.mkdir(DestGameDir)
    
    for file in FileResources:
        SourceDir = os.path.join(SourceGameDir, os.path.dirname(file))
        DestDir = os.path.join(DestGameDir, os.path.dirname(file))
        
        srcfile = os.path.join(SourceGameDir, file)
        atlas(srcfile, DestDir)
        
        copytree(SourceDir,DestDir)
        copyfiles()
        
    copytree(os.path.join(SourceGameDir,"Bin"),os.path.join(DestGameDir,"Bin"))

    if(Paths != []):
        SrcGameExe = os.path.normpath(os.path.join(SourceGameDir,"Bin/Game.exe"))
        DstGameExe = os.path.normpath(os.path.join(DestGameDir,"Bin/Game.exe"))
        IconPath = os.path.normpath(os.path.join(os.path.join(SourceGameDir,GamePath),IconPath))
    
        os.chdir("../upx")
        
        exe = 'upx.exe -d %(dstgame)s' % \
        {
            'dstgame' : DstGameExe, \
        }
            
        subprocess.call(exe)
        
        os.chdir("../reshacker")
        
        exe = 'reshacker.exe -addoverwrite %(destgame)s,%(destgame)s,%(iconpath)s,icongroup,100,' % \
                {'srcgame' : SrcGameExe, \
                 'destgame' : DstGameExe, \
                 'iconpath' : IconPath, \
                }
        
        subprocess.call(exe)
        
        os.chdir("../upx")
    
        exe = 'upx.exe -9 %(dstgame)s' % \
        {
            'dstgame' : DstGameExe, \
        }
            
        subprocess.call(exe)
    
    l = []
        
    def make_list(arg, dirn, arg1):
        l.append(dirn)
     
    os.path.walk(DestGameDir, make_list, None)
    l.reverse()

    for dirn in l:
        try:
            os.rmdir(dirn)
        except:
            pass
    
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
   
    copytonewfolder(appfile_path,folder_path)
    
main()
