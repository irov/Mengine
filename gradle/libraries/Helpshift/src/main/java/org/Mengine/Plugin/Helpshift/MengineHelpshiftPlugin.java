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
            case HelpshiftEvent.CONVERSATION_STATUS:
                this.logInfo(
                        data.get(HelpshiftEvent.DATA_LATEST_ISSUE_ID) + " " +
                                data.get(HelpshiftEvent.DATA_LATEST_ISSUE_PUBLISH_ID) + " " +
                                data.get(HelpshiftEvent.DATA_IS_ISSUE_OPEN)
                );
                break;
            case HelpshiftEvent.WIDGET_TOGGLE:
                this.logInfo(data.get(HelpshiftEvent.DATA_SDK_VISIBLE) + " ");
                break;
            case HelpshiftEvent.CONVERSATION_START:
                this.logInfo(data.get(HelpshiftEvent.DATA_MESSAGE) + " ");
                break;

            case HelpshiftEvent.MESSAGE_ADD:
                this.logInfo(data.get(HelpshiftEvent.DATA_MESSAGE_BODY) + " ");

                if (data.get(HelpshiftEvent.DATA_MESSAGE_TYPE).equals(HelpshiftEvent.DATA_MESSAGE_TYPE_ATTACHMENT)) {
                    this.logInfo("user sent an attachment");
                } else {
                    this.logInfo("user sent an Text");
                }
                break;

            case HelpshiftEvent.CSAT_SUBMIT:
                this.logInfo(data.get(HelpshiftEvent.DATA_CSAT_RATING) + " ");
                this.logInfo(data.get(HelpshiftEvent.DATA_ADDITIONAL_FEEDBACK) + " ");
                break;
            case HelpshiftEvent.CONVERSATION_END:
            case HelpshiftEvent.CONVERSATION_REJECTED:
            case HelpshiftEvent.CONVERSATION_RESOLVED:
            case HelpshiftEvent.CONVERSATION_REOPENED:
                break;
        }
    }

    @Override
    public void onUserAuthenticationFailure(HelpshiftAuthenticationFailureReason reason) {
        this.logInfo("Helpshift onUserAuthenticationFailure ->" + reason);
        switch (reason) {
            case REASON_AUTH_TOKEN_NOT_PROVIDED:
            case REASON_INVALID_AUTH_TOKEN:
            case UNKNOWN:
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
