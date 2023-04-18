package org.Mengine.Plugin.LocalNotifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Build;

public class NotificationPublisher extends BroadcastReceiver {
    public static String NOTIFICATION_ID = "NOTIFICATION_ID";
    public static String NOTIFICATION = "notification";

    @SuppressWarnings("deprecation")
    private Notification getParcelableExtra(Intent intent) {
        Notification notification;
        if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            notification = intent.getParcelableExtra(NOTIFICATION, Notification.class);
        } else {
            notification = intent.getParcelableExtra(NOTIFICATION);
        }

        return notification;
    }

    public void onReceive(Context context, Intent intent) {
        NotificationManager notificationManager = (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);

        Notification notification = this.getParcelableExtra(intent);

        int id = intent.getIntExtra(NOTIFICATION_ID, 0);
        notificationManager.notify(id, notification);
    }
}
