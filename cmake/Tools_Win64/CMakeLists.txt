CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(Tools_WIN64)

SET(MENGINE_REPOSITORY ${CMAKE_CURRENT_LIST_DIR}/../..)

INCLUDE(${MENGINE_REPOSITORY}/cmake/build_options_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/dev_options_template.cmake)

SET(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED OFF CACHE BOOLEAN "MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED" FORCE)

INCLUDE(${MENGINE_REPOSITORY}/cmake/base_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/macro_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/win64_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/mengine_template.cmake)

SET_MENGINE_ENVIRONMENT(WINDOWS DIRECTX9 WIN32 MSVC)

SET(CMAKE_TEMP_DIR ${CMAKE_BINARY_DIR}/Tools)

# plugins
ADD_PLUGIN(MENGINE_PLUGIN_DEVELOPMENTCONVERTER ON ON "MENGINE_PLUGIN_DEVELOPMENTCONVERTER")
ADD_PLUGIN(MENGINE_PLUGIN_ASTRALAX ON ON "MENGINE_PLUGIN_ASTRALAX")
ADD_PLUGIN(MENGINE_PLUGIN_XMLTOBIN ON ON "MENGINE_PLUGIN_XMLTOBIN")

SET_MENGINE_SOLUTIONS_OUTPUT_DIRECTORY()

INCLUDE_DIRECTORIES(${MENGINE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${MENGINE_SOURCE_DIR}/Tools)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/stdex/include)

ADD_SUBDIRECTORY(${MENGINE_SOURCE_DIR}/Environment ${CMAKE_TEMP_DIR}/Environment)
ADD_SUBDIRECTORY(${MENGINE_SOURCE_DIR}/PrecompiledHeader ${CMAKE_TEMP_DIR}/PrecompiledHeader)
ADD_SUBDIRECTORY(${MENGINE_SOURCE_DIR}/Config ${CMAKE_TEMP_DIR}/Config)
ADD_SUBDIRECTORY(${MENGINE_SOURCE_DIR}/Kernel ${CMAKE_TEMP_DIR}/Kernel)
ADD_SUBDIRECTORY(${MENGINE_SOURCE_DIR}/Engine ${CMAKE_TEMP_DIR}/Engine)
ADD_SUBDIRECTORY(${MENGINE_SOURCE_DIR}/Metacode ${CMAKE_TEMP_DIR}/Metacode)

set(SOURCE_DIR_SYSTEMS ${MENGINE_SOURCE_DIR}/Systems)
set(TEMP_DIR_SYSTEMS ${CMAKE_TEMP_DIR}/Systems)
set(SOURCE_DIR_SERVICES ${MENGINE_SOURCE_DIR}/Services)
set(TEMP_DIR_SERVICES ${CMAKE_TEMP_DIR}/Services)
set(SOURCE_DIR_PLUGINS ${MENGINE_SOURCE_DIR}/Plugins)
set(TEMP_DIR_PLUGINS ${CMAKE_TEMP_DIR}/Plugins)

