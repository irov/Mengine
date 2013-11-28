#	include "HotspotMousePickerAdapter.h"

#   include "Interface/PlayerInterface.h"

#   include "HotspotMousePickerVisitor.h"

#   include "HotSpot.h"
#   include "Camera2D.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HotspotMousePickerAdapter::HotspotMousePickerAdapter()
		: m_hotspot(nullptr)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	void HotspotMousePickerAdapter::setHotspot( HotSpot * _hotspot )
	{
        m_hotspot = _hotspot;
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot * HotspotMousePickerAdapter::getHotspot() const
	{
		return m_hotspot;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotspotMousePickerAdapter::pick( const mt::vec2f& _point, const RenderCameraInterface * _camera, Arrow * _arrow )
	{
		if( m_hotspot->isActivate() == false )
		{
			return false;
		}

        Layer * layer = m_hotspot->getLayer();

        if( layer == nullptr )
        {
            return false;
        }

        HotspotMousePickerVisitor mp(m_hotspot, _camera, _point, _arrow);

        bool result = mp.test( layer );

        return result;
	}
	//////////////////////////////////////////////////////////////////////////
    bool HotspotMousePickerAdapter::onMouseEnter( const mt::vec2f & _point )
    {
        uint32 color = m_hotspot->getDebugColor();

        color &= 0xFF000000;
        color |= 0x00FF0000;

        m_hotspot->setDebugColor( color );

        bool handle = MousePickerAdapter::onMouseEnter( _point );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotspotMousePickerAdapter::onMouseLeave()
    {
        uint32 color = m_hotspot->getDebugColor();

        color &= 0xFF000000;
        color |= 0x00FFFFFF;

        m_hotspot->setDebugColor( color );

        MousePickerAdapter::onMouseLeave();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * HotspotMousePickerAdapter::getPickerEmbed()
    {
        PyObject * embed = m_hotspot->getEmbed();

        return embed;
    }
    //////////////////////////////////////////////////////////////////////////
    Eventable * HotspotMousePickerAdapter::getPickerEventable()
    {
        Eventable * eventable = static_cast<Eventable *>(m_hotspot);

        return eventable;
    }
}
