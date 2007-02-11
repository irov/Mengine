#	include "lua_boost.h"

#	include <stdio.h>

void cpp_test( int a, int b )
{	
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

void cpp_test_2( float a, float b, const char * str )
{	
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

	printf("%f, [%s]\n", a + b, str );
}

float add_vec( float a, float b )
{
	return a + b;
}

void print_f( float f )
{
	printf("%f\n",f);
}

void print( const char * _ch )
{
	printf("%s\n",_ch);
}

void main()
{
	printf("lua init ok\n");

	lua_boost::lua_boost lb;

	lb.reg_function("cpp_test",&cpp_test);
	lb.reg_function("cpp_test_2",&cpp_test_2);
	lb.reg_function("print",&print);
	lb.reg_function("add_vec",&add_vec);
	lb.reg_function("print_f",&print_f);
	
	
	printf("lua register function ok\n");

	lb.do_file( "lua_test.lua" );
	printf("lua dofile lua_test.lua ok\n");

	lua_boost::lua_functor lf1 = lb.functor("lua_test");
	lua_boost::lua_functor lf2 = lb.functor("lua_test_2");
	
	int res1 = lf1.call() % 1 % 2 % "bububu" % lua_boost::ret<int>();
	printf("lua call_function lua_test(1,2) = %d ok\n", res1 );

	int res2 = lf2.call() % lua_boost::ret<int>();
	printf("lua call_function lua_test_2() = %d ok\n", res2 );
}