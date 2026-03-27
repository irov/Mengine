package org.Mengine.Plugin.Pilot;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.StringRes;

import org.Mengine.Base.MengineUtils;
import org.json.JSONObject;

import java.util.Map;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFragmentAnalytics;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerLogger;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineParamLoggerException;
import org.Mengine.Base.MengineParamLoggerMessage;
import org.Mengine.Base.MenginePluginExtensionManager;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import org.pilot.sdk.Pilot;
import org.pilot.sdk.PilotAction;
import org.pilot.sdk.PilotActionListener;
import org.pilot.sdk.PilotConfig;
import org.pilot.sdk.PilotException;
import org.pilot.sdk.PilotLogAttributeBuilder;
import org.pilot.sdk.PilotLogConfigBuilder;
import org.pilot.sdk.PilotLogLevel;
import org.pilot.sdk.PilotLoggerListener;
import org.pilot.sdk.PilotMetricConfigBuilder;
import org.pilot.sdk.PilotSessionAttributeBuilder;
import org.pilot.sdk.PilotSessionListener;

/**
 * Mengine plugin wrapper for Pilot SDK.
 *
 * Bridges Pilot SDK lifecycle to Mengine service lifecycle,
 * redirects SDK logs through Mengine logging system,
 * and forwards session events to C++ via nativeCall().
 *
 * Register in getAndroidPlugins():
 *   "org.Mengine.Plugin.Pilot.MenginePilotPlugin"
 */
public class MenginePilotPlugin extends MengineService implements MengineListenerApplication, MengineListenerLogger, PilotSessionListener, PilotActionListener, PilotLoggerListener {

    public static final String SERVICE_NAME = "Pilot";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final @StringRes int METADATA_API_URL = R.string.mengine_pilot_api_url;
    public static final @StringRes int METADATA_API_TOKEN = R.string.mengine_pilot_api_token;

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        String apiUrl = this.getResourceString(METADATA_API_URL);
        String apiToken = this.getResourceString(METADATA_API_TOKEN);

        if (apiToken == null || apiToken.isEmpty()) {
            this.logWarning("Pilot API token not configured, plugin disabled");
            return;
        }

        this.logInfo("%s: %s"
            , this.getResourceName(METADATA_API_URL)
            , MengineUtils.getRedactedValue(apiUrl)
        );

        this.logInfo("%s: %s"
            , this.getResourceName(METADATA_API_TOKEN)
            , MengineUtils.getRedactedValue(apiToken)
        );

        this.logInfo("Initializing Pilot SDK v%s", Pilot.VERSION);

        PilotSessionAttributeBuilder sessionAttrs = new PilotSessionAttributeBuilder()
            .put("is_publish", application.isBuildPublish())
            .put("is_debug", BuildConfig.DEBUG)
            .put("install_id", application.getInstallId())
            .put("install_timestamp", application.getInstallTimestamp())
            .put("install_version", application.getInstallVersion())
            .put("install_rnd", application.getInstallRND())
            .put("session_index", application.getSessionIndex())
            .put("session_timestamp", application.getSessionTimestamp())
            .putProvider("user_id", application::getUserId)
            .putProvider("acquisition_network", application::getAcquisitionNetwork)
            .putProvider("acquisition_campaign", application::getAcquisitionCampaign);

        PilotLogAttributeBuilder logAttrs = new PilotLogAttributeBuilder()
            .putProvider("screen_type", MengineFragmentAnalytics::getScreenType)
            .putProvider("screen_name", MengineFragmentAnalytics::getScreenName);

        PilotLogConfigBuilder logConfig = new PilotLogConfigBuilder()
            .setLogLevel(PilotLogLevel.INFO)
            .setLoggerListener(this)
            .setAttributes(logAttrs);

        PilotMetricConfigBuilder metricConfig = new PilotMetricConfigBuilder()
            .setEnabled(true)
            .setSampleIntervalMs(200);

        PilotConfig config = new PilotConfig.Builder(apiUrl, apiToken)
            .setSessionListener(this)
            .setActionListener(this)
            .setSessionAttributes(sessionAttrs)
            .setLogConfig(logConfig)
            .setMetricConfig(metricConfig)
            .build();

