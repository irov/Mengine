#pragma once

#include "Kernel/Resource.h"
#include "Kernel/Params.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ETiledMapOrientation
    {
        ETMO_ORTHOGONAL,
        ETMO_ISOMETRIC,
        ETMO_HEXAGONAL
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETiledMapObjectShape
    {
        ETMOS_RECTANGLE,
        ETMOS_ELLIPSE,
        ETMOS_POINT,
        ETMOS_POLYGON,
        ETMOS_POLYLINE,
        ETMOS_TILE,
        ETMOS_TEXT
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETiledMapPropertyType
    {
        ETMPT_STRING,
        ETMPT_BOOL,
        ETMPT_INTEGER,
        ETMPT_FLOAT,
        ETMPT_COLOR,
        ETMPT_FILE,
        ETMPT_OBJECT
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Params TiledMapProperties;
    typedef Vector<mt::vec2f> VectorTiledMapPoints;
    //////////////////////////////////////////////////////////////////////////
    struct TiledMapObject
    {
        uint32_t id;
        String name;
        String type;
        String className;
        ETiledMapObjectShape shape;
        bool visible;
        mt::vec2f position;
        mt::vec2f size;
        float rotation;
        uint32_t gid;
        VectorTiledMapPoints points;
        TiledMapProperties properties;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TiledMapObject> VectorTiledMapObjects;
    //////////////////////////////////////////////////////////////////////////
    struct TiledMapLayer
    {
        uint32_t id;
        String name;
        bool visible;
        float opacity;
        mt::vec2f offset;
        TiledMapProperties properties;
        VectorTiledMapObjects objects;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TiledMapLayer> VectorTiledMapLayers;
    //////////////////////////////////////////////////////////////////////////
    struct TiledMapTile
    {
        int32_t x;
        int32_t y;
        uint32_t gid;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TiledMapTile> VectorTiledMapTiles;
    //////////////////////////////////////////////////////////////////////////
    struct TiledMapTileLayer
        : public TiledMapLayer
    {
        VectorTiledMapTiles tiles;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TiledMapTileLayer> VectorTiledMapTileLayers;
    //////////////////////////////////////////////////////////////////////////
    struct TiledMapTileImage
    {
        uint32_t localId;
        String resourceName;
        uint32_t imageWidth;
        uint32_t imageHeight;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TiledMapTileImage> VectorTiledMapTileImages;
    //////////////////////////////////////////////////////////////////////////
    struct TiledMapTileset
    {
        uint32_t firstGid;
        uint32_t tileCount;
        uint32_t columns;
        uint32_t tileWidth;
        uint32_t tileHeight;
        uint32_t margin;
        uint32_t spacing;
        uint32_t imageWidth;
        uint32_t imageHeight;
        mt::vec2f offset;
        String name;
        String resourceName;
        VectorTiledMapTileImages tileImages;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TiledMapTileset> VectorTiledMapTilesets;
    //////////////////////////////////////////////////////////////////////////
    class ResourceTiledMap
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceTiledMap );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceTiledMap();
        ~ResourceTiledMap() override;

    public:
        ETiledMapOrientation getOrientation() const;
        uint32_t getColumnCount() const;
        uint32_t getRowCount() const;
        uint32_t getTileWidth() const;
        uint32_t getTileHeight() const;
        uint32_t getHexSideLength() const;
        uint32_t getStaggerAxis() const;
        uint32_t getStaggerIndex() const;
        uint32_t getTileCount() const;

        const VectorTiledMapTilesets & getTilesets() const;
        const VectorTiledMapTileLayers & getTileLayers() const;
        const VectorTiledMapLayers & getObjectLayers() const;
        const TiledMapProperties & getMapProperties() const;

        const TiledMapObject * getObject( uint32_t _layerIndex, uint32_t _objectIndex ) const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void clearData_();

    protected:
        VectorTiledMapTilesets m_tilesets;
        VectorTiledMapTileLayers m_tileLayers;
        VectorTiledMapLayers m_objectLayers;
        TiledMapProperties m_mapProperties;
        ETiledMapOrientation m_orientation;
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_tileWidth;
        uint32_t m_tileHeight;
        uint32_t m_hexSideLength;
        uint32_t m_staggerAxis;
        uint32_t m_staggerIndex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceTiledMap> ResourceTiledMapPtr;
    //////////////////////////////////////////////////////////////////////////
}
