#	include <list>

namespace	Menge
{
	class	State;

	class	StackFSM
	{
	public:
		StackFSM();
		void	update( float _timing );
		void	addState( State * _action );
		void	runState( State * _action );
		void	terminateStates();
	private:
		typedef std::list<State*> StateList;
		StateList	m_stateList;
		State * m_currentAction;
	};
}