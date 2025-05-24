package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.Map;

public class MengineParamLoggerException {
    public MengineParamLoggerException(@NonNull String category, @NonNull Throwable exception, @NonNull Map<String, Object> attributes) {
        this.EXCEPTION_CATEGORY = category;
        this.EXCEPTION_THROWABLE = exception;
        this.EXCEPTION_ATTRIBUTES = attributes;
    }

    @NonNull public final String EXCEPTION_CATEGORY;
    @NonNull public final Throwable EXCEPTION_THROWABLE;
    @NonNull public final Map<String, Object> EXCEPTION_ATTRIBUTES;
}