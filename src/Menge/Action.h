#	pragma once

namespace Menge
{
	class	Action
	{
	public:
		Action();
		virtual ~Action();
	public: 
		virtual void	update( float _timing );
		virtual void	run();
		virtual void	end();
		virtual bool	isEnded() const;
	protected:
		bool	m_isEnded;
	};
}