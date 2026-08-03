ADD_SUBDIRECTORY(ToolUtils)
ADD_SUBDIRECTORY(ToolEngine)

macro(ADD_MENGINE_TOOL_EXECUTABLE)
    if(WIN32)
        ADD_MENGINE_EXECUTABLE_WIN32_CONSOLE()
    else()
        ADD_MENGINE_EXECUTABLE_UNIX()
    endif()

    if(APPLE)
        SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
            BUILD_RPATH "${THIRDPARTY_DIR}/astralax_macos/lib;@loader_path"
        )
    endif()
endmacro()

macro(ADD_MENGINE_TOOL_EXECUTABLE_GUI)
    if(WIN32)
        ADD_MENGINE_EXECUTABLE_WIN32()
    else()
        ADD_MENGINE_EXECUTABLE_UNIX()
    endif()

    if(APPLE)
        SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
            BUILD_RPATH "${THIRDPARTY_DIR}/astralax_macos/lib;@loader_path"
        )
    endif()
endmacro()

ADD_SUBDIRECTORY(AekCompiler)
ADD_SUBDIRECTORY(MengineConverter)
ADD_SUBDIRECTORY(Metawrite)
ADD_SUBDIRECTORY(ExtractorDXT1)
ADD_SUBDIRECTORY(ExtractorETC1)
ADD_SUBDIRECTORY(NodeDebugger)
ADD_SUBDIRECTORY(Ravinggen)

ADD_SUBDIRECTORY(MovieResource)
ADD_SUBDIRECTORY(MovieCompiler)
ADD_SUBDIRECTORY(DazzleCompiler)
ADD_SUBDIRECTORY(AlphaSpreading)
ADD_SUBDIRECTORY(AlphaPremultiplier)
ADD_SUBDIRECTORY(AstralaxCompiler)
ADD_SUBDIRECTORY(ImagePolygonize)
ADD_SUBDIRECTORY(ImageTrimmer)
ADD_SUBDIRECTORY(TexturePacker)
#ADD_SUBDIRECTORY(PolygonTriangulator)

MENGINE_SUBFOLDER()
