#pragma once

#include "Kernel/Surface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	class SurfaceSolidColor
		: public Surface        
	{
	public:
        SurfaceSolidColor();
		~SurfaceSolidColor() override;

	public:
		void setSolidColor( const ColourValue & _color );
		const ColourValue & getSolidColor() const;

        void setSolidSize( const mt::vec2f & _size );
        const mt::vec2f & getSolidSize() const;        

    public:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        const ColourValue & getColor() const override;

        void correctUV( uint32_t _index, mt::vec2f & _out, const mt::vec2f & _in ) override;

	protected:
		bool _update( float _current, float _time ) override;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const override;

	protected:
		ColourValue m_color;
        mt::vec2f m_size;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SurfaceSolidColor> SurfaceSolidColorPtr;
}
