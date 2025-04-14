package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

public class MengineService implements MengineServiceInterface {
    private MengineApplication m_application;
    private MengineActivity m_activity;
    private String m_serviceName;
    private boolean m_embedding;
    private Boolean m_availableStatus = null;

    public boolean onInitialize(@NonNull MengineApplication application, String serviceName, boolean embedding) {
        m_application = application;
        m_serviceName = serviceName;
        m_embedding = embedding;

        return true;
    }

    public void onFinalize(@NonNull MengineApplication application) {
        m_application = null;
    }

    public Bundle onSave(@NonNull MengineApplication application) {
        return null;
    }

    public void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
    }

    public MengineApplication getMengineApplication() {
        return m_application;
    }

    public void setMengineActivity(@NonNull MengineActivity activity) {
        m_activity = activity;
    }

    public void removeMengineActivity() {
        m_activity = null;
    }

    public MengineActivity getMengineActivity() {
        return m_activity;
    }

    @Override
    public String getServiceName() {
        return m_serviceName;
    }

    @SuppressWarnings("unchecked")
    public <T> T getService(Class<T> cls) {
        T plugin = m_application.getService(cls);

        return plugin;
    }

    public Object newInstance(String name, boolean exist, Object ... args) {
        Object instance = MengineUtils.newInstance(m_serviceName, name, exist, args);

        return instance;
    }

    public boolean isAvailable() {
        if (m_availableStatus == null) {
            m_availableStatus = this.onAvailable(m_application);
        }

        return m_availableStatus;
    }

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
        String message = MengineLog.buildTotalMsg(format, args);

        throw new MengineServiceInvalidInitializeException(message);
    }

    public void makeToastLong(long delayed, String format, Object ... args) {
        if (m_activity == null) {
            return;
        }

        String message = MengineLog.buildTotalMsg(format, args);

        MengineUtils.makeToastDelayed(m_activity, message, delayed);
    }

    public void sendEvent(MengineEvent event, Object ... args) {
        m_application.sendEvent(event, args);
    }

    public void runOnUiThread(Runnable action) {
        if (m_activity == null) {
            return;
        }

        m_activity.runOnUiThread(action);
    }

    public void setState(@NonNull @Size(min = 1L,max = 1024L) String name, Object value) {
        m_application.setState(name, value);
    }

    public String logVerbose(String format, Object ... args) {
        String m = MengineLog.logVerbose("Mengine" + m_serviceName, format, args);

        return m;
    }

    public String logDebug(String format, Object ... args) {
        String m = MengineLog.logDebug("Mengine" + m_serviceName, format, args);

        return m;
    }

    public String logInfo(String format, Object ... args) {
        String m = MengineLog.logInfo("Mengine" + m_serviceName, format, args);

        return m;
    }

    public String logMessage(String format, Object ... args) {
        String m = MengineLog.logMessage("Mengine" + m_serviceName, format, args);

        return m;
    }

    public String logMessageProtected(String format, Object ... args) {
        String m = MengineLog.logMessageProtected("Mengine" + m_serviceName, format, args);

        return m;
    }

    public String logMessageRelease(String format, Object ... args) {
        String m = MengineLog.logMessageRelease("Mengine" + m_serviceName, format, args);

        return m;
    }

    public String logWarning(String format, Object ... args) {
        String m = MengineLog.logWarning("Mengine" + m_serviceName, format, args);

        return m;
    }

    public String logError(String format, Object ... args) {
        String m = MengineLog.logError("Mengine" + m_serviceName, format, args);

        return m;
    }

    public void assertionError(String format, Object ... args) {
        MengineUtils.throwAssertionError("Mengine" + m_serviceName, null, format, args);
    }

    public MengineAnalyticsEventBuilder buildEvent(@Size(min = 1L,max = 40L) String name) {
        MengineAnalyticsEventBuilder eventBuilder = MengineAnalytics.buildEvent(name);

        return eventBuilder;
    }

    public void pythonCall(String method, Object ... args) {
        if (m_activity == null) {
            return;
        }

        m_activity.pythonCall("Mengine" + m_serviceName, method, args);
    }

    public void activateSemaphore(String name) {
        if (m_activity == null) {
            return;
        }

        m_activity.activateSemaphore(name);
    }

    public void deactivateSemaphore(String name) {
        if (m_activity == null) {
            return;
        }

        m_activity.deactivateSemaphore(name);
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