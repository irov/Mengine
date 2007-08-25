#	pragma once

namespace Menge
{
	class	State
	{
	public:
		State();
		virtual ~State();
	public: 
		virtual void	update( float _timing );
		virtual void	run();
		virtual void	end();
		virtual bool	isEnded() const;
	protected:
		bool	m_isEnded;
	};
}