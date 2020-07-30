#include "TrafficMap.h"

namespace Mengine
{
    const uint32_t TrafficMapSize = 6;
    const uint32_t mapSize = 36;

    TrafficMap::TrafficMap()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TrafficMap::~TrafficMap()
    {
    }
    void TrafficMap::clearMap()
    {
        m_trafficMap.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void        TrafficMap::initMap()
    {
        for( uint32_t i = 0; i != mapSize; i++ )
        {
            m_trafficMap.push_back( ECellType::ECellType_EMPTY );
        }

        m_trafficMap.shrink_to_fit();
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficMap::setCell( uint32_t _x, uint32_t _y, ECellType _cellType )
    {
        uint32_t index = _y * TrafficMapSize + _x;

        m_trafficMap[index] = _cellType;
    }
    //////////////////////////////////////////////////////////////////////////
    ECellType TrafficMap::getCell( int32_t _x, int32_t _y ) const
    {
        if( _x < 0 || _y < 0 )
        {
            return ECellType::ECellType_HORIZONTAL_LEFT;
        }
        uint32_t index = _y * TrafficMapSize + _x;
        ECellType returnValue = m_trafficMap[index];

        return  returnValue;
    }
};