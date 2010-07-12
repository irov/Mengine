//#	pragma once
//
//#	include "ResourceMesh.h"
//#	include <vector>
////class MeshInterface;
//
//namespace Menge
//{
//#define MAX_VERTICES				65534
//#define MAX_TRIANGLES				65534
//#define MAX_GROUPS					255
//#define MAX_MATERIALS				128
//#define MAX_JOINTS					128
//#define MAX_TEXTURE_FILENAME_SIZE	128
//
//#define SELECTED		1
//#define HIDDEN			2
//#define SELECTED2		4
//#define DIRTY			8
//#define ISKEY			16
//#define NEWLYCREATED	32
//#define MARKED			64
//
//#define SPHEREMAP		0x80
//#define HASALPHA		0x40
//#define COMBINEALPHA    0x20
//
//#define TRANSPARENCY_MODE_SIMPLE				0
//#define TRANSPARENCY_MODE_DEPTHSORTEDTRIANGLES	1
//#define TRANSPARENCY_MODE_ALPHAREF				2
//
//	struct ms3d_vertex_t
//	{
//		unsigned char flags;
//		float vertex[3];
//		char boneId;
//		unsigned char referenceCount;
//		char boneIds[3];
//		unsigned char weights[3];
//		unsigned int extra;
//		float renderColor[3];
//	};
//
//	struct ms3d_triangle_t
//	{
//		unsigned short flags;
//		unsigned short vertexIndices[3];
//		float vertexNormals[3][3];
//		float s[3];
//		float t[3];
//		float normal[3];
//		unsigned char smoothingGroup;
//		unsigned char groupIndex;
//	};
//
//	struct ms3d_group_t
//	{
//		unsigned char flags;
//		char name[32];
//		std::vector<unsigned short> triangleIndices;
//		char materialIndex;
//		std::vector<char> comment;
//	};
//
//	struct ms3d_material_t
//	{
//		char name[32];
//		float ambient[4];
//		float diffuse[4];
//		float specular[4];
//		float emissive[4];
//		float shininess;
//		float transparency;
//		unsigned char mode;
//		char texture[MAX_TEXTURE_FILENAME_SIZE];
//		char alphamap[MAX_TEXTURE_FILENAME_SIZE];
//		unsigned char id;
//		std::vector<char> comment;
//	};
//
//	struct ms3d_keyframe_t
//	{
//		float time;
//		float key[3];
//	};
//
//	struct ms3d_tangent_t
//	{
//		float tangentIn[3];
//		float tangentOut[3];
//	};
//
//	struct ms3d_joint_t
//	{
//		unsigned char flags;
//		char name[32];
//		char parentName[32];
//
//		float rot[3];
//		float pos[3];
//
//		std::vector<ms3d_keyframe_t> rotationKeys;
//		std::vector<ms3d_keyframe_t> positionKeys;
//		std::vector<ms3d_tangent_t> tangents;
//
//		std::vector<char> comment;
//		float color[3];
//
//		// used for rendering
//		int parentIndex;
//		float matLocalSkeleton[3][4];
//		float matGlobalSkeleton[3][4];
//
//		float matLocal[3][4];
//		float matGlobal[3][4];
//	};
//
//	class ResourceMeshMS3D
//		: public ResourceMesh
//	{
//		RESOURCE_DECLARE( ResourceMeshMS3D )
//
//	public:
//		ResourceMeshMS3D();
//
//	public:
//		void loader( XmlElement * _xml ) override;
//
//	public:
//
//		int GetNumGroups() const;
//		ms3d_group_t *GetGroup(int index);
//
//		int GetNumTriangles() const;
//		ms3d_triangle_t *GetTriangle(int index);
//
//		int GetNumVertices() const;
//		ms3d_vertex_t *GetVertex(int index);
//
//		int GetNumMaterials() const;
//		ms3d_material_t *GetMaterial(int index);
//
//		int GetNumJoints() const;
//		ms3d_joint_t *GetJoint(int index);
//
//		//const std::string & getMeshName() const;
//		//const std::string & getSkeletonName() const;
//		//const std::string & getMaterialName() const;
//		const std::vector<Vertex3D>& getVertexData() override;
//		const std::vector<uint16>& getIndexData() override;
//
//	protected:
//		bool _compile() override;
//		void _release() override;
//
//	private:
//
//		std::vector<ms3d_vertex_t> m_vertices;
//		std::vector<ms3d_triangle_t> m_triangles;
//		std::vector<ms3d_group_t> m_groups;
//		std::vector<ms3d_material_t> m_materials;
//
//		std::vector<Vertex3D> m_tvertices;
//		std::vector<uint16> m_indices;
//
//		float m_animationFps;
//		float m_currentTime;
//		int m_totalFrames;
//
//		std::vector<ms3d_joint_t> m_joints;
//		std::vector<char> m_comment;
//
//		float m_jointSize;
//		int m_transparencyMode;
//		float m_alphaRef;
//
//		////
//
//		//std::string m_meshName;
//		//std::string m_skeletonName;
//		//std::string m_materialName;
//	};
//}
