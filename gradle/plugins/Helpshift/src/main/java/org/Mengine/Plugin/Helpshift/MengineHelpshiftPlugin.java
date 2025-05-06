package org.Mengine.Plugin.Helpshift;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerPushToken;
import org.Mengine.Base.MengineListenerRemoteMessage;
import org.Mengine.Base.MengineParamRemoteMessage;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import com.helpshift.Helpshift;
import com.helpshift.HelpshiftAuthenticationFailureReason;
import com.helpshift.HelpshiftEvent;
import com.helpshift.HelpshiftEventsListener;
import com.helpshift.UnsupportedOSVersionException;
import com.helpshift.HelpshiftInstallException;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

import androidx.annotation.NonNull;

public class MengineHelpshiftPlugin extends MengineService implements HelpshiftEventsListener, MengineListenerRemoteMessage, MengineListenerApplication, MengineListenerPushToken {
    public static final String SERVICE_NAME = "Helpshift";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final String METADATA_PLATFORM_ID = "mengine.helpshift.platform_id";
    public static final String METADATA_DOMAIN = "mengine.helpshift.domain";

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        Map<String, Object> config = new HashMap<>();

        if (BuildConfig.DEBUG == true) {
            config.put("enableLogging", true);
        } else {
            config.put("enableLogging", false);
        }

        config.put("enableInAppNotification", true);

        String MengineHelpshiftPlugin_PlatformId = this.getMetaDataString(METADATA_PLATFORM_ID);

        this.logInfo("%s: %s"
            , METADATA_PLATFORM_ID
            , MengineUtils.getRedactedValue(MengineHelpshiftPlugin_PlatformId)
        );

        String MengineHelpshiftPlugin_Domain = this.getMetaDataString(METADATA_DOMAIN);

        this.logInfo("%s: %s"
            , METADATA_DOMAIN
            , MengineUtils.getRedactedValue(MengineHelpshiftPlugin_Domain)
        );

        try {
            Helpshift.install(application, MengineHelpshiftPlugin_PlatformId, MengineHelpshiftPlugin_Domain, config);
        } catch (final UnsupportedOSVersionException e) {
            this.invalidInitialize("Android OS versions prior to Lollipop (< SDK 21) are not supported.");

            return;
        } catch (final HelpshiftInstallException e) {
            this.invalidInitialize("install exception: %s"
                , e.getMessage()
            );

            return;
        }

