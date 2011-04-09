#	pragma once

#	include "Node.h"

namespace Menge
{
	class Sprite;
	class ResourceMovie;

	class Movie
		: public Node
	{
	public:
		Movie();

	public:
		void setResourceMovie( const ConstString & _resourceName );
		const ConstString & getResourceMovie() const;

	public:
		std::size_t play();
		bool stop();
		
	public:
		void setFirstFrame();
		void setLastFrame();

	protected:
		void loader( BinParser * _parser );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		bool _activate() override;

	protected:
		void _update( float _timing ) override;

	protected:
		void _setEventListener( PyObject * _embed ) override;

	protected:
		void movieEnd_();

	private:
		void activateLayer_( std::size_t _index ) const;
	
	protected:
		ConstString m_resourceMovieName;
		
		ResourceMovie * m_resourceMovie;

		typedef std::map<size_t, Node *> TMapNode;
		TMapNode m_nodies;

		float m_timing;
		float m_out;

		bool m_play;

		std::size_t m_enumerator;
	};
}