package org.Mengine.Plugin.Helpshift;

import android.os.Bundle;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MenginePluginPushTokenListener;
import org.Mengine.Base.MenginePluginRemoteMessageListener;
import org.Mengine.Base.MengineRemoteMessageParam;

import com.helpshift.Helpshift;
import com.helpshift.HelpshiftAuthenticationFailureReason;
import com.helpshift.HelpshiftEvent;
import com.helpshift.HelpshiftEventsListener;
import com.helpshift.UnsupportedOSVersionException;
import com.helpshift.HelpshiftInstallException;

import java.util.HashMap;
import java.util.Map;

import androidx.annotation.NonNull;

public class MengineHelpshiftPlugin extends MenginePlugin implements HelpshiftEventsListener, MenginePluginRemoteMessageListener, MenginePluginActivityListener, MenginePluginPushTokenListener {
    public static final String SERVICE_NAME = "MengineHelpshift";
    public static final boolean SERVICE_EMBEDDING = true;

    public static final String METADATA_PLATFORM_ID = "mengine.helpshift.platform_id";
    public static final String METADATA_DOMAIN = "mengine.helpshift.domain";

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        Map<String, Object> config = new HashMap<>();

        if (BuildConfig.DEBUG == true) {
            config.put("enableLogging", true);
        } else {
            config.put("enableLogging", false);
        }

        int screenOrientation = activity.getRequestedOrientation();

        config.put("screenOrientation", screenOrientation);
        config.put("notificationIcon", R.drawable.mengine_helpshift_notification_icon);

        String MengineHelpshiftPlugin_PlatformId = this.getMetaDataString(METADATA_PLATFORM_ID);

        this.logInfo("%s: %s"
            , METADATA_PLATFORM_ID
            , MengineHelpshiftPlugin_PlatformId
        );

        String MengineHelpshiftPlugin_Domain = this.getMetaDataString(METADATA_DOMAIN);

        this.logInfo("%s: %s"
            , METADATA_DOMAIN
            , MengineHelpshiftPlugin_Domain
        );

