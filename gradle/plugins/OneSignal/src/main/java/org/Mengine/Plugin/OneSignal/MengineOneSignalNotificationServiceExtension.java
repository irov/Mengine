package org.Mengine.Plugin.OneSignal;

import android.content.Context;
import android.graphics.Color;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
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
        Log.i("OneSignal", "Received Notification Data: " + data);

        // Example of modifying the notification's accent color
        OSMutableNotification mutableNotification = notification.mutableCopy();
        mutableNotification.setExtender(builder -> {
            // Sets the accent color to Green on Android 5+ devices.
            // Accent color controls icon and action buttons on Android 5+. Accent color does not change app title on Android 10+
            builder.setColor(new BigInteger("FF00FF00", 16).intValue());

            // Sets the notification Title to Red
            String title = notification.getTitle();
            Spannable spannableTitle = new SpannableString(title);
            spannableTitle.setSpan(new ForegroundColorSpan(Color.RED),0,title.length(),0);
            builder.setContentTitle(spannableTitle);

            // Sets the notification Body to Blue
            String body = notification.getBody();
            Spannable spannableBody = new SpannableString(body);
            spannableBody.setSpan(new ForegroundColorSpan(Color.BLUE),0, body.length(),0);
            builder.setContentText(spannableBody);

            //Force remove push from Notification Center after 30 seconds
            builder.setTimeoutAfter(30000);

            return builder;
        });

        // If complete isn't call within a time period of 25 seconds, OneSignal internal logic will show the original notification
        // To omit displaying a notification, pass `null` to complete()
        notificationReceivedEvent.complete(mutableNotification);
    }
}