ADD_SUBDIRECTORY(${SOURCE_DIR_SYSTEMS}/Win32TimeSystem ${TEMP_DIR_SYSTEMS}/Win32TimeSystem)
ADD_SUBDIRECTORY(${SOURCE_DIR_SYSTEMS}/Win32UnicodeSystem ${TEMP_DIR_SYSTEMS}/Win32UnicodeSystem)
ADD_SUBDIRECTORY(${SOURCE_DIR_SYSTEMS}/Win32ThreadSystem ${TEMP_DIR_SYSTEMS}/Win32ThreadSystem)
ADD_SUBDIRECTORY(${SOURCE_DIR_SYSTEMS}/Win32AllocatorSystem ${TEMP_DIR_SYSTEMS}/Win32AllocatorSystem)
ADD_SUBDIRECTORY(${SOURCE_DIR_SYSTEMS}/Win32DateTimeSystem ${TEMP_DIR_SYSTEMS}/Win32DateTimeSystem)
ADD_SUBDIRECTORY(${SOURCE_DIR_SYSTEMS}/Win32FileSystem ${TEMP_DIR_SYSTEMS}/Win32FileSystem)

ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/DocumentService ${TEMP_DIR_SERVICES}/DocumentService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/NotificationService ${TEMP_DIR_SERVICES}/NotificationService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/EnumeratorService ${TEMP_DIR_SERVICES}/EnumeratorService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/FactoryService ${TEMP_DIR_SERVICES}/FactoryService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/ProviderService ${TEMP_DIR_SERVICES}/ProviderService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/StringizeService ${TEMP_DIR_SERVICES}/StringizeService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/ArchiveService ${TEMP_DIR_SERVICES}/ArchiveService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/FileService ${TEMP_DIR_SERVICES}/FileService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/CodecService ${TEMP_DIR_SERVICES}/CodecService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/DataService ${TEMP_DIR_SERVICES}/DataService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/MemoryService ${TEMP_DIR_SERVICES}/MemoryService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/ConverterService ${TEMP_DIR_SERVICES}/ConverterService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/OptionsService ${TEMP_DIR_SERVICES}/OptionsService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/LoggerService ${TEMP_DIR_SERVICES}/LoggerService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/PluginService ${TEMP_DIR_SERVICES}/PluginService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/ThreadService ${TEMP_DIR_SERVICES}/ThreadService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/ConfigService ${TEMP_DIR_SERVICES}/ConfigService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/TextService ${TEMP_DIR_SERVICES}/TextService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/PrototypeService ${TEMP_DIR_SERVICES}/PrototypeService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/VocabularyService ${TEMP_DIR_SERVICES}/VocabularyService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/LoaderService ${TEMP_DIR_SERVICES}/LoaderService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/SecureService ${TEMP_DIR_SERVICES}/SecureService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/TimelineService ${TEMP_DIR_SERVICES}/TimelineService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/TimepipeService ${TEMP_DIR_SERVICES}/TimepipeService)
ADD_SUBDIRECTORY(${SOURCE_DIR_SERVICES}/ModuleService ${TEMP_DIR_SERVICES}/ModuleService)

ADD_SUBDIRECTORY(${MENGINE_SOURCE_DIR}/Platforms/Win32Platform ${CMAKE_TEMP_DIR}/Platforms/Win32Platform)

ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/DevelopmentConverterPlugin ${TEMP_DIR_PLUGINS}/DevelopmentConverterPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/ImageCodecPlugin ${TEMP_DIR_PLUGINS}/ImageCodecPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/XmlToBinPlugin ${TEMP_DIR_PLUGINS}/XmlToBinPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/ZipPlugin ${TEMP_DIR_PLUGINS}/ZipPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/LZ4Plugin ${TEMP_DIR_PLUGINS}/LZ4Plugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/MoviePlugin ${TEMP_DIR_PLUGINS}/MoviePlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/Win32FindPython3Plugin ${TEMP_DIR_PLUGINS}/Win32FindPython3Plugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/JPEGPlugin ${TEMP_DIR_PLUGINS}/JPEGPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/PNGPlugin ${TEMP_DIR_PLUGINS}/PNGPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/PVRTCPlugin ${TEMP_DIR_PLUGINS}/PVRTCPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/WebPPlugin ${TEMP_DIR_PLUGINS}/WebPPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/TheoraPlugin ${TEMP_DIR_PLUGINS}/TheoraPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/OggVorbisPlugin ${TEMP_DIR_PLUGINS}/OggVorbisPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/AstralaxPlugin ${TEMP_DIR_PLUGINS}/AstralaxPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/MetabufPlugin ${TEMP_DIR_PLUGINS}/MetabufPlugin)
ADD_SUBDIRECTORY(${SOURCE_DIR_PLUGINS}/DazzlePlugin ${TEMP_DIR_PLUGINS}/DazzlePlugin)

ADD_SUBDIRECTORY(${MENGINE_SOURCE_DIR}/Tools ${CMAKE_TEMP_DIR}/Tools)