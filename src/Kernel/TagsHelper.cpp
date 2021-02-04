#include "TagsHelper.h"

namespace Mengine
{
    namespace Helper
    {
        String tagsToString( const Tags & _tags )
        {
            String str;

            str += '[';

            const VectorConstString & values = _tags.getValues();

            for( const ConstString & value : values )
            {
                str.append( value.c_str(), value.size() );

                str += ' ';
            }

            str += ']';

            return str;
        }
    }
}