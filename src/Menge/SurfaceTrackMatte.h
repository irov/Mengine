#	pragma once

#	include "Kernel/Surface.h"
#	include "Kernel/Resource.h"

namespace Menge
{
	class SurfaceTrackMatte
		: public Surface
		, public Resource
	{
	public:
		SurfaceTrackMatte();
		~SurfaceTrackMatte();

	public:
		void setResourceImage( const ResourceImagePtr & _resourceImage );
		const ResourceImagePtr & getResourceImage() const;

	public:
		void setResourceTrackMatteImage( const ResourceImagePtr & _resourceTrackMatteImage );
		const ResourceImagePtr & getResourceTrackMatteImage() const;
	
	protected:
		void _update( float _current, float _timing ) override;

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, uint32_t _enumerator ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( float _time, uint32_t _enumerator ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const override;

	protected:
		ResourceHolder<ResourceImage> m_resourceImage;
		ResourceHolder<ResourceImage> m_resourceTrackMatteImage;
	};
}
