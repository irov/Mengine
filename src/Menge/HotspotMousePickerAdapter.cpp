#	include "HotspotMousePickerAdapter.h"

#	include "Interface/ApplicationInterface.h"

#   include "HotspotMousePickerVisitor.h"

#   include "HotSpot.h"

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
	bool HotspotMousePickerAdapter::pick( const mt::vec2f& _point, const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, Arrow * _arrow )
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

		HotspotMousePickerVisitor mp(m_hotspot, _viewport, _camera, _point, _arrow);
        bool result = mp.test( layer );

        return result;
	}
	//////////////////////////////////////////////////////////////////////////
    bool HotspotMousePickerAdapter::onMouseEnter( const mt::vec2f & _point )
    {
        m_hotspot->onPickerEnter();

        bool handle = MousePickerAdapter::onMouseEnter( _point );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotspotMousePickerAdapter::onMouseLeave()
    {
        m_hotspot->onPickerLeave();

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
