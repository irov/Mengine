#pragma once

#include "Config/Config.h"

#include <algorithm>
#include <string>
#include <vector>

namespace Mengine
{
    namespace StdAlgorithm
    {
        using std::find;
        using std::find_if;
        using std::remove_if;
        using std::erase_if;
        using std::for_each;
        using std::transform;
        using std::generate_n;
        using std::sort;
        using std::stable_sort;
        using std::reverse;
        using std::count;
        using std::count_if;
        using std::all_of;
        using std::any_of;
        using std::remove;
        using std::copy;
        using std::fill;
        using std::fill_n;
        using std::min;
        using std::max;
        using std::clamp;
        using std::min_element;
        using std::max_element;
        using std::swap;

        template<class Container, class T>
        bool container_has( const Container & _container, const T & _value )
        {
            typename Container::const_iterator it_begin = _container.begin();
            typename Container::const_iterator it_end = _container.end();
            bool exists = std::find( it_begin, it_end, _value ) != it_end;

            return exists;
        }
    }
}
