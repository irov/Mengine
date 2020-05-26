#pragma once

#include "Config/Typedef.h"

#include "Interface/DataInterface.h"

#include "Kernel/Magic.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/RenderIndex.h"

#include "Kernel/Factorable.h"

#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DECLARE_MAGIC_NUMBER( MAGIC_AEK, 'A', 'E', 'K', '1', 37 );
    //////////////////////////////////////////////////////////////////////////
    const uint32_t MOVIE_KEY_FRAME_FORMAT_VERSION = 1;
    //////////////////////////////////////////////////////////////////////////
    struct MovieFrameSource
    {
        mt::vec3f anchorPoint;
        mt::vec3f position;
        mt::vec3f rotation;
        mt::vec3f scale;
        float opacity;
        float volume;
    };
    //////////////////////////////////////////////////////////////////////////
    const uint32_t MOVIE_KEY_FRAME_IMMUTABLE_ANCHOR_POINT = 0x00000001;
    const uint32_t MOVIE_KEY_FRAME_IMMUTABLE_POSITION = 0x00000002;
    const uint32_t MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_X = 0x00000004;
    const uint32_t MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_Y = 0x00000008;
    const uint32_t MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_Z = 0x00000010;
    const uint32_t MOVIE_KEY_FRAME_IMMUTABLE_SCALE = 0x00000020;
    const uint32_t MOVIE_KEY_FRAME_IMMUTABLE_OPACITY = 0x00000040;
    const uint32_t MOVIE_KEY_FRAME_IMMUTABLE_VOLUME = 0x00000080;
    //////////////////////////////////////////////////////////////////////////
    struct MovieLayerFrame
    {
        uint32_t count;

        mt::vec3f * anchorPoint;
        mt::vec3f * position;
        mt::vec3f * scale;
        float * rotation_x;
        float * rotation_y;
        float * rotation_z;
        float * opacity;
        float * volume;

        uint32_t immutable_mask;

        MovieFrameSource source;
        bool immutable;
    };
    //////////////////////////////////////////////////////////////////////////
    struct MovieLayerTimeRemap
    {
        uint32_t layerId;

        float * times;
    };
    //////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MOVIE_SHAPE_MAX_VERTEX
#define MENGINE_MOVIE_SHAPE_MAX_VERTEX 128
#endif
    //////////////////////////////////////////////////////////////////////////
#	define MENGINE_MOVIE_SHAPE_MAX_INDICES ((MENGINE_MOVIE_SHAPE_MAX_VERTEX - 2) * 3)
    //////////////////////////////////////////////////////////////////////////
    struct MovieFrameShape
    {
        mt::vec2f * pos;
        mt::vec2f * uv;
        RenderIndex * indices;

        uint16_t vertexCount;
        uint16_t indexCount;
    };
    //////////////////////////////////////////////////////////////////////////
    struct MovieLayerShapes
    {
        uint32_t layerId;

        uint32_t shapes_size;
        MovieFrameShape * shapes;
    };
    //////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_MOVIE_POLYGON_MAX_VERTEX
#	define MENGINE_MOVIE_POLYGON_MAX_VERTEX 128
#endif
    //////////////////////////////////////////////////////////////////////////
    struct MovieLayerPolygon
    {
        uint32_t layerId;

        mt::vec2f * polygon;
        uint8_t vertexCount;
    };
    //////////////////////////////////////////////////////////////////////////
    class MovieFramePackInterface
        : public DataInterface
    {
    public:
        virtual bool hasLayer( uint32_t _layerIndex ) const = 0;
        virtual const MovieLayerFrame & getLayer( uint32_t _layerIndex ) const = 0;

    public:
        virtual bool getLayerFrame( uint32_t _layerIndex, uint32_t _frameIndex, MovieFrameSource * _frame ) const = 0;
        virtual bool getLayerFrameInterpolate( uint32_t _layerIndex, uint32_t _frameIndex, float _t, MovieFrameSource * _frame ) const = 0;

    public:
        virtual bool getLayerTimeRemap( uint32_t _layerIndex, uint32_t _frameIndex, float * _time ) const = 0;
        virtual bool getLayerShape( uint32_t _layerIndex, uint32_t _frameIndex, const MovieFrameShape ** _shape ) const = 0;
        virtual bool getLayerPolygon( uint32_t _layerIndex, const mt::vec2f ** _polygon, uint8_t * _vertexCount ) const = 0;

    public:
        virtual bool isLayerPermanentlyHide( uint32_t _layerIndex ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MovieFramePackInterface> MovieFramePackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}