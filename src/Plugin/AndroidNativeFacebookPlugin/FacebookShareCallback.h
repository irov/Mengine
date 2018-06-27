//
// Created by Alex Korzh on 6/27/18.
//

class FacebookShareCallback {
public:
    virtual void onShareSuccess(const char *postId) = 0;
    virtual void onShareCancel() = 0;
    virtual void onShareError(char* exception) = 0;
};