        try {
            MengineApplication application = this.getMengineApplication();

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
    public void onDestroy(MengineActivity activity) {
        Helpshift.setHelpshiftEventsListener(null);
    }

    @Override
    public void onMenginePushToken(MengineApplication application, String token) {
        Helpshift.registerPushToken(token);
    }

    @Override
    public boolean onMengineRemoteMessageReceived(MengineApplication application, MengineRemoteMessageParam message) {
        Map<String, String> data = message.REMOTEMESSAGE_DATA;

        String origin = data.get("origin");
        if (origin != null && origin.equals("helpshift")) {
            Helpshift.handlePush(data);

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
                this.logMessage("onEventOccurred SDK_SESSION_STARTED");

                this.pythonCall("onHelpshiftSessionStarted");
                break;
            case HelpshiftEvent.SDK_SESSION_ENDED:
                this.logMessage("onEventOccurred SDK_SESSION_ENDED");

                this.pythonCall("onHelpshiftSessionEnded");
                break;
            case HelpshiftEvent.RECEIVED_UNREAD_MESSAGE_COUNT:
                Object DATA_MESSAGE_COUNT = data.get(HelpshiftEvent.DATA_MESSAGE_COUNT);
                Object DATA_MESSAGE_COUNT_FROM_CACHE = data.get(HelpshiftEvent.DATA_MESSAGE_COUNT_FROM_CACHE);

                this.logMessage("onEventOccurred RECEIVED_UNREAD_MESSAGE_COUNT DATA_MESSAGE_COUNT: %s DATA_MESSAGE_COUNT_FROM_CACHE: %s"
                    , DATA_MESSAGE_COUNT
                    , DATA_MESSAGE_COUNT_FROM_CACHE
                );

                this.pythonCall("onHelpshiftUnreadMessageCount", DATA_MESSAGE_COUNT, DATA_MESSAGE_COUNT_FROM_CACHE);
                break;
            case HelpshiftEvent.CONVERSATION_STATUS:
                Object DATA_LATEST_ISSUE_ID = data.get(HelpshiftEvent.DATA_LATEST_ISSUE_ID);
                Object DATA_LATEST_ISSUE_PUBLISH_ID = data.get(HelpshiftEvent.DATA_LATEST_ISSUE_PUBLISH_ID);
                Object DATA_IS_ISSUE_OPEN = data.get(HelpshiftEvent.DATA_IS_ISSUE_OPEN);

                this.logMessage("onEventOccurred CONVERSATION_STATUS DATA_LATEST_ISSUE_ID: %s DATA_LATEST_ISSUE_PUBLISH_ID: %s DATA_IS_ISSUE_OPEN: %s"
                    , DATA_LATEST_ISSUE_ID
                    , DATA_LATEST_ISSUE_PUBLISH_ID
                    , DATA_IS_ISSUE_OPEN
                );

                this.pythonCall("onHelpshiftConversationStatus", DATA_LATEST_ISSUE_ID, DATA_LATEST_ISSUE_PUBLISH_ID, DATA_IS_ISSUE_OPEN);
                break;
            case HelpshiftEvent.WIDGET_TOGGLE:
                Object DATA_SDK_VISIBLE = data.get(HelpshiftEvent.DATA_SDK_VISIBLE);

                this.logMessage("onEventOccurred WIDGET_TOGGLE DATA_SDK_VISIBLE: %s"
                    , DATA_SDK_VISIBLE
                );

                this.pythonCall("onHelpshiftWidgetToggle", DATA_SDK_VISIBLE);
                break;
            case HelpshiftEvent.CONVERSATION_START:
                Object DATA_MESSAGE = data.get(HelpshiftEvent.DATA_MESSAGE);

                this.logMessage("onEventOccurred CONVERSATION_START DATA_MESSAGE: %s"
                    , DATA_MESSAGE
                );

                this.pythonCall("onHelpshiftConversationStart", DATA_MESSAGE);
                break;
            case HelpshiftEvent.MESSAGE_ADD:
                Object DATA_MESSAGE_TYPE = data.get(HelpshiftEvent.DATA_MESSAGE_TYPE);
                Object DATA_MESSAGE_BODY = data.get(HelpshiftEvent.DATA_MESSAGE_BODY);
                Object DATA_MESSAGE_TYPE_ATTACHMENT = data.get(HelpshiftEvent.DATA_MESSAGE_TYPE_ATTACHMENT);
                Object DATA_MESSAGE_TYPE_TEXT = data.get(HelpshiftEvent.DATA_MESSAGE_TYPE_TEXT);

                this.logMessage("onEventOccurred MESSAGE_ADD DATA_MESSAGE_TYPE: %s DATA_MESSAGE_BODY: %s DATA_MESSAGE_TYPE_ATTACHMENT: %s DATA_MESSAGE_TYPE_TEXT: %s"
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

                this.logMessage("onEventOccurred CSAT_SUBMIT DATA_CSAT_RATING: %s DATA_ADDITIONAL_FEEDBACK: %s"
                    , DATA_CSAT_RATING
                    , DATA_ADDITIONAL_FEEDBACK
                );

                this.pythonCall("onHelpshiftCSATSubmit", DATA_CSAT_RATING, DATA_ADDITIONAL_FEEDBACK);
                break;
            case HelpshiftEvent.CONVERSATION_END:
                this.logMessage("onEventOccurred CONVERSATION_END");

                this.pythonCall("onHelpshiftConversationEnd");
                break;
            case HelpshiftEvent.CONVERSATION_REJECTED:
                this.logMessage("onEventOccurred CONVERSATION_REJECTED");

                this.pythonCall("onHelpshiftConversationReject");
                break;
            case HelpshiftEvent.CONVERSATION_RESOLVED:
                this.logMessage("onEventOccurred CONVERSATION_RESOLVED");

                this.pythonCall("onHelpshiftConversationResolved");
                break;
            case HelpshiftEvent.CONVERSATION_REOPENED:
                this.logMessage("onEventOccurred CONVERSATION_REOPENED");

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
        this.logMessage("showFAQs");

        MengineActivity activity = this.getMengineActivity();

        Map<String, Object> config = new HashMap<>();
        Helpshift.showFAQs(activity, config);
    }

    public void showConversation() {
        this.logMessage("showConversation");

        MengineActivity activity = this.getMengineActivity();

        Map<String, Object> config = new HashMap<>();
        Helpshift.showConversation(activity, config);
    }

    public void showFAQSection(final String sectionPublishId) {
        this.logMessage("showFAQSection sectionPublishId: %s"
            , sectionPublishId
        );

        MengineActivity activity = this.getMengineActivity();

        Map<String, Object> config = new HashMap<>();
        Helpshift.showFAQSection(activity, sectionPublishId, config);
    }

    public void showSingleFAQ(final String publishId) {
        this.logMessage("showSingleFAQ publishId: %s"
            , publishId
        );

        MengineActivity activity = this.getMengineActivity();

        Map<String, Object> config = new HashMap<>();
        Helpshift.showSingleFAQ(activity, publishId, config);
    }

    public void setLanguage(final String language) {
        this.logMessage("setLanguage language: %s"
            , language
        );

        Helpshift.setLanguage(language);
    }
}
