#	pragma once

#	include "Interface/MovieKeyFrameInterface.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"

#	include <vector>

#	include <stdint.h>
#	include <memory.h>

namespace Menge
{       
	class MovieFramePack
		: public MovieFramePackInterface
	{
	public:
		MovieFramePack();
		~MovieFramePack();
        
    public:
        void initialize( size_t _size );

	public:
		MovieLayerFrame & initializeLayer( size_t _layerIndex, size_t _count, bool _immutable );

	public:
        bool hasLayer( size_t _layerIndex ) const override;
		const MovieLayerFrame & getLayer( size_t _layerIndex ) const override;

	public:
		bool getLayerFrame( size_t _layerIndex, size_t _frameIndex, MovieFrameSource & _frame ) const override;
		bool getLayerFrameInterpolate( size_t _layerIndex, size_t _frameIndex, float _t, MovieFrameSource & _frame ) const override;

    public:
        bool getLayerTimeRemap( size_t _layerIndex, size_t _frameIndex, float & _time ) const override;

	public:
		bool getLayerShape( size_t _layerIndex, size_t _frameIndex, const MovieFrameShape ** _shape ) const override;

	public:
        void addLayerTimeRemap( size_t _layerIndex, const MovieLayerTimeRemap & _timeremap );
		MovieLayerTimeRemap & mutableLayerTimeRemap( size_t _layerIndex );

	public:
		void addLayerShape( size_t _layerIndex, const MovieLayerShapes & _shapes );
		MovieLayerShapes & mutableLayerShape( size_t _layerIndex );

	public:
		inline const TVectorMovieFrameLayer & getLayers() const;
		inline const TVectorMovieLayerTimeRemap & getTimeremap() const;
		inline const TVectorMovieLayerShapes & getShapes() const;
			
	protected:
		TVectorMovieFrameLayer m_layers;
        TVectorMovieLayerTimeRemap m_timeremap;
		TVectorMovieLayerShapes m_shapes;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const TVectorMovieFrameLayer & MovieFramePack::getLayers() const
	{
		return m_layers;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const TVectorMovieLayerTimeRemap & MovieFramePack::getTimeremap() const
	{
		return m_timeremap;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const TVectorMovieLayerShapes & MovieFramePack::getShapes() const
	{
		return m_shapes;
	}
}