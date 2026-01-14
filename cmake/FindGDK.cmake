# FindGDK.cmake
# Finds GDK SDK and sets GDK_INCLUDE_DIR and GDK_LIB_DIR variables

# GDK SDK version
# Can be set via -DMENGINE_GDK_VERSION=251001 or in CMakeCache
if(NOT DEFINED MENGINE_GDK_VERSION)
    set(MENGINE_GDK_VERSION "251001" CACHE STRING "GDK SDK version (e.g., 251001)")
endif()

MESSAGE("MENGINE_GDK_VERSION: ${MENGINE_GDK_VERSION}")

# GDK SDK paths
# Try to find GDK SDK from common installation locations
set(GDK_INCLUDE_DIR "")
set(GDK_LIB_DIR "")

# Check environment variable first
if(DEFINED ENV{GDK_PATH})
    set(GDK_ROOT $ENV{GDK_PATH})
    # Try new layout (October 2025+)
    if(EXISTS "${GDK_ROOT}/windows/include")
        set(GDK_INCLUDE_DIR "${GDK_ROOT}/windows/include")
        set(GDK_LIB_DIR "${GDK_ROOT}/windows/lib/x64")
    # Try old layout
    elseif(EXISTS "${GDK_ROOT}/GRDK/GameKit/Include")
        set(GDK_INCLUDE_DIR "${GDK_ROOT}/GRDK/GameKit/Include")
        set(GDK_LIB_DIR "${GDK_ROOT}/GRDK/GameKit/Lib/x64")
    endif()
endif()

# Try to find GDK SDK from standard installation location using specified version
if(NOT GDK_INCLUDE_DIR AND EXISTS "C:/Program Files (x86)/Microsoft GDK")
    set(GDK_ROOT "C:/Program Files (x86)/Microsoft GDK/${MENGINE_GDK_VERSION}")
    
    # Try new layout (October 2025+)
    if(EXISTS "${GDK_ROOT}/windows/include")
        set(GDK_INCLUDE_DIR "${GDK_ROOT}/windows/include")
        set(GDK_LIB_DIR "${GDK_ROOT}/windows/lib/x64")
    # Try old layout
    elseif(EXISTS "${GDK_ROOT}/GRDK/GameKit/Include")
        set(GDK_INCLUDE_DIR "${GDK_ROOT}/GRDK/GameKit/Include")
        set(GDK_LIB_DIR "${GDK_ROOT}/GRDK/GameKit/Lib/x64")
    endif()
endif()

# Fallback: auto-detect latest version if specified version not found
if(NOT GDK_INCLUDE_DIR AND EXISTS "C:/Program Files (x86)/Microsoft GDK")
    file(GLOB GDK_VERSIONS "C:/Program Files (x86)/Microsoft GDK/*")
    MESSAGE("GDK_VERSIONS: ${GDK_VERSIONS}")
    if(GDK_VERSIONS)
        list(SORT GDK_VERSIONS)
        list(REVERSE GDK_VERSIONS)
        list(GET GDK_VERSIONS 0 GDK_ROOT)
        # Try new layout (October 2025+)
        if(EXISTS "${GDK_ROOT}/windows/include")
            set(GDK_INCLUDE_DIR "${GDK_ROOT}/windows/include")
            set(GDK_LIB_DIR "${GDK_ROOT}/windows/lib/x64")
        # Try old layout
        elseif(EXISTS "${GDK_ROOT}/GRDK/GameKit/Include")
            set(GDK_INCLUDE_DIR "${GDK_ROOT}/GRDK/GameKit/Include")
            set(GDK_LIB_DIR "${GDK_ROOT}/GRDK/GameKit/Lib/x64")
        endif()
    endif()
endif()

if(GDK_INCLUDE_DIR)
    MESSAGE("GDK_INCLUDE_DIR: ${GDK_INCLUDE_DIR}")
    # Make variables available globally
    set(GDK_INCLUDE_DIR "${GDK_INCLUDE_DIR}" CACHE PATH "GDK SDK include directory")
    # Add to global include directories so all targets can use it
    INCLUDE_DIRECTORIES(${GDK_INCLUDE_DIR})
else()
    MESSAGE(WARNING "GDK SDK include directory not found. Please set GDK_PATH environment variable or install GDK SDK.")
endif()

if(GDK_LIB_DIR)
    MESSAGE("GDK_LIB_DIR: ${GDK_LIB_DIR}")
    # Make variables available globally
    set(GDK_LIB_DIR "${GDK_LIB_DIR}" CACHE PATH "GDK SDK library directory")
    # Add to global link directories so all targets can use it
    LINK_DIRECTORIES(${GDK_LIB_DIR})
endif()

set(GDK_FOUND TRUE)
if(NOT GDK_INCLUDE_DIR)
    set(GDK_FOUND FALSE)
endif()
