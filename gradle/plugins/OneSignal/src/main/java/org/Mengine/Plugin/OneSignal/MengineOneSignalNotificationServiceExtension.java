package org.Mengine.Plugin.OneSignal;

import android.content.Context;
import android.graphics.Color;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.ForegroundColorSpan;
import org.json.JSONObject;
import java.math.BigInteger;

import com.onesignal.OSNotification;
import com.onesignal.OSMutableNotification;
import com.onesignal.OSNotificationReceivedEvent;
import com.onesignal.OneSignal.OSRemoteNotificationReceivedHandler;

public class MengineOneSignalNotificationServiceExtension implements OSRemoteNotificationReceivedHandler  {
    @Override
    public void remoteNotificationReceived(Context context, OSNotificationReceivedEvent notificationReceivedEvent) {
        OSNotification notification = notificationReceivedEvent.getNotification();

        JSONObject data = notification.getAdditionalData();

        OSMutableNotification mutableNotification = notification.mutableCopy();
        mutableNotification.setExtender(builder -> {
            builder.setColor(new BigInteger("FF00FF00", 16).intValue());

            String title = notification.getTitle();
            Spannable spannableTitle = new SpannableString(title);
            spannableTitle.setSpan(new ForegroundColorSpan(Color.RED),0,title.length(),0);
            builder.setContentTitle(spannableTitle);

            String body = notification.getBody();
            Spannable spannableBody = new SpannableString(body);
            spannableBody.setSpan(new ForegroundColorSpan(Color.BLUE),0, body.length(),0);
            builder.setContentText(spannableBody);

            builder.setTimeoutAfter(30000);

            return builder;
        });

        notificationReceivedEvent.complete(mutableNotification);
    }
}
