#pragma once

#include "Interface/ServiceInterface.h"

#include "math/vec3.h"

#ifndef MENGINE_LIGHTING3D_POINT_LIGHT_MAX
#define MENGINE_LIGHTING3D_POINT_LIGHT_MAX 4
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class Lighting3DServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Lighting3DService" )

    public:
        virtual void setAmbient( const mt::vec3f & _color ) = 0;
        virtual const mt::vec3f & getAmbient() const = 0;

    public:
        virtual void setDirectionalLight( const mt::vec3f & _dir, const mt::vec3f & _color, float _intensity ) = 0;
        virtual void clearDirectionalLight() = 0;
        virtual bool hasDirectionalLight() const = 0;

    public:
        virtual uint32_t acquirePointLight() = 0;
        virtual void releasePointLight( uint32_t _slot ) = 0;

        virtual void setPointLight( uint32_t _slot, const mt::vec3f & _pos, float _radius, const mt::vec3f & _color, float _intensity ) = 0;

    public:
        virtual void packUploadBlock( float * _dst ) const = 0;

    public:
        static constexpr uint32_t getUploadBlockVec4Count()
        {
            return 3u + 2u * MENGINE_LIGHTING3D_POINT_LIGHT_MAX;
        }
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define LIGHTING3D_SERVICE()\
    ((Mengine::Lighting3DServiceInterface *)SERVICE_GET(Mengine::Lighting3DServiceInterface))
//////////////////////////////////////////////////////////////////////////
