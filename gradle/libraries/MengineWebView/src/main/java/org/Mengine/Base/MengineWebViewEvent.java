package org.Mengine.Base;

import androidx.annotation.Nullable;

import org.json.JSONObject;

public final class MengineWebViewEvent {
    public static final int QUIT = 1;
    public static final int ATTACH = 2;
    public static final int DETACH = 3;
    public static final int NAVIGATION = 4;
    public static final int FAILED = 5;
    public static final int LOADED = 6;
    public static final int MESSAGE = 7;
    public static final int HISTORY = 8;
    public static final int SNAPSHOT = 9;
    public static final int EXTERNAL = 10;
    public static final int RELOAD = 11;
    public static final int COMPLETE_REQUEST = 12;
    public static final int MEMORY = 13;
    public static final int FOCUS = 15;
    public static final int INTENT = 16;
    public static final int LOCALE = 17;
    public static final int CAPTURE = 18;
    public static final int ACTIVITY_START = 19;
    public static final int ACTIVITY_STOP = 20;
    public static final int ACTIVITY_RESUME = 21;
    public static final int ACTIVITY_PAUSE = 22;
    public static final int LOGS_READ = 23;

    public final int id;
    @Nullable
    public final JSONObject data;

    public MengineWebViewEvent(int id, @Nullable JSONObject data) {
        this.id = id;
        this.data = data;
    }
}
