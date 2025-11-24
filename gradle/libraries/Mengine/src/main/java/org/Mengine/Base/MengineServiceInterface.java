package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.AnyRes;
import androidx.annotation.BoolRes;
import androidx.annotation.IntegerRes;
import androidx.annotation.NonNull;
import androidx.annotation.Size;
import androidx.annotation.StringRes;

import org.json.JSONObject;

import java.util.Map;

public interface MengineServiceInterface {
    default boolean onAvailable(@NonNull MengineApplication application) {
        //Override

        return true;
    }

    void onAppInitialize(@NonNull MengineApplication application, @NonNull String serviceName, boolean embedding);
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
    MengineTag getServiceTag();

    <T> T getService(Class<T> cls);
    Object newInstance(@NonNull String name, boolean required, Object ... args);

    boolean isAvailable();
    boolean isEmbedding();

    boolean hasOption(@NonNull String option);
    int getOptionValueInteger(@NonNull String option, int defaultValue);
    long getOptionValueLong(@NonNull String option, long defaultValue);
    String getOptionValueString(@NonNull String option, String defaultValue);

    void setStatisticInteger(@Size(min = 1L, max = 40L) String key, long value);
    void increaseStatisticInteger(@Size(min = 1L, max = 40L) String key, long value);
    void decreaseStatisticInteger(@Size(min = 1L, max = 40L) String key, long value);
    void setStatisticDouble(@Size(min = 1L, max = 40L) String key, double value);
    void increaseStatisticDouble(@Size(min = 1L, max = 40L) String key, double value);
    void decreaseStatisticDouble(@Size(min = 1L, max = 40L) String key, double value);

    JSONObject getServiceConfig();
    String getServiceConfigOptString(@NonNull String key, @NonNull String defaultValue);

    boolean runOnUiThread(String doc, Runnable action);

    String getUserId();

    void setState(@NonNull @Size(min = 1L, max = 1024L) String name, Object value);

    String logVerbose(@NonNull String format, Object ... args);
    String logDebug(@NonNull String format, Object ... args);
    String logInfo(@NonNull String format, Object ... args);
    String logMessage(@NonNull String format, Object ... args);
    String logMessageRelease(@NonNull String format, Object ... args);
    String logWarning(@NonNull String format, Object ... args);
    String logError(@NonNull String format, Object ... args);

    void logException(@NonNull Throwable e, @NonNull Map<String, Object> attributes);

    void assertionError(@NonNull String format, Object ... args);

    MengineAnalyticsEventBuilderInterface buildEvent(@Size(min = 1L, max = 40L) String name);

    void nativeCall(@NonNull String method, Object ... args);

    void activateSemaphore(@NonNull String name);
    void deactivateSemaphore(@NonNull String name);

    String getResourceName(@AnyRes int id);
    boolean getResourceBoolean(@BoolRes int id);
    int getResourceInteger(@IntegerRes int id);
    String getResourceString(@StringRes int id);
}
