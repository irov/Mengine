package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineAdResponseInterface {
    void onAdRevenuePaid(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, String placement, double revenue);
    void onAdShowSuccess(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, String placement);
    void onAdShowFailed(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, String placement, int errorCode);
    void onAdUserRewarded(@NonNull MengineAdMediation mediation, @NonNull MengineAdFormat format, String placement, String label, int amount);
}