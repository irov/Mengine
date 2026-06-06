#include "MetalRenderProgramVariableDynamic.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"
#include "Config/StdAlgorithm.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        static void packVariables( const Vector<MetalRenderProgramVariableDynamic::ProgramVariableDesc> & _variables, Vector<float> & _container )
        {
            _container.clear();

            for( const MetalRenderProgramVariableDynamic::ProgramVariableDesc & v : _variables )
            {
                if( v.count == 0 )
                {
                    continue;
                }

                _container.insert( _container.end(), v.data.begin(), v.data.end() );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderProgramVariableDynamic::MetalRenderProgramVariableDynamic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderProgramVariableDynamic::~MetalRenderProgramVariableDynamic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderProgramVariableDynamic::initialize( uint32_t _vertexCount, uint32_t _pixelCount )
    {
        m_vertexVariables.resize( _vertexCount );
        m_pixelVariables.resize( _pixelCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgramVariableDynamic::finalize()
    {
        m_vertexVariables.clear();
        m_pixelVariables.clear();

        m_dataFloats.clear();
        m_pixelFloats.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgramVariableDynamic::setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        if( _index >= (uint32_t)m_vertexVariables.size() )
        {
            m_vertexVariables.resize( _index + 1 );
        }

        ProgramVariableDesc & v = m_vertexVariables[_index];
        StdString::strcpy_safe( v.uniform, _uniform, 32 );
        v.size = _size;
        v.count = _count;
        v.data.assign( _values, _values + _size * _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgramVariableDynamic::setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        if( _index >= (uint32_t)m_pixelVariables.size() )
        {
            m_pixelVariables.resize( _index + 1 );
        }

        ProgramVariableDesc & v = m_pixelVariables[_index];
        StdString::strcpy_safe( v.uniform, _uniform, 32 );
        v.size = _size;
        v.count = _count;
        v.data.assign( _values, _values + _size * _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgramVariableDynamic::updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        if( _index >= (uint32_t)m_pixelVariables.size() )
        {
            return;
        }

        ProgramVariableDesc & v = m_pixelVariables[_index];

        MENGINE_UNUSED( _size );
        MENGINE_UNUSED( _count );

        StdAlgorithm::copy( _values, _values + v.size * v.count, v.data.data() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderProgramVariableDynamic::apply( id<MTLRenderCommandEncoder> _encoder )
    {
        Detail::packVariables( m_vertexVariables, m_dataFloats );
        Detail::packVariables( m_pixelVariables, m_pixelFloats );

        // In Metal, uniform variables are passed via buffer bindings
        // Vertex variables go to vertex buffer index 2 (index 0 is vertex data)
        if( !m_dataFloats.empty() )
        {
            [_encoder setVertexBytes:m_dataFloats.data()
                              length:m_dataFloats.size() * sizeof( float )
                             atIndex:2];
        }

        // Fragment variables go to fragment buffer index 1
        if( !m_pixelFloats.empty() )
        {
            [_encoder setFragmentBytes:m_pixelFloats.data()
                                length:m_pixelFloats.size() * sizeof( float )
                               atIndex:1];
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
