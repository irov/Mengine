package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerAdRevenue extends MengineServiceInterface {
    void onMengineAdRevenue(@NonNull MengineApplication application, @NonNull MengineAdRevenueParam revenue);
}