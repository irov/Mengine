#	pragma once

#	include "Kernel/Surface.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	class SurfaceSolidColor
		: public Surface        
	{
	public:
        SurfaceSolidColor();
		~SurfaceSolidColor();

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

        const ColourValue & getColour() const override;

	protected:
		bool update( float _current, float _timing ) override;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const;

	protected:
		ColourValue m_color;
        mt::vec2f m_size;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<SurfaceSolidColor> SurfaceSolidColorPtr;
}
