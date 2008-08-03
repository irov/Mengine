#	include "ResourceMeshMS3D.h"

#	include "ResourceImplement.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

namespace Menge
{
	struct S3DVertex
	{
		float Pos[3];
		float Normal[3];
		float Color[3];
		float TCoords[2];
	};

	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceMeshMS3D );
	//////////////////////////////////////////////////////////////////////////
	ResourceMeshMS3D::ResourceMeshMS3D( const ResourceFactoryParam & _params )
		: ResourceMesh( _params )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMeshMS3D::loader( XmlElement * _xml )
	{
		ResourceMesh::loader( _xml );
		//XML_SWITCH_NODE(_xml)
		//{
			//XML_CASE_ATTRIBUTE_NODE( "Mesh", "Name", m_meshName );
			//XML_CASE_ATTRIBUTE_NODE( "Skeleton", "Name", m_skeletonName );
			//XML_CASE_ATTRIBUTE_NODE( "Material", "Name", m_materialName );
		//}

		//_compile();
	}
	long filePos = 0;
	//////////////////////////////////////////////////////////////////////////
	template<class T> T * read_buffer(unsigned char *& buffer,int count = 1)
	{
		T * data = (T*)buffer;
		buffer+=sizeof(T)*count;
		filePos+=sizeof(T)*count;
		return data;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMeshMS3D::_compile()
	{
		DataStreamInterface * file = Holder<FileEngine>::hostage()->openFile( m_params.category + m_filename );
		std::size_t fileSize = file->size();

		filePos = 0;

		unsigned char * buffer = new unsigned char[fileSize];
		std::size_t read = file->read(buffer, fileSize);

		if (read != fileSize)
		{
			delete [] buffer;
			assert(0);
			return false;
		}

		char * id = read_buffer<char>(buffer,10);

		if (strncmp(id, "MS3D000000", 10) != 0)
		{
			assert(0);
			return false;
		}

		int version = *read_buffer<int>(buffer);

		if (version != 4)
		{
			assert(0);
			return false;
		}

		unsigned short numVertices = *read_buffer<unsigned short>(buffer);

		m_vertices.resize(numVertices);
		//m_tvertices.resize(numVertices);

		for (unsigned short i = 0; i < numVertices; i++)
		{
			m_vertices[i].flags = *read_buffer<unsigned char>(buffer);
			m_vertices[i].vertex[0] = *read_buffer<float>(buffer);
			m_vertices[i].vertex[1] = *read_buffer<float>(buffer);
			m_vertices[i].vertex[2] = *read_buffer<float>(buffer);
			m_vertices[i].boneId = *read_buffer<char>(buffer);
			m_vertices[i].referenceCount = *read_buffer<unsigned char>(buffer);
		}
		/*///
		for (int i = 0; i < numVertices; i++)
		{
			m_tvertices[i].x = m_vertices[i].vertex[0];
			m_tvertices[i].y = m_vertices[i].vertex[1];
			m_tvertices[i].z = m_vertices[i].vertex[2];
			//m_vertices[i].boneId = *read_buffer<char>(buffer);
			//m_vertices[i].referenceCount = *read_buffer<unsigned char>(buffer);
		}
		///*/
		unsigned short numTriangles = *read_buffer<unsigned short>(buffer);
		m_triangles.resize(numTriangles);
		
		//m_indices.resize( numTriangles * 3 );
		for (unsigned short i = 0; i < numTriangles; i++)
		{

			m_triangles[i].flags = *read_buffer<unsigned short>(buffer);
			m_triangles[i].vertexIndices[0] = *read_buffer<unsigned short>(buffer);
			m_triangles[i].vertexIndices[1] = *read_buffer<unsigned short>(buffer);
			m_triangles[i].vertexIndices[2] = *read_buffer<unsigned short>(buffer);

			m_triangles[i].vertexNormals[0][0] = *read_buffer<float>(buffer);
			m_triangles[i].vertexNormals[0][1] = *read_buffer<float>(buffer);
			m_triangles[i].vertexNormals[0][2] = *read_buffer<float>(buffer);

			m_triangles[i].vertexNormals[1][0] = *read_buffer<float>(buffer);
			m_triangles[i].vertexNormals[1][1] = *read_buffer<float>(buffer);
			m_triangles[i].vertexNormals[1][2] = *read_buffer<float>(buffer);

			m_triangles[i].vertexNormals[2][0] = *read_buffer<float>(buffer);
			m_triangles[i].vertexNormals[2][1] = *read_buffer<float>(buffer);
			m_triangles[i].vertexNormals[2][2] = *read_buffer<float>(buffer);

			/*///
			m_tvertices[m_triangles[i].vertexIndices[0]].nx = m_triangles[i].vertexNormals[0][0];
			m_tvertices[m_triangles[i].vertexIndices[0]].ny = m_triangles[i].vertexNormals[0][1];
			m_tvertices[m_triangles[i].vertexIndices[0]].nz = m_triangles[i].vertexNormals[0][2];
			m_tvertices[m_triangles[i].vertexIndices[1]].nx = m_triangles[i].vertexNormals[1][0];
			m_tvertices[m_triangles[i].vertexIndices[1]].ny = m_triangles[i].vertexNormals[1][1];
			m_tvertices[m_triangles[i].vertexIndices[1]].nz = m_triangles[i].vertexNormals[1][2];
			m_tvertices[m_triangles[i].vertexIndices[2]].nx = m_triangles[i].vertexNormals[2][0];
			m_tvertices[m_triangles[i].vertexIndices[2]].nx = m_triangles[i].vertexNormals[2][1];
			m_tvertices[m_triangles[i].vertexIndices[2]].nx = m_triangles[i].vertexNormals[2][2];
			m_indices[i*3 + 0] = m_triangles[i].vertexIndices[0];
			m_indices[i*3 + 1] = m_triangles[i].vertexIndices[1];
			m_indices[i*3 + 2] = m_triangles[i].vertexIndices[2];
			///*/

			m_triangles[i].s[0] = *read_buffer<float>(buffer);
			m_triangles[i].s[1] = *read_buffer<float>(buffer);
			m_triangles[i].s[2] = *read_buffer<float>(buffer);

			m_triangles[i].t[0] = *read_buffer<float>(buffer);
			m_triangles[i].t[1] = *read_buffer<float>(buffer);
			m_triangles[i].t[2] = *read_buffer<float>(buffer);

			m_triangles[i].smoothingGroup = *read_buffer<unsigned char>(buffer);
			m_triangles[i].groupIndex = *read_buffer<unsigned char>(buffer);
		}

		unsigned short numGroups = *read_buffer<unsigned short>(buffer);
		m_groups.resize(numGroups);

		for (unsigned short i = 0; i < numGroups; i++)
		{
			m_groups[i].flags = *read_buffer<unsigned char>(buffer);

			buffer+=sizeof(char)*32;
			filePos+=sizeof(char)*32;

			//m_groups[i].name = read_buffer<char>(buffer,32);

			unsigned short numGroupTriangles = *read_buffer<unsigned short>(buffer);
			m_groups[i].triangleIndices.resize(numGroupTriangles);

			if (numGroupTriangles > 0)
			{
				for(unsigned short j = 0; j <numGroupTriangles; j++)
				{
					m_groups[i].triangleIndices[j] = *read_buffer<unsigned short>(buffer);
				}
			}

			m_groups[i].materialIndex = *read_buffer<char>(buffer);
		}

		unsigned short numMaterials = *read_buffer<unsigned short>(buffer);
		m_materials.resize(numMaterials);

		for (unsigned short i = 0; i < numMaterials; i++)
		{
			buffer+=sizeof(char)*32;
			filePos+=sizeof(char)*32;

			m_materials[i].ambient[0] = *read_buffer<float>(buffer);
			m_materials[i].ambient[1] = *read_buffer<float>(buffer);
			m_materials[i].ambient[2] = *read_buffer<float>(buffer);
			m_materials[i].ambient[3] = *read_buffer<float>(buffer);

			m_materials[i].diffuse[0] = *read_buffer<float>(buffer);
			m_materials[i].diffuse[1] = *read_buffer<float>(buffer);
			m_materials[i].diffuse[2] = *read_buffer<float>(buffer);
			m_materials[i].diffuse[3] = *read_buffer<float>(buffer);

			m_materials[i].specular[0] = *read_buffer<float>(buffer);
			m_materials[i].specular[1] = *read_buffer<float>(buffer);
			m_materials[i].specular[2] = *read_buffer<float>(buffer);
			m_materials[i].specular[3] = *read_buffer<float>(buffer);

			m_materials[i].emissive[0] = *read_buffer<float>(buffer);
			m_materials[i].emissive[1] = *read_buffer<float>(buffer);
			m_materials[i].emissive[2] = *read_buffer<float>(buffer);
			m_materials[i].emissive[3] = *read_buffer<float>(buffer);

			m_materials[i].shininess = *read_buffer<float>(buffer);
			m_materials[i].transparency = *read_buffer<float>(buffer);

			m_materials[i].mode = *read_buffer<unsigned char>(buffer);

			buffer+=sizeof(char)*MAX_TEXTURE_FILENAME_SIZE;
			buffer+=sizeof(char)*MAX_TEXTURE_FILENAME_SIZE;
			filePos+=sizeof(char)*MAX_TEXTURE_FILENAME_SIZE;
			filePos+=sizeof(char)*MAX_TEXTURE_FILENAME_SIZE;

			//fread(m_materials[i].texture, sizeof(char), MAX_TEXTURE_FILENAME_SIZE, fp);
			//fread(m_materials[i].alphamap, sizeof(char), MAX_TEXTURE_FILENAME_SIZE, fp);

			m_materials[i].ambient[3] = m_materials[i].transparency;
			m_materials[i].diffuse[3] = m_materials[i].transparency;
			m_materials[i].specular[3] = m_materials[i].transparency;
			m_materials[i].emissive[3] = m_materials[i].transparency;
		}

		m_animationFps = *read_buffer<float>(buffer);

		if (m_animationFps < 1.0f)
		{
			m_animationFps = 1.0f;
		}

		m_currentTime = *read_buffer<float>(buffer);
		m_totalFrames = *read_buffer<int>(buffer);

		unsigned short numJoints = *read_buffer<unsigned short>(buffer);
		m_joints.resize(numJoints);

		for (unsigned short i = 0; i < numJoints; i++)
		{
			m_joints[i].flags = *read_buffer<unsigned char>(buffer);

			buffer+=sizeof(char)*32;
			buffer+=sizeof(char)*32;
			filePos+=sizeof(char)*32;
			filePos+=sizeof(char)*32;
			//fread(m_joints[i].name, sizeof(char), 32, fp);
			//fread(m_joints[i].parentName, sizeof(char), 32, fp);

			m_joints[i].rot[0] = *read_buffer<float>(buffer);
			m_joints[i].rot[1] = *read_buffer<float>(buffer);
			m_joints[i].rot[2] = *read_buffer<float>(buffer);

			m_joints[i].pos[0] = *read_buffer<float>(buffer);
			m_joints[i].pos[1] = *read_buffer<float>(buffer);
			m_joints[i].pos[2] = *read_buffer<float>(buffer);

			unsigned short numKeyFramesRot = *read_buffer<unsigned short>(buffer);
			m_joints[i].rotationKeys.resize(numKeyFramesRot);

			unsigned short numKeyFramesPos = *read_buffer<unsigned short>(buffer);
			m_joints[i].positionKeys.resize(numKeyFramesPos);

			for (int j = 0; j < numKeyFramesRot; j++)
			{
				m_joints[i].rotationKeys[j].time = *read_buffer<float>(buffer);

				m_joints[i].rotationKeys[j].key[0] = *read_buffer<float>(buffer);
				m_joints[i].rotationKeys[j].key[1] = *read_buffer<float>(buffer);
				m_joints[i].rotationKeys[j].key[2] = *read_buffer<float>(buffer);

				m_joints[i].rotationKeys[j].time *= m_animationFps;
			}

			for (int j = 0; j < numKeyFramesPos; j++)
			{
				m_joints[i].rotationKeys[j].time = *read_buffer<float>(buffer);

				m_joints[i].rotationKeys[j].key[0] = *read_buffer<float>(buffer);
				m_joints[i].rotationKeys[j].key[1] = *read_buffer<float>(buffer);
				m_joints[i].rotationKeys[j].key[2] = *read_buffer<float>(buffer);

				m_joints[i].positionKeys[j].time *= m_animationFps;
			}
		}

		//skipping commentars!!
		if (filePos < fileSize)
		{
			int subVersion = 0;

			subVersion = *read_buffer<int>(buffer);

			if (subVersion == 1)
			{
				int numComments = 0;
				size_t commentSize = 0;

				numComments = *read_buffer<int>(buffer);

				for (int i = 0; i < numComments; i++)
				{
					int index = *read_buffer<int>(buffer);
					commentSize = *read_buffer<int>(buffer);
					if (commentSize > 0)
					{
						buffer+=sizeof(char)*commentSize;
						filePos+=sizeof(char)*commentSize;
					}
				}

				numComments = *read_buffer<int>(buffer);
				for (int i = 0; i < numComments; i++)
				{
					int index = *read_buffer<int>(buffer);
					commentSize = *read_buffer<int>(buffer);
					if (commentSize > 0)
					{
						buffer+=sizeof(char)*commentSize;
						filePos+=sizeof(char)*commentSize;
					}
				}

				numComments = *read_buffer<int>(buffer);
				for (int i = 0; i < numComments; i++)
				{
					int index = *read_buffer<int>(buffer);
					commentSize = *read_buffer<int>(buffer);
					if (commentSize > 0)
					{
						buffer+=sizeof(char)*commentSize;
						filePos+=sizeof(char)*commentSize;
					}
				}

				numComments = *read_buffer<int>(buffer);
				if (numComments == 1)
				{
					commentSize = *read_buffer<int>(buffer);
					if (commentSize > 0)
					{
						buffer+=sizeof(char)*commentSize;
						filePos+=sizeof(char)*commentSize;
					}
				}
			}
			else
			{
				assert(0);
			}
		}

		if (filePos < fileSize)
		{
			int subVersion = *read_buffer<int>(buffer);

			if (subVersion == 2)
			{
				for (int i = 0; i < numVertices; i++)
				{
					m_vertices[i].boneIds[0] = *read_buffer<char>(buffer);
					m_vertices[i].boneIds[1] = *read_buffer<char>(buffer);
					m_vertices[i].boneIds[2] = *read_buffer<char>(buffer);

					m_vertices[i].weights[0] = *read_buffer<unsigned char>(buffer);
					m_vertices[i].weights[1] = *read_buffer<unsigned char>(buffer);
					m_vertices[i].weights[2] = *read_buffer<unsigned char>(buffer);
					m_vertices[i].extra = *read_buffer<unsigned int>(buffer);
				}
			}
			else if (subVersion == 1)
			{
				for (int i = 0; i < numVertices; i++)
				{
					m_vertices[i].boneIds[0] = *read_buffer<char>(buffer);
					m_vertices[i].boneIds[1] = *read_buffer<char>(buffer);
					m_vertices[i].boneIds[2] = *read_buffer<char>(buffer);

					m_vertices[i].weights[0] = *read_buffer<unsigned char>(buffer);
					m_vertices[i].weights[1] = *read_buffer<unsigned char>(buffer);
					m_vertices[i].weights[2] = *read_buffer<unsigned char>(buffer);
				}
			}
			else
			{
				assert(0);
			}
		}

		//дальше абсолютно бесполезная инфа
		if (filePos < fileSize)
		{
			int subVersion = *read_buffer<int>(buffer);

			if (subVersion == 1)
			{
				for (int i = 0; i < numJoints; i++)
				{
					buffer+=sizeof(float)*3;
					filePos+=sizeof(float)*3;
				}
			}
			else
			{
				assert(0);
			}
		}

		if (filePos < fileSize)
		{
			int subVersion = *read_buffer<int>(buffer);
			if (subVersion == 1)
			{
				m_jointSize = *read_buffer<float>(buffer);
				m_transparencyMode = *read_buffer<int>(buffer);
				m_alphaRef = *read_buffer<float>(buffer);
			}
			else
			{
				assert(0);
			}
		}
		//
		buffer -= filePos;	// return to the beginnig of memory buffer
		delete [] buffer;

		Holder<FileEngine>::hostage()->closeStream(file);

		//std::vector<S3DVertex> *Vertices;
		//std::vector<unsigned short> Indices;

		//S3DVertex v;
		TVertex v;

		for( int i = 0; i < numTriangles; ++i )
		{
			unsigned int tmp = m_groups[m_triangles[i].groupIndex].materialIndex;
			//Vertices = &AnimatedMesh->getMeshBuffers()[tmp]->Vertices_Standard;

			for( unsigned short j = 0; j < 3; ++j )
			{
				v.uv[0] = m_triangles[i].s[j];
				v.uv[1] = m_triangles[i].t[j];

				v.n[0] = m_triangles[i].vertexNormals[j][0];
				v.n[1] = m_triangles[i].vertexNormals[j][1];
				v.n[2] = m_triangles[i].vertexNormals[j][2];

				// flip x and z, fucking milkshape do this while converting from 3DS
				v.pos[0] = m_vertices[m_triangles[i].vertexIndices[j]].vertex[0];
				v.pos[1] = m_vertices[m_triangles[i].vertexIndices[j]].vertex[1];
				v.pos[2] = m_vertices[m_triangles[i].vertexIndices[j]].vertex[2];

				int index = -1;

				for( std::vector<TVertex>::size_type iV = 0; iV < m_tvertices.size(); ++iV )
				{
					TVertex v2 = m_tvertices[iV];

					if( v.col == v2.col	&&v.pos[0] == v2.pos[0] && v.pos[1] == v2.pos[1] && v.pos[2] == v2.pos[2] &&
						v.n[0] == v2.n[0] && v.n[1] == v2.n[1] && v.n[2] == v2.n[2]&&
						v.uv[0] == v2.uv[0] && v.uv[1] == v2.uv[1]
					)
					{
						index = (int)iV;
						break;
					}
				}

				if(index == -1)
				{
					index = m_tvertices.size();
					m_tvertices.push_back(v);
				}
				m_indices.push_back(index);
			}
		}

		return true;

	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMeshMS3D::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	int ResourceMeshMS3D::GetNumGroups() const
	{
		return (int) m_groups.size();
	}
	//////////////////////////////////////////////////////////////////////////
	ms3d_group_t *ResourceMeshMS3D::GetGroup(int index)
	{
		return &m_groups[index];
	}
	//////////////////////////////////////////////////////////////////////////
	int ResourceMeshMS3D::GetNumTriangles() const
	{
		return (int) m_triangles.size();
	}
	//////////////////////////////////////////////////////////////////////////
	ms3d_triangle_t *ResourceMeshMS3D::GetTriangle(int index)
	{
		return &m_triangles[index];
	}
	//////////////////////////////////////////////////////////////////////////
	int ResourceMeshMS3D::GetNumVertices() const
	{
		return (int) m_vertices.size();
	}
	//////////////////////////////////////////////////////////////////////////
	ms3d_vertex_t *ResourceMeshMS3D::GetVertex(int index)
	{
		return &m_vertices[index];
	}
	//////////////////////////////////////////////////////////////////////////
	int ResourceMeshMS3D::GetNumMaterials() const
	{
		return (int) m_materials.size();
	}
	//////////////////////////////////////////////////////////////////////////
	ms3d_material_t *ResourceMeshMS3D::GetMaterial(int index)
	{
		return &m_materials[index];
	}
	//////////////////////////////////////////////////////////////////////////
	int ResourceMeshMS3D::GetNumJoints() const
	{
		return (int) m_joints.size();
	}
	//////////////////////////////////////////////////////////////////////////
	ms3d_joint_t *ResourceMeshMS3D::GetJoint(int index)
	{
		return &m_joints[index];
	}
	//////////////////////////////////////////////////////////////////////////
	const std::vector<TVertex>& ResourceMeshMS3D::getVertexData()
	{
		return m_tvertices;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::vector<uint16>& ResourceMeshMS3D::getIndexData()
	{
		return m_indices;
	}
	/*const std::string & ResourceMesh::getMeshName() const
	{
	return m_meshName;
	}*/
	//////////////////////////////////////////////////////////////////////////
	/*const std::string & ResourceMesh::getSkeletonName() const
	{
	return m_skeletonName;
	}*/
	//////////////////////////////////////////////////////////////////////////
	/*	const std::string & ResourceMesh::getMaterialName() const
	{
	return m_materialName;
	}*/
	//////////////////////////////////////////////////////////////////////////
}