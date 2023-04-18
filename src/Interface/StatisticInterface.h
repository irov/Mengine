#pragma once

#include "Config/Char.h"

#ifndef MENGINE_STATISTIC_MAX_COUNT
#define MENGINE_STATISTIC_MAX_COUNT 256
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<uint32_t ID>
    struct Statistic;
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_STATISTIC_DECLARE_BEGIN()\
    static constexpr uint32_t MENGINE_STATISTIC_ENUMERATOR_BEGIN = MENGINE_CODE_LINE
        //////////////////////////////////////////////////////////////////////////
#define MENGINE_STATISTIC_DECLARE_END()\
    static constexpr uint32_t MENGINE_STATISTIC_ENUMERATOR_COUNT = MENGINE_CODE_LINE - MENGINE_STATISTIC_ENUMERATOR_BEGIN - 1;\
    static_assert(MENGINE_STATISTIC_ENUMERATOR_COUNT < MENGINE_STATISTIC_MAX_COUNT)
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_STATISTIC_DECLARE(NAME)\
    static constexpr uint32_t NAME = MENGINE_CODE_LINE - MENGINE_STATISTIC_ENUMERATOR_BEGIN;\
    template<> struct Statistic<NAME> { static const Char * getName() { return #NAME; } }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_STATISTIC_DECLARE_BEGIN();
    MENGINE_STATISTIC_DECLARE( STATISTIC_ALLOCATOR_NEW );
    MENGINE_STATISTIC_DECLARE( STATISTIC_ALLOCATOR_FREE );
    MENGINE_STATISTIC_DECLARE( STATISTIC_ALLOCATOR_SIZE );
    MENGINE_STATISTIC_DECLARE( STATISTIC_OPEN_FILE_COUNT );
    MENGINE_STATISTIC_DECLARE( STATISTIC_OPEN_FILE_READ_BYTES );
    MENGINE_STATISTIC_DECLARE( STATISTIC_OPEN_FILE_WRITE_BYTES );
    MENGINE_STATISTIC_DECLARE( STATISTIC_CURRENT_SCENE_NAME );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_FRAME_COUNT );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_PERFRAME_DRAWINDEXPRIMITIVES );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_PERFRAME_FILLRATE );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_PERFRAME_OBJECTS );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_PERFRAME_TRIANGLES );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_PERFRAME_BATCHES );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_IMAGE_NEW );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_IMAGE_FREE );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_IMAGE_COUNT );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_IMAGE_SIZE );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_IMAGE_LOCK_COUNT );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_IMAGE_LOCK_PIXEL );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_INDEX_BUFFER_NEW );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_INDEX_BUFFER_FREE );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_INDEX_BUFFER_COUNT );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_INDEX_BUFFER_SIZE );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_INDEX_BUFFER_LOCK_NEW );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_INDEX_BUFFER_LOCK_SIZE );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_VERTEX_BUFFER_NEW );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_VERTEX_BUFFER_FREE );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_VERTEX_BUFFER_COUNT );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_VERTEX_BUFFER_SIZE );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_VERTEX_BUFFER_LOCK_NEW );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_VERTEX_BUFFER_LOCK_SIZE );
    MENGINE_STATISTIC_DECLARE_END();
    //////////////////////////////////////////////////////////////////////////
#undef MENGINE_STATISTIC_DECLARE_BEGIN
#undef MENGINE_STATISTIC_DECLARE_END
#undef MENGINE_STATISTIC_DECLARE
    //////////////////////////////////////////////////////////////////////////
}