package org.Mengine.Base;

import androidx.annotation.AnyRes;
import androidx.annotation.BoolRes;
import androidx.annotation.IntegerRes;
import androidx.annotation.NonNull;
import androidx.annotation.Size;
import androidx.annotation.StringRes;

import org.json.JSONObject;

import java.util.Arrays;
import java.util.Map;

public class MengineService implements MengineServiceInterface {
    private MengineApplication m_application;
    private MengineActivity m_activity;
    private String m_serviceName;
    private MengineTag m_serviceTag;
    private boolean m_embedding;
    private Boolean m_availableStatus = null;
    private Boolean m_availableAnalytics = null;

    @Override
    public void onAppInitialize(@NonNull MengineApplication application, String serviceName, boolean embedding) {
        m_application = application;
        m_serviceName = serviceName;
        m_embedding = embedding;

        m_serviceTag = MengineTag.of("Mengine" + m_serviceName);
    }

    @Override
    public void onAppFinalize(@NonNull MengineApplication application) {
        m_application = null;
    }

    @Override
    public void onActivityInitialize(@NonNull MengineActivity activity) {
        m_activity = activity;
    }

    @Override
    public void onActivityFinalize(@NonNull MengineActivity activity) {
        m_activity = null;
    }

    @Override
    public MengineApplication getMengineApplication() {
        return m_application;
    }

    @Override
    public MengineActivity getMengineActivity() {
        return m_activity;
    }

    @Override
    public String getServiceName() {
        return m_serviceName;
    }

    @Override
    public MengineTag getServiceTag() {
        return m_serviceTag;
    }

    @SuppressWarnings("unchecked")
    @Override
    public <T> T getService(Class<T> cls) {
        T plugin = m_application.getService(cls);

        return plugin;
    }

    @Override
    public Object newInstance(String name, boolean required, Object ... args) {
        ClassLoader cl = this.getClass().getClassLoader();

        Object instance = MengineUtils.newInstance(cl, m_serviceTag, name, required, args);

        return instance;
    }

    @Override
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

    @Override
    public boolean hasOption(String option) {
        boolean value = MengineUtils.hasOption(m_application, option);

        return value;
    }

    @Override
    public int getOptionValueInteger(String option, int defaultValue) {
        int value = MengineUtils.getOptionValueInteger(m_application, option, defaultValue);

        return value;
    }

    @Override
    public long getOptionValueLong(String option, long defaultValue) {
        long value = MengineUtils.getOptionValueLong(m_application, option, defaultValue);

        return value;
    }

    @Override
    public String getOptionValueString(String option, String defaultValue) {
        String value = MengineUtils.getOptionValueString(m_application, option, defaultValue);

        return value;
    }

    @Override
    public void setStatisticInteger(@Size(min = 1L,max = 40L) String key, long value) {
        MengineStatistic.setInteger(key, value);
    }

    @Override
    public void increaseStatisticInteger(@Size(min = 1L,max = 40L) String key, long value) {
        if (value == 0L) {
            return;
        }

        MengineStatistic.increaseInteger(key, value);
    }

    @Override
    public void decreaseStatisticInteger(@Size(min = 1L,max = 40L) String key, long value) {
        if (value == 0L) {
            return;
        }

        MengineStatistic.decreaseInteger(key, value);
    }

    @Override
    public void setStatisticDouble(@Size(min = 1L,max = 40L) String key, double value) {
        MengineStatistic.setDouble(key, value);
    }

    @Override
    public void increaseStatisticDouble(@Size(min = 1L,max = 40L) String key, double value) {
        if (value == 0.0) {
            return;
        }

        MengineStatistic.increaseDouble(key, value);
    }

    @Override
    public void decreaseStatisticDouble(@Size(min = 1L,max = 40L) String key, double value) {
        if (value == 0.0) {
            return;
        }

        MengineStatistic.decreaseDouble(key, value);
    }

    @Override
    public boolean runOnUiThread(String doc, Runnable action) {
        if (m_activity == null) {
            this.logError("invalid run [%s] on UI thread"
                , doc
            );

            return false;
        }

        m_activity.runOnUiThread(action);

        return true;
    }

    @Override
    public String getUserId() {
        String userId = m_application.getUserId();

        return userId;
    }

