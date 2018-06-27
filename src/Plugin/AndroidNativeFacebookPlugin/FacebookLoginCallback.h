//
// Created by Alex Korzh on 6/27/18.
//

class FacebookLoginCallback {
public:
    virtual void onLoginSuccess(char *accessToken) = 0;
    virtual void onLoginCancel() = 0;
    virtual void onLoginError(char *exception) = 0;
};