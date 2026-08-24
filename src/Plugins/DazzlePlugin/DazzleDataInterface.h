#pragma once

#include "Interface/DataInterface.h"

#include "DazzleIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DazzleDataInterface
        : public DataInterface
    {
    public:
        virtual const dz_effect_t * getDazzleEffect() const = 0;

    public:
        virtual dz_result_t setDazzleEmitterTexture( const dz_shape_mask_source_t * _source ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DazzleDataInterface, DataInterface> DazzleDataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
