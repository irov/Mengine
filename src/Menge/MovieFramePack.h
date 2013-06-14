#	pragma once

#   include "Interface/MovieKeyFrameInterface.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"

#	include <vector>

#	include <memory.h>

namespace Menge
{
	class MovieFramePack
        : public MovieFramePackInterface
	{
	public:
		MovieFramePack( size_t _size );

	public:
		void initializeLayer( size_t _layerIndex, size_t _count, bool _immutable );

	public:
        bool hasLayer( size_t _layerIndex ) const override;
		const MovieLayerFrame & getLayer( size_t _layerIndex ) const override;

	public:
		bool getLayerFrame( size_t _layerIndex, size_t _frameIndex, MovieFrameSource & _frame ) const override;
		bool getLayerFrameLast( size_t _layerIndex, MovieFrameSource & _frame ) const;

		bool getLayerImmutableFrame( size_t _layerIndex, MovieFrameSource & _frame ) const;

	public:
		bool isLayerEmpty( size_t _layerIndex ) const;
		bool isLayerImmutable( size_t _layerIndex ) const;

	public:
		void setLayerImmutableFrame( size_t _layerIndex, const MovieFrameSource & _frame );
		void addLayerFrame( size_t _layerIndex, const MovieFrameSource & _frame );
			

    public:
        const Polygon * getPolygon( size_t _index ) const override;

    public:
        size_t addPolygon( const Polygon & _polygon );

	protected:
		size_t m_size;
		TVectorMovieFrameLayer m_layers;

        typedef std::vector<Polygon> TVectorPolygons;
        TVectorPolygons m_polygons;
	};
}