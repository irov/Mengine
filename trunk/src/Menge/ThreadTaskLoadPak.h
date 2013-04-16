///*
// *	TaskLoadPak.h
// *
// *	Created by _Berserk_ on 8.5.2009
// *	Copyright 2009 Menge. All rights reserved.
// *
// */
//
//#	pragma once
//
//#	include "Config/Typedef.h"
//#	include "Core/ConstString.h"
//
//#	include "pybind/types.hpp"
//
//#	include "Task.h"
//
//namespace Menge
//{
//	class TaskLoadPak
//		: public Task
//	{
//	public:
//		TaskLoadPak( const ConstString& _pakName, const ConstString & _pakType, PyObject* _doneCallback );
//		~TaskLoadPak();
//
//	public:
//		void preMain() override;
//		void main() override;
//		void postMain() override;
//
//	protected:
//		ConstString m_pakName;
//		String m_pakPath;
//		ConstString m_pakType;
//		PyObject* m_doneCallback;
//	};
//}	// namespace Menge
