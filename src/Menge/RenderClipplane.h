#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Kernel/Node.h"

namespace Menge
{
	class Viewport;
	
	class RenderClipplane
		: public Node
        , public RenderClipplaneInterface
	{
	public:
		RenderClipplane();
		
	protected:
		bool _activate() override;
		
	public:
		void setPlanes( uint32_t _count, const mt::planef * _planes );
		void setViewport( const Viewport & _viewport );

	public:
		uint32_t getCount() const override;
		const mt::planef & getPlane( uint32_t _index ) const override;

	protected:
		void _invalidateWorldMatrix() override;

		void invalidateClipplanes_();
		void updateClipplanes_() const;

	protected:
		uint32_t m_count;
		mt::planef m_planes[6];

		mutable mt::planef m_planesWM[6];
		mutable bool m_invalidateClipplanes;		
	};
	//////////////////////////////////////////////////////////////////////////
	inline void RenderClipplane::invalidateClipplanes_()
	{
		m_invalidateClipplanes = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t RenderClipplane::getCount() const
	{
		return m_count;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::planef & RenderClipplane::getPlane( uint32_t _index ) const
	{
		if( m_invalidateClipplanes == true )
		{
			this->updateClipplanes_();
		}

		return m_planesWM[_index];
	}
}
