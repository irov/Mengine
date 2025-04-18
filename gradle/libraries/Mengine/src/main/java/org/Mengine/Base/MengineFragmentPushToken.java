package org.Mengine.Base;

public class MengineFragmentPushToken extends MengineFragment<MengineListenerPushToken> {
    public static MengineFragmentPushToken INSTANCE = null;

    MengineFragmentPushToken() {
        super(MengineListenerPushToken.class);

        INSTANCE = this;
    }

    public void changePushToken(String token) {
        this.propagate(MengineListenerPushToken::onMengineChangePushToken, token);
    }
}
