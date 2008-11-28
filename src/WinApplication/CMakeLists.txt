
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

ADD_EXECUTABLE( WinApplication ${SRC_FILES} )