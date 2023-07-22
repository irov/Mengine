package org.Mengine.Plugin.Helpshift;

import android.os.Bundle;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import com.helpshift.Helpshift;
import com.helpshift.HelpshiftAuthenticationFailureReason;
import com.helpshift.HelpshiftEvent;
import com.helpshift.HelpshiftEventsListener;
import com.helpshift.UnsupportedOSVersionException;
import com.helpshift.HelpshiftInstallException;

import java.util.HashMap;
import java.util.Map;

import androidx.annotation.NonNull;

public class MengineHelpshiftPlugin extends MenginePlugin implements HelpshiftEventsListener {
    public static final String PLUGIN_NAME = "Helpshift";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_PLATFORM_ID = "mengine.helpshift.platform_id";
    public static final String PLUGIN_METADATA_DOMAIN = "mengine.helpshift.domain";

    /**
     * <p>
     * void showFAQs(Throwable throwable)
     * void showConversation(String msg)
     * <p>
     * void showFAQSection(final String sectionPublishId)
     * void showSingleFAQ(final String publishId)
     * <p>
     * language contains has
     * [en, de, es, fr, it, ru, zh-Hans,zh-Hant, zh-HK, zh-Hant-HK, zh-SG, zh-Hant-SG, pt,
     * ko, ja, tr, nl, cs, hu,, th, sl, vi, ar, pl, no, sv, fi, ro, el, da, ms, iw, sk, uk,
     * ca, hr, bn, bg, gu, hi, kn, lv, ml, mr, pa, fa, ta, te]
     * void setLanguage(final String language)
     *
     *
     * Events
     * -onHelpshiftSessionStarted
     * -onHelpshiftSessionEnded
     * -onHelpshiftReceivedUnreadMessage(int count, boolean fromCache)
     * -onHelpshiftConversationStatus(String issueId,String publishId,boolean issueOpen)
     * -onHelpshiftWidgetToggle(boolean visible);
     * -onHelpshiftMessageAdd(String body,String type);
     * -onHelpshiftConverstationStart(String text);
     * -onHelpshiftCSATSubmit"(String rating,String feedback);
     * -onHelpshiftConverstationEnded
     * -onHelpshiftConverstationRejected
     * -onHelpshiftConverstationResolved
     * -onHelpshiftConverstationReopen

     * -onHelpshiftAuthenticationFailureInvalidAuthToken
     * -onHelpshiftAuthenticationFailureTokenNotProvided
     * -onHelpshiftAuthenticationFailureUnknown
     */

    @Override
    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_PUSH_TOKEN) {
            final String token = (String)args[0];

            Helpshift.registerPushToken(token);
        }
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        Map<String, Object> config = new HashMap<>();

        if (BuildConfig.DEBUG) {
            config.put("enableLogging", true);
        } else {
            config.put("enableLogging", false);
        }

        int screenOrientation = activity.getRequestedOrientation();

        config.put("screenOrientation", screenOrientation);
        config.put("notificationIcon", R.drawable.ic_stat_onesignal_default);

        String MengineHelpshiftPlugin_PlatformId = activity.getMetaDataString(PLUGIN_METADATA_PLATFORM_ID);

        if (MengineHelpshiftPlugin_PlatformId == null) {
            this.invalidInitialize("invalid setup meta data [%s]", PLUGIN_METADATA_PLATFORM_ID);

            return;
        }

        String MengineHelpshiftPlugin_Domain = activity.getMetaDataString(PLUGIN_METADATA_DOMAIN);

        if (MengineHelpshiftPlugin_Domain == null) {
            this.invalidInitialize("invalid setup meta data [mengine.helpshift.domain]");

            return;
        }

        try {
            MengineApplication application = this.getMengineApplication();

            Helpshift.install(application, MengineHelpshiftPlugin_PlatformId, MengineHelpshiftPlugin_Domain, config);
        } catch (UnsupportedOSVersionException e) {
            this.invalidInitialize("Android OS versions prior to Lollipop (< SDK 21) are not supported.");

            return;
        } catch (HelpshiftInstallException e) {
            this.invalidInitialize("install exception: %s"
                , e.getLocalizedMessage()
            );

            return;
        }

        Helpshift.setHelpshiftEventsListener(this);
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
                this.logError("onEventOccurred UNKNOWN: %s"
                    , eventName
                );

                break;
        }
    }

    @Override
    public void onUserAuthenticationFailure(HelpshiftAuthenticationFailureReason reason) {
        switch (reason) {
            case REASON_AUTH_TOKEN_NOT_PROVIDED:
                this.logError("onUserAuthenticationFailure REASON_AUTH_TOKEN_NOT_PROVIDED");

                this.pythonCall("onHelpshiftAuthenticationFailureAuthTokenNotProvided");
            case REASON_INVALID_AUTH_TOKEN:
                this.logError("onUserAuthenticationFailure REASON_INVALID_AUTH_TOKEN");

                this.pythonCall("onHelpshiftAuthenticationFailureInvalidAuthToken");
            case UNKNOWN:
                this.logError("onUserAuthenticationFailure UNKNOWN");

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
