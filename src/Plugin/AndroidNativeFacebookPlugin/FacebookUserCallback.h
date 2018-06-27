//
// Created by Alex Korzh on 6/27/18.
//

class FacebookUserCallback {
public:
    virtual void onUserFetchSuccess(char *userObject, char *response) = 0;
};
