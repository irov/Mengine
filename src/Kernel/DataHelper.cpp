#include "DataHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void stringFromData( const Data & _data, String * const _str )
        {
            const Data::value_type * data_buffer = _data.data();
            Data::size_type data_size = _data.size();

            _str->assign( (const String::value_type *)data_buffer, data_size );
        }
        //////////////////////////////////////////////////////////////////////////
        void dataFromString( const String & _str, Data * const _data )
        {
            const String::value_type * str_buffer = _str.c_str();
            String::size_type str_size = _str.size();

            const Data::value_type * data_begin = (const Data::value_type *)str_buffer;
            const Data::value_type * data_end = data_begin + str_size;

            _data->assign( data_begin, data_end );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
