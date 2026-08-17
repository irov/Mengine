#include "TiledMapScriptEmbedding.h"

#include "ResourceTiledMap.h"
#include "TiledMap2D.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TiledMapScriptEmbedding::TiledMapScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TiledMapScriptEmbedding::~TiledMapScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMapScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::enum_<ETiledMapOrientation>( _kernel, "ETiledMapOrientation" )
            .def( "ETMO_ORTHOGONAL", ETMO_ORTHOGONAL )
            .def( "ETMO_ISOMETRIC", ETMO_ISOMETRIC )
            .def( "ETMO_HEXAGONAL", ETMO_HEXAGONAL )
            ;

        pybind::enum_<ETiledMapObjectShape>( _kernel, "ETiledMapObjectShape" )
            .def( "ETMOS_RECTANGLE", ETMOS_RECTANGLE )
            .def( "ETMOS_ELLIPSE", ETMOS_ELLIPSE )
            .def( "ETMOS_POINT", ETMOS_POINT )
            .def( "ETMOS_POLYGON", ETMOS_POLYGON )
            .def( "ETMOS_POLYLINE", ETMOS_POLYLINE )
            .def( "ETMOS_TILE", ETMOS_TILE )
            .def( "ETMOS_TEXT", ETMOS_TEXT )
            ;

        pybind::interface_<ResourceTiledMap, pybind::bases<Resource>>( _kernel, "ResourceTiledMap", false )
            ;

        pybind::interface_<TiledMap2D, pybind::bases<Node>>( _kernel, "TiledMap2D", false )
            .def( "setResourceTiledMap", &TiledMap2D::setResourceTiledMap )
            .def( "getResourceTiledMap", &TiledMap2D::getResourceTiledMap )
            .def( "setMaterialName", &TiledMap2D::setMaterialName )
            .def( "getMaterialName", &TiledMap2D::getMaterialName )
            .def( "getOrientation", &TiledMap2D::getOrientation )
            .def( "getColumnCount", &TiledMap2D::getColumnCount )
            .def( "getRowCount", &TiledMap2D::getRowCount )
            .def( "getTileWidth", &TiledMap2D::getTileWidth )
            .def( "getTileHeight", &TiledMap2D::getTileHeight )
            .def( "getTileCount", &TiledMap2D::getTileCount )
            .def( "getTileLayerCount", &TiledMap2D::getTileLayerCount )
            .def( "getObjectLayerCount", &TiledMap2D::getObjectLayerCount )
            .def( "getTileLayerName", &TiledMap2D::getTileLayerName )
            .def( "getObjectLayerName", &TiledMap2D::getObjectLayerName )
            .def( "setTileLayerVisible", &TiledMap2D::setTileLayerVisible )
            .def( "isTileLayerVisible", &TiledMap2D::isTileLayerVisible )
            .def( "getObjectCount", &TiledMap2D::getObjectCount )
            .def( "getObjectId", &TiledMap2D::getObjectId )
            .def( "getObjectName", &TiledMap2D::getObjectName )
            .def( "getObjectType", &TiledMap2D::getObjectType )
            .def( "getObjectClass", &TiledMap2D::getObjectClass )
            .def( "getObjectShape", &TiledMap2D::getObjectShape )
            .def( "getObjectPosition", &TiledMap2D::getObjectPosition )
            .def( "getObjectSize", &TiledMap2D::getObjectSize )
            .def( "getObjectRotation", &TiledMap2D::getObjectRotation )
            .def( "getObjectPointCount", &TiledMap2D::getObjectPointCount )
            .def( "getObjectPoint", &TiledMap2D::getObjectPoint )
            .def( "getMapProperties", &TiledMap2D::getMapProperties )
            .def( "getTileLayerProperties", &TiledMap2D::getTileLayerProperties )
            .def( "getObjectLayerProperties", &TiledMap2D::getObjectLayerProperties )
            .def( "getObjectProperties", &TiledMap2D::getObjectProperties )
            .def( "getBatchCount", &TiledMap2D::getBatchCount )
            .def( "getVertexCount", &TiledMap2D::getVertexCount )
            .def( "getIndexCount", &TiledMap2D::getIndexCount )
            .def( "getResidentTextureMemoryBytes", &TiledMap2D::getResidentTextureMemoryBytes )
            ;

        Helper::registerScriptWrapping<ResourceTiledMap>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<TiledMap2D>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMapScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "ETMO_ORTHOGONAL", nullptr );
        _kernel->remove_from_module( "ETMO_ISOMETRIC", nullptr );
        _kernel->remove_from_module( "ETMO_HEXAGONAL", nullptr );
        _kernel->remove_from_module( "ETMOS_RECTANGLE", nullptr );
        _kernel->remove_from_module( "ETMOS_ELLIPSE", nullptr );
        _kernel->remove_from_module( "ETMOS_POINT", nullptr );
        _kernel->remove_from_module( "ETMOS_POLYGON", nullptr );
        _kernel->remove_from_module( "ETMOS_POLYLINE", nullptr );
        _kernel->remove_from_module( "ETMOS_TILE", nullptr );
        _kernel->remove_from_module( "ETMOS_TEXT", nullptr );

        _kernel->remove_scope<TiledMap2D>();
        _kernel->remove_scope<ResourceTiledMap>();

        Helper::unregisterScriptWrapping<TiledMap2D>();
        Helper::unregisterScriptWrapping<ResourceTiledMap>();
    }
    //////////////////////////////////////////////////////////////////////////
}
