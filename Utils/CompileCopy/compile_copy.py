import compiler
import os
import sys
import os.path
import shutil
import string
import compileall
import re
import xml.dom.minidom
import subprocess
import struct

from Tkinter import *
import tkFileDialog, tkMessageBox

#skipped extensions
bad_ext = ['.py']  
#skipped files
bad_files = ['thumbs.db','Thumbs.db']
#skipped folders
bad_dirs = ['thumbnails','.svn']
#files with bad extension, but needed. Example - log file
good_files = []

copy_files = []

atlas_width = 1024
atlas_height = 1024

# use halftexel
halftexel_use = False

# jpg quality, in percent
jpg_quality = 95

allowed_type = ['ResourceImageDefault'] #,'ResourceImageSet','ResourceImageCell']

def formreslist(src):
    dom = xml.dom.minidom.parse(src)

    app_tag = dom.getElementsByTagName("Application")[0]
    resources = app_tag.getElementsByTagName("Resource")

    resource_list = []
    
    for resource in resources:
        resource_list.append(resource.getAttribute("File"))
        
    #TODO kill unique resources
    
    return resource_list
        
def copyfiles():
    for copiedfile in copy_files:
        src = os.path.basename(copiedfile)
        shutil.copy2(src,copiedfile)
        os.remove(src)	
    
    del copy_files[:]
        
def copytree(src, dst):
    compileall.compile_dir(src, rx=re.compile('/[.]svn'), force=True)
    
    names = os.listdir(src)

    os.makedirs(dst)

    for name in names:
        if (name in bad_dirs) or (name in bad_files):
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

#   input - resource.xml
#   output - game\resources\resource.default, ...
        
def get_resource_path(dom,src):
    resources_tag = dom.getElementsByTagName("Resources")[0]
    resource = resources_tag.getElementsByTagName("Resource")
    
    if(resource == []):
        return []
                    
    path_resources = resource[0].getAttribute("Path");
    
    path_resources = os.path.join(src,path_resources)
    
    resource_list = []
    
    for node in resource[0].childNodes:
        if node.nodeType == node.ELEMENT_NODE:
            resource_list.append(os.path.join(path_resources,node.localName)+".resource")
    
    return resource_list

def read_output_file(src):
    dom = xml.dom.minidom.parse(src)

    app_tag = dom.getElementsByTagName("resources")[0]
    resources = app_tag.getElementsByTagName("resource")

    resource_list = []
    
    for resource in resources:
        resource_list.append(resource.getAttribute("value"))
        
    return resource_list
    
def atlas(src,destdir):
    dom = xml.dom.minidom.parse(src)

    print "getting resource list from xml"
    
    resource_list = get_resource_path(dom,os.path.dirname(src))
       
    gamedir = os.path.dirname(src)
    
    for resource in resource_list:

        resource_output_name = os.path.basename(resource)
        
        exe = 'AtlasMaker.exe %(resource_name)s %(output_resource)s %(width)i %(height)i %(gamedir)s %(halftexel)i %(jpgquality)i' % \
            {'resource_name' : resource, \
             'output_resource' : resource_output_name,\
             'width' : atlas_width, \
             'height' : atlas_height, \
             'gamedir' : gamedir, \
             'halftexel' : halftexel_use, \
             'jpgquality' : jpg_quality \
            }
        
        subprocess.call(exe)
        
        
        lines = read_output_file("output.xml")
        
        copy_files.append(os.path.join(destdir,os.path.join("Resource",lines[0])))
        
        for line in lines[1:]:
            line = os.path.join(destdir,line)
            copy_files.append(line)
            print line
                   
        bad_files.append(resource)
        
        dom = xml.dom.minidom.parse(resource)
        resources = dom.getElementsByTagName("DataBlock")[0].getElementsByTagName("Resource")
        
        for resource in resources:
            type = resource.getAttribute("Type")
            noAtlas = resource.getAttribute("NoAtlas") != ''
        
            if(type in allowed_type and noAtlas == False):
                files = resource.getElementsByTagName("File")
                if(files != []):
                    for file in files:
                        value = file.getAttribute("Path")
                        value = os.path.join(gamedir,value).lower()
                        bad_files.append(os.path.normpath(value))

        
        dom.unlink()

def copytonewfolder(src, dst):
    # get game directory from 'e:/game/bin/app.xml', so game dir is 'e:/game/'
    game_dir = os.path.dirname(os.path.dirname(src))
        
    # if dest path exist, just return.
    if os.path.exists(dst):
        print "output directory already exist!"
        return
    
    os.mkdir(dst)
    # get resources list from application.xml. Example "Game\\Resource\\Resource.xml"
    filelist = formreslist(src)
    
    if filelist == []:
        print "xml's resources are empty!"
        return
    
    # for each xml' resources - copy resource folders
    for file in filelist:
        srcdir = os.path.join(game_dir,os.path.dirname(file))
        destdir = os.path.join(dst,os.path.dirname(file))
        
        srcfile = os.path.join(game_dir,file)
        atlas(srcfile,destdir)
        
        copytree(srcdir,destdir)
        copyfiles()
        
    copytree(os.path.join(game_dir,"Bin"),os.path.join(dst,"Bin"))
    
    print "done!"
    
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
