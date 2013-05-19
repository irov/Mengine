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
	bool HotspotMousePickerAdapter::pick( const mt::vec2f& _point, Arrow * _arrow )
	{
        Layer * layer = m_hotspot->getLayer();

        if( layer == nullptr )
        {
            return false;
        }

        Camera2D * camera = PLAYER_SERVICE(m_serviceProvider)
            ->getCamera2D();

        HotspotMousePickerVisitor mp(m_hotspot, camera, _point, _arrow);

        bool result = mp.test( layer );

        return result;
	}
	//////////////////////////////////////////////////////////////////////////
    bool HotspotMousePickerAdapter::onMouseEnter()
    {
        uint32 color = m_hotspot->getDebugColor();

        color &= 0xFF000000;
        color |= 0x00FF0000;

        m_hotspot->setDebugColor( color );

        bool handle = MousePickerAdapter::onMouseEnter();

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
    //////////////////////////////////////////////////////////////////////////
	bool HotspotMousePickerAdapter::isPickerActive() const
	{
        if( m_hotspot->isFreeze() == true )
        {
            return false;
        }

        if( m_hotspot->isActivate() == false )
        {
            return false;
        }

        return true;
	}
}
