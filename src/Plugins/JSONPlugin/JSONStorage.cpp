#include "JSONStorage.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONStorage::JSONStorage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorage::~JSONStorage()
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    void JSONStorage::setJSON( const jpp::object & _json )
    {
        m_json = _json;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONStorage::mergeJSON( const jpp::object & _json, bool _mergeArray )
    {
        jpp::merge_mode_e mode = _mergeArray == false ? jpp::merge_mode_e::update : jpp::merge_mode_e::update_with_array;

        bool successful = jpp::merge( m_json, _json, mode );

        return successful;
    }
}