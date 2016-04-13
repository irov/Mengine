#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"

#   include "ResourceMovie2.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class Movie2
		: public Node
		, public Animatable
	{
	public:
		Movie2();

    public:
		void setResourceMovie2( const ResourceMovie2Ptr & _resourceMovie );
		const ResourceMovie2Ptr & getResourceMovie2() const;

	public:
		void setCompositionName( const ConstString & _name );
		const ConstString & getCompositionName() const;

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, uint32_t _enumerator ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( float _time, uint32_t _enumerator ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	protected:
		bool _compile() override;
		void _release() override;
		
	protected:
		bool _activate() override;
		void _deactivate() override;

		void _afterActivate() override;

	protected:
		void _update( float _current, float _timing ) override;

	protected:
		void _render( const RenderObjectState * _state ) override;

	protected:
		ResourceHolder<ResourceMovie2> m_resourceMovie2;

		ConstString m_compositionName;

		aeMovieComposition * m_composition;

		struct Mesh
		{
			TVectorRenderVertex2D vertices;
			TVectorRenderIndices indices;

			RenderMaterialInterfacePtr material;
		};

		typedef stdex::vector<Mesh> TVectorMesh;
		TVectorMesh m_meshes;
	};
}