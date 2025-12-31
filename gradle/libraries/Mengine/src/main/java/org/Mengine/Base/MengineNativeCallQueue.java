package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.List;

public class MengineNativeCallQueue {
    private static final MengineTag TAG = MengineTag.of("MNGNativeCallQueue");

    private static class NativeCall {
        final public String plugin;
        final public String method;
        final public Object[] args;

        public NativeCall(String plugin, String method, Object[] args) {
            this.plugin = plugin;
            this.method = method;
            this.args = args;
        }
    }

    private static final Object m_syncCalls = new Object();
    private static final List<NativeCall> m_calls = new ArrayList<>();
    private static final List<NativeCall> m_callsAux = new ArrayList<>();

    public static void pushCall(@NonNull String plugin, String method, Object ... args) {
        if (BuildConfig.DEBUG == true) {
            MengineLog.logInfo(TAG, "native call pushed: plugin [%s] method [%s] args [%s]"
                , plugin
                , method
                , java.util.Arrays.toString(args)
            );
        }

        synchronized (m_syncCalls) {
            m_calls.add(new NativeCall(plugin, method, args));
        }
    }

    public static void processCalls(@NonNull MengineApplication application) {
        synchronized (m_syncCalls) {
            if (m_calls.isEmpty() == true) {
                return;
            }

            m_callsAux.addAll(m_calls);
            m_calls.clear();
        }

        for (NativeCall call : m_callsAux) {
            final String plugin = call.plugin;
            final String method = call.method;
            final Object[] args = call.args;

            application.setState("native.call", plugin + "." + method);

            MengineNative.AndroidKernelService_call(plugin, method, args);
        }

        m_callsAux.clear();
    }
}

