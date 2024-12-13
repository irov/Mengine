package org.Mengine.Base;

import androidx.annotation.Size;

import java.lang.reflect.Field;
import java.util.Locale;

public class MengineService implements MengineServiceInterface {
    private MengineApplication m_application;
    private MengineActivity m_activity;
    private String m_serviceName;
    private boolean m_embedding;
    private Boolean m_availableStatus = null;

    public boolean onInitialize(MengineApplication application, String serviceName, boolean embedding) {
        m_application = application;
        m_serviceName = serviceName;
        m_embedding = embedding;

        return true;
    }

    public void onFinalize(MengineApplication application) {
        m_application = null;
    }

    public MengineApplication getMengineApplication() {
        return m_application;
    }

    public void setMengineActivity(MengineActivity activity) {
        m_activity = activity;
    }

    public MengineActivity getMengineActivity() {
        return m_activity;
    }

    public String getServiceName() {
        return m_serviceName;
    }

    public <T> T newInstance(String name, boolean exist) {
        T instance = MengineUtils.newInstance(m_serviceName, name, exist);

        return instance;
    }

    public boolean isAvailable() {
        if( m_availableStatus == null ) {
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

    protected void invalidInitialize(String format, Object ... args) throws MengineServiceInvalidInitializeException {
        String message = MengineLog.buildTotalMsg(format, args);

        throw new MengineServiceInvalidInitializeException(message);
    }

    public void makeToastLong(String text, long delayed) {
        if (m_activity == null) {
            return;
        }

        MengineUtils.makeToastDelayed(m_activity, text, delayed);
    }

    public void sendEvent(MengineEvent event, Object ... args) {
        MengineApplication application = this.getMengineApplication();

        application.sendEvent(event, args);
    }

    public void runOnUiThread(Runnable action) {
        if (m_activity == null) {
            return;
        }

        m_activity.runOnUiThread(action);
    }

    public void setState(String name, Object value) {
        m_application.setState(name, value);
    }

    public String logVerbose(String format, Object ... args) {
        String m = MengineLog.logVerbose(m_serviceName, format, args);

        return m;
    }

    public String logDebug(String format, Object ... args) {
        String m = MengineLog.logDebug(m_serviceName, format, args);

        return m;
    }

    public String logInfo(String format, Object ... args) {
        String m = MengineLog.logInfo(m_serviceName, format, args);

        return m;
    }

    public String logMessage(String format, Object ... args) {
        String m = MengineLog.logMessage(m_serviceName, format, args);

        return m;
    }

    public String logMessageRelease(String format, Object ... args) {
        String m = MengineLog.logMessageRelease(m_serviceName, format, args);

        return m;
    }

    public String logWarning(String format, Object ... args) {
        String m = MengineLog.logWarning(m_serviceName, format, args);

        return m;
    }

    public String logError(String format, Object ... args) {
        String m = MengineLog.logError(m_serviceName, format, args);

        return m;
    }

    public void assertionError(String format, Object ... args) {
        String msg = String.format(Locale.US, format, args);

        throw new AssertionError("[" + m_serviceName + "] " + msg);
    }

    public MengineAnalyticsEventBuilder buildEvent(@Size(min = 1L,max = 40L) String name) {
        MengineAnalyticsEventBuilder eventBuilder = MengineAnalytics.buildEvent(name);

        return eventBuilder;
    }

    public void pythonCall(String method, Object ... args) {
        if (m_activity == null) {
            return;
        }

        m_activity.pythonCall(m_serviceName, method, args);
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
            this.invalidInitialize("invalid get meta data [%s]", name);
        }

        return false;
    }

    public int getMetaDataInteger(String name) throws MengineServiceInvalidInitializeException {
        try {
            int value = m_application.getMetaDataInteger(name);

            return value;
        } catch (final RuntimeException e) {
            this.invalidInitialize("invalid get meta data [%s]", name);
        }

        return 0;
    }

    public long getMetaDataLong(String name) throws MengineServiceInvalidInitializeException {
        try {
            long value = m_application.getMetaDataInteger(name);

            return value;
        } catch (final RuntimeException e) {
            this.invalidInitialize("invalid get meta data [%s]", name);
        }

        return 0;
    }

    public String getMetaDataString(String name) throws MengineServiceInvalidInitializeException {
        try {
            String value = m_application.getMetaDataString(name);

            return value;
        } catch (final RuntimeException e) {
            this.invalidInitialize("invalid get meta data [%s]", name);
        }

        return null;
    }
}