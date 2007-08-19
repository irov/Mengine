#	include <list>

namespace	Menge
{
	class	Action;

	class	ActionScheduler
	{
	public:
		ActionScheduler();
		void	update( float _timing );
		void	addAction( Action * _action );
		void	runAction( Action * _action );
	private:
		typedef std::list<Action*> ActionList;
		ActionList	m_actionList;
		Action * m_currentAction;
	};
}