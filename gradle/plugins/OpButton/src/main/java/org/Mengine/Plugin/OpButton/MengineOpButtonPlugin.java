package org.Mengine.Plugin.OpButton;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.StringRes;

import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerAdRevenue;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerAnalytics;
import org.Mengine.Base.MengineListenerInAppPurchase;
import org.Mengine.Base.MengineListenerLogger;
import org.Mengine.Base.MengineLog;
import org.Mengine.Base.MengineParamAdRevenue;
import org.Mengine.Base.MengineParamAnalyticsEvent;
import org.Mengine.Base.MengineParamInAppProduct;
import org.Mengine.Base.MengineParamInAppPurchase;
import org.Mengine.Base.MengineParamLoggerException;
import org.Mengine.Base.MengineParamLoggerMessage;
import org.Mengine.Base.MenginePluginExtensionManager;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineTag;
import org.Mengine.Base.MengineUtils;

import org.json.JSONObject;

import org.opbutton.sdk.OpButton;
import org.opbutton.sdk.OpButtonAction;
import org.opbutton.sdk.OpButtonActionListener;
import org.opbutton.sdk.OpButtonActionType;
import org.opbutton.sdk.OpButtonButton;
import org.opbutton.sdk.OpButtonConfig;
import org.opbutton.sdk.OpButtonException;
import org.opbutton.sdk.OpButtonLayout;
import org.opbutton.sdk.OpButtonLogConfigBuilder;
import org.opbutton.sdk.OpButtonLogLevel;
import org.opbutton.sdk.OpButtonLoggerListener;
import org.opbutton.sdk.OpButtonMetricConfigBuilder;
import org.opbutton.sdk.OpButtonSessionAttributeBuilder;
import org.opbutton.sdk.OpButtonSessionListener;
import org.opbutton.sdk.OpButtonTab;
import org.opbutton.sdk.OpButtonUI;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class MengineOpButtonPlugin extends MengineService implements MengineListenerApplication, MengineListenerLogger, MengineListenerAnalytics, MengineListenerAdRevenue, MengineListenerInAppPurchase, OpButtonSessionListener, OpButtonActionListener, OpButtonLoggerListener {
    public static final String SERVICE_NAME = "OpButton";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final @StringRes int METADATA_API_URL = R.string.mengine_opbutton_api_url;
    public static final @StringRes int METADATA_API_TOKEN = R.string.mengine_opbutton_api_token;

    private static final long METRIC_SAMPLE_INTERVAL_MILLIS = 200;

    private final Map<String, List<OpButtonButton>> m_commandButtons = new LinkedHashMap<>();

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        String apiUrl = this.getResourceString(METADATA_API_URL);
        String apiToken = this.getResourceString(METADATA_API_TOKEN);

        if (apiToken == null) {
            this.logWarning("OpButton API token not configured, plugin disabled");

            return;
        }

        if (apiToken.isEmpty() == true) {
            this.logWarning("OpButton API token not configured, plugin disabled");

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

        this.logInfo("Initializing OpButton SDK v%s", OpButton.VERSION);

        OpButtonSessionAttributeBuilder sessionAttrs = new OpButtonSessionAttributeBuilder();
        boolean publish = application.isBuildPublish();
        sessionAttrs.put("is_publish", publish);
        sessionAttrs.put("is_debug", BuildConfig.DEBUG);
        String installId = application.getInstallId();
        sessionAttrs.put("install_id", installId);
        long installTimestamp = application.getInstallTimestamp();
        sessionAttrs.put("install_timestamp", installTimestamp);
        String installVersion = application.getInstallVersion();
        sessionAttrs.put("install_version", installVersion);
        long installRND = application.getInstallRND();
        sessionAttrs.put("install_rnd", installRND);
        long sessionIndex = application.getSessionIndex();
        sessionAttrs.put("session_index", sessionIndex);
        long sessionTimestamp = application.getSessionTimestamp();
        sessionAttrs.put("session_timestamp", sessionTimestamp);
        sessionAttrs.putProvider("user_id", application::getUserId)
            .putProvider("acquisition_network", application::getAcquisitionNetwork)
            .putProvider("acquisition_campaign", application::getAcquisitionCampaign);

        OpButtonLogConfigBuilder logConfig = new OpButtonLogConfigBuilder()
            .setEnabled(true)
            .setLogLevel(OpButtonLogLevel.INFO);

        OpButtonMetricConfigBuilder metricConfig = new OpButtonMetricConfigBuilder()
            .setEnabled(true)
            .setSampleIntervalMs(METRIC_SAMPLE_INTERVAL_MILLIS);

        OpButtonConfig config = new OpButtonConfig.Builder(apiUrl, apiToken)
            .setLoggerListener(this)
            .setSessionListener(this)
            .setActionListener(this)
            .setSessionAttributes(sessionAttrs)
            .setLogConfig(logConfig)
            .setMetricConfig(metricConfig)
            .build();

        Context applicationContext = application.getApplicationContext();
        OpButton.initialize(config, applicationContext);
    }

    @Override
    public void onAppPost(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        MenginePluginExtensionManager.createExtensions("OpButton", application);

        boolean initialized = OpButton.isInitialized();
        this.activateSemaphore("OpButtonReady", initialized);
    }

    @Override
    public void onAppFinalize(@NonNull MengineApplication application) {
        MenginePluginExtensionManager.finalizeExtensions("OpButton", application);

        Set<String> commandTabs = m_commandButtons.keySet();
        List<String> tabs = new ArrayList<>(commandTabs);

        for (String tab : tabs) {
            this.removeCommandTabOnMainThread(tab);
        }

        this.deactivateSemaphore("OpButtonReady");
        OpButton.shutdown();
        super.onAppFinalize(application);
    }

    public boolean isInitialized() {
        if (OpButton.isInitialized() == false) {
            return false;
        }

        return true;
    }

    public void addCommandButton(@NonNull String tabName, @NonNull String identifier, @NonNull String title) {
        MengineUtils.performOnMainThread(() -> this.addCommandButtonOnMainThread(tabName, identifier, title));
    }

    private void addCommandButtonOnMainThread(@NonNull String tabName, @NonNull String identifier, @NonNull String title) {
        if (OpButton.isInitialized() == false) {
            return;
        }

        OpButtonUI ui = OpButton.getUI();
        OpButtonTab tab = ui.getTab(tabName);

        if (tab == null) {
            tab = ui.addTab(tabName);
            tab.setId(tabName);
        }

        OpButtonLayout layout = tab.getLayout();

        if (layout == null) {
            layout = tab.vertical();
        }

        OpButtonButton button = layout.addButton(title);
        button.setId(identifier);
        button.onClick(action -> this.nativeCall("onOpButtonCommand", tabName, identifier));
        List<OpButtonButton> buttons = m_commandButtons.computeIfAbsent(tabName, key -> new ArrayList<>());
        buttons.add(button);
    }

    public void removeCommandTab(@NonNull String tabName) {
        MengineUtils.performOnMainThread(() -> this.removeCommandTabOnMainThread(tabName));
    }

    private void removeCommandTabOnMainThread(@NonNull String tabName) {
        List<OpButtonButton> buttons = m_commandButtons.remove(tabName);

        if (buttons == null) {
            return;
        }

        for (OpButtonButton button : buttons) {
            button.onClick(null);
        }

        if (OpButton.isInitialized() == false) {
            return;
        }

        OpButtonUI ui = OpButton.getUI();
        ui.removeTab(tabName);
    }

    public void sendLog(@NonNull OpButtonLogLevel level, @NonNull String message) {
        OpButton.log(level, message);
    }

    private static void putIfNotNull(@NonNull Map<String, Object> map, @NonNull String key, @Nullable Object value) {
        if (value == null) {
            return;
        }

        map.put(key, value);
    }

    private static void putAllIfNotNull(@NonNull Map<String, Object> target, @Nullable Map<String, Object> values) {
        if (values == null) {
            return;
        }

        if (values.isEmpty() == true) {
            return;
        }

        for (Map.Entry<String, Object> entry : values.entrySet()) {
            Object value = entry.getValue();

            if (value == null) {
                continue;
            }

            String key = entry.getKey();
            target.put(key, value);
        }
    }

    private static String getOpButtonAdFormat(@Nullable MengineAdFormat adFormat) {
        if (adFormat == MengineAdFormat.ADFORMAT_BANNER) {
            return "BANNER";
        }

        if (adFormat == MengineAdFormat.ADFORMAT_MREC) {
            return "MREC";
        }

        if (adFormat == MengineAdFormat.ADFORMAT_LEADER) {
            return "LEADER";
        }

        if (adFormat == MengineAdFormat.ADFORMAT_INTERSTITIAL) {
            return "INTERSTITIAL";
        }

        if (adFormat == MengineAdFormat.ADFORMAT_APPOPEN) {
            return "APPOPEN";
        }

        if (adFormat == MengineAdFormat.ADFORMAT_REWARDED) {
            return "REWARDED";
        }

        if (adFormat == MengineAdFormat.ADFORMAT_REWARDED_INTERSTITIAL) {
            return "REWARDED_INTERSTITIAL";
        }

        if (adFormat == MengineAdFormat.ADFORMAT_NATIVE) {
            return "NATIVE";
        }

        return "UNKNOWN";
    }

    private static String getOpButtonAdMediation(@Nullable MengineAdMediation adMediation) {
        if (adMediation == MengineAdMediation.ADMEDIATION_APPLOVINMAX) {
            return "appLovin";
        }

        if (adMediation == MengineAdMediation.ADMEDIATION_ADMOB) {
            return "adMob";
        }

        return "unknown";
    }

    public void sendAnalyticsEvent(@NonNull MengineParamAnalyticsEvent param) {
        Map<String, Object> metadata = new LinkedHashMap<>();
        MengineOpButtonPlugin.putAllIfNotNull(metadata, param.ANALYTICS_BASES);
        MengineOpButtonPlugin.putAllIfNotNull(metadata, param.ANALYTICS_PARAMETERS);

        OpButton.event(param.ANALYTICS_NAME, metadata);
    }

    public void sendAdRevenue(@NonNull MengineParamAdRevenue revenue) {
        Map<String, Object> metadata = new LinkedHashMap<>();
        String adPlatform = MengineOpButtonPlugin.getOpButtonAdMediation(revenue.ADREVENUE_MEDIATION);
        MengineOpButtonPlugin.putIfNotNull(metadata, "ad_platform", adPlatform);
        MengineOpButtonPlugin.putIfNotNull(metadata, "ad_unit_name", revenue.ADREVENUE_ADUNITID);
        MengineOpButtonPlugin.putIfNotNull(metadata, "ad_source", revenue.ADREVENUE_NETWORK);
        String adFormat = MengineOpButtonPlugin.getOpButtonAdFormat(revenue.ADREVENUE_FORMAT);
        MengineOpButtonPlugin.putIfNotNull(metadata, "ad_format", adFormat);
        MengineOpButtonPlugin.putIfNotNull(metadata, "placement", revenue.ADREVENUE_PLACEMENT);
        MengineOpButtonPlugin.putIfNotNull(metadata, "precision", revenue.ADREVENUE_REVENUE_PRECISION);
        metadata.put("value", revenue.ADREVENUE_REVENUE_VALUE);
        MengineOpButtonPlugin.putIfNotNull(metadata, "currency", revenue.ADREVENUE_REVENUE_CURRENCY);

        OpButton.revenue("ad_impression", metadata);
    }

    public void sendInAppProducts(@NonNull List<MengineParamInAppProduct> products) {
        int productCount = products.size();
        List<Map<String, Object>> catalog = new ArrayList<>(productCount);

        for (MengineParamInAppProduct product : products) {
            Map<String, Object> entry = new LinkedHashMap<>();
            MengineOpButtonPlugin.putIfNotNull(entry, "product_id", product.INAPPPRODUCT_ID);
            MengineOpButtonPlugin.putIfNotNull(entry, "product_type", product.INAPPPRODUCT_TYPE);
            MengineOpButtonPlugin.putIfNotNull(entry, "name", product.INAPPPRODUCT_NAME);
            MengineOpButtonPlugin.putIfNotNull(entry, "title", product.INAPPPRODUCT_TITLE);
            MengineOpButtonPlugin.putIfNotNull(entry, "description", product.INAPPPRODUCT_DESCRIPTION);
            entry.put("price_amount_micros", product.INAPPPRODUCT_PRICE_AMOUNT_MICROS);
            MengineOpButtonPlugin.putIfNotNull(entry, "formatted_price", product.INAPPPRODUCT_PRICE_FORMATTED);
            MengineOpButtonPlugin.putIfNotNull(entry, "price_currency_code", product.INAPPPRODUCT_PRICE_CURRENCY_CODE);
            catalog.add(entry);
        }

        OpButton.setInAppProducts(catalog);
    }

    public void sendOwnedInAppProducts(@NonNull Set<String> products) {
        List<String> ownedProducts = new ArrayList<>(products);
        Collections.sort(ownedProducts);

        OpButton.setOwnedInAppProducts(ownedProducts);
    }

    public void sendInAppPurchase(@NonNull MengineParamInAppPurchase purchase) {
        List<String> productIds = new ArrayList<>();

        if (purchase.INAPPPURCHASE_PRODUCTS != null) {
            productIds.addAll(purchase.INAPPPURCHASE_PRODUCTS);
        }

        Map<String, Object> metadata = new LinkedHashMap<>();
        metadata.put("quantity", purchase.INAPPPURCHASE_QUANTITY);
        metadata.put("acknowledged", purchase.INAPPPURCHASE_ACKNOWLEDGED);
        metadata.put("consumable", purchase.INAPPPURCHASE_CONSUMABLE);

        OpButton.purchaseInApp(purchase.INAPPPURCHASE_TRANSACTION, productIds, metadata);
    }

    public void acknowledgeAction(@NonNull String actionId, @Nullable JSONObject payload) {
        OpButton.acknowledgeAction(actionId, payload);
    }

    @Override
    public void onMengineAnalyticsEvent(@NonNull MengineApplication application, @NonNull MengineParamAnalyticsEvent param) {
        this.sendAnalyticsEvent(param);
    }

    @Override
    public void onMengineAnalyticsScreenView(@NonNull MengineApplication application, @NonNull String screenType, @NonNull String screenName) {
        OpButton.changeScreen(screenType, screenName);
    }

    @Override
    public void onMengineAdRevenue(@NonNull MengineApplication application, @NonNull MengineParamAdRevenue revenue) {
        this.sendAdRevenue(revenue);
    }

    @Override
    public void onMengineSetInAppProducts(@NonNull MengineApplication application, @NonNull List<MengineParamInAppProduct> products) {
        this.sendInAppProducts(products);
    }

    @Override
    public void onMenginePurchaseInAppProduct(@NonNull MengineApplication application, @NonNull MengineParamInAppPurchase purchase) {
        this.sendInAppPurchase(purchase);
    }

    @Override
    public void onMengineOwnedInAppProducts(@NonNull MengineApplication application, @NonNull Set<String> products) {
        this.sendOwnedInAppProducts(products);
    }

    @Override
    public void onOpButtonActionReceived(@NonNull OpButtonAction action) {
        this.logInfo("Action received: widget=%s type=%s", action.getWidgetId(), action.getActionType());

        String actionId = action.getId();
        int widgetId = action.getWidgetId();
        OpButtonActionType actionType = action.getActionType();
        String actionTypeValue = actionType.getValue();
        this.nativeCall("onOpButtonActionReceived", actionId, widgetId, actionTypeValue);
    }

    @Override
    public void onOpButtonSessionConnecting() {
        this.logInfo("Connecting to OpButton...");
        this.nativeCall("onOpButtonSessionConnecting");
    }

    @Override
    public void onOpButtonSessionWaitingApproval(@NonNull String requestId) {
        this.logInfo("Waiting for approval: %s", requestId);
        this.nativeCall("onOpButtonSessionWaitingApproval", requestId);
    }

    @Override
    public void onOpButtonSessionStarted(@NonNull String sessionToken) {
        this.logInfo("Session started");
        this.nativeCall("onOpButtonSessionStarted");
    }

    @Override
    public void onOpButtonSessionClosed() {
        this.logInfo("Session closed");
        this.nativeCall("onOpButtonSessionClosed");
    }

    @Override
    public void onOpButtonSessionRejected() {
        this.logWarning("Connection rejected");
        this.nativeCall("onOpButtonSessionRejected");
    }

    @Override
    public void onOpButtonSessionAuthFailed() {
        this.logError("Authentication failed: invalid API token");
        this.nativeCall("onOpButtonSessionAuthFailed");
    }

    @Override
    public void onOpButtonSessionError(@NonNull OpButtonException exception) {
        this.logException(exception, Map.of());

        String message = exception.getMessage();
        this.nativeCall("onOpButtonSessionError", message);
    }

    @Override
    public void onMengineLog(@NonNull MengineApplication application, @NonNull MengineParamLoggerMessage message) {
        MengineTag serviceTag = this.getServiceTag();

        if (message.MESSAGE_CATEGORY.equals(serviceTag) == true) {
            return;
        }

        OpButtonLogLevel level;

        switch (message.MESSAGE_LEVEL) {
            case MengineLog.LM_FATAL:
                level = OpButtonLogLevel.CRITICAL;
                break;
            case MengineLog.LM_ERROR:
                if (MengineLog.isFilter(message.MESSAGE_FILTER, MengineLog.LFILTER_EXCEPTION) == true) {
                    level = OpButtonLogLevel.EXCEPTION;
                } else {
                    level = OpButtonLogLevel.ERROR;
                }
                break;
            case MengineLog.LM_WARNING:
                level = OpButtonLogLevel.WARNING;
                break;
            case MengineLog.LM_MESSAGE_RELEASE:
            case MengineLog.LM_MESSAGE:
            case MengineLog.LM_INFO:
                level = OpButtonLogLevel.INFO;
                break;
            case MengineLog.LM_DEBUG:
            case MengineLog.LM_VERBOSE:
                level = OpButtonLogLevel.DEBUG;
                break;
            default:
                return;
        }

        OpButton.log(level, message.MESSAGE_DATA, message.MESSAGE_CATEGORY.toString(), message.MESSAGE_THREAD);
    }

    @Override
    public void onMengineException(@NonNull MengineApplication application, @NonNull MengineParamLoggerException exception) {
        MengineTag serviceTag = this.getServiceTag();

        if (exception.EXCEPTION_CATEGORY.equals(serviceTag) == true) {
            return;
        }

        String message = exception.EXCEPTION_THROWABLE.getMessage();
        if (message == null) {
            message = exception.EXCEPTION_THROWABLE.toString();
        }
        OpButton.log(OpButtonLogLevel.EXCEPTION, message, exception.EXCEPTION_CATEGORY.toString(), exception.EXCEPTION_THREAD);
    }

    @Override
    public void onOpButtonLoggerMessage(@NonNull OpButtonLogLevel level, @NonNull String tag, @NonNull String message, @Nullable Throwable throwable) {
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
