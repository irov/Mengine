package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import java.util.Arrays;

public class MengineService implements MengineServiceInterface {
    private MengineApplication m_application;
    private String m_serviceName;
    private String m_serviceTag;
    private boolean m_embedding;
    private Boolean m_availableStatus = null;

    @Override
    public boolean onInitialize(@NonNull MengineApplication application, String serviceName, boolean embedding) {
        m_application = application;
        m_serviceName = serviceName;
        m_embedding = embedding;

        m_serviceTag = "Mengine" + m_serviceName;

        return true;
    }

    @Override
    public void onFinalize(@NonNull MengineApplication application) {
        m_application = null;
    }

    public MengineApplication getMengineApplication() {
        return m_application;
    }

    @Override
    public String getServiceName() {
        return m_serviceName;
    }

    @Override
    public String getServiceTag() {
        return m_serviceTag;
    }

    @SuppressWarnings("unchecked")
    public <T> T getService(Class<T> cls) {
        T plugin = m_application.getService(cls);

        return plugin;
    }

    public Object newInstance(String name, boolean exist, Object ... args) {
        ClassLoader cl = this.getClass().getClassLoader();

        Object instance = MengineUtils.newInstance(cl, m_serviceName, name, exist, args);

        return instance;
    }

    public boolean isAvailable() {
        if (m_availableStatus == null) {
            m_availableStatus = this.onAvailable(m_application);
        }

        return m_availableStatus;
    }

    @Override
    public boolean isEmbedding() {
        return m_embedding;
    }

    public boolean hasOption(String option) {
        boolean value = m_application.hasOption(option);

        return value;
    }

    public int getOptionValueInteger(String option, int defaultValue) {
        int value = m_application.getOptionValueInteger(option, defaultValue);

        return value;
    }

    public long getOptionValueLong(String option, long defaultValue) {
        long value = m_application.getOptionValueLong(option, defaultValue);

        return value;
    }

    public String getOptionValueString(String option, String defaultValue) {
        String value = m_application.getOptionValueString(option, defaultValue);

        return value;
    }

    public void setStatisticInteger(@Size(min = 1L,max = 40L) String key, long value) {
        MengineStatistic.setInteger(key, value);
    }

    public void increaseStatisticInteger(@Size(min = 1L,max = 40L) String key, long value) {
        MengineStatistic.increaseInteger(key, value);
    }

    public void decreaseStatisticInteger(@Size(min = 1L,max = 40L) String key, long value) {
        MengineStatistic.decreaseInteger(key, value);
    }

    public void increaseStatisticDouble(@Size(min = 1L,max = 40L) String key, double value) {
        MengineStatistic.increaseDouble(key, value);
    }

    public void decreaseStatisticDouble(@Size(min = 1L,max = 40L) String key, double value) {
        MengineStatistic.decreaseDouble(key, value);
    }

    protected void invalidInitialize(String format, Object ... args) throws MengineServiceInvalidInitializeException {
        this.setState("invalid.service", m_serviceName);

        String message = MengineLog.buildTotalMsg(format, args);

        throw new MengineServiceInvalidInitializeException(message);
    }

    public void makeToastDelayed(long delayed, String format, Object ... args) {
        if (MengineActivity.INSTANCE == null) {
            this.logError("invalid make toast format: %s args: %s"
                , format
                , Arrays.toString(args)
            );

            return;
        }

        MengineUtils.makeToastDelayed(MengineActivity.INSTANCE, delayed, format, args);
    }

    public boolean runOnUiThread(String doc, Runnable action) {
        if (MengineActivity.INSTANCE == null) {
            this.logError("invalid run [%s] on UI thread"
                , doc
            );

            return false;
        }

        MengineActivity.INSTANCE.runOnUiThread(action);

        return true;
    }

    public void setState(@NonNull @Size(min = 1L,max = 1024L) String name, Object value) {
        m_application.setState(name, value);
    }

    public String logVerbose(String format, Object ... args) {
        String t = this.getServiceTag();
        String m = MengineLog.logVerbose(t, format, args);

        return m;
    }

    public String logDebug(String format, Object ... args) {
        String t = this.getServiceTag();
        String m = MengineLog.logDebug(t, format, args);

        return m;
    }

    public String logInfo(String format, Object ... args) {
        String t = this.getServiceTag();
        String m = MengineLog.logInfo(t, format, args);

        return m;
    }

    public String logMessage(String format, Object ... args) {
        String t = this.getServiceTag();
        String m = MengineLog.logMessage(t, format, args);

        return m;
    }

    public String logMessageProtected(String format, Object ... args) {
        String t = this.getServiceTag();
        String m = MengineLog.logMessageProtected(t, format, args);

        return m;
    }

    public String logMessageRelease(String format, Object ... args) {
        String t = this.getServiceTag();
        String m = MengineLog.logMessageRelease(t, format, args);

        return m;
    }

    public String logWarning(String format, Object ... args) {
        String t = this.getServiceTag();
        String m = MengineLog.logWarning(t, format, args);

        return m;
    }

    public String logError(String format, Object ... args) {
        String t = this.getServiceTag();
        String m = MengineLog.logError(t, format, args);

        return m;
    }

    public void assertionError(String format, Object ... args) {
        String t = this.getServiceTag();
        MengineUtils.throwAssertionError(t, null, format, args);
    }

    public MengineAnalyticsEventBuilder buildEvent(@Size(min = 1L,max = 40L) String name) {
        MengineAnalyticsEventBuilder eventBuilder = MengineAnalytics.buildEvent(name);

        return eventBuilder;
    }

    public void pythonCall(String method, Object ... args) {
        String t = this.getServiceTag();
        m_application.pythonCall(t, method, args);
    }

    public void activateSemaphore(String name) {
        m_application.activateSemaphore(name);
    }

    public void deactivateSemaphore(String name) {
        m_application.deactivateSemaphore(name);
    }

    public boolean getMetaDataBoolean(String name) throws MengineServiceInvalidInitializeException {
        try {
            boolean value = m_application.getMetaDataBoolean(name);

            return value;
        } catch (final RuntimeException e) {
            this.invalidInitialize("invalid get meta data: %s e: %s", name, e.getMessage());
        }

        return false;
    }

    public int getMetaDataInteger(String name) throws MengineServiceInvalidInitializeException {
        try {
            int value = m_application.getMetaDataInteger(name);

            return value;
        } catch (final RuntimeException e) {
            this.invalidInitialize("invalid get meta data: %s e: %s", name, e.getMessage());
        }

        return 0;
    }

    public long getMetaDataLong(String name) throws MengineServiceInvalidInitializeException {
        try {
            long value = m_application.getMetaDataInteger(name);

            return value;
        } catch (final RuntimeException e) {
            this.invalidInitialize("invalid get meta data: %s e: %s", name, e.getMessage());
        }

        return 0;
    }

    public String getMetaDataString(String name) throws MengineServiceInvalidInitializeException {
        try {
            String value = m_application.getMetaDataString(name);

            return value;
        } catch (final RuntimeException e) {
            this.invalidInitialize("invalid get meta data: %s e: %s", name, e.getMessage());
        }

        return null;
    }

    protected boolean getPreferenceBoolean(@NonNull String name, boolean defaultValue) {
        boolean value = MenginePreferences.getPreferenceBoolean(m_application, m_serviceName, name, defaultValue);

        return value;
    }

    protected void setPreferenceBoolean(@NonNull String name, boolean value) {
        MenginePreferences.setPreferenceBoolean(m_application, m_serviceName, name, value);
    }
}