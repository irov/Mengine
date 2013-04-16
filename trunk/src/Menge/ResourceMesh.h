//#	pragma once
//
//#	include "ResourceReference.h"
//#	include "Vertex.h"
//#	include <vector>
//
//class MeshInterface;
//
//namespace Menge
//{
//	class ResourceMesh
//		: public ResourceReference
//	{
//		RESOURCE_DECLARE( ResourceMesh )
//
//	public:
//		ResourceMesh();
//
//	public:
//		typedef std::vector<Vertex3D> TVectorVertex3D;
//		typedef std::vector<uint16> TVectorIndicies;
//
//	public:
//		const ConstString& getMeshName() const;
//		const String& getFileName() const;
//		//const std::string & getSkeletonName() const;
//		//const std::string & getMaterialName() const;
//		virtual const TVectorVertex3D & getVertexData();
//		virtual const TVectorIndicies & getIndexData();
//		virtual void addTiming( float _timing ) {};
//
//	protected:
//		virtual void loader( BinParser * _parser ) override;
//
//	protected:
//		bool _compile() override;
//		void _release() override;
//
//	protected:
//		String m_filename;
//		ConstString m_meshName;
//		//std::string m_skeletonName;
//		//std::string m_materialName;
//	};
//}