    @Override
    public void setState(@NonNull @Size(min = 1L,max = 1024L) String name, Object value) {
        m_application.setState(name, value);
    }

    @Override
    public String logVerbose(String format, Object ... args) {
        MengineTag t = this.getServiceTag();
        String m = MengineLog.logVerbose(t, format, args);

        return m;
    }

    @Override
    public String logDebug(String format, Object ... args) {
        MengineTag t = this.getServiceTag();
        String m = MengineLog.logDebug(t, format, args);

        return m;
    }

    @Override
    public String logInfo(String format, Object ... args) {
        MengineTag t = this.getServiceTag();
        String m = MengineLog.logInfo(t, format, args);

        return m;
    }

    @Override
    public String logMessage(String format, Object ... args) {
        MengineTag t = this.getServiceTag();
        String m = MengineLog.logMessage(t, format, args);

        return m;
    }

    @Override
    public String logMessageProtected(String format, Object ... args) {
        MengineTag t = this.getServiceTag();
        String m = MengineLog.logMessageProtected(t, format, args);

        return m;
    }

    @Override
    public String logMessageRelease(String format, Object ... args) {
        MengineTag t = this.getServiceTag();
        String m = MengineLog.logMessageRelease(t, format, args);

        return m;
    }

    @Override
    public String logWarning(String format, Object ... args) {
        MengineTag t = this.getServiceTag();
        String m = MengineLog.logWarning(t, format, args);

        return m;
    }

    @Override
    public String logError(String format, Object ... args) {
        MengineTag t = this.getServiceTag();
        String m = MengineLog.logError(t, format, args);

        return m;
    }

    @Override
    public void logException(@NonNull Throwable e, @NonNull Map<String, Object> attributes) {
        MengineTag t = this.getServiceTag();
        MengineLog.logException(t, e, attributes);
    }

    @Override
    public void assertionError(String format, Object ... args) {
        MengineTag t = this.getServiceTag();
        MengineUtils.throwAssertionError(t, null, format, args);
    }

    private boolean availableAnalytics() {
        if (m_availableAnalytics == null) {
            JSONObject config_analytics = MengineFragmentRemoteConfig.INSTANCE.getRemoteConfigValue("plugin_" + m_serviceName);

            if (config_analytics == null) {
                m_availableAnalytics = false;
            } else {
                boolean enabled = config_analytics.optBoolean("enable_analytics", false);

                m_availableAnalytics = enabled;
            }
        }

        return m_availableAnalytics;
    }

    @Override
    public MengineAnalyticsEventBuilderInterface buildEvent(@Size(min = 1L,max = 40L) String name) {
        if (this.availableAnalytics() == false) {
            MengineAnalyticsEventBuilderInterface eventBuilderDummy = MengineAnalytics.buildEventDummy(name);
            eventBuilderDummy.addParameterString("service", m_serviceName);

            return eventBuilderDummy;
        }

        MengineAnalyticsEventBuilderInterface eventBuilder = MengineAnalytics.buildEvent(name);
        eventBuilder.addParameterString("service", m_serviceName);

        return eventBuilder;
    }

    @Override
    public void nativeCall(String method, Object ... args) {
        MengineTag t = this.getServiceTag();
        m_application.nativeCall(t.toString(), method, args);
    }

    @Override
    public void activateSemaphore(String name) {
        m_application.activateSemaphore(name);
    }

    @Override
    public void deactivateSemaphore(String name) {
        m_application.deactivateSemaphore(name);
    }

    @Override
    public String getResourceName(@AnyRes int id) {
        String value = m_application.getResourceName(id);

        return value;
    }

    @Override
    public boolean getResourceBoolean(@BoolRes int id) {
        boolean value = m_application.getResourceBoolean(id);

        return value;
    }

    @Override
    public int getResourceInteger(@IntegerRes int id) {
        int value = m_application.getResourceInteger(id);

        return value;
    }

    @Override
    public String getResourceString(@StringRes int id) {
        String value = m_application.getResourceString(id);

        return value;
    }

    protected void invalidInitialize(String format, Object ... args) throws MengineServiceInvalidInitializeException {
        this.setState("invalid.service", m_serviceName);

        String message = MengineLog.buildTotalMsg(format, args);

        throw new MengineServiceInvalidInitializeException(message);
    }
}