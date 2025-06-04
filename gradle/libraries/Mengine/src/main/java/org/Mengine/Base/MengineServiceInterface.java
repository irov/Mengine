package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

import java.util.Map;

public interface MengineServiceInterface {
    default boolean onAvailable(@NonNull MengineApplication application) {
        //Override

        return true;
    }

    void onAppInitialize(@NonNull MengineApplication application, String serviceName, boolean embedding);
    void onAppFinalize(@NonNull MengineApplication application);

    void onActivityInitialize(@NonNull MengineActivity activity);
    void onActivityFinalize(@NonNull MengineActivity activity);

    MengineApplication getMengineApplication();
    MengineActivity getMengineActivity();

    default void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
        //Override
    }

    default Bundle onSave(@NonNull MengineApplication application) {
        return null;
    }

    String getServiceName();
    String getServiceTag();

    <T> T getService(Class<T> cls);
    Object newInstance(String name, boolean required, Object ... args);

    boolean isAvailable();
    boolean isEmbedding();

    boolean hasOption(String option);
    int getOptionValueInteger(String option, int defaultValue);
    long getOptionValueLong(String option, long defaultValue);
    String getOptionValueString(String option, String defaultValue);

    void setStatisticInteger(@Size(min = 1L,max = 40L) String key, long value);
    void increaseStatisticInteger(@Size(min = 1L,max = 40L) String key, long value);
    void decreaseStatisticInteger(@Size(min = 1L,max = 40L) String key, long value);
    void increaseStatisticDouble(@Size(min = 1L,max = 40L) String key, double value);
    void decreaseStatisticDouble(@Size(min = 1L,max = 40L) String key, double value);

    void makeToastDelayed(long delayed, String format, Object ... args);
    boolean runOnUiThread(String doc, Runnable action);

    String getUserId();

    void setState(@NonNull @Size(min = 1L,max = 1024L) String name, Object value);

    String logVerbose(String format, Object ... args);
    String logDebug(String format, Object ... args);
    String logInfo(String format, Object ... args);
    String logMessage(String format, Object ... args);
    String logMessageProtected(String format, Object ... args);
    String logMessageRelease(String format, Object ... args);
    String logWarning(String format, Object ... args);
    String logError(String format, Object ... args);

    void logException(@NonNull Throwable e, @NonNull Map<String, Object> attributes);

    void assertionError(String format, Object ... args);

    MengineAnalyticsEventBuilder buildEvent(@Size(min = 1L,max = 40L) String name);

    void pythonCall(String method, Object ... args);

    void activateSemaphore(String name);
    void deactivateSemaphore(String name);

    boolean getMetaDataBoolean(String name) throws MengineServiceInvalidInitializeException;
    int getMetaDataInteger(String name) throws MengineServiceInvalidInitializeException;
    long getMetaDataLong(String name) throws MengineServiceInvalidInitializeException;
    String getMetaDataString(String name) throws MengineServiceInvalidInitializeException;
}
