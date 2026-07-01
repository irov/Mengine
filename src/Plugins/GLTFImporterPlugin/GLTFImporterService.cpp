#include "GLTFImporterService.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Vector.h"
#include "Kernel/VectorMat4f.h"

#include "Config/StdString.h"

#include "cgltf.h"

SERVICE_FACTORY( GLTFImporterService, Mengine::GLTFImporterService );

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const cgltf_attribute * findAttribute( const cgltf_primitive * _prim, cgltf_attribute_type _type, int _index = 0 )
        {
            for( cgltf_size i = 0; i != _prim->attributes_count; ++i )
            {
                const cgltf_attribute & attr = _prim->attributes[i];

                if( attr.type == _type && attr.index == _index )
                {
                    return &attr;
                }
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t packColorRGBA8( float r, float g, float b, float a )
        {
            auto sat = []( float v ) -> uint32_t
            {
                if( v < 0.f ) v = 0.f;
                if( v > 1.f ) v = 1.f;
                return (uint32_t)(v * 255.0f + 0.5f);
            };

            return  sat( r )
                | (sat( g ) << 8)
                | (sat( b ) << 16)
                | (sat( a ) << 24);
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f transformPosition( const mt::mat4f & _m, const mt::vec3f & _p )
        {
            mt::vec3f r;
            r.x = _p.x * _m.v0.x + _p.y * _m.v1.x + _p.z * _m.v2.x + _m.v3.x;
            r.y = _p.x * _m.v0.y + _p.y * _m.v1.y + _p.z * _m.v2.y + _m.v3.y;
            r.z = _p.x * _m.v0.z + _p.y * _m.v1.z + _p.z * _m.v2.z + _m.v3.z;
            return r;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f transformDirection( const mt::mat4f & _m, const mt::vec3f & _d )
        {
            mt::vec3f r;
            r.x = _d.x * _m.v0.x + _d.y * _m.v1.x + _d.z * _m.v2.x;
            r.y = _d.x * _m.v0.y + _d.y * _m.v1.y + _d.z * _m.v2.y;
            r.z = _d.x * _m.v0.z + _d.y * _m.v1.z + _d.z * _m.v2.z;
            return r;
        }
        //////////////////////////////////////////////////////////////////////////
        static MemoryInterfacePtr slurpContent( const ContentInterfacePtr & _content )
        {
            InputStreamInterfacePtr stream = _content->openInputStreamFile( false, false, MENGINE_DOCUMENT_FUNCTION );

            if( stream == nullptr )
            {
                return nullptr;
            }

            MemoryInterfacePtr memory = Helper::createMemoryStream( stream, MENGINE_DOCUMENT_FUNCTION );

            _content->closeInputStreamFile( stream );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        static cgltf_data * parseGLTF( const void * _buffer, size_t _size, const Char * _diagPath )
        {
            cgltf_options options = {};
            cgltf_data * data = nullptr;

            cgltf_result r = cgltf_parse( &options, _buffer, _size, &data );

            if( r != cgltf_result_success )
            {
                LOGGER_ERROR( "cgltf_parse failed for '%s' (code %d)"
                    , _diagPath
                    , (int)r
                );

                return nullptr;
            }

            r = cgltf_load_buffers( &options, data, nullptr );

            if( r != cgltf_result_success )
            {
                LOGGER_ERROR( "cgltf_load_buffers failed for '%s' (code %d) - "
                    "external .bin not supported, repack as .glb"
                    , _diagPath
                    , (int)r
                );

                cgltf_free( data );

                return nullptr;
            }

            return data;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class TVertex, class TWriteVertex>
        static bool appendPrimitiveStatic(
            const cgltf_primitive * _prim,
            const mt::mat4f & _preTransform,
            Vector<TVertex> & _vertices,
            VectorRenderIndex32 & _indices,
            const TWriteVertex & _writeVertex )
        {
            const cgltf_attribute * positions = findAttribute( _prim, cgltf_attribute_type_position );

            if( positions == nullptr || positions->data == nullptr )
            {
                LOGGER_ERROR( "gltf primitive missing POSITION attribute" );
                return false;
            }

            if( _prim->indices == nullptr )
            {
                LOGGER_ERROR( "gltf primitive without indices is not supported" );
                return false;
            }

            const cgltf_attribute * normals = findAttribute( _prim, cgltf_attribute_type_normal );
            const cgltf_attribute * tangents = findAttribute( _prim, cgltf_attribute_type_tangent );
            const cgltf_attribute * uv0 = findAttribute( _prim, cgltf_attribute_type_texcoord, 0 );
            const cgltf_attribute * uv1 = findAttribute( _prim, cgltf_attribute_type_texcoord, 1 );
            const cgltf_attribute * colors = findAttribute( _prim, cgltf_attribute_type_color, 0 );

            const cgltf_size vc = positions->data->count;

            const uint32_t baseVertex = (uint32_t)_vertices.size();

            _vertices.resize( _vertices.size() + vc );

            for( cgltf_size v = 0; v != vc; ++v )
            {
                TVertex & out = _vertices[baseVertex + v];

                float pos[3] = {0.f, 0.f, 0.f};
                cgltf_accessor_read_float( positions->data, v, pos, 3 );
                mt::vec3f localPos( pos[0], pos[1], pos[2] );
                mt::vec3f worldPos = transformPosition( _preTransform, localPos );

                float n[3] = {0.f, 1.f, 0.f};
                if( normals != nullptr ) { cgltf_accessor_read_float( normals->data, v, n, 3 ); }
                mt::vec3f worldN = transformDirection( _preTransform, mt::vec3f( n[0], n[1], n[2] ) );

                float t[4] = {1.f, 0.f, 0.f, 1.f};
                if( tangents != nullptr ) { cgltf_accessor_read_float( tangents->data, v, t, 4 ); }
                mt::vec3f worldT = transformDirection( _preTransform, mt::vec3f( t[0], t[1], t[2] ) );

                float c[4] = {1.f, 1.f, 1.f, 1.f};
                if( colors != nullptr ) { cgltf_accessor_read_float( colors->data, v, c, 4 ); }
                uint32_t packedColor = packColorRGBA8( c[0], c[1], c[2], c[3] );

                float uvA[2] = {0.f, 0.f};
                if( uv0 != nullptr ) { cgltf_accessor_read_float( uv0->data, v, uvA, 2 ); }
                float uvB[2] = {0.f, 0.f};
                if( uv1 != nullptr ) { cgltf_accessor_read_float( uv1->data, v, uvB, 2 ); }

                _writeVertex( out
                    , worldPos
                    , worldN
                    , mt::vec4f( worldT.x, worldT.y, worldT.z, t[3] )
                    , packedColor
                    , mt::vec2f( uvA[0], uvA[1] )
                    , mt::vec2f( uvB[0], uvB[1] )
                    , (cgltf_size)v
                    , _prim );
            }

            const cgltf_accessor * idx = _prim->indices;

            for( cgltf_size i = 0; i != idx->count; ++i )
            {
                cgltf_uint val = 0;
                cgltf_accessor_read_uint( idx, i, &val, 1 );

                _indices.push_back( baseVertex + (uint32_t)val );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    GLTFImporterService::GLTFImporterService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GLTFImporterService::~GLTFImporterService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GLTFImporterService::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GLTFImporterService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GLTFImporterService::importMesh( const ContentInterfacePtr & _content
        , const mt::mat4f & _preTransform
        , uint32_t _meshIndex
        , const ResourceMesh3DPtr & _resource )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _content, "null content" );
        MENGINE_ASSERTION_MEMORY_PANIC( _resource, "null resource" );

        MemoryInterfacePtr memory = Detail::slurpContent( _content );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "failed to read gltf '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        cgltf_data * data = Detail::parseGLTF( memory->getBuffer(), memory->getSize()
            , Helper::getContentFullPath( _content ).c_str() );

        if( data == nullptr )
        {
            return false;
        }

        if( _meshIndex >= data->meshes_count )
        {
            LOGGER_ERROR( "gltf '%s' meshIndex %u out of range (count=%zu)"
                , Helper::getContentFullPath( _content ).c_str()
                , _meshIndex
                , (size_t)data->meshes_count
            );
            cgltf_free( data );
            return false;
        }

        const cgltf_mesh & mesh = data->meshes[_meshIndex];

        VectorRenderVertex3D vertices;
        VectorRenderIndex32 indices;

        auto writer = []( RenderVertex3D & _out
            , const mt::vec3f & _pos
            , const mt::vec3f & _normal
            , const mt::vec4f & _tangent
            , uint32_t _color
            , const mt::vec2f & _uv0
            , const mt::vec2f & _uv1
            , cgltf_size /*_v*/
            , const cgltf_primitive * /*_prim*/ ) -> void
        {
            _out.position = _pos;
            _out.normal = _normal;
            _out.tangent = _tangent;
            _out.color = _color;
            _out.uv[0] = _uv0;
            _out.uv[1] = _uv1;
        };

        for( cgltf_size p = 0; p != mesh.primitives_count; ++p )
        {
            const cgltf_primitive & prim = mesh.primitives[p];

            if( prim.type != cgltf_primitive_type_triangles )
            {
                LOGGER_WARNING( "gltf '%s' primitive %zu is not triangles, skipping"
                    , Helper::getContentFullPath( _content ).c_str()
                    , (size_t)p
                );
                continue;
            }

            if( Detail::appendPrimitiveStatic( &prim, _preTransform, vertices, indices, writer ) == false )
            {
                cgltf_free( data );
                return false;
            }
        }

        _resource->setVertices( vertices );
        _resource->setIndices( indices );

        cgltf_free( data );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GLTFImporterService::importSkinnedMesh( const ContentInterfacePtr & _content
        , const mt::mat4f & _preTransform
        , uint32_t _meshIndex
        , const ResourceSkinnedMesh3DPtr & _resource )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _content, "null content" );
        MENGINE_ASSERTION_MEMORY_PANIC( _resource, "null resource" );

        MemoryInterfacePtr memory = Detail::slurpContent( _content );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "failed to read gltf '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        cgltf_data * data = Detail::parseGLTF( memory->getBuffer(), memory->getSize()
            , Helper::getContentFullPath( _content ).c_str() );

        if( data == nullptr )
        {
            return false;
        }

        if( _meshIndex >= data->meshes_count )
        {
            LOGGER_ERROR( "gltf '%s' meshIndex %u out of range (count=%zu)"
                , Helper::getContentFullPath( _content ).c_str()
                , _meshIndex
                , (size_t)data->meshes_count
            );
            cgltf_free( data );
            return false;
        }

        const cgltf_mesh & mesh = data->meshes[_meshIndex];

        const cgltf_skin * skin = nullptr;
        for( cgltf_size n = 0; n != data->nodes_count; ++n )
        {
            const cgltf_node & node = data->nodes[n];
            if( node.mesh == &mesh && node.skin != nullptr )
            {
                skin = node.skin;
                break;
            }
        }

        if( skin == nullptr )
        {
            LOGGER_ERROR( "gltf '%s' mesh %u has no associated skin"
                , Helper::getContentFullPath( _content ).c_str()
                , _meshIndex
            );
            cgltf_free( data );
            return false;
        }

        VectorRenderVertexSkinned3D vertices;
        VectorRenderIndex32 indices;

        auto writer = []( RenderVertexSkinned3D & _out
            , const mt::vec3f & _pos
            , const mt::vec3f & _normal
            , const mt::vec4f & _tangent
            , uint32_t _color
            , const mt::vec2f & _uv0
            , const mt::vec2f & _uv1
            , cgltf_size _v
            , const cgltf_primitive * _prim ) -> void
        {
            _out.position = _pos;
            _out.normal = _normal;
            _out.tangent = _tangent;
            _out.color = _color;
            _out.uv[0] = _uv0;
            _out.uv[1] = _uv1;

            const cgltf_attribute * joints = Detail::findAttribute( _prim, cgltf_attribute_type_joints, 0 );
            const cgltf_attribute * weights = Detail::findAttribute( _prim, cgltf_attribute_type_weights, 0 );

            cgltf_uint j[4] = {0, 0, 0, 0};
            float w[4] = {0.f, 0.f, 0.f, 0.f};

            if( joints != nullptr ) { cgltf_accessor_read_uint( joints->data, _v, j, 4 ); }
            if( weights != nullptr ) { cgltf_accessor_read_float( weights->data, _v, w, 4 ); }

            for( int k = 0; k != 4; ++k )
            {
                _out.boneIndices[k] = (uint8_t)(j[k] < 255 ? j[k] : 255);
                _out.boneWeights[k] = w[k];
            }
        };

        for( cgltf_size p = 0; p != mesh.primitives_count; ++p )
        {
            const cgltf_primitive & prim = mesh.primitives[p];

            if( prim.type != cgltf_primitive_type_triangles )
            {
                LOGGER_WARNING( "gltf '%s' primitive %zu is not triangles, skipping"
                    , Helper::getContentFullPath( _content ).c_str()
                    , (size_t)p
                );
                continue;
            }

            if( Detail::appendPrimitiveStatic( &prim, _preTransform, vertices, indices, writer ) == false )
            {
                cgltf_free( data );
                return false;
            }
        }

        VectorMat4f ibm;

        if( skin->inverse_bind_matrices != nullptr )
        {
            ibm.resize( skin->joints_count );

            for( cgltf_size i = 0; i != skin->joints_count; ++i )
            {
                float m[16];
                cgltf_accessor_read_float( skin->inverse_bind_matrices, i, m, 16 );

                mt::mat4f & dst = ibm[i];
                dst.v0 = mt::vec4f( m[0],  m[1],  m[2],  m[3]  );
                dst.v1 = mt::vec4f( m[4],  m[5],  m[6],  m[7]  );
                dst.v2 = mt::vec4f( m[8],  m[9],  m[10], m[11] );
                dst.v3 = mt::vec4f( m[12], m[13], m[14], m[15] );
            }
        }
        else
        {
            ibm.resize( skin->joints_count );
            for( mt::mat4f & m : ibm )
            {
                mt::ident_m4( &m );
            }
        }

        _resource->setVertices( vertices );
        _resource->setIndices( indices );
        _resource->setInverseBindMatrices( ibm );

        cgltf_free( data );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
