package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public class MengineParamLoggerMessage {
    public MengineParamLoggerMessage(@NonNull MengineTag category, @NonNull String thread, int level, int filter, @Nullable String file, int line, @Nullable String function, @NonNull String data) {
        this.MESSAGE_CATEGORY = category;
        this.MESSAGE_THREAD = thread;
        this.MESSAGE_LEVEL = level;
        this.MESSAGE_FILTER = filter;
        this.MESSAGE_FILE = file;
        this.MESSAGE_LINE = line;
        this.MESSAGE_FUNCTION = function;
        this.MESSAGE_DATA = data;
    }

    @NonNull public final MengineTag MESSAGE_CATEGORY;
    @NonNull public final String MESSAGE_THREAD;
    public final int MESSAGE_LEVEL;
    public final int MESSAGE_FILTER;
    @Nullable public final String MESSAGE_FILE;
    public final int MESSAGE_LINE;
    @Nullable public final String MESSAGE_FUNCTION;
    @NonNull public final String MESSAGE_DATA;
}