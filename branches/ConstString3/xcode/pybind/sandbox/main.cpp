#	include <pybind/pybind.hpp>

#	include <stdio.h>

class Bar
{
public:
	void foo()
	{
		printf("foo\n");
	}

	void min( float _a, float _b )
	{
		float m = _a < _b? _a : _b;
		printf("min(%f, %f) == %f\n", _a, _b, m);
	}

	int getData()
	{
		return data;
	}

	void setData( int _data )
	{
		data = _data;
	}

protected:
	int data;
};


void test_Bar( Bar * _bar )
{
	_bar->foo();
	_bar->min(3.f, 4.f);
}

struct Vec2
{
	Vec2()
	{}

	Vec2( float _x, float _y )
		: x(_x)
		, y(_y)
	{}

	float x;
	float y;
};

class Helper
{
public:
	void helper_for_script_function( Bar * _bar, float _a, float _b )
	{
		_bar->min(_a * 2.f, _b + 2.f);
	}
};

void main()
{
	pybind::initialize();

	PyObject * module = pybind::module_init("MyModule");

	pybind::set_currentmodule( module );

	Helper * helper = new Helper;

	pybind::class_<Bar>("Bar")
		.def("foo", &Bar::foo)
		.def("min", &Bar::min)
		.def_property("data", &Bar::getData, &Bar::setData)
		.def_static("helper_for_script_function", helper, &Helper::helper_for_script_function)
		;

	pybind::class_<Vec2>("Vec2")
		.def( pybind::init<float, float>() )
		.def_member( "x", &Vec2::x)
		.def_member( "y", &Vec2::y)
		;

	pybind::def("test", &test_Bar );
		
	bool exist;
	pybind::module_import("Test1", exist);
}