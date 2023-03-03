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
    MENGINE_STATISTIC_DECLARE( STATISTIC_CURRENT_SCENE_NAME );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_FRAME );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_PERFRAME_DRAWINDEXPRIMITIVES );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_PERFRAME_FILLRATE );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_PERFRAME_OBJECTS );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_PERFRAME_TRIANGLES );
    MENGINE_STATISTIC_DECLARE( STATISTIC_RENDER_PERFRAME_BATCHES );
    
    MENGINE_STATISTIC_DECLARE_END();
    //////////////////////////////////////////////////////////////////////////
#undef MENGINE_STATISTIC_DECLARE_BEGIN
#undef MENGINE_STATISTIC_DECLARE_END
#undef MENGINE_STATISTIC_DECLARE
    //////////////////////////////////////////////////////////////////////////
}