        Pilot.initialize(config, application.getApplicationContext());
    }

    @Override
    public void onAppPost(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        MenginePluginExtensionManager.createExtensions("Pilot", application);
    }

    @Override
    public void onAppFinalize(@NonNull MengineApplication application) {
        MenginePluginExtensionManager.finalizeExtensions("Pilot", application);

        Pilot.shutdown();
        super.onAppFinalize(application);
    }

    // ── Public API ──

    public void sendLog(@NonNull PilotLogLevel level, @NonNull String message) {
        Pilot.log(level, message);
    }

    public void acknowledgeAction(@NonNull String actionId, @Nullable JSONObject payload) {
        Pilot.acknowledgeAction(actionId, payload);
    }

    // ── PilotActionListener ──

    @Override
    public void onPilotActionReceived(@NonNull PilotAction action) {
        this.logInfo("Action received: widget=%s type=%s", action.getWidgetId(), action.getActionType());

        this.nativeCall("onPilotActionReceived", action.getId(), action.getWidgetId(), action.getActionType().getValue());
    }

    // ── PilotSessionListener ──

    @Override
    public void onPilotSessionConnecting() {
        this.logInfo("Connecting to Pilot...");
        this.nativeCall("onPilotSessionConnecting");
    }

    @Override
    public void onPilotSessionWaitingApproval(@NonNull String requestId) {
        this.logInfo("Waiting for approval: %s", requestId);
        this.nativeCall("onPilotSessionWaitingApproval", requestId);
    }

    @Override
    public void onPilotSessionStarted(@NonNull String sessionToken) {
        this.logInfo("Session started");
        this.nativeCall("onPilotSessionStarted");
    }

    @Override
    public void onPilotSessionClosed() {
        this.logInfo("Session closed");
        this.nativeCall("onPilotSessionClosed");
    }

    @Override
    public void onPilotSessionRejected() {
        this.logWarning("Connection rejected");
        this.nativeCall("onPilotSessionRejected");
    }

    @Override
    public void onPilotSessionAuthFailed() {
        this.logError("Authentication failed: invalid API token");
        this.nativeCall("onPilotSessionAuthFailed");
    }

    @Override
    public void onPilotSessionError(@NonNull PilotException exception) {
        this.logException(exception, Map.of());
        this.nativeCall("onPilotSessionError", exception.getMessage());
    }

    // ── MengineListenerLogger ──

    @Override
    public void onMengineLog(@NonNull MengineApplication application, @NonNull MengineParamLoggerMessage message) {
        PilotLogLevel level;

        switch (message.MESSAGE_LEVEL) {
            case MengineLog.LM_FATAL:
                level = PilotLogLevel.CRITICAL;
                break;
            case MengineLog.LM_ERROR:
                if (MengineLog.isFilter(message.MESSAGE_FILTER, MengineLog.LFILTER_EXCEPTION)) {
                    level = PilotLogLevel.EXCEPTION;
                } else {
                    level = PilotLogLevel.ERROR;
                }
                break;
            case MengineLog.LM_WARNING:
                level = PilotLogLevel.WARNING;
                break;
            case MengineLog.LM_MESSAGE_RELEASE:
            case MengineLog.LM_MESSAGE:
            case MengineLog.LM_INFO:
                level = PilotLogLevel.INFO;
                break;
            case MengineLog.LM_DEBUG:
            case MengineLog.LM_VERBOSE:
                level = PilotLogLevel.DEBUG;
                break;
            default:
                return;
        }

        Pilot.log(level, message.MESSAGE_DATA, message.MESSAGE_CATEGORY.toString(), message.MESSAGE_THREAD);
    }

    @Override
    public void onMengineException(@NonNull MengineApplication application, @NonNull MengineParamLoggerException exception) {
        Pilot.log(PilotLogLevel.EXCEPTION, exception.EXCEPTION_THROWABLE.getMessage(), exception.EXCEPTION_CATEGORY.toString(), exception.EXCEPTION_THREAD);
    }

    // ── PilotLoggerListener ──

    @Override
    public void onPilotLoggerMessage(@NonNull PilotLogLevel level, @NonNull String tag, @NonNull String message, @Nullable Throwable throwable) {
        switch (level) {
            case DEBUG:
                this.logDebug("[%s] %s", tag, message);
                break;
            case INFO:
                this.logInfo("[%s] %s", tag, message);
                break;
            case WARNING:
                this.logWarning("[%s] %s", tag, message);
                break;
            case ERROR:
            case CRITICAL:
                this.logError("[%s] %s", tag, message);
                break;
            case EXCEPTION:
                this.logException(throwable, Map.of("tag", tag, "message", message));
                break;
        }
    }
}
