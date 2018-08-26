#pragma once

#include "Interface/COMInterface.h"

#include "math/vec2.h"
#include "math/uv4.h"
#include "Kernel/String.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class COMSurfaceVideoIterface
        : public COMInterface
    {
    public:
        //virtual void setResourceVideo(const ResourceVideoPtr & _resourceVideo) = 0;
        //virtual const ResourceVideoPtr & getResourceVideo() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class COMResourceImageDefaultInterface
        : public COMInterface
    {
    public:
        virtual void setup( const FilePath & _imagePath, const ConstString & _codecType, const mt::uv4f & _uv_image, const mt::uv4f & _uv_alpha, const mt::vec2f & _maxSize ) = 0;
    };
}