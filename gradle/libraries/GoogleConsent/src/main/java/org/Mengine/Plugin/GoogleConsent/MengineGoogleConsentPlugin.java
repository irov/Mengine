package org.Mengine.Plugin.GoogleConsent;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import android.content.Context;
import android.os.Bundle;

import com.google.ads.consent.ConsentForm;
import com.google.ads.consent.ConsentFormListener;
import com.google.ads.consent.ConsentInfoUpdateListener;
import com.google.ads.consent.ConsentInformation;
import com.google.ads.consent.ConsentStatus;

import java.net.MalformedURLException;
import java.net.URL;

public class MengineGoogleConsentPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "GoogleConsent";

    private ConsentForm m_consentForm;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        final Context context = activity.getBaseContext();

        String google_publisher_id = activity.getString(R.string.google_publisher_id);

        ConsentInformation consentInformation = ConsentInformation.getInstance(context);
        String[] publisherIds = {google_publisher_id};

        MengineGoogleConsentPlugin.this.logInfo("request consent info: %s"
            , google_publisher_id
        );

        consentInformation.requestConsentInfoUpdate(publisherIds, new ConsentInfoUpdateListener() {
            @Override
            public void onConsentInfoUpdated(ConsentStatus consentStatus) {
                MengineGoogleConsentPlugin.this.logInfo("get consent status: %s"
                    , consentStatus.toString()
                );

                if (consentStatus == ConsentStatus.UNKNOWN) {
                    boolean EEA = consentInformation.isRequestLocationInEeaOrUnknown();

                    MengineGoogleConsentPlugin.this.logInfo("get consent EEA: %s"
                            , EEA == true ? "true" : "false"
                    );

                    if (EEA == true) {
                        MengineGoogleConsentPlugin.this.requestConsentFromUser();
                    } else {
                        activity.sendEvent("ConsentStatus", 2);
                    }
                } else if ( consentStatus == ConsentStatus.NON_PERSONALIZED ) {
                    activity.sendEvent("ConsentStatus", 1);
                } else if ( consentStatus == ConsentStatus.PERSONALIZED ) {
                    activity.sendEvent("ConsentStatus", 2);
                }
            }

            @Override
            public void onFailedToUpdateConsentInfo(String errorDescription) {
                MengineGoogleConsentPlugin.this.logError("User's consent status failed to update: %s"
                    , errorDescription
                );

                activity.sendEvent("ConsentStatus", -1);
            }
        });
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        m_consentForm = null;
    }

    void requestConsentFromUser() {
        MengineActivity activity = this.getActivity();

        String privacy_policy_url = activity.getString(R.string.privacy_policy_url);

        URL privacyUrl = null;

        try {
            privacyUrl = new URL(privacy_policy_url);
        } catch (MalformedURLException e) {
            MengineGoogleConsentPlugin.this.logError("privacy policy url error: %s"
                , e.toString()
            );

            return;
        }

        ConsentForm form = new ConsentForm.Builder(activity, privacyUrl)
            .withListener(new ConsentFormListener() {
                @Override
                public void onConsentFormLoaded() {
                    MengineGoogleConsentPlugin.this.logInfo("Consent form loaded successfully");

                    m_consentForm.show();
                }

                @Override
                public void onConsentFormOpened() {
                    MengineGoogleConsentPlugin.this.logInfo("Consent form was displayed");
                }

                @Override
                public void onConsentFormClosed(ConsentStatus consentStatus, Boolean userPrefersAdFree) {
                    MengineGoogleConsentPlugin.this.logInfo("Consent form was closed");
                }

                @Override
                public void onConsentFormError(String errorDescription) {
                    MengineGoogleConsentPlugin.this.logError("Consent form error: %s"
                        , errorDescription
                    );
                }
            })
            .withPersonalizedAdsOption()
            .withNonPersonalizedAdsOption()
            .build();

        m_consentForm = form;

        m_consentForm.load();
    }
}
