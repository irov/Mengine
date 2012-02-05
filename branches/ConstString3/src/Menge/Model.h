#	pragma once

#	include "Node.h"
#	include "Animatable.h"

#	include "ResourceModel.h"

#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	class ResourceModel;
	class Texture;

	class Model
		: public Node
		, public Animatable
	{
	public:
		Model();
		~Model();

	public:
		void setModelResource( const ConstString & _resourceName );
		const ConstString & getModelResource() const;

	public:
		bool _play() override;
		bool _restart( size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _render( Camera2D * _camera ) override;

	protected:
		ConstString m_resourceName;
		ResourceModel * m_resourceModel;

		bool m_loop;

		float m_timing;

		struct RenderModelMesh
		{
			AnimationMesh::FrameMesh frame;
			
			TVectorIndices::size_type indices_size;

			IBHandle ibHandle;
			const Texture * texture;

			bool active;
		};

		typedef std::vector<RenderModelMesh> TVectorRenderModelMesh;
		TVectorRenderModelMesh m_renderModelMesh;
	};
}