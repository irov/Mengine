#pragma once

#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ECellType
    {
        ECellType_EMPTY,
        ECellType_HORIZONTAL_LEFT,
        ECellType_HORIZONTAL_RIGHT,
        ECellType_VERTICAL_UP,
        ECellType_VERTICAL_DOWN,
        ECellType_PLAYER_V_UP,
        ECellType_PLAYER_V_DOWN,
    };
    //////////////////////////////////////////////////////////////////////////
    class TrafficMap
    {
    public:
        TrafficMap();
        ~TrafficMap();

    public:
        void clearMap();
        void initMap();
        void setCell( uint32_t _x, uint32_t _y, ECellType _cellType );
        ECellType getCell( int32_t _x, int32_t _y ) const;
    protected:
        typedef Vector<ECellType> VectorECellType;
        VectorECellType m_trafficMap;
    };
};