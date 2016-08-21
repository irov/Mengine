#	pragma once

#	include "Kernel/Surface.h"
#	include "Kernel/Resource.h"

#   include "ResourceAnimation.h"

namespace Menge
{
	class SurfaceImageSequence
		: public Surface
		, public Resource		
	{
	public:
		SurfaceImageSequence();
		~SurfaceImageSequence();

	public:
		void setResourceAnimation( const ResourceAnimationPtr & _resourceAnimation );
		const ResourceAnimationPtr & getResourceAnimation() const;

	public:
		uint32_t getFrameCount() const;
		
		float getFrameDelay( uint32_t  _frame ) const;		

		void setCurrentFrame( uint32_t _frame );
		uint32_t getCurrentFrame() const;

	protected:
		void _setTiming( float _timming ) override;
		float _getTiming() const override;

		void _setFirstFrame() override;
		void _setLastFrame() override;

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, uint32_t _enumerator ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( float _time, uint32_t _enumerator ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	protected:
		void _update( float _current, float _timing ) override;

		bool _compile() override;
		void _release() override;

		void _setEventListener( const pybind::dict & _listener ) override;

	protected:
		uint32_t getFrame_( float _timing, float & _delthaTiming ) const;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const;

	protected:
		ResourceHolder<ResourceAnimation> m_resourceAnimation;

		float m_frameTiming;

		uint32_t m_currentFrame;
	};
}
