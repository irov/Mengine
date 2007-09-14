#	include <list>

namespace	Menge
{
	class	State;

	//! StackFSM - dynamic stack finite automat
	/*! 
	* 
	*/
	class	StackFSM
	{
	public:
		StackFSM();
		~StackFSM();
		//! StackFSM - dynamic stack finite automat. update automato
		/*!
		\param _timing type the first argument.
		*/
		void	update( float _timing );
		//! StackFSM - dynamic stack finite automat. push state
		/*!
		\param _state type the first argument.
		*/
		void	push( State * _state );
		//! StackFSM - dynamic stack finite automat. terminate all states in stack and execute _state
		/*!
		\param _state type the first argument.
		*/
		void	execute( State * _state );
		//! StackFSM - dynamic stack finite automat. terminate all states in stack
		/*!
		*
		*/
		void	terminate();
		//! StackFSM - dynamic stack finite automat. pop state
		/*!
		*
		*/
		void	pop();
		//! StackFSM - dynamic stack finite automat. get state from top of stack
		/*!
		*
		*/
		void	next();

	private:
		typedef std::list<State*> StateList;
		StateList	m_stateList;
		State * m_currentState;
	};
}