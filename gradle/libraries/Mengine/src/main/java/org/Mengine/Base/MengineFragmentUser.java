package org.Mengine.Base;

public class MengineFragmentUser extends MengineFragment<MengineListenerUser> {
    public static MengineFragmentUser INSTANCE = null;

    MengineFragmentUser() {
        super(MengineListenerUser.class);

        INSTANCE = this;
    }

    public void changeUserId(String oldUserId, String newUserId) {
        this.propagate(MengineListenerUser::onMengineChangeUserId, oldUserId, newUserId);
    }

    public void removeUserData() {
        this.propagate(MengineListenerUser::onMengineRemoveUserData);
    }
}