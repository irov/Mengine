// ***************************************************************
//  tswitch.h   version:  1.0   ·  date: 04/05/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	pragma once

/*
[tswitch( T , V )]
T - тип перебора.
V - значение с которым сравниваем.
*/
#	define tswitch( T , V )\
	if( int __tswitch_check = 0 );\
	else for(\
	const T &__tswitch_value(V);\
	__tswitch_check == 0;\
	++__tswitch_check )\
	{int __tswitch_open = 0;\

#	define tcase( D )\
	}if( __tswitch_open || __tswitch_value == D ){__tswitch_open = 1;

#	define tdefault break;__tswitch_open = 0;};
#	define tbreak break;__tswitch_open = 0;
