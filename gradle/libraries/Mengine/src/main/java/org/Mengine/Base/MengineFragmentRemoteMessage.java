package org.Mengine.Base;

public class MengineFragmentRemoteMessage extends MengineFragment<MengineListenerRemoteMessage> {
    public static MengineFragmentRemoteMessage INSTANCE = null;

    MengineFragmentRemoteMessage() {
        super(MengineListenerRemoteMessage.class);

        INSTANCE = this;
    }

    public boolean remoteMessageReceived(MengineParamRemoteMessage message) {
        if( this.propagateB(MengineListenerRemoteMessage::onMengineRemoteMessageReceived, message) == true ) {
            return true;
        }

        return false;
    }

    public void remoteMessageDeleted() {
        this.propagate(MengineListenerRemoteMessage::onMengineRemoteMessageDeleted);
    }

    public void remoteMessageNewToken(String token) {
        this.propagate(MengineListenerRemoteMessage::onMengineRemoteMessageNewToken, token);
    }
}
