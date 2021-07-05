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
        MENGINE_ASSERTION_FATAL( jpp::once( m_json ) == true );
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONStorage::setJSON( const jpp::object & _json )
    {
        m_json = _json;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object JSONStorage::modifyJSON()
    {
        return m_json;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object JSONStorage::copyJSON() const
    {
        jpp::object j = jpp::copy( m_json );

        return j;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONStorage::mergeJSON( const jpp::object & _json, bool _copy, bool _recurive, bool _mergeArray )
    {
        jpp::merge_mode_e mode = _mergeArray == false ? jpp::merge_mode_e::update : jpp::merge_mode_e::update_with_array;

        bool successful = jpp::merge( m_json, _json, _copy, _recurive, mode );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}