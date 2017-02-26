CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

project(SDL2_mixer)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/SDL2_mixer)

# OUR SOURCES
set(SOURCES
    ${SOURCE_DIR}/effect_position.c
    ${SOURCE_DIR}/effect_stereoreverse.c
    ${SOURCE_DIR}/effects_internal.c
    ${SOURCE_DIR}/effects_internal.h
    ${SOURCE_DIR}/fluidsynth.c
    ${SOURCE_DIR}/fluidsynth.h
    ${SOURCE_DIR}/load_aiff.c
    ${SOURCE_DIR}/load_aiff.h
    ${SOURCE_DIR}/load_flac.c
    ${SOURCE_DIR}/load_flac.h
    ${SOURCE_DIR}/load_mp3.c
    ${SOURCE_DIR}/load_mp3.h
    ${SOURCE_DIR}/load_ogg.c
    ${SOURCE_DIR}/load_ogg.h
    ${SOURCE_DIR}/load_voc.c
    ${SOURCE_DIR}/load_voc.h
    ${SOURCE_DIR}/mixer.c
    ${SOURCE_DIR}/music.c
    ${SOURCE_DIR}/music_cmd.c
    ${SOURCE_DIR}/music_cmd.h
    ${SOURCE_DIR}/music_flac.c
    ${SOURCE_DIR}/music_flac.h
    ${SOURCE_DIR}/music_mad.c
    ${SOURCE_DIR}/music_mad.h
    ${SOURCE_DIR}/music_mod.c
    ${SOURCE_DIR}/music_mod.h
    ${SOURCE_DIR}/music_modplug.c
    ${SOURCE_DIR}/music_modplug.h
    ${SOURCE_DIR}/music_ogg.c
    ${SOURCE_DIR}/music_ogg.h
    ${SOURCE_DIR}/SDL_mixer.h
    ${SOURCE_DIR}/wavestream.c
    ${SOURCE_DIR}/wavestream.h
)

include_directories(${SOURCE_DIR}/lib)
include_directories(${THIRDPARTY_DIR}/SDL2/include)

add_definitions(-DWAV_MUSIC)

SET(FORCE_STATIC_VCRT ON CACHE BOOL "Force /MT for static VC runtimes" FORCE)
add_library(${PROJECT_NAME} STATIC ${SOURCES})
