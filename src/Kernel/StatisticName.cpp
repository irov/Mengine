#include "StatisticName.h"

#include "Kernel/Array.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            template<size_t Index>
            constexpr const Char * createStatisticNameTableFunction()
            {
                const Char * name = Statistic<Index>::getName();

                return name;
            }
            //////////////////////////////////////////////////////////////////////////
            template<size_t ... Index>
            constexpr auto createStatisticNameTable( std::index_sequence<Index ...> )
            {
                return Array<const Char *, sizeof ... (Index)>{{Detail::createStatisticNameTableFunction<Index>() ...}};
            }
            //////////////////////////////////////////////////////////////////////////
            template<size_t Size>
            constexpr auto createStatisticNameTable()
            {
                return Detail::createStatisticNameTable( std::make_index_sequence<Size>{} );
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getStatisticName( uint32_t _id )
        {
            const Char * name = Detail::createStatisticNameTable<MENGINE_STATISTIC_MAX_COUNT>()[_id];

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
