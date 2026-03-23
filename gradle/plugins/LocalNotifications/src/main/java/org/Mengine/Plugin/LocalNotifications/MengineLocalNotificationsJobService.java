package org.Mengine.Plugin.LocalNotifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.job.JobParameters;
import android.app.job.JobService;
import android.content.Context;
import android.os.PersistableBundle;

public class MengineLocalNotificationsJobService extends JobService {
    public static PersistableBundle notificationBundle(int id, String title, String content){
        PersistableBundle bundle = new PersistableBundle();
        bundle.putInt(MengineLocalNotificationsConstants.KEY_ID, id);
        bundle.putString(MengineLocalNotificationsConstants.KEY_CONTENT, content);
        bundle.putString(MengineLocalNotificationsConstants.KEY_TITLE, title);

        return bundle;
    }

    @Override
    public boolean onStartJob(JobParameters params) {
        PersistableBundle bundle = params.getExtras();
        String title = bundle.getString(MengineLocalNotificationsConstants.KEY_TITLE);
        String content = bundle.getString(MengineLocalNotificationsConstants.KEY_CONTENT);
        int id = bundle.getInt(MengineLocalNotificationsConstants.KEY_ID);

        Context context = this.getApplicationContext();

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) {
            Notification notification = MengineLocalNotificationsPlugin.getNotification(context, id, title, content);
            NotificationManager notificationManager = this.getSystemService(NotificationManager.class);

            if (notificationManager == null) {
                return false;
            }

            notificationManager.notify(id, notification);
        }

        return false;
    }

    @Override
    public boolean onStopJob(JobParameters params) {
        return false;
    }
}
