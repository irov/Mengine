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
		void setAutoPlay( bool _value );
		void setLoop( bool _value );
		void setComplete( bool _value );

	public:
		void play();
		void stop();
		void complete();

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

	private:
		bool aplyComplete_();
		void activateLayer_( std::size_t _index ) const;
	
	protected:
		ConstString m_resourceMovieName;
		
		ResourceMovie * m_resourceMovie;

		typedef std::vector<Node *> TVectorNode;
		TVectorNode m_nodies;

		float m_lasting;
		float m_timing;
		float m_out;

		bool m_complete;
		bool m_play;

		bool m_autoPlay;
		bool m_loop;
	};
}