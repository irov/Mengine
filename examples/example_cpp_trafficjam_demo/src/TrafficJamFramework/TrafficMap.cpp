#include "TrafficMap.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static MENGINE_CONSTEXPR uint32_t TRAFFIC_MAP_SIZE = 6;
    static MENGINE_CONSTEXPR uint32_t MAP_SIZE = 36;
    //////////////////////////////////////////////////////////////////////////
    TrafficMap::TrafficMap()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TrafficMap::~TrafficMap()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficMap::clearMap()
    {
        m_trafficMap.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficMap::initMap()
    {
        for( uint32_t i = 0; i != MAP_SIZE; i++ )
        {
            m_trafficMap.push_back( ECellType::ECellType_EMPTY );
        }

        m_trafficMap.shrink_to_fit();
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficMap::setCell( uint32_t _x, uint32_t _y, ECellType _cellType )
    {
        uint32_t index = _y * TRAFFIC_MAP_SIZE + _x;

        m_trafficMap[index] = _cellType;
    }
    //////////////////////////////////////////////////////////////////////////
    ECellType TrafficMap::getCell( int32_t _x, int32_t _y ) const
    {
        if( _x < 0 || _y < 0 )
        {
            return ECellType::ECellType_HORIZONTAL_LEFT;
        }
        uint32_t index = _y * TRAFFIC_MAP_SIZE + _x;
        ECellType returnValue = m_trafficMap[index];

        return  returnValue;
    }
};