package org.Mengine.Plugin.GoogleConsent;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import android.content.Context;
import android.os.Bundle;

import com.google.ads.consent.ConsentInfoUpdateListener;
import com.google.ads.consent.ConsentInformation;
import com.google.ads.consent.ConsentStatus;

public class MengineGoogleConsentPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "GoogleConsent";

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
                    activity.sendEvent("ConsentStatus", 0);
                } else if ( consentStatus == ConsentStatus.NON_PERSONALIZED ) {
                    activity.sendEvent("ConsentStatus", 1);
                } else if ( consentStatus == ConsentStatus.PERSONALIZED ) {
                    activity.sendEvent("ConsentStatus", 2);
                }
            }

            @Override
            public void onFailedToUpdateConsentInfo(String errorDescription) {
                // User's consent status failed to update.
                MengineGoogleConsentPlugin.this.logError("failed to update consent: %s"
                    , errorDescription
                );

                activity.sendEvent("ConsentStatus", -1);
            }
        });
    }
}
