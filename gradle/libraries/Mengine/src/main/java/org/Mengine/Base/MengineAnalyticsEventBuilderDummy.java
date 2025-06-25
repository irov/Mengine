package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.Size;

public class MengineAnalyticsEventBuilderDummy implements MengineAnalyticsEventBuilderInterface {
    private final String m_name;

    MengineAnalyticsEventBuilderDummy(@NonNull @Size(min = 1L,max = 40L) String name) {
        m_name = name;
    }

    @Override
    public MengineAnalyticsEventBuilderInterface addParameterBoolean(@NonNull @Size(min = 1L,max = 40L) String key, boolean value) {

        return this;
    }

    @Override
    public MengineAnalyticsEventBuilderInterface addParameterString(@NonNull @Size(min = 1L,max = 40L) String key, @NonNull @Size(min = 0L,max = 100L) String value) {
        return this;
    }

    @Override
    public MengineAnalyticsEventBuilderInterface addParameterException(@NonNull @Size(min = 1L,max = 40L) String key, @Nullable Exception e) {
        return this;
    }

    @Override
    public MengineAnalyticsEventBuilderInterface addParameterThrowable(@NonNull @Size(min = 1L,max = 40L) String key, @NonNull Throwable e) {
        return this;
    }

    @Override
    public MengineAnalyticsEventBuilderInterface addParameterJSON(@NonNull @Size(min = 1L,max = 40L) String key, @NonNull @Size(min = 0L,max = 100L) String value) {
        return this;
    }

    @Override
    public MengineAnalyticsEventBuilderInterface addParameterLong(@NonNull @Size(min = 1L,max = 40L) String key, long value) {
        return this;
    }

    @Override
    public MengineAnalyticsEventBuilderInterface addParameterDouble(@NonNull @Size(min = 1L,max = 40L) String key, double value) {
        return this;
    }

    @Override
    public long logAndFlush() {
        long timestamp = this.log();

        return timestamp;
    }

    @Override
    public long log() {
        long timestamp = MengineUtils.getTimestamp();

        return timestamp;
    }
}