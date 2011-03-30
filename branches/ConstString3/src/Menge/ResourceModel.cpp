#	include "ResourceModel.h"

#	include "ResourceImplement.h"

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
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceModel::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_NODE( Protocol::AnimationMesh )
			{
				m_animationMeshes.push_back( AnimationMesh() );
				AnimationMesh & animationModel = m_animationMeshes.back();

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::AnimationMesh_Name, animationModel.name );
					BIN_CASE_ATTRIBUTE( Protocol::AnimationMesh_Texture, animationModel.texture );
					BIN_CASE_ATTRIBUTE( Protocol::AnimationMesh_FrameRate, animationModel.frameRate );
					BIN_CASE_ATTRIBUTE( Protocol::AnimationMesh_FrameBegin, animationModel.frameBegin );
					BIN_CASE_ATTRIBUTE( Protocol::AnimationMesh_FrameEnd, animationModel.frameEnd );
					BIN_CASE_ATTRIBUTE( Protocol::AnimationMesh_FrameStep, animationModel.frameStep );	
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
			BIN_CASE_ATTRIBUTE( Protocol::Indices_Value, _animationMesh.indicies );

			BIN_CASE_NODE( Protocol::Frame )
			{
				_animationMesh.frameMeshes.push_back( AnimationMesh::FrameMesh() );
				AnimationMesh::FrameMesh & frameMesh = _animationMesh.frameMeshes.back();

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE(Protocol::Frame_Value, frameMesh.frame );
				}
			
				BIN_PARSE_METHOD_ARG1( this, &ResourceModel::loaderVertices_, frameMesh.vertecies );
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

			if( mesh.texture.empty() == true )
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

				resource->addImagePath( mesh.texture, mt::vec2f(-1.f,-1.f) );
			}

			mesh.resourceImage = ResourceManager::get()
				->getResourceT<ResourceImageDefault>(c_imageResource);

			if( mesh.resourceImage == 0 )
			{
				return false;
			}
		}

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
