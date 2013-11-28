#	pragma once

#   include "MousePickerAdapter.h"

namespace Menge
{
    class HotSpot;

	class HotspotMousePickerAdapter
		: public MousePickerAdapter
	{
	public:
		HotspotMousePickerAdapter();

    public:
        void setHotspot( HotSpot * _hotspot );
		HotSpot * getHotspot() const;

    public:
        bool pick( const mt::vec2f& _point, const RenderCameraInterface * _camera, Arrow * _arrow ) override;

    public:
        bool onMouseEnter( const mt::vec2f & _point ) override;
        void onMouseLeave() override;

    public:
        PyObject * getPickerEmbed() override;

    public:
        Eventable * getPickerEventable() override;  

	protected:       
		HotSpot * m_hotspot;
	};
}
