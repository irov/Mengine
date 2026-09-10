package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.Size;

public interface MengineAnalyticsEventBuilderInterface {
    MengineAnalyticsEventBuilderInterface addParameterBoolean(@NonNull @Size(min = 1, max = 40) String key, boolean value);
    MengineAnalyticsEventBuilderInterface addParameterString(@NonNull @Size(min = 1, max = 40) String key, @NonNull @Size(min = 0L,max = 100L) String value);
    MengineAnalyticsEventBuilderInterface addParameterException(@NonNull @Size(min = 1, max = 40) String key, @Nullable Exception e);
    MengineAnalyticsEventBuilderInterface addParameterThrowable(@NonNull @Size(min = 1, max = 40) String key, @NonNull Throwable e);
    MengineAnalyticsEventBuilderInterface addParameterJSON(@NonNull @Size(min = 1, max = 40) String key, @NonNull @Size(min = 0L,max = 100L) String value);
    MengineAnalyticsEventBuilderInterface addParameterLong(@NonNull @Size(min = 1, max = 40) String key, long value);
    MengineAnalyticsEventBuilderInterface addParameterDouble(@NonNull @Size(min = 1, max = 40) String key, double value);
    long logAndFlush();
    long log();
}