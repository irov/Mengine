package org.Mengine.Plugin.Helpshift;

import android.content.pm.ActivityInfo;
import android.os.Handler;
import android.os.Looper;

import androidx.annotation.NonNull;

import com.helpshift.Helpshift;
import com.helpshift.HelpshiftAuthenticationFailureReason;
import com.helpshift.HelpshiftEvent;
import com.helpshift.HelpshiftEventsListener;
import com.helpshift.UnsupportedOSVersionException;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import java.util.HashMap;
import java.util.Map;


public class MengineHelpshiftPlugin extends MenginePlugin implements HelpshiftEventsListener {

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
     * -sessionStarted
     * -sessionEnded
     * -receivedUnreadMessage(int count, boolean fromCache)
     * -conversationStatus(String issueId,String publishId,boolean issueOpen)
     * -eventWidgetToggle(boolean visible);
     * -eventMessageAdd(String body,String type);
     * -converstationStart(String text);
     * -eventCSATSubmit"(String rating,String feedback);
     * -converstationEnded
     * -converstationRejected
     * -converstationResolved
     * -converstationReopen

     * -authenticationInvalidAuthToken
     * -authenticationTokenNotProvided
     * -authenticationUnknownError
     */

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("Helpshift");
    }

    @Override
    public void onMengineCreateApplication(MengineActivity activity) {
        Map<String, Object> config = new HashMap<>();

        if (BuildConfig.DEBUG) {
            config.put("enableLogging", true);
        } else {
            config.put("enableLogging", false);
        }

        if (activity.getConfigValueBoolean("HelpshiftPlugin", "screenOrientationPortrait", false)) {
            config.put("screenOrientation", ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT);
        } else {
            config.put("screenOrientation", ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
        }

        config.put("notificationIcon", R.drawable.ic_stat_onesignal_default);

//        config.put("enableInAppNotification", false);
//        config.put("notificationLargeIcon", R.drawable.notification_large_icon);
//        config.put("notificationSoundId",  R.raw.notification_sound);
//        config.put("notificationChannelId",  "mengine_channel_id");
//        config.put("fullPrivacy", true);

        String HELPSHIFT_PlatformId = activity.getConfigValue("HelpshiftPlugin", "PlatformId", "");
        String HELPSHIFT_Domain = activity.getConfigValue("HelpshiftPlugin", "Domain", "");

        try {
            Helpshift.install(activity.getApplication(), HELPSHIFT_PlatformId, HELPSHIFT_Domain, config);

            Helpshift.setHelpshiftEventsListener(MengineHelpshiftPlugin.this);
        } catch (UnsupportedOSVersionException e) {
            this.logError("Android OS versions prior to Lollipop (< SDK 21) are not supported.");
        }



    //- HelpshiftEventsListener
    @Override
    public void onEventOccurred(@NonNull String eventName, Map<String, Object> data) {
        this.logInfo("Helpshift call event %s", eventName);

        switch (eventName) {
            case HelpshiftEvent.SDK_SESSION_STARTED:
                this.logInfo("Helpshift session started.");

                this.pythonCall("sessionStarted");
                break;
            case HelpshiftEvent.SDK_SESSION_ENDED:
                this.logInfo("Helpshift session ended.");

                this.pythonCall("sessionEnded");
                break;
            case HelpshiftEvent.RECEIVED_UNREAD_MESSAGE_COUNT:
                int count = (int) data.get(HelpshiftEvent.DATA_MESSAGE_COUNT);
                boolean fromCache = (boolean) data.get(HelpshiftEvent.DATA_MESSAGE_COUNT_FROM_CACHE);

                this.logInfo("Unread count: " + count + ", From Cache: " + fromCache);

                this.pythonCall("receivedUnreadMessage", count, fromCache);
                break;
            case HelpshiftEvent.CONVERSATION_STATUS:
                String issueId = data.get(HelpshiftEvent.DATA_LATEST_ISSUE_ID).toString();
                String publishId = data.get(HelpshiftEvent.DATA_LATEST_ISSUE_PUBLISH_ID).toString();
                boolean issueOpen = (boolean) data.get(HelpshiftEvent.DATA_IS_ISSUE_OPEN);

                this.logInfo("Issue ID: " + issueId + ", Publish ID: " + publishId + ", Is issue open: " + issueOpen);

                this.pythonCall("conversationStatus", issueId, publishId, issueOpen);
                break;
            case HelpshiftEvent.WIDGET_TOGGLE:
                boolean visible = (boolean) data.get(HelpshiftEvent.DATA_SDK_VISIBLE);

                this.logInfo("Is chat screen visible:" + visible);

                this.pythonCall("eventWidgetToggle", visible);
                break;

            case HelpshiftEvent.MESSAGE_ADD:
                String body = data.get(HelpshiftEvent.DATA_MESSAGE_BODY).toString();
                String type = data.get(HelpshiftEvent.DATA_MESSAGE_TYPE).toString();

                this.logInfo("New message added with body: " + body + ", with type:" + type);

                this.pythonCall("eventMessageAdd", body, type);
                break;
            case HelpshiftEvent.CONVERSATION_START:
                String text = data.get(HelpshiftEvent.DATA_MESSAGE).toString();

                this.logInfo("Conversation started with text: " + text);

                this.pythonCall("converstationStart", text);
                break;
            case HelpshiftEvent.CSAT_SUBMIT:
                String rating = data.get(HelpshiftEvent.DATA_CSAT_RATING).toString();
                String feedback = data.get(HelpshiftEvent.DATA_ADDITIONAL_FEEDBACK).toString();

                this.logInfo("CSAT Submitted with rating: " + rating + ", with feedback:" + feedback);

                this.pythonCall("eventCSATSubmit", rating, feedback);
                break;
            case HelpshiftEvent.CONVERSATION_END:
                this.logInfo("Conversation ended.");

                this.pythonCall("converstationEnded");
                break;
            case HelpshiftEvent.CONVERSATION_REJECTED:
                this.logInfo("Conversation rejected.");

                this.pythonCall("converstationRejected");
                break;
            case HelpshiftEvent.CONVERSATION_RESOLVED:
                this.logInfo("Conversation resolved.");

                this.pythonCall("converstationResolved");
                break;
            case HelpshiftEvent.CONVERSATION_REOPENED:
                this.logInfo("Conversation reopened.");

                this.pythonCall("converstationReopen");
                break;
        }
    }

    @Override
    public void onUserAuthenticationFailure(HelpshiftAuthenticationFailureReason reason) {
        this.logInfo("Helpshift onUserAuthenticationFailure ->" + reason);
        switch (reason) {
            case REASON_AUTH_TOKEN_NOT_PROVIDED:
                this.logError("Helpshift authentication Failed - AuthTokenNotProvided");
                break;
            case REASON_INVALID_AUTH_TOKEN:
                this.logError("Helpshift authentication Failed - InvalidAuthToken");
                break;
            case UNKNOWN:
                this.logError("Helpshift authentication Failed - UNKNOWN");
                break;
        }

    }
    //- End HelpshiftEventsListener


    public void showFAQs() {
        Map<String, Object> config = new HashMap<>();
        Helpshift.showFAQs(this.getActivity(), config);
    }

    public void showConversation() {
        Map<String, Object> config = new HashMap<>();
        Helpshift.showConversation(this.getActivity(), config);
    }

    public void showFAQSection(final String sectionPublishId) {
        Map<String, Object> config = new HashMap<>();
        Helpshift.showFAQSection(this.getActivity(), sectionPublishId, config);
    }

    public void showSingleFAQ(final String publishId) {
        Map<String, Object> config = new HashMap<>();
        Helpshift.showSingleFAQ(this.getActivity(), publishId, config);
    }

    //    lang vars - en, de, es, fr, it, ru, zh-Hans,zh-Hant, zh-HK, zh-Hant-HK, zh-SG, zh-Hant-SG, pt, ko, ja, tr, nl, cs, hu,, th, sl, vi, ar, pl, no, sv, fi, ro, el, da, ms, iw, sk, uk, ca, hr, bn, bg, gu, hi, kn, lv, ml, mr, pa, fa, ta, te
    public void setLanguage(final String language) {
        Helpshift.setLanguage(language);
    }

}
