package org.Mengine.Base;

public class MengineFragmentUser extends MengineFragment<MengineListenerUser> {
    public static MengineFragmentUser INSTANCE = null;

    MengineFragmentUser() {
        super(MengineListenerUser.class);

        INSTANCE = this;
    }

    public void changeUserId(String userId) {
        this.propagate(MengineListenerUser::onMengineChangeUserId, userId);
    }

    public void removeUserData() {
        this.propagate(MengineListenerUser::onMengineRemoveUserData);
    }
}