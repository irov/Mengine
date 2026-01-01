package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineAdResponseInterface {
    void onAdShowSuccess(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, @NonNull String placement);
    void onAdShowFailed(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, @NonNull String placement, int errorCode);
    void onAdRevenuePaid(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, @NonNull String placement, double revenue);
    void onAdUserRewarded(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, @NonNull String placement, String label, int amount);
}