package org.Mengine.Plugin.OneSignal;

import androidx.annotation.NonNull;

import com.onesignal.notifications.IDisplayableMutableNotification;
import com.onesignal.notifications.INotificationReceivedEvent;
import com.onesignal.notifications.INotificationServiceExtension;

import org.Mengine.Base.MengineFragmentRemoteMessage;
import org.Mengine.Base.MengineRemoteMessageParam;
import org.Mengine.Base.MengineUtils;
import org.json.JSONObject;

import java.util.Map;

public class MengineOneSignalNotificationServiceExtension implements INotificationServiceExtension {
    @Override
    public void onNotificationReceived(@NonNull INotificationReceivedEvent event) {

        IDisplayableMutableNotification notification = event.getNotification();

        String notificationId = notification.getNotificationId();
        String collapseId = notification.getCollapseId();

        JSONObject additionalData = notification.getAdditionalData();

        Map<String, Object> data = MengineUtils.jsonObjectToMap(additionalData);

        MengineRemoteMessageParam message = new MengineRemoteMessageParam();
        message.REMOTEMESSAGE_ID = notificationId;
        message.REMOTEMESSAGE_FROM = "OneSignal";
        message.REMOTEMESSAGE_COLLAPSE_KEY = collapseId;
        message.REMOTEMESSAGE_DATA = data;

        MengineFragmentRemoteMessage.INSTANCE.remoteMessageReceived(message);
    }
}