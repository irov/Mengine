#include "AppleDetail.h"
#include "AppleString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void AppleGetParamsNSDictionary( NSDictionary * _dictionary, MapParams * const _map )
        {
            if( _dictionary == nil )
            {
                return;
            }
            
            [_dictionary enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL * stop) {
                ConstString key_str = Helper::NSStringToConstString( key );
                const Char * value_str = [[NSString stringWithFormat:@"%@", value] UTF8String];
                
                _map->emplace(std::make_pair(key_str, String(value_str)));
            }];
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
