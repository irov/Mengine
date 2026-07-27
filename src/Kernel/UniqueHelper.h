#pragma once

#include "Config/StdAlgorithm.h"

#include <functional>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class C, class I>
        typename C::iterator findUnique( C & _container, const I & _id )
        {
            return StdAlgorithm::find_if( _container.begin(), _container.end(), [&_id]( const typename C::value_type & _value )
            {
                return _value.id == _id;
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class C, class I>
        typename C::const_iterator findUnique( const C & _container, const I & _id )
        {
            return StdAlgorithm::find_if( _container.begin(), _container.end(), [&_id]( const typename C::value_type & _value )
            {
                return _value.id == _id;
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class C, class I, class P>
        typename C::iterator findUnique( C & _container, const I & _id, P _projection )
        {
            return StdAlgorithm::find_if( _container.begin(), _container.end(), [&_id, _projection]( const typename C::value_type & _value )
            {
                return std::invoke( _projection, _value ) == _id;
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class C, class I, class P>
        typename C::const_iterator findUnique( const C & _container, const I & _id, P _projection )
        {
            return StdAlgorithm::find_if( _container.begin(), _container.end(), [&_id, _projection]( const typename C::value_type & _value )
            {
                return std::invoke( _projection, _value ) == _id;
            } );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
