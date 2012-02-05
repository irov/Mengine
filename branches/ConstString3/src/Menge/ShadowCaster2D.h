//#	pragma once
//#	include "math/vec2.h"
//#	include "math/polygon.h"
//#	include "Node.h"
//
//namespace Menge
//{
//	class ShadowCaster2D
//		: public Node
//	{
//	public:
//		ShadowCaster2D();
//
//	public:
//		void addVertex(const mt::vec2f & _vertex);
//		const mt::vec2f & getVertex( size_t i ) const;
//
//		mt::vec2f getEdge( size_t i ) const;
//		size_t size() const;
//
//	public:
//		void loader( XmlElement * _xml ) override;
//		void parser( BinParser * _parser ) override;
//
//	protected:
//		bool _activate() override;
//		void _deactivate() override;
//
//		bool _compile() override;
//		void _release() override;
//
//		void _update( float _timing ) override;
//
//#	ifndef MENGE_MASTER_RELEASE
//		void _debugRender( Camera2D * _camera, unsigned int _debugMask ) override;
//#	endif
//
//	private:
//		mt::polygon m_poly;
//	};
//};
