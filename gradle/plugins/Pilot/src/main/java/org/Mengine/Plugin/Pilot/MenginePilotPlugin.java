package org.Mengine.Plugin.Pilot;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.StringRes;

import org.json.JSONObject;

import java.util.Map;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import org.pilot.sdk.Pilot;
import org.pilot.sdk.PilotAction;
import org.pilot.sdk.PilotActionListener;
import org.pilot.sdk.PilotConfig;
import org.pilot.sdk.PilotException;
import org.pilot.sdk.PilotLogLevel;
import org.pilot.sdk.PilotLogger;
import org.pilot.sdk.PilotPanel;
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
public class MenginePilotPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity, PilotSessionListener, PilotActionListener {

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

        this.logInfo("Initializing Pilot SDK: %s", apiUrl);

        PilotConfig config = new PilotConfig.Builder(apiUrl, apiToken)
            .setLogLevel(PilotLogLevel.INFO)
            .setLogger(new MenginePilotLogger())
            .build();

        Pilot.initialize(config);
        Pilot.addSessionListener(this);
        Pilot.addActionListener(this);
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        if (Pilot.isInitialized()) {
            Pilot.connect();
        }
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        if (Pilot.isInitialized()) {
            Pilot.disconnect();
        }
    }

    @Override
    public void onAppFinalize(@NonNull MengineApplication application) {
        Pilot.shutdown();
        super.onAppFinalize(application);
    }

    // ── Public API ──

    public void submitPanel(@NonNull PilotPanel panel) {
        Pilot.submitPanel(panel);
    }

    public void updatePanel() {
        Pilot.updatePanel();
    }

    public void sendLog(@NonNull String level, @NonNull String message) {
        Pilot.log(PilotLogLevel.valueOf(level.toUpperCase()), message);
    }

    public void acknowledgeAction(@NonNull String actionId, @Nullable JSONObject payload) {
        Pilot.acknowledgeAction(actionId, payload);
    }

    // ── PilotActionListener ──

    @Override
    public void onPilotActionReceived(@NonNull PilotAction action) {
        this.logInfo("Action received: widget=%s type=%s", action.getWidgetId(), action.getActionType());

        this.nativeCall("onPilotActionReceived", action.getId(), action.getWidgetId(), action.getActionType());
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
    public void onPilotSessionError(@NonNull PilotException exception) {
        this.logException(exception, Map.of());
        this.nativeCall("onPilotSessionError", exception.getMessage());
    }

    // ── Logger redirect ──

    /**
     * Routes Pilot SDK logs through Mengine logging system.
     */
    private class MenginePilotLogger implements PilotLogger {
        @Override
        public void log(@NonNull PilotLogLevel level, @NonNull String tag, @NonNull String message, @Nullable Throwable throwable) {
            switch (level) {
                case DEBUG:
                    MenginePilotPlugin.this.logDebug("[%s] %s", tag, message);
                    break;
                case INFO:
                    MenginePilotPlugin.this.logInfo("[%s] %s", tag, message);
                    break;
                case WARNING:
                    MenginePilotPlugin.this.logWarning("[%s] %s", tag, message);
                    break;
                case ERROR:
                case CRITICAL:
                    MenginePilotPlugin.this.logError("[%s] %s", tag, message);
                    break;
                case EXCEPTION:
                    MenginePilotPlugin.this.logException(throwable, Map.of("tag", tag, "message", message));
                    break;
            }
        }
    }
}
