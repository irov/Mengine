lua-5.0.3 
- http://www.lua.org/ftp/lua-5.0.3.tar.gz

luabind 
[ luabind-0.7 ] - 
http://downloads.sourceforge.net/luabind/luabind-0.7.zip?use_mirror=heanet

дописать в config.hpp 

#	define LUABIND_EXPORT __declspec(dllexport)

#	define LUABIND_IMPORT __declspec(dllimport)

tinyxml
[ tinyxml-2.5.2 ] -
http://sourceforge.net/project/showfiles.php?group_id=13559&package_id=20883