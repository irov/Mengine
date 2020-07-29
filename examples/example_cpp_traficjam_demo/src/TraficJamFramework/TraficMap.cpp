#include "TraficMap.h"

namespace Mengine
{
    const uint32_t traficMapSize = 6;
    const uint32_t mapSize = 36;

    TraficMap::TraficMap()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TraficMap::~TraficMap()
    {
    }
    void TraficMap::clearMap()
    {
        traficMap.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void        TraficMap::initMap()
    {
        for( uint32_t i = 0; i != mapSize; i++ )
        {
            traficMap.push_back( ECellType::ECellType_EMPTY );
        }

        traficMap.shrink_to_fit();
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficMap::setCell( uint32_t _x, uint32_t _y, ECellType _cellType )
    {
        uint32_t index = _y * traficMapSize + _x;

        traficMap[index] = _cellType;
    }
    //////////////////////////////////////////////////////////////////////////
    ECellType TraficMap::getCell( int32_t _x, int32_t _y ) const
    {
        uint32_t index = _y * traficMapSize + _x;
        ECellType returnValue = traficMap[index];

        return  returnValue;
    }
};