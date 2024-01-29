package org.Mengine.Plugin.MAR;

import android.app.AlertDialog;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.KeyEvent;

import com.mar.sdk.MARCloudCallBack;
import com.mar.sdk.MARCode;
import com.mar.sdk.MARSDK;
import com.mar.sdk.PayParams;
import com.mar.sdk.UserExtraData;
import com.mar.sdk.gg.MARGgPlatform;
import com.mar.sdk.gg.control.MggControl;
import com.mar.sdk.platform.MARExitListener;
import com.mar.sdk.platform.MARInitListener;
import com.mar.sdk.platform.MARPlatform;
import com.mar.sdk.verify.UGameArchive;
import com.mar.sdk.verify.UToken;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MenginePluginKeyListener;
import org.Mengine.Base.MenginePluginEngineListener;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Calendar;
import java.util.TimeZone;

public class MengineMARPlugin extends MenginePlugin implements MARInitListener, MenginePluginApplicationListener, MenginePluginKeyListener, MenginePluginEngineListener, MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "MarSDK";
    public static final boolean PLUGIN_EMBEDDING = true;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        MARPlatform.getInstance().init(activity, this);
    }

    public long getNetworkTime() {
        long networkTime = MARSDK.getInstance().getNetworkTime();

        return networkTime;
    }

    public void setSeverUserID(String userID) {
        this.logMessage("setSeverUserID userID: %s"
            , userID
        );

        MARSDK.getInstance().setSeverUserID(userID);
    }

    public String getSeverUserID() {
        String severUserID = MARSDK.getInstance().getSeverUserID();

        return severUserID;
    }

    public void setServerToken(String token) {
        this.logMessage("setServerToken token: %s"
            , token
        );

        MARSDK.getInstance().setServerToken(token);
    }

    public String getServerToken() {
        String serverToken = MARSDK.getInstance().getServerToken();

        return serverToken;
    }

    public void login() {
        this.logMessage("login");

        MengineActivity activity = this.getMengineActivity();

        MARPlatform.getInstance().login(activity);
    }

    public void loginCustom(final String loginType) {
        this.logMessage("login custom loginType: %s"
            , loginType
        );

        MengineActivity activity = this.getMengineActivity();

        MARPlatform.getInstance().loginCustom(activity, loginType);
    }

    private PayParams makePayParams(String jsonData) {
        PayParams params = new PayParams();

        try {
            JSONObject json = new JSONObject(jsonData);

            String productId = json.optString("productId", params.getProductId());
            String productName = json.optString("productName", params.getProductName());
            String productDesc = json.optString("productDesc", params.getProductDesc());
            int price = json.optInt("price", params.getPrice());
            int ratio = json.optInt("ratio", params.getRatio());
            int buyNum = json.optInt("buyNum", params.getBuyNum());
            int coinNum = json.optInt("coinNum", params.getCoinNum());
            String serverId = json.optString("serverId", params.getServerId());
            String serverName = json.optString("serverName", params.getServerName());
            String roleId = json.optString("roleId", params.getRoleId());
            String roleName = json.optString("roleName", params.getRoleName());
            int roleLevel = json.optInt("roleLevel", params.getRoleLevel());
            String payNotifyUrl = json.optString("payNotifyUrl", params.getPayNotifyUrl());
            String vip = json.optString("vip", params.getVip());
            String orderID = json.optString("orderID", params.getOrderID());
            String extension = json.optString("extension", params.getExtension());
            String channelOrderID = json.optString("channelOrderID", params.getChannelOrderID());
            int state = json.optInt("state", params.getState());

            params.setProductId(productId);
            params.setProductName(productName);
            params.setProductDesc(productDesc);
            params.setPrice(price);
            params.setRatio(ratio);
            params.setBuyNum(buyNum);
            params.setCoinNum(coinNum);
            params.setServerId(serverId);
            params.setServerName(serverName);
            params.setRoleId(roleId);
            params.setRoleName(roleName);
            params.setRoleLevel(roleLevel);
            params.setPayNotifyUrl(payNotifyUrl);
            params.setVip(vip);
            params.setOrderID(orderID);
            params.setExtension(extension);
            params.setChannelOrderID(channelOrderID);
            params.setState(state);
        } catch (JSONException e) {
            this.logError("[ERROR] makePayParams jsonData: %s error: %s"
                , jsonData
                , e.getMessage()
            );

            return null;
        }

        return params;
    }

    public boolean pay(String jsonData) {
        this.logMessage("pay json: %s"
            , jsonData
        );

        PayParams params = this.makePayParams(jsonData);

        if (params == null) {
            return false;
        }

        this.logMessage("pay params: %s"
            , params
        );

        MengineActivity activity = this.getMengineActivity();
        MARPlatform.getInstance().pay(activity, params);

        return true;
    }

    public void redeemCode(String code) {
        this.logMessage("try redeem code: %s"
            , code
        );

        MARPlatform.getInstance().exchangeGift(code);
    }

    public boolean showAd() {
        boolean videoFlag = MARGgPlatform.getInstance().getVideoFlag();

        this.logMessage("show ad videoFlag: %b"
            , videoFlag
        );

        if (videoFlag == false) {
            return false;
        }

        MARGgPlatform.getInstance().showVideo();

        return true;
    }

    public void updateData(String gameArchive, int serialNumber) {
        this.logMessage("update data: %s serialNumber: %d"
            , gameArchive
            , serialNumber
        );

        MARPlatform.getInstance().updateGameArchive(gameArchive, serialNumber, new MARCloudCallBack() {
            @Override
            public void onCloudResult(UGameArchive var1) {
                int status = var1.getStatus();
                int serialNumber = var1.getSerialNumber();
                String gameArchive = var1.getGameArchive();
                String extraData = var1.getExtraData();

                MengineMARPlugin.this.logMessage("update data to server status: %d serialNumber: %d gameArchive: %s extraData: %s"
                    , status
                    , serialNumber
                    , gameArchive
                    , extraData
                );

                MengineMARPlugin.this.pythonCall("onMarSDKUpdateData", status, serialNumber, gameArchive, extraData);
            }
        });
    }

    public void getData(int serialNumber) {
        this.logMessage("get data: %d"
            , serialNumber
        );

        MARPlatform.getInstance().getGameArchive(serialNumber, new MARCloudCallBack() {
            @Override
            public void onCloudResult(UGameArchive var1) {
                int status = var1.getStatus();
                int serialNumber = var1.getSerialNumber();
                String gameArchive = var1.getGameArchive();
                String extraData = var1.getExtraData();

                MengineMARPlugin.this.logMessage("get data from server status: %s serialNumber: %d gameArchive: %s extraData: %s"
                    , status
                    , serialNumber
                    , gameArchive
                    , extraData
                );

                MengineMARPlugin.this.pythonCall("onMarSDKGetData", status, serialNumber, gameArchive, extraData);
            }
        });
    }

    public boolean pasteCode() {
        this.logMessage("pasteCode");

        try {
            String code = "";

            Context context = MengineActivity.getContext();

            ClipboardManager clipboard = (ClipboardManager)context.getSystemService(Context.CLIPBOARD_SERVICE);

            boolean hasPrimaryClip = clipboard.hasPrimaryClip();

            if (hasPrimaryClip == true) {
                ClipData clip = clipboard.getPrimaryClip();
                ClipData.Item item = clip.getItemAt(0);

                Object data = item.getText();

                if (data != null) {
                    code = data.toString();
                }
            }

            this.logMessage("try to paste code: %s"
                , code
            );

            this.pythonCall("onMarSDKSaveClipboard", hasPrimaryClip, code);
        } catch (Exception e) {
            this.logError("[ERROR] pasteCode exception: %s"
                , e.getMessage()
            );

            return false;
        }

        return true;
    }

    private UserExtraData makeUserExtraData(String jsonData) {
        UserExtraData data = new UserExtraData();

        try {
            JSONObject json = new JSONObject(jsonData);

            int dataType = json.has("dataType") == true ? json.getInt("dataType") : data.getDataType();
            String roleID = json.has("roleID") == true ? json.getString("roleID") : data.getRoleID();
            String roleName = json.has("roleName") == true ? json.getString("roleName") : data.getRoleName();
            String roleLevel = json.has("roleLevel") == true ? json.getString("roleLevel") : data.getRoleLevel();
            int serverID = json.has("serverID") == true ? json.getInt("serverID") : data.getServerID();
            String serverName = json.has("serverName") == true ? json.getString("serverName") : data.getServerName();
            int moneyNum = json.has("moneyNum") == true ? json.getInt("moneyNum") : data.getMoneyNum();
            long roleCreateTime = json.has("roleCreateTime") == true ? json.getLong("roleCreateTime") : data.getRoleCreateTime();
            long roleLevelUpTime = json.has("roleLevelUpTime") == true ? json.getLong("roleLevelUpTime") : data.getRoleLevelUpTime();
            String vip = json.has("vip") == true ? json.getString("vip") : data.getVip();
            int roleGender = json.has("roleGender") == true ? json.getInt("roleGender") : data.getRoleGender();
            String professionID = json.has("professionID") == true ? json.getString("professionID") : data.getProfessionID();
            String professionName = json.has("professionName") == true ? json.getString("professionName") : data.getProfessionName();
            String power = json.has("power") == true ? json.getString("power") : data.getPower();
            String partyID = json.has("partyID") == true ? json.getString("partyID") : data.getPartyID();
            String partyName = json.has("partyName") == true ? json.getString("partyName") : data.getPartyName();
            String partyMasterID = json.has("partyMasterID") == true ? json.getString("partyMasterID") : data.getPartyMasterID();
            String partyMasterName = json.has("partyMasterName") == true ? json.getString("partyMasterName") : data.getPartyMasterName();

            data.setDataType(dataType);
            data.setMoneyNum(moneyNum);
            data.setRoleCreateTime(roleCreateTime);
            data.setRoleID(roleID);
            data.setRoleName(roleName);
            data.setRoleLevel(roleLevel);
            data.setRoleLevelUpTime(roleLevelUpTime);
            data.setServerID(serverID);
            data.setServerName(serverName);
            data.setVip(vip);
            data.setRoleGender(roleGender);
            data.setProfessionID(professionID);
            data.setProfessionName(professionName);
            data.setPower(power);
            data.setPartyID(partyID);
            data.setPartyName(partyName);
            data.setPartyMasterID(partyMasterID);
            data.setPartyMasterName(partyMasterName);
        } catch (JSONException e) {
            this.logError("[ERROR] makeUserExtraData jsonData: %s error: %s"
                , jsonData
                , e.getMessage()
            );

            return null;
        }

        return data;
    }

    public boolean submitExtraData(String jsonData) {
        this.logMessage("submitExtraData jsonData: %s"
            , jsonData
        );

        UserExtraData data = this.makeUserExtraData(jsonData);

        if (data == null) {
            return false;
        }

        this.logMessage("submitExtraData data: %s"
            , data
        );

        MARPlatform.getInstance().submitExtraData(data);

        return true;
    }

    public void visitorLogin() {
        this.logMessage("visitorLogin");

        MARPlatform.getInstance().visitorLogin();
    }

    public void reqAdControlInfo() {
        this.logMessage("reqAdControlInfo");

        MggControl.getInstance().reqAdControlInfo();
    }

    public void setPropDeliveredComplete(String orderID) {
        this.logMessage("setPropDeliveredComplete orderID: %s"
            , orderID
        );

        MARPlatform.getInstance().setPropDeliveredComplete(orderID);
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        MARSDK.getInstance().onAppCreateAll(application);
        MARSDK.getInstance().onAppCreate(application);
    }

    @Override
    public void onAppTerminate(MengineApplication application) {
        MARSDK.getInstance().onTerminate();
    }

    @Override
    public void onAppAttachBaseContext(MengineApplication application, Context base) {
        MARSDK.getInstance().onAppAttachBaseContext(application, base);
    }

    @Override
    public void onAppConfigurationChanged(MengineApplication application, Configuration newConfig) {
        MARSDK.getInstance().onAppConfigurationChanged(application, newConfig);
    }

    @Override
    public void onActivityResultBefore(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        MARSDK.getInstance().onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onStart(MengineActivity activity) {
        MARSDK.getInstance().onStart();
    }

    @Override
    public void onPause(MengineActivity activity) {
        MARSDK.getInstance().onPause();
    }

    @Override
    public void onResume(MengineActivity activity) {
        MARSDK.getInstance().onResume();
    }

    @Override
    public void onNewIntent(MengineActivity activity, Intent intent) {
        MARSDK.getInstance().onNewIntent(intent);
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        MARSDK.getInstance().onDestroy();
    }

    @Override
    public void onRestart(MengineActivity activity) {
        MARSDK.getInstance().onRestart();
    }

    @Override
    public void onConfigurationChanged(MengineActivity activity, Configuration newConfig){
        MARSDK.getInstance().onConfigurationChanged(newConfig);
    }

    @Override
    public boolean dispatchKeyEvent(MengineActivity activity, KeyEvent event) {
        int action = event.getAction();
        int keyCode = event.getKeyCode();

        if (keyCode == KeyEvent.KEYCODE_BACK && action == KeyEvent.ACTION_DOWN) {
            this.logMessage("exit MARSDK");

            MARPlatform.getInstance().exitSDK( new MARExitListener() {
                @Override
                public void onGameExit() {
                    //游戏自己的退出确认框

                    AlertDialog.Builder builder = new AlertDialog.Builder(activity);
                    builder.setTitle("退出确认");
                    builder.setMessage("主公，现在还早，要不要再玩一会？");
                    builder.setCancelable(true);
                    builder.setPositiveButton("好吧",
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                                //这里什么都不用做
                            }
                        });

                    builder.setNeutralButton("一会再玩",
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                                //退出游戏
                                activity.finish();

                                System.exit(0);
                            }
                        });

                    builder.show();
                }
            });

            return true;
        }

        return false;
    }

    @Override
    public void onRequestPermissionsResult(MengineActivity activity, int requestCode, String[] permissions, int[] grantResults) {
        MARSDK.getInstance().onRequestPermissionResult(requestCode, permissions, grantResults);
    }

    @Override
    public void onInitResult(int code, String msg) {
        this.logMessage("onInitResult code: %d msg: %s"
            , code
            , msg
        );

        String versionName = MARSDK.getInstance().getSDKVersionName();
        String versionCode = MARSDK.getInstance().getSDKVersionCode();

        this.logMessage("MARSDK Version: %s code: %s"
            , versionName
            , versionCode
        );

        switch(code) {
            case MARCode.CODE_INIT_SUCCESS: {
                this.logMessage("init success");

                this.activateSemaphore("onMarSDKInitSuccess");
            } break;
            case MARCode.CODE_INIT_FAIL: {
                this.logError("[ERROR] init fail");

                this.activateSemaphore("onMarSDKInitFail");
            } break;
            default: {
                this.logWarning("init undefined code: %d msg: %s"
                    , code
                    , msg
                );
            } break;
        }
    }

    @Override
    public void onLoginResult(int code, UToken uToken) {
        this.logMessage("onLoginResult code: %d uToken: %s"
            , code
            , uToken
        );

        switch(code) {
            case MARCode.CODE_LOGIN_SUCCESS: {
                int gameType = MARSDK.getInstance().getGameType();
                boolean isFreeFlag = MggControl.getInstance().getFreeFlag();

                this.logMessage("login success gameType: %d isFreeFlag: %b"
                    , gameType
                    , isFreeFlag
                );

                this.pythonCall("onMarSDKLoginSuccess", gameType, isFreeFlag);
            } break;
            case MARCode.CODE_LOGIN_FAIL: {
                int gameType = MARSDK.getInstance().getGameType();

                this.logWarning("login fail gameType: %d"
                    , gameType
                );

                this.pythonCall("onMarSDKLoginFail", gameType);
            } break;
            case MARCode.CODE_LOGIN_TIMEOUT: {
                this.logWarning("login timeout");

                this.pythonCall("onMarSDKLoginTimeout");
            } break;
            default: {
                this.logWarning("login undefined code: %d"
                    , code
                );
            }
        }
    }
    
    @Override
    public void onSwitchAccount(UToken uToken) {
        this.logMessage("onSwitchAccount uToken: %s"
            , uToken
        );

        if (uToken != null) {
            this.pythonCall("onMarSDKSwitchAccount");
        }
    }
    
    @Override
    public void onLogout() {
        this.logMessage("onLogout");

        this.pythonCall("onMarSDKLogout");
    }    
    
    @Override
    public void onPayResult(int code, String msg) {
        this.logMessage("onPayResult code: %d msg: %s"
            , code
            , msg
        );
        
        try {
            JSONObject json = new JSONObject(msg);

            String productId = json.getString("productId");
            int payResult = json.getInt("payResult");

            if (payResult == 0) {
                String orderId = json.getString("orderId");

                this.logMessage("pay complete productId: %s orderId: %s"
                    , productId
                    , orderId
                );

                this.pythonCall("onMarSDKPaySuccess", productId, orderId);
            } else {
                this.logError("[ERROR] pay fail productId: %s [result %d]"
                    , productId
                    , payResult
                );

                this.pythonCall("onMarSDKPayFail", productId);
            }
        } catch (JSONException e) {
            this.logError("[ERROR] pay msg: %s JSONException: %s"
                , msg
                , e.getMessage()
            );

            String message = e.getMessage();

            this.pythonCall("onMarSDKPayError", code, msg, message);
        }
    }

    @Override
    public void onRedeemResult(String msg) {
        this.logMessage("onRedeemResult msg: %s"
            , msg
        );

        try {
            JSONObject json = new JSONObject(msg);

            int propNumber = json.getInt("propNumber");
            String propType = json.getString("propType");
            String message = json.getString("msg");

            this.pythonCall("onMarSDKRedeemResult", propNumber, propType, message);
        } catch(JSONException e) {
            this.logError("[ERROR] onRedeemResult msg: %s JSONException: %s"
                , msg
                , e.getMessage()
            );

            String message = e.getMessage();

            this.pythonCall("onMarSDKRedeemError", msg, message);
        }
    }

    @Override
    public void onResult(int code, String msg) {
        switch (code) {
            case MARCode.CODE_AD_VIDEO_CALLBACK: {
                this.logMessage("onResult CODE_AD_VIDEO_CALLBACK: %s"
                    , msg
                );

                String watchAdTime = this.getCurrentTime();

                this.pythonCall("onMarSDKAdVideoCallback", msg, watchAdTime);
            } break;
            default: {
                this.logMessage("onResult code: %d msg: %s"
                    , code
                    , msg
                );

                this.pythonCall("onMarSDKResult", code, msg);
            } break;
        }
    }

    private String getCurrentTime() {
        TimeZone tz = TimeZone.getTimeZone("Asia/Beijing");
        Calendar calendar = Calendar.getInstance(tz);
        int DATE = calendar.get(Calendar.DATE);
        int MONTH = calendar.get(Calendar.MONTH);
        int YEAR = calendar.get(Calendar.YEAR);
        String time = "" + DATE + "/" + MONTH + "/" + YEAR;

        return time;
    }
}
