#pragma once

#include "MovieKeyFrameInterface.h"

#include "Kernel/Vector.h"

#include "math/vec2.h"
#include "math/vec3.h"

namespace Mengine
{
    class MovieFramePack
        : public MovieFramePackInterface
    {
    public:
        MovieFramePack();
        ~MovieFramePack() override;

    public:
        bool acquire() override;
        void release() override;

    public:
        void initializeLayers( uint32_t _size );
        void initializeTimeremap( uint32_t _size );
        void initializeShapes( uint32_t _size );
        void initializePolygons( uint32_t _size );

    public:
        MovieLayerFrame & setupLayer( uint32_t _layerIndex, uint32_t _count, bool _immutable );

    public:
        bool hasLayer( uint32_t _layerIndex ) const override;
        const MovieLayerFrame & getLayer( uint32_t _layerIndex ) const override;

    public:
        bool getLayerFrame( uint32_t _layerIndex, uint32_t _frameIndex, MovieFrameSource * const _frame ) const override;
        bool getLayerFrameInterpolate( uint32_t _layerIndex, uint32_t _frameIndex, float _t, MovieFrameSource * const _frame ) const override;

    public:
        bool getLayerTimeRemap( uint32_t _layerIndex, uint32_t _frameIndex, float * const _time ) const override;
        bool getLayerShape( uint32_t _layerIndex, uint32_t _frameIndex, const MovieFrameShape ** const _shape ) const override;
        bool getLayerPolygon( uint32_t _layerIndex, const mt::vec2f ** const _polygon, uint8_t * const _vertexCount ) const override;

    public:
        bool isLayerPermanentlyHide( uint32_t _layerIndex ) const override;

    public:
        Pointer allocateMemory( size_t _size ) const override;

    public:
        MovieLayerTimeRemap & mutableLayerTimeRemap( uint32_t _index );
        MovieLayerShapes & mutableLayerShape( uint32_t _index );
        MovieLayerPolygon & mutableLayerPolygon( uint32_t _index );

    protected:
        uint32_t m_sizeLayers;
        uint32_t m_sizeTimeremap;
        uint32_t m_sizeShapes;
        uint32_t m_sizePolygons;

        MovieLayerFrame * m_layers;
        MovieLayerTimeRemap * m_timeremap;
        MovieLayerShapes * m_shapes;
        MovieLayerPolygon * m_polygons;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MovieFramePack> MovieFramePackPtr;
    //////////////////////////////////////////////////////////////////////////
}