#	include "ResourceModel.h"

#	include "ResourceImplement.h"

#	include "RenderEngine.h"
#	include "ResourceManager.h"

#	include "Consts.h"
#	include "Logger/Logger.h"

#	include "BinParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceModel );
	//////////////////////////////////////////////////////////////////////////
	ResourceModel::ResourceModel()
		: m_material(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorAnimationMesh & ResourceModel::getAnimationMeshes() const
	{
		return m_animationMeshes;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceModel::getFrameMeshFirst( const AnimationMesh & _am, AnimationMesh::FrameMesh & _frame )
	{
		if( _am.frameMeshes.empty() == true )
		{
			return false;
		}

		_frame = _am.frameMeshes.front();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceModel::getFrameMeshLast( const AnimationMesh & _am, AnimationMesh::FrameMesh & _frame )
	{
		if( _am.frameMeshes.empty() == true )
		{
			return false;
		}

		_frame = _am.frameMeshes.back();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		static float s_linerp( float _in, float _out, float _scale )
		{
			return _in + ( _out - _in ) * _scale; 
		}

		static void s_linerp_v2( float * _out, const float * _first, const float * _second, float _scale )
		{
			_out[0] = s_linerp(_first[0], _second[0], _scale);
			_out[1] = s_linerp(_first[1], _second[1], _scale);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceModel::getFrameMesh( const AnimationMesh & _am, float _timing, AnimationMesh::FrameMesh & _frame ) const
	{
		if( _timing < _am.in )
		{
			return false;
		}

		if( _timing >= _am.out )
		{
			return false;
		}

		float relation_time = _timing - _am.in;

		std::size_t index = std::size_t(relation_time / m_duration);

		const AnimationMesh::FrameMesh & frame_1 = _am.frameMeshes[index+0];
		const AnimationMesh::FrameMesh & frame_2 = _am.frameMeshes[index+1];

		float time_1 = (index + 0) * m_duration;
		float time_2 = (index + 1) * m_duration;

		float timeScale = ( relation_time - time_1 ) / ( time_2 - time_1 );

		for( TVectorVertex2D::size_type 
			it = 0,
			it_end = _am.verticesSize;
		it != it_end;
		++it )
		{
			Helper::s_linerp_v2( _frame.vertices[it].pos, frame_1.vertices[it].pos, frame_2.vertices[it].pos, timeScale );
			Helper::s_linerp_v2( _frame.vertices[it].uv, frame_1.vertices[it].uv, frame_2.vertices[it].uv, timeScale );
			
			_frame.vertices[it].color = frame_1.vertices[it].color;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const Material * ResourceModel::getMaterial() const
	{
		return m_material;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceModel::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE( Protocol::Duration_Value, m_duration );

			BIN_CASE_NODE( Protocol::AnimationMesh )
			{
				m_animationMeshes.push_back( AnimationMesh() );
				AnimationMesh & animationModel = m_animationMeshes.back();

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::AnimationMesh_Name, animationModel.name );
					BIN_CASE_ATTRIBUTE( Protocol::AnimationMesh_Texture, animationModel.textureName );
					BIN_CASE_ATTRIBUTE( Protocol::AnimationMesh_In, animationModel.in );
					BIN_CASE_ATTRIBUTE( Protocol::AnimationMesh_Out, animationModel.out );
				}

				BIN_PARSE_METHOD_ARG1( this, &ResourceModel::loaderAnimationMesh_, animationModel );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceModel::loaderAnimationMesh_( BinParser * _parser, AnimationMesh & _animationMesh )
	{
		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE( Protocol::Indices_Value, _animationMesh.indices );
			BIN_CASE_ATTRIBUTE( Protocol::Vertices_Size, _animationMesh.verticesSize );

			BIN_CASE_NODE( Protocol::Frame )
			{
				_animationMesh.frameMeshes.push_back( AnimationMesh::FrameMesh() );
				AnimationMesh::FrameMesh & frameMesh = _animationMesh.frameMeshes.back();
			
				BIN_PARSE_METHOD_ARG1( this, &ResourceModel::loaderVertices_, frameMesh.vertices );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceModel::loaderVertices_( BinParser * _parser, TVectorVertex2D & _vertices )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Vertex )
			{
				mt::vec2f coordinate;
				mt::vec2f uv;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE(Protocol::Vertex_Coordinate, coordinate);
					BIN_CASE_ATTRIBUTE(Protocol::Vertex_UV, uv);
				}

				_vertices.push_back( Vertex2D() );
				Vertex2D & vertex = _vertices.back();
				vertex.pos[0] = coordinate.x;
				vertex.pos[1] = coordinate.y;
				vertex.pos[2] = 0.f;

				ColourValue color(1.f,1.f,1.f,1.f);
				vertex.color = color.getAsARGB();

				vertex.uv[0] = uv.x;
				vertex.uv[1] = uv.y;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceModel::_compile()
	{
		for( TVectorAnimationMesh::iterator
			it = m_animationMeshes.begin(),
			it_end = m_animationMeshes.end();
		it != it_end;
		++it )
		{
			AnimationMesh & mesh = *it;

			if( mesh.textureName.empty() == true )
			{
				return false;
			}

			String imageResource = "Model";
			imageResource += Helper::to_str(mesh.name);

			ConstString c_imageResource(imageResource);

			if( ResourceManager::get()->hasResource(c_imageResource) == false )
			{
				ResourceImageDefault * resource = ResourceManager::get()
					->createResourceT<ResourceImageDefault>( m_category, m_group, c_imageResource, Consts::get()->c_ResourceImageDefault );

				resource->addImagePath( mesh.textureName, mt::vec2f(-1.f,-1.f) );
			}

			mesh.resourceImage = ResourceManager::get()
				->getResourceT<ResourceImageDefault>(c_imageResource);

			if( mesh.resourceImage == 0 )
			{
				return false;
			}

			mesh.texture = mesh.resourceImage->getTexture( 0 );

			if( mesh.texture == 0 )
			{
				return false;
			}

			if( mesh.indices.empty() == true )
			{
				return false;
			}

			//std::fill( mesh.indices.begin(), mesh.indices.end(), 0 );

			const TVectorIndices::value_type * indices_buffer = &mesh.indices[0];
			TVectorIndices::size_type indices_size = mesh.indices.size();

			mesh.ibHandle = RenderEngine::get()
				->createIndicesBuffer( indices_buffer, indices_size );

			if( mesh.ibHandle == 0 )
			{
				return false;
			}
		}

		const MaterialGroup * materialGroup = RenderEngine::get()
			->getMaterialGroup( CONST_STRING(Sprite) );

		if( materialGroup == 0 )
		{
			return false;
		}

		m_material = materialGroup->getMaterial( TAM_CLAMP, TAM_CLAMP );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceModel::_release()
	{
		for( TVectorAnimationMesh::iterator
			it = m_animationMeshes.begin(),
			it_end = m_animationMeshes.end();
		it != it_end;
		++it )
		{
			AnimationMesh & mesh = *it;

			ResourceManager::get()
				->releaseResource(mesh.resourceImage);
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
