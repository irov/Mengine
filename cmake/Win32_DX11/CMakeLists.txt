CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET(MENGINE_PROJECT_NAME "Mengine_WIN32_DX11" CACHE STRING "Mengine project name")

PROJECT(${MENGINE_PROJECT_NAME})

SET(MENGINE_REPOSITORY ${CMAKE_CURRENT_LIST_DIR}/../..)

INCLUDE(${MENGINE_REPOSITORY}/cmake/build_options_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/dev_options_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/base_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/macro_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/win32_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/mengine_template.cmake)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

SET_MENGINE_ENVIRONMENT(WINDOWS DIRECTX11 WIN32 MSVC)

ADD_MENGINE_FRAMEWORK()

# platform
ADD_PLATFORM(Win32Platform "MENGINE_PLATFORM")

# systems
if(MENGINE_BUILD_TYPE_DEBUG)
    ADD_SYSTEM(MENGINE_SYSTEM_ALLOCATOR DebugAllocatorSystem "MENGINE_SYSTEM_ALLOCATOR")
else()
    ADD_SYSTEM(MENGINE_SYSTEM_ALLOCATOR Win32AllocatorSystem "MENGINE_SYSTEM_ALLOCATOR")
endif()

ADD_SYSTEM(MENGINE_SYSTEM_SOUND OpenALSoundSystem "MENGINE_SYSTEM_SOUND")
ADD_SYSTEM(MENGINE_SYSTEM_FILE Win32FileSystem "MENGINE_SYSTEM_FILE")
ADD_SYSTEM(MENGINE_SYSTEM_DATETIME Win32DateTimeSystem "MENGINE_SYSTEM_DATETIME")
ADD_SYSTEM(MENGINE_SYSTEM_UNICODE NativeUnicodeSystem "MENGINE_SYSTEM_UNICODE")
ADD_SYSTEM(MENGINE_SYSTEM_THREAD Win32ThreadSystem "MENGINE_SYSTEM_THREAD")
ADD_SYSTEM(MENGINE_SYSTEM_TIME Win32TimeSystem "MENGINE_SYSTEM_TIME")
ADD_SYSTEM(MENGINE_SYSTEM_CRYPTOGRAPHY Win32CryptographySystem "MENGINE_SYSTEM_CRYPTOGRAPHY")
ADD_SYSTEM(MENGINE_SYSTEM_RENDER DX11RenderSystem "MENGINE_SYSTEM_RENDER")
ADD_SYSTEM(MENGINE_SYSTEM_HTTP Win32HttpSystem "MENGINE_SYSTEM_HTTP")

