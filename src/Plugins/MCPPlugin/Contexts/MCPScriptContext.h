#pragma once

#include "Kernel/Map.h"
#include "Kernel/Set.h"
#include "Kernel/String.h"

#include "jpp/jpp.hpp"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "pybind/object.hpp"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPScriptContext
    {
    public:
        struct SourceDesc
        {
            String logicalPath;
            String source;
        };

    public:
        MCPScriptContext();

        void finalize();
        size_t getHandleCount() const;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        bool jsonToScript( const jpp::object & _value, pybind::object * const _result, String * const _error ) const;
        bool scriptToJSON( const pybind::object & _value, uint32_t _maxDepth, uint32_t _maxItems, uint32_t _start, jpp::object * const _result, String * const _error );
        bool resolveTarget( const jpp::object & _target, pybind::object * const _result, String * const _error ) const;
        bool resolveHandle( uint64_t _handle, pybind::object * const _result, String * const _error ) const;
        bool retainObject( const pybind::object & _value, uint64_t * const _handle, String * const _error );
        PyObject * getScope( const Char * _scope, const Char * _module, String * const _error ) const;

        void setSource( const String & _module, SourceDesc && _source );
        const SourceDesc * findSource( const String & _module ) const;
        uint32_t releaseHandles( const jpp::array & _handles );

    protected:
        bool scriptToJSON_( const pybind::object & _value, uint32_t _depth, uint32_t _maxDepth, uint32_t _maxItems, uint32_t _start, Set<PyObject *> * const _visited, jpp::object * const _result, String * const _error );
        uint64_t retainHandle_( const pybind::object & _value, String * const _error );

    protected:
        Map<uint64_t, pybind::object> m_handles;
        Map<String, SourceDesc> m_sources;
        mutable pybind::object m_globals;
        uint64_t m_nextHandle;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
}
