#	pragma once

template<typename O, typename I> inline
O this_cast_template(O,I val)
{
	return static_cast<O>(val);
}

#	define this_cast(val) (this_cast_template(this,val))