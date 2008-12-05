
ADD_FILTER(
LoggerConsole
	LoggerConsole.cpp
	LoggerConsole.h
)

ADD_FILTER(
Resources
	menge.ico
	resource.h
	WinApplication.rc
)
	
ADD_FILTER(
src
	main.cpp
	SystemDLL.cpp
	SystemDLL.h
	WinApplication.cpp
	WinApplication.h
	WinApplication.def
	WinTimer.cpp
	WinTimer.h
)

SET( LIBS
debug pybind_d.lib
debug python25_d.lib
debug libpngd.lib
debug zlibd.lib
debug libjpegd.lib
debug libtheora_d.lib
optimized pybind.lib
optimized python25.lib
optimized libPNG.lib
optimized zlib.lib
optimized libJPEG.lib
optimized libtheora.lib
)

ADD_EXECUTABLE( WinApplication ${SRC_FILES} )
TARGET_LINK_LIBRARIES( WinApplication ${LIBS} )
