#	pragma once

namespace Menge
{
	//! State - state of finite automat
	/*! 
	* 
	*/
	class	State
	{
	public:
		virtual ~State(){};
		//! State - state of finite automat. update state. 
		/*!
		*/
		virtual void	update( float _timing ) = 0;
		//! State - state of finite automat. run state. 
		/*!
		*/
		virtual void	run() = 0;
		//! State - state of finite automat. end state. 
		/*!
		*/
		virtual void	end() = 0;
		//! State - state of finite automat. check if state is terminated
		/*!
		\return termination flag
		*/
		virtual bool	isEnded() const = 0;
	};
}