# plugins
ADD_PLUGIN(MENGINE_PLUGIN_MENGINE ON ON "MENGINE_PLUGIN_MENGINE")
ADD_PLUGIN(MENGINE_PLUGIN_SIMPLEFRAMEWORK OFF OFF "MENGINE_PLUGIN_SIMPLEFRAMEWORK")
ADD_PLUGIN(MENGINE_PLUGIN_PYTHONFRAMEWORK ON OFF "MENGINE_PLUGIN_PYTHONFRAMEWORK")
ADD_PLUGIN(MENGINE_PLUGIN_UIFRAMEWORK ON OFF "MENGINE_PLUGIN_UIFRAMEWORK")
ADD_PLUGIN(MENGINE_PLUGIN_DEVELOPMENTCONVERTER ON ON "MENGINE_PLUGIN_DEVELOPMENTCONVERTER")
ADD_PLUGIN(MENGINE_PLUGIN_NODEDEBUGRENDER ON OFF "MENGINE_PLUGIN_NODEDEBUGRENDER")
ADD_PLUGIN(MENGINE_PLUGIN_DEBUGPANEL ON OFF "MENGINE_PLUGIN_DEBUGPANEL")
ADD_PLUGIN(MENGINE_PLUGIN_TEXTVALIDATE ON OFF "MENGINE_PLUGIN_TEXTVALIDATE")
ADD_PLUGIN(MENGINE_PLUGIN_RESOURCEVALIDATE ON OFF "MENGINE_PLUGIN_RESOURCEVALIDATE")
ADD_PLUGIN(MENGINE_PLUGIN_RESOURCEPREFETCHER ON OFF "MENGINE_PLUGIN_RESOURCEPREFETCHER")
ADD_PLUGIN(MENGINE_PLUGIN_RESOURCECONVERT ON OFF "MENGINE_PLUGIN_RESOURCECONVERT")
ADD_PLUGIN(MENGINE_PLUGIN_RESOURCEDEBUGGER ON OFF "MENGINE_PLUGIN_RESOURCEDEBUGGER")
ADD_PLUGIN(MENGINE_PLUGIN_NODEDEBUGGER ON ON "MENGINE_PLUGIN_NODEDEBUGGER")
ADD_PLUGIN(MENGINE_PLUGIN_METABUF ON OFF "MENGINE_PLUGIN_METABUF")
ADD_PLUGIN(MENGINE_PLUGIN_IMAGE_CODEC ON OFF "MENGINE_PLUGIN_IMAGE_CODEC")
ADD_PLUGIN(MENGINE_PLUGIN_WAV ON OFF "MENGINE_PLUGIN_WAV")
ADD_PLUGIN(MENGINE_PLUGIN_AMPLIFIER ON OFF "MENGINE_PLUGIN_AMPLIFIER")
ADD_PLUGIN(MENGINE_PLUGIN_OGG_VORBIS ON OFF "MENGINE_PLUGIN_OGG_VORBIS")
ADD_PLUGIN(MENGINE_PLUGIN_ZIP ON OFF "MENGINE_PLUGIN_ZIP")
ADD_PLUGIN(MENGINE_PLUGIN_LZ4 ON OFF "MENGINE_PLUGIN_LZ4")
ADD_PLUGIN(MENGINE_PLUGIN_INI ON OFF "MENGINE_PLUGIN_INI")
ADD_PLUGIN(MENGINE_PLUGIN_JSON ON OFF "MENGINE_PLUGIN_JSON")
ADD_PLUGIN(MENGINE_PLUGIN_TEXTUREPACKER ON OFF "MENGINE_PLUGIN_TEXTUREPACKER")
ADD_PLUGIN(MENGINE_PLUGIN_PVRTC OFF OFF "MENGINE_PLUGIN_PVRTC")
ADD_PLUGIN(MENGINE_PLUGIN_ETC1 OFF OFF "MENGINE_PLUGIN_ETC1")
ADD_PLUGIN(MENGINE_PLUGIN_DDS ON OFF "MENGINE_PLUGIN_DDS")
ADD_PLUGIN(MENGINE_PLUGIN_JPEG ON OFF "MENGINE_PLUGIN_JPEG")
ADD_PLUGIN(MENGINE_PLUGIN_PNG ON OFF "MENGINE_PLUGIN_PNG")
ADD_PLUGIN(MENGINE_PLUGIN_WEBP ON OFF "MENGINE_PLUGIN_WEBP")
ADD_PLUGIN(MENGINE_PLUGIN_MOVIE ON OFF "MENGINE_PLUGIN_MOVIE")
ADD_PLUGIN(MENGINE_PLUGIN_VIDEO ON OFF "MENGINE_PLUGIN_VIDEO")
ADD_PLUGIN(MENGINE_PLUGIN_AV1 ON OFF "MENGINE_PLUGIN_AV1")
ADD_PLUGIN(MENGINE_PLUGIN_THEORA ON OFF "MENGINE_PLUGIN_THEORA")
ADD_PLUGIN(MENGINE_PLUGIN_BITMAPFONT ON OFF "MENGINE_PLUGIN_BITMAPFONT")
ADD_PLUGIN(MENGINE_PLUGIN_TTF ON OFF "MENGINE_PLUGIN_TTF")
ADD_PLUGIN(MENGINE_PLUGIN_FE ON OFF "MENGINE_PLUGIN_FE")
ADD_PLUGIN(MENGINE_PLUGIN_SPINE OFF OFF "MENGINE_PLUGIN_SPINE")
ADD_PLUGIN(MENGINE_PLUGIN_DAZZLE ON OFF "MENGINE_PLUGIN_DAZZLE")
ADD_PLUGIN_IF(MENGINE_PLUGIN_ASTRALAX ON ON "MENGINE_PLUGIN_ASTRALAX" ${THIRDPARTY_DIR}/astralax)
ADD_PLUGIN_IF(MENGINE_PLUGIN_STEAM ON ON "MENGINE_PLUGIN_STEAM" ${THIRDPARTY_DIR}/steam)
ADD_PLUGIN(MENGINE_PLUGIN_OZZ_ANIMATION ON ON "MENGINE_PLUGIN_OZZ_ANIMATION")
ADD_PLUGIN(MENGINE_PLUGIN_ANDROID_NATIVE_PYTHON OFF OFF "MENGINE_PLUGIN_ANDROID_NATIVE_PYTHON")
ADD_PLUGIN(MENGINE_PLUGIN_BOX2D ON OFF "MENGINE_PLUGIN_BOX2D")
ADD_PLUGIN(MENGINE_PLUGIN_GRAPHICS ON OFF "MENGINE_PLUGIN_GRAPHICS")
ADD_PLUGIN(MENGINE_PLUGIN_GOAP ON OFF "MENGINE_PLUGIN_GOAP")
ADD_PLUGIN(MENGINE_PLUGIN_IMGUI ON OFF "MENGINE_PLUGIN_IMGUI")
ADD_PLUGIN(MENGINE_PLUGIN_AREAOFINTEREST ON OFF "MENGINE_PLUGIN_AREAOFINTEREST")
ADD_PLUGIN(MENGINE_PLUGIN_PATHFINDER ON OFF "MENGINE_PLUGIN_PATHFINDER")
ADD_PLUGIN(MENGINE_PLUGIN_CAMERADEBUGGIZMO ON OFF "MENGINE_PLUGIN_CAMERADEBUGGIZMO")
ADD_PLUGIN(MENGINE_PLUGIN_FILEMODIFYHOOK ON OFF "MENGINE_PLUGIN_FILEMODIFYHOOK")
ADD_PLUGIN(MENGINE_PLUGIN_XMLTOBIN ON ON "MENGINE_PLUGIN_XMLTOBIN")
ADD_PLUGIN(MENGINE_PLUGIN_WIN32_XLSEXPORT ON ON "MENGINE_PLUGIN_WIN32_XLSEXPORT")
ADD_PLUGIN(MENGINE_PLUGIN_MEMORYUSAGEMONITOR ON ON "MENGINE_PLUGIN_MEMORYUSAGEMONITOR")
ADD_PLUGIN(MENGINE_PLUGIN_OPTICK ON ON "MENGINE_PLUGIN_OPTICK")
ADD_PLUGIN(MENGINE_PLUGIN_RENDERDOC ON ON "MENGINE_PLUGIN_RENDERDOC")
ADD_PLUGIN(MENGINE_PLUGIN_DEVTODEBUG ON OFF "MENGINE_PLUGIN_DEVTODEBUG")
ADD_PLUGIN(MENGINE_PLUGIN_CACHALOT OFF OFF "MENGINE_PLUGIN_CACHALOT")

ADD_PLUGIN(MENGINE_PLUGIN_WIN32_ANTIFREEZEMONITOR ON ON "MENGINE_PLUGIN_WIN32_ANTIFREEZEMONITOR")
ADD_PLUGIN(MENGINE_PLUGIN_WIN32_CRITICALERRORSMONITOR ON ON "MENGINE_PLUGIN_WIN32_CRITICALERRORSMONITOR")
ADD_PLUGIN(MENGINE_PLUGIN_WIN32_SOCKET ON ON "MENGINE_PLUGIN_WIN32_SOCKET")
ADD_PLUGIN(MENGINE_PLUGIN_WIN32_SENTRY ON OFF "MENGINE_PLUGIN_WIN32_SENTRY")

#MENGINE_ADD_DEFINITION(MENGINE_RENDER_TEXTURE_RGBA)
MENGINE_ADD_DEFINITION(MENGINE_RENDER_COLOR_RGBA)
#MENGINE_ADD_DEFINITION(MENGINE_SETJMP_UNSUPPORTED)

SET_MENGINE_SOLUTIONS_OUTPUT_DIRECTORY()

ADD_SUBDIRECTORY(${MENGINE_SOURCE_DIR} ${CMAKE_TEMP_DIR}/src)

ADD_SUBDIRECTORY(${MENGINE_SOURCE_DIR}/Applications/Win32Application ${CMAKE_TEMP_DIR}/Win32Application)