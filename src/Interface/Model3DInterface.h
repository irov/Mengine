#pragma once

#include "Interface/DataInterface.h"

#include "Config/Floats.h"

#include "Kernel/Magic.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/RenderIndex.h"

#include "Kernel/Factorable.h"

#include "math/vec3.h"

#include "Config/Vector.h"

namespace Mengine
{
    DECLARE_MAGIC_NUMBER( MAGIC_MDL, 'M', 'D', 'L', '1', 2 );

    struct Model3DFrame
    {
        mt::vec3f cameraPosition;
        mt::vec3f cameraDirection;
        mt::vec3f cameraUp;

        mt::vec3f * position;
        mt::vec2f * uv;
        RenderIndex * indecies;
    };

    class Model3DInterface
        : public DataInterface
    {
    public:
        virtual uint32_t getFrameCount() const = 0;
        virtual uint32_t getVertexCount() const = 0;
        virtual uint32_t getIndicesCount() const = 0;
        virtual float getCameraFOV() const = 0;
        virtual float getCameraAspect() const = 0;
        virtual float getCameraRightSign() const = 0;

    public:
        virtual float getFrameDelay() const = 0;

    public:
        virtual const Model3DFrame * getFrame( uint32_t _frameId ) const = 0;
    };

    typedef IntrusivePtr<Model3DInterface> Model3DInterfacePtr;
}