        Helpshift.setHelpshiftEventsListener(this);
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication activity) {
        Helpshift.setHelpshiftEventsListener(null);
    }

    @Override
    public void onMengineChangePushToken(@NonNull MengineApplication application, String token) {
        Helpshift.registerPushToken(token);
    }

    @Override
    public boolean onMengineRemoteMessageReceived(@NonNull MengineApplication application, @NonNull MengineParamRemoteMessage message) {
        Map<String, Object> data = message.REMOTEMESSAGE_DATA;

        String origin = (String)data.get("origin");
        if (Objects.equals(origin, "helpshift") == true) {
            Map<String, String> helpshifData = new HashMap<>();
            data.putAll(message.REMOTEMESSAGE_DATA);
            Helpshift.handlePush(helpshifData);

            return true;
        }

        return false;
    }

    @Override
    public void onMengineRemoteMessageNewToken(MengineApplication application, String token) {
        Helpshift.registerPushToken(token);
    }

    @Override
    public void onEventOccurred(@NonNull String eventName, Map<String, Object> data) {
        switch (eventName) {
            case HelpshiftEvent.SDK_SESSION_STARTED:
                this.logInfo("onEventOccurred SDK_SESSION_STARTED");

                this.pythonCall("onHelpshiftSessionStarted");
                break;
            case HelpshiftEvent.SDK_SESSION_ENDED:
                this.logInfo("onEventOccurred SDK_SESSION_ENDED");

                this.pythonCall("onHelpshiftSessionEnded");
                break;
            case HelpshiftEvent.RECEIVED_UNREAD_MESSAGE_COUNT:
                Object DATA_MESSAGE_COUNT = data.get(HelpshiftEvent.DATA_MESSAGE_COUNT);
                Object DATA_MESSAGE_COUNT_FROM_CACHE = data.get(HelpshiftEvent.DATA_MESSAGE_COUNT_FROM_CACHE);

                this.logInfo("onEventOccurred RECEIVED_UNREAD_MESSAGE_COUNT DATA_MESSAGE_COUNT: %s DATA_MESSAGE_COUNT_FROM_CACHE: %s"
                    , DATA_MESSAGE_COUNT
                    , DATA_MESSAGE_COUNT_FROM_CACHE
                );

                this.pythonCall("onHelpshiftUnreadMessageCount", DATA_MESSAGE_COUNT, DATA_MESSAGE_COUNT_FROM_CACHE);
                break;
            case HelpshiftEvent.CONVERSATION_STATUS:
                Object DATA_LATEST_ISSUE_ID = data.get(HelpshiftEvent.DATA_LATEST_ISSUE_ID);
                Object DATA_LATEST_ISSUE_PUBLISH_ID = data.get(HelpshiftEvent.DATA_LATEST_ISSUE_PUBLISH_ID);
                Object DATA_IS_ISSUE_OPEN = data.get(HelpshiftEvent.DATA_IS_ISSUE_OPEN);

                this.logInfo("onEventOccurred CONVERSATION_STATUS DATA_LATEST_ISSUE_ID: %s DATA_LATEST_ISSUE_PUBLISH_ID: %s DATA_IS_ISSUE_OPEN: %s"
                    , DATA_LATEST_ISSUE_ID
                    , DATA_LATEST_ISSUE_PUBLISH_ID
                    , DATA_IS_ISSUE_OPEN
                );

                this.pythonCall("onHelpshiftConversationStatus", DATA_LATEST_ISSUE_ID, DATA_LATEST_ISSUE_PUBLISH_ID, DATA_IS_ISSUE_OPEN);
                break;
            case HelpshiftEvent.WIDGET_TOGGLE:
                Object DATA_SDK_VISIBLE = data.get(HelpshiftEvent.DATA_SDK_VISIBLE);

                this.logInfo("onEventOccurred WIDGET_TOGGLE DATA_SDK_VISIBLE: %s"
                    , DATA_SDK_VISIBLE
                );

                this.pythonCall("onHelpshiftWidgetToggle", DATA_SDK_VISIBLE);
                break;
            case HelpshiftEvent.CONVERSATION_START:
                Object DATA_MESSAGE = data.get(HelpshiftEvent.DATA_MESSAGE);

                this.logInfo("onEventOccurred CONVERSATION_START DATA_MESSAGE: %s"
                    , DATA_MESSAGE
                );

                this.pythonCall("onHelpshiftConversationStart", DATA_MESSAGE);
                break;
            case HelpshiftEvent.MESSAGE_ADD:
                Object DATA_MESSAGE_TYPE = data.get(HelpshiftEvent.DATA_MESSAGE_TYPE);
                Object DATA_MESSAGE_BODY = data.get(HelpshiftEvent.DATA_MESSAGE_BODY);
                Object DATA_MESSAGE_TYPE_ATTACHMENT = data.get(HelpshiftEvent.DATA_MESSAGE_TYPE_ATTACHMENT);
                Object DATA_MESSAGE_TYPE_TEXT = data.get(HelpshiftEvent.DATA_MESSAGE_TYPE_TEXT);

                this.logInfo("onEventOccurred MESSAGE_ADD DATA_MESSAGE_TYPE: %s DATA_MESSAGE_BODY: %s DATA_MESSAGE_TYPE_ATTACHMENT: %s DATA_MESSAGE_TYPE_TEXT: %s"
                    , DATA_MESSAGE_TYPE
                    , DATA_MESSAGE_BODY
                    , DATA_MESSAGE_TYPE_ATTACHMENT
                    , DATA_MESSAGE_TYPE_TEXT
                );

                this.pythonCall("onHelpshiftMessageAdd", DATA_MESSAGE_TYPE, DATA_MESSAGE_BODY, DATA_MESSAGE_TYPE_ATTACHMENT, DATA_MESSAGE_TYPE_TEXT);
                break;
            case HelpshiftEvent.CSAT_SUBMIT:
                Object DATA_CSAT_RATING = data.get(HelpshiftEvent.DATA_CSAT_RATING);
                Object DATA_ADDITIONAL_FEEDBACK = data.get(HelpshiftEvent.DATA_ADDITIONAL_FEEDBACK);

                this.logInfo("onEventOccurred CSAT_SUBMIT DATA_CSAT_RATING: %s DATA_ADDITIONAL_FEEDBACK: %s"
                    , DATA_CSAT_RATING
                    , DATA_ADDITIONAL_FEEDBACK
                );

                this.pythonCall("onHelpshiftCSATSubmit", DATA_CSAT_RATING, DATA_ADDITIONAL_FEEDBACK);
                break;
            case HelpshiftEvent.CONVERSATION_END:
                this.logInfo("onEventOccurred CONVERSATION_END");

                this.pythonCall("onHelpshiftConversationEnd");
                break;
            case HelpshiftEvent.CONVERSATION_REJECTED:
                this.logInfo("onEventOccurred CONVERSATION_REJECTED");

                this.pythonCall("onHelpshiftConversationReject");
                break;
            case HelpshiftEvent.CONVERSATION_RESOLVED:
                this.logInfo("onEventOccurred CONVERSATION_RESOLVED");

                this.pythonCall("onHelpshiftConversationResolved");
                break;
            case HelpshiftEvent.CONVERSATION_REOPENED:
                this.logInfo("onEventOccurred CONVERSATION_REOPENED");

                this.pythonCall("onHelpshiftConversationReopened");
                break;
            default:
                this.logError("[ERROR] onEventOccurred UNKNOWN: %s"
                    , eventName
                );

                break;
        }
    }

    @Override
    public void onUserAuthenticationFailure(HelpshiftAuthenticationFailureReason reason) {
        switch (reason) {
            case REASON_AUTH_TOKEN_NOT_PROVIDED:
                this.logError("[ERROR] onUserAuthenticationFailure REASON_AUTH_TOKEN_NOT_PROVIDED");

                this.pythonCall("onHelpshiftAuthenticationFailureAuthTokenNotProvided");
            case REASON_INVALID_AUTH_TOKEN:
                this.logError("[ERROR] onUserAuthenticationFailure REASON_INVALID_AUTH_TOKEN");

                this.pythonCall("onHelpshiftAuthenticationFailureInvalidAuthToken");
            case UNKNOWN:
                this.logError("[ERROR] onUserAuthenticationFailure UNKNOWN");

                this.pythonCall("onHelpshiftAuthenticationFailureUnknown");
                break;
        }
    }

    public void showFAQs() {
        if (MengineActivity.INSTANCE == null) {
            this.logWarning("showFAQs invalid activity");

            return;
        }

        this.logInfo("showFAQs");

        Map<String, Object> config = new HashMap<>();
        Helpshift.showFAQs(MengineActivity.INSTANCE, config);
    }

    public void showConversation() {
        if (MengineActivity.INSTANCE == null) {
            this.logWarning("showConversation invalid activity");

            return;
        }

        this.logInfo("showConversation");

        Map<String, Object> config = new HashMap<>();
        Helpshift.showConversation(MengineActivity.INSTANCE, config);
    }

    public void showFAQSection(final String sectionPublishId) {
        if (MengineActivity.INSTANCE == null) {
            this.logWarning("showFAQSection invalid activity");

            return;
        }

        this.logInfo("showFAQSection sectionPublishId: %s"
            , sectionPublishId
        );

        Map<String, Object> config = new HashMap<>();
        Helpshift.showFAQSection(MengineActivity.INSTANCE, sectionPublishId, config);
    }

    public void showSingleFAQ(final String publishId) {
        if (MengineActivity.INSTANCE == null) {
            this.logWarning("showSingleFAQ invalid activity");

            return;
        }

        this.logInfo("showSingleFAQ publishId: %s"
            , publishId
        );

        Map<String, Object> config = new HashMap<>();
        Helpshift.showSingleFAQ(MengineActivity.INSTANCE, publishId, config);
    }

    public void setLanguage(final String language) {
        this.logInfo("setLanguage language: %s"
            , language
        );

        Helpshift.setLanguage(language);
    }
}
