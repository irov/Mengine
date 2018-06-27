//
// Created by Alex Korzh on 6/27/18.
//

class FacebookLoginCallback {
public:
    virtual void onLoginSuccess(FacebookLoginResultWrapper loginResult) = 0;
    virtual void onLoginCancel() = 0;
    virtual void onLoginError(char *exception) = 0;
};