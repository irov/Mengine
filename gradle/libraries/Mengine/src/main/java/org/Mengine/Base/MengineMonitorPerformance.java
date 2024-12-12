package org.Mengine.Base;

import java.util.Map;
import java.util.Timer;

public class MengineMonitorPerformance extends MengineService implements MengineListenerApplication, MengineListenerEngine, MengineListenerAnalytics {
    public static final String SERVICE_NAME = "MengineMPerformance";

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
    private long m_totalUsedMemory = 0;

    @Override
    public void onAppInit(MengineApplication application, boolean isMainProcess) {
        if (isMainProcess == false) {
            return;
        }

        m_traceStartup = application.startPerformanceTrace(TRACE_STARTUP, Map.of());

        this.createTraceFPS(application, "Initialize", "Initialize");
        this.createTraceMemory(application, "Initialize", "Initialize");
    }

    @Override
    public void onAppTerminate(MengineApplication application) {
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

            this.logMessage("FPS: %d", fps);
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

            m_measurementMemoryCount = 0;
            m_totalUsedMemory = 0;
        }
    }

    protected void traceMemory() {
        synchronized (this) {
            Runtime runtime = Runtime.getRuntime();

            long usedMemory = runtime.totalMemory() - runtime.freeMemory();

            m_measurementMemoryCount += 1;
            m_totalUsedMemory += usedMemory;

            long averageUsedMemory = m_totalUsedMemory / m_measurementMemoryCount;
            long averageUsedMemoryMB = averageUsedMemory / 1024 / 1024;

            m_traceMemory.putMetric("used_memory", averageUsedMemoryMB);

            this.logMessage("Memory: %d", averageUsedMemory);
        }
    }

    @Override
    public void onMenginePlatformRun(MengineApplication application) {
        m_traceStartup.stop();
        m_traceStartup = null;

        m_timerFPS = MengineUtils.scheduleAtFixedRate(0, 1000, () -> {
            MengineMonitorPerformance.this.traceFPS();
        });

        m_timerMemory = MengineUtils.scheduleAtFixedRate(0, 1000, () -> {
            MengineMonitorPerformance.this.traceMemory();
        });
    }

    @Override
    public void onMengineAnalyticsScreenView(MengineApplication application, String screenType, String screenName) {
        synchronized (this) {
            this.createTraceFPS(application, screenType, screenName);
            this.createTraceMemory(application, screenType, screenName);
        }
    }
}