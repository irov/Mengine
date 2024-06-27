#include "NotificationName.h"

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
            constexpr const Char * createNotificationNameTableFunction()
            {
                const Char * name = Notificator<Index>::getName();

                return name;
            }
            //////////////////////////////////////////////////////////////////////////
            template<size_t ... Index>
            constexpr auto createNotificationNameTable( std::index_sequence<Index ...> )
            {
                return Array<const Char *, sizeof ... (Index)>{{Detail::createNotificationNameTableFunction<Index>() ...}};
            }
            //////////////////////////////////////////////////////////////////////////
            template<size_t Size>
            constexpr auto createNotificationNameTable()
            {
                return Detail::createNotificationNameTable( std::make_index_sequence<Size>{} );
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getNotificationName( NotificatorId _id )
        {
            const Char * name = Detail::createNotificationNameTable<MENGINE_NOTIFICATOR_MAX_COUNT>()[_id];

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
