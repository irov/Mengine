package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.Map;
import java.util.Timer;

public class MengineMonitorPerformanceService extends MengineService implements MengineListenerApplication, MengineListenerEngine, MengineListenerAnalytics {
    public static final String SERVICE_NAME = "MonitorPerformance";

    private static final String TRACE_STARTUP = "mng_startup";
    private static final String TRACE_FPS = "mng_fps";
    private static final String TRACE_MEMORY = "mng_memory";

    private MenginePerformanceTrace m_traceStartup;

    private MenginePerformanceTrace m_traceFPS;
    private Timer m_timerFPS;
    private long m_lastRenderFrameCount = 0;
    private long m_lastRenderFrameTimestamp = 0;
    private long m_totalRenderFrameCount = 0;
    private long m_totalRenderFrameTimestamp = 0;

    private MenginePerformanceTrace m_traceMemory;
    private Timer m_timerMemory;
    private long m_measurementMemoryCount = 0;
    private long m_totalJVMUsedMemory = 0;
    private long m_totalNativeUsedMemory = 0;
    private long m_totalRenderTextureMemory = 0;

    @Override
    public void onAppInit(@NonNull MengineApplication application, boolean isMainProcess) {
        if (isMainProcess == false) {
            return;
        }

        m_traceStartup = application.startPerformanceTrace(TRACE_STARTUP, Map.of());
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        if (m_timerFPS != null) {
            m_timerFPS.cancel();
            m_timerFPS = null;
        }

        if (m_timerMemory != null) {
            m_timerMemory.cancel();
            m_timerMemory = null;
        }

        if (m_traceStartup != null) {
            m_traceStartup.stop();
            m_traceStartup = null;
        }

        if (m_traceFPS != null) {
            m_traceFPS.stop();
            m_traceFPS = null;
        }

        if (m_traceMemory != null) {
            m_traceMemory.stop();
            m_traceMemory = null;
        }
    }

    protected void createTraceFPS(MengineApplication application, String screenType, String screenName) {
        synchronized (this) {
            if (m_traceFPS != null) {
                m_traceFPS.stop();
                m_traceFPS = null;
            }

            m_traceFPS = application.startPerformanceTrace(TRACE_FPS, Map.of());

            m_traceFPS.putAttribute("screen_type", screenType);
            m_traceFPS.putAttribute("screen_name", screenName);

            m_totalRenderFrameCount = 0;
            m_totalRenderFrameTimestamp = 0;
        }
    }

    protected void traceFPS() {
        synchronized (this) {
            long renderFrameTimestamp = MengineUtils.getTimestamp();
            long renderFrameCount = MengineNative.AndroidStatistic_getRenderFrameCount();

            long frameCount = renderFrameCount - m_lastRenderFrameCount;
            long frameTime = renderFrameTimestamp - m_lastRenderFrameTimestamp;

            if (frameTime == 0) {
                return;
            }

            m_totalRenderFrameCount += frameCount;
            m_totalRenderFrameTimestamp += frameTime;

            long fps = m_totalRenderFrameCount * 1000 / m_totalRenderFrameTimestamp;

            m_traceFPS.putMetric("fps", fps);

            m_lastRenderFrameCount = renderFrameCount;
            m_lastRenderFrameTimestamp = renderFrameTimestamp;
        }
    }

    protected void createTraceMemory(MengineApplication application, String screenType, String screenName) {
        synchronized (this) {
            if (m_traceMemory != null) {
                m_traceMemory.stop();
                m_traceMemory = null;
            }

            m_traceMemory = application.startPerformanceTrace(TRACE_MEMORY, Map.of());

            m_traceMemory.putAttribute("screen_type", screenType);
            m_traceMemory.putAttribute("screen_name", screenName);

            long usedJVMMemory = MengineUtils.getUsedJVMMemory();
            long usedNativeMemory = MengineNative.AndroidStatistic_getAllocatorSize();
            long usedRenderTextureMemory = MengineNative.AndroidStatistic_getRenderTextureAllocSize();

            long usedJVMMemoryMB = usedJVMMemory / 1024 / 1024;
            long usedNativeMemoryMB = usedNativeMemory / 1024 / 1024;
            long usedRenderTextureMemoryMB = usedRenderTextureMemory / 1024 / 1024;

            m_traceMemory.putMetric("used_jvm_memory", usedJVMMemoryMB);
            m_traceMemory.putMetric("used_native_memory", usedNativeMemoryMB);
            m_traceMemory.putMetric("used_render_texture_memory", usedRenderTextureMemoryMB);

            m_measurementMemoryCount = 0;
            m_totalJVMUsedMemory = 0;
            m_totalNativeUsedMemory = 0;
            m_totalRenderTextureMemory = 0;
        }
    }

    protected void traceMemory() {
        synchronized (this) {
            long usedJVMMemory = MengineUtils.getUsedJVMMemory();
            long usedNativeMemory = MengineNative.AndroidStatistic_getAllocatorSize();
            long usedRenderTextureMemory = MengineNative.AndroidStatistic_getRenderTextureAllocSize();

            m_measurementMemoryCount += 1;
            m_totalJVMUsedMemory += usedJVMMemory;
            m_totalNativeUsedMemory += usedNativeMemory;
            m_totalRenderTextureMemory += usedRenderTextureMemory;

            long averageJVMUsedMemory = m_totalJVMUsedMemory / m_measurementMemoryCount;
            long averageJVMUsedMemoryMB = averageJVMUsedMemory / 1024 / 1024;

            long averageNativeUsedMemory = m_totalNativeUsedMemory / m_measurementMemoryCount;
            long averageNativeUsedMemoryMB = averageNativeUsedMemory / 1024 / 1024;

            long averageRenderTextureMemory = m_totalRenderTextureMemory / m_measurementMemoryCount;
            long averageRenderTextureMemoryMB = averageRenderTextureMemory / 1024 / 1024;

            m_traceMemory.putMetric("used_jvm_memory", averageJVMUsedMemoryMB);
            m_traceMemory.putMetric("used_native_memory", averageNativeUsedMemoryMB);
            m_traceMemory.putMetric("used_render_texture_memory", averageRenderTextureMemoryMB);

            this.logDebug("Memory JVM: %dMb %dKb Native: %dMb %dKb Texture: %dMb %dKb"
                , averageJVMUsedMemoryMB
                , (averageJVMUsedMemory / 1024) % 1024
                , averageNativeUsedMemoryMB
                , (averageNativeUsedMemory / 1024) % 1024
                , averageRenderTextureMemoryMB
                , (averageRenderTextureMemory / 1024) % 1024
            );
        }
    }

    @Override
    public void onMenginePlatformRun(@NonNull MengineApplication application) {
        m_traceStartup.stop();
        m_traceStartup = null;

        m_timerFPS = MengineUtils.scheduleAtFixedRate(0, 1000, () -> {
            this.traceFPS();
        });

        m_timerMemory = MengineUtils.scheduleAtFixedRate(0, 5000, () -> {
            this.traceMemory();
        });
    }

    @Override
    public void onMengineAnalyticsScreenView(@NonNull MengineApplication application, String screenType, String screenName) {
        synchronized (this) {
            this.createTraceFPS(application, screenType, screenName);
            this.createTraceMemory(application, screenType, screenName);
        }
    }
}