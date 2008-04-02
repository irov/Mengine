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

#skipped extensions
bad_ext = ['.py']  
#skipped files
bad_files = ['thumbs.db']
#skipped folders
bad_dirs = ['thumbnails', '.svn']
#files with bad extension, but needed. Example - log file
good_files = []

copy_files = []

allowed_type = ['ResourceImageDefault','ResourceImageSet','ResourceImageCell']

def formreslist(src):
    dom = xml.dom.minidom.parse(src)

    app_tag = dom.getElementsByTagName("Application")[0]
    resources = app_tag.getElementsByTagName("Resource")

    resource_list = []
    
    for resource in resources:
        resource_list.append(resource.getAttribute("File"))
	
    #TODO kill unique resources
    
    return resource_list
	
def copyfiles(files):
    for file in files:
	src = os.path.basename(file)
	shutil.copy2(src, file)
	copy_files.remove(file)
	os.remove(src)	
	
def copytree(src, dst):
    compileall.compile_dir(src, rx=re.compile('/[.]svn'), force=True)
    
    names = os.listdir(src)

    os.mkdir(dst)

    for name in names:
        if (name in bad_dirs) or (name in bad_files):
            continue
	
        ext = os.path.splitext(name)[1]
        
        if (ext in bad_ext) and not (name in good_files):
            continue
        
        srcname = os.path.join(src, name)
	
	if (srcname in bad_files):
            continue
	
        dstname = os.path.join(dst, name)
        
        if os.path.isdir(srcname):
        	copytree(srcname, dstname)
        else:
                shutil.copy2(srcname, dstname)

		
def copytonewfolder(src, dst):
    # if dest path exist, just return.
    if os.path.exists(dst):
	return
    
    os.mkdir(dst)
    # get resources list from application.xml. Example "Game\\Resource\\Resource.xml"
    filelist = formreslist(src)
    
    # for each xml' resources - copy resource folders
    for file in filelist:
	basedir = os.path.dirname(file)
	# TestDir\\Game
	destdir = os.path.join(dst,basedir)
	# 
	atlas(file,destdir)
	copytree(basedir,destdir)  
	
	copyfiles(copy_files)

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
    
def atlas(src,destdir):
    dom = xml.dom.minidom.parse(src)

    resource_list = get_resource_path(dom,os.path.dirname(src))
       
    for resource in resource_list:
	
	exe = 'AtlasCreationTool.exe %(resource_name)s %(output_resource)s %(width)i %(height)i' % \
	    {'resource_name' : resource, 'output_resource' : 'output','width' : 512, 'height' : 512}
	
	subprocess.call(exe)

	copydir = os.path.join(os.path.dirname(destdir),os.path.dirname(resource))

	f = open("output.txt")
	line = f.readline()
	f.close()
	
	lines = line.split()
	
	for line in lines:
	    line = os.path.join(copydir,line)
	    copy_files.append(line)
	    print line

	
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
			value = os.path.normpath(value)
			bad_files.append(value)
			print value
			
	dom.unlink()

def main():
    copytonewfolder("application_d.xml","TestDir")

main()