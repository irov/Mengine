package org.Mengine.Plugin.Advertising;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import android.content.Context;
import android.os.Bundle;
import android.os.AsyncTask;

import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import com.google.android.gms.common.GooglePlayServicesNotAvailableException;
import com.google.android.gms.common.GooglePlayServicesRepairableException;

import java.io.IOException;

public class MengineAdvertisingPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "Advertising";

    private static String m_advertisingId;
    private static boolean m_advertisingLimitTrackingEnabled;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        GetAdvertisingIdTask task = new GetAdvertisingIdTask(activity);

        task.execute();
    }

    private class GetAdvertisingIdTask extends AsyncTask<String, Integer, AdvertisingIdClient.Info> {
        private MengineActivity m_activity;

        public GetAdvertisingIdTask(MengineActivity activity) {
            super();
            this.m_activity = activity;
        }

        @Override
        protected AdvertisingIdClient.Info doInBackground(String... strings) {
            final Context context = this.m_activity.getApplicationContext();

            AdvertisingIdClient.Info adInfo = null;

            try {
                adInfo = AdvertisingIdClient.getAdvertisingIdInfo(context);
            } catch (IOException e) {
                e.printStackTrace();
            } catch (GooglePlayServicesNotAvailableException e) {
                e.printStackTrace();
            } catch (GooglePlayServicesRepairableException e) {
                e.printStackTrace();
            }

            return adInfo;
        }

        @Override
        protected void onPostExecute(AdvertisingIdClient.Info adInfo) {
            if (adInfo == null ) {
                MengineAdvertisingPlugin.m_advertisingId = "00000000-0000-0000-0000-000000000000";
                MengineAdvertisingPlugin.m_advertisingLimitTrackingEnabled = true;
            } else if( adInfo.isLimitAdTrackingEnabled() == true) {
                MengineAdvertisingPlugin.m_advertisingId = "00000000-0000-0000-0000-000000000000";
                MengineAdvertisingPlugin.m_advertisingLimitTrackingEnabled = true;
            } else {
                MengineAdvertisingPlugin.m_advertisingId = adInfo.getId();
                MengineAdvertisingPlugin.m_advertisingLimitTrackingEnabled = false;
            }

            MengineAdvertisingPlugin.this.logInfo("AdvertisingId: %s [%s]"
                , MengineAdvertisingPlugin.m_advertisingId
                , MengineAdvertisingPlugin.m_advertisingLimitTrackingEnabled == true ? "true" : "false"
            );

            this.m_activity.sendEvent("AdvertisingId", MengineAdvertisingPlugin.m_advertisingId, MengineAdvertisingPlugin.m_advertisingLimitTrackingEnabled);
        }
    }
}
