#include "StatisticDetail.h"

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
            MENGINE_CONSTEXPR const Char * createStatisticNameTableFunction()
            {
                const Char * name = Statistic<Index>::getName();

                return name;
            }
            //////////////////////////////////////////////////////////////////////////
            template<size_t ... Index>
            MENGINE_CONSTEXPR auto createStatisticNameTable( std::index_sequence<Index ...> )
            {
                return Array<const Char *, sizeof ... (Index)>{{Detail::createStatisticNameTableFunction<Index>() ...}};
            }
            //////////////////////////////////////////////////////////////////////////
            template<size_t Size>
            MENGINE_CONSTEXPR auto createStatisticNameTable()
            {
                return Detail::createStatisticNameTable( std::make_index_sequence<Size>{} );
            }
            //////////////////////////////////////////////////////////////////////////
            template<size_t Index>
            MENGINE_CONSTEXPR bool createStatisticDefaultEnabledTableFunction()
            {
                bool defaultEnabled = Statistic<Index>::isDefaultEnabled();

                return defaultEnabled;
            }
            //////////////////////////////////////////////////////////////////////////
            template<size_t ... Index>
            MENGINE_CONSTEXPR auto createStatisticDefaultEnabledTable( std::index_sequence<Index ...> )
            {
                return Array<bool, sizeof ... (Index)>{{Detail::createStatisticDefaultEnabledTableFunction<Index>() ...}};
            }
            //////////////////////////////////////////////////////////////////////////
            template<size_t Size>
            MENGINE_CONSTEXPR auto createStatisticDefaultEnabledTable()
            {
                return Detail::createStatisticDefaultEnabledTable( std::make_index_sequence<Size>{} );
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getStatisticName( StatisticId _id )
        {
            const Char * name = Detail::createStatisticNameTable<MENGINE_STATISTIC_MAX_COUNT>()[_id];

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isStatisticDefaultEnabled( StatisticId _id )
        {
            bool defaultEnabled = Detail::createStatisticDefaultEnabledTable<MENGINE_STATISTIC_MAX_COUNT>()[_id];

            return defaultEnabled;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
