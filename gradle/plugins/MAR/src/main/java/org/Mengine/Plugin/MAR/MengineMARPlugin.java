package org.Mengine.Plugin.MAR;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginKeyListener;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;

import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;

import com.mar.sdk.MARCallBack;
import com.mar.sdk.MARCode;
import com.mar.sdk.MARSDK;
import com.mar.sdk.PayParams;
import com.mar.sdk.UserExtraData;
import com.mar.sdk.gg.MARGgPlatform;
import com.mar.sdk.gg.control.MggControl;
import com.mar.sdk.platform.MARExitListener;
import com.mar.sdk.platform.MARInitListener;
import com.mar.sdk.platform.MARPlatform;
import com.mar.sdk.verify.UToken;

import org.json.JSONObject;

import android.view.KeyEvent;

import java.util.Calendar;
import java.util.TimeZone;

public class MengineMARPlugin extends MenginePlugin implements MARInitListener, MenginePluginApplicationListener, MenginePluginKeyListener {
    public static final String PLUGIN_NAME = "MarSDK";
    public static final boolean PLUGIN_EMBEDDING = true;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        MARPlatform.getInstance().init(activity, this);
    }

    public void login() {
        this.logMessage("login");

        MengineActivity activity = this.getActivity();

        MARPlatform.getInstance().login(activity);
    }

    public void loginCustom(final String loginType) {
        this.logMessage("login custom loginType: %s", loginType);

        MengineActivity activity = this.getActivity();

        MARPlatform.getInstance().loginCustom(activity, loginType);
    }

    public void pay(int coinNum, String productID, String productName, String productDesc, int price) {
        this.logMessage("pay coinNum: %d productID: %s productName: %s productDesc: %s price: %d", coinNum, productID, productName, productDesc, price);

        MengineActivity activity = this.getActivity();

        PayParams params = new PayParams();
        params.setCoinNum(coinNum);        //当前玩家身上拥有的游戏币数量

        params.setPrice(price);        //单位 元
        params.setProductId(productID);    //产品ID
        params.setProductName(productName);    //产品名称
        params.setProductDesc(productDesc);    //产品描述

        params.setBuyNum(1);		//购买数量，固定1

        params.setExtension(System.currentTimeMillis()+"");	//游戏自定义数据，充值成功，回调游戏服的时候，会原封不动返回
        params.setRoleId("100");		//角色ID
        params.setRoleLevel(10);		//角色等级
        params.setRoleName("test_112");	//角色名称
        params.setServerId("10");		//服务器ID
        params.setServerName("测试");		//服务器名称
        params.setVip("1");			//角色VIP等级

        this.logMessage("pay params: %s"
            , params.toString()
        );

        MARPlatform.getInstance().pay(activity, params);
    }

    public void redeemCode(String code) {
        this.logMessage("try redeem code %s", code);

        MARPlatform.getInstance().exchangeGift(code);
    }

    public void showAd() {
        this.logMessage("show ad");

        if (MARGgPlatform.getInstance().getVideoFlag() == true) {
            MARGgPlatform.getInstance().showVideo();
        }
    }

    public void updateData(String json_str) {
        this.logMessage("update data: %s", json_str);

        MARPlatform.getInstance().updateGameArchive(json_str,1);
    }

    public void marSDKGetData(int serialNumber) {
        this.logMessage("get data [%d]", serialNumber);

        MARPlatform.getInstance().getGameArchive(serialNumber, new MARCallBack() {
            @Override
            public void onCallBack(String var1) {
                MengineMARPlugin.this.logMessage("get data from server, start...");

                MengineMARPlugin.this.pythonCall("onMarSDKGetData", var1);
            }
        });
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
    public void onMengineInitializeBaseServices(MengineActivity activity) {
    }

    @Override
    public void onMengineCreateApplication(MengineActivity activity) {
    }

    @Override
    public void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        MARSDK.getInstance().onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onStart(MengineActivity activity){
        MARSDK.getInstance().onStart();
    }

    @Override
    public void onPause(MengineActivity activity){
        MARSDK.getInstance().onPause();
    }

    @Override
    public void onResume(MengineActivity activity){
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
        this.logMessage("marsdk.onInitResult code: %d msg: %s", code, msg);

        switch(code) {
            case MARCode.CODE_INIT_SUCCESS:
                this.logMessage("marsdk init success");

                this.activateSemaphore("onMarSDKInitSuccess");
                break;
            case MARCode.CODE_INIT_FAIL:
                this.logError("marsdk init fail");

                this.activateSemaphore("onMarSDKInitFail");
                break;
        }
    }

    @Override
    public void onLoginResult(int code, UToken uToken) {
        this.logMessage("marsdk.onLoginResult code: %d uToken: %s"
            , code
            , uToken
        );

        switch(code) {
            case MARCode.CODE_LOGIN_SUCCESS: {
                this.logMessage("marsdk login success, game type [%d]"
                    , MARSDK.getInstance().getGameType()
                );

                int gameType = MARSDK.getInstance().getGameType();

                if ((gameType == 1 || gameType == 3) && !MggControl.getInstance().getFreeFlag()) {
                    MggControl.getInstance().reqAdControlInfo();
                }

                this.submitExtraData(UserExtraData.TYPE_ENTER_GAME);

                this.pythonCall("onMarSDKLoginSuccess");
            } break;
            case MARCode.CODE_LOGIN_FAIL: {
                this.logError("marsdk login fail");

                if (MARSDK.getInstance().getGameType() == 1) {
                    MARPlatform.getInstance().visitorLogin();
                }

                this.pythonCall("onMarSDKLoginFail");
            } break;
        }
    }
    
    @Override
    public void onSwitchAccount(UToken uToken) {
        this.logMessage("marsdk.onSwitchAccount uToken: %s"
            , uToken
        );

        if (uToken != null) {
            this.pythonCall("onMarSDKSwitchAccount");
        }
    }
    
    @Override
    public void onLogout() {
        this.logMessage("marsdk.onLogout");

        this.pythonCall("onMarSDKLogout");
    }    
    
    @Override
    public void onPayResult(int code, String msg) {
        this.logMessage("marsdk.onPayResult code: %d msg: %s", code, msg);
        
        try {
            JSONObject json = new JSONObject(msg);

            String productId = json.getString("productId");

            if (json.getInt("payResult") == 0) {
                String orderId = json.getString("orderId");

                this.logMessage("pay complete orderId: %s", orderId);

                this.setPropDeliveredComplete(orderId);

                this.pythonCall("onMarSDKPaySuccess", productId);
            } else {
                this.logError("pay fail");

                this.pythonCall("onMarSDKPayFail", productId);
            }
        } catch (Exception e) {
            this.logError("pay error");

            e.printStackTrace();
        }
    }

    @Override
    public void onRedeemResult(String msg) {
        this.logMessage("marsdk.onRedeemResult msg: %s", msg);

        int result;
        try {
            result = 0;

            JSONObject json = new JSONObject(msg);

            int propNumber = json.getInt("propNumber");
            String propType = json.getString("propType");
            String message = json.getString("msg");

            this.pythonCall("onMarSDKRedeemResult", result, propNumber, propType, message);
        } catch(Exception e) {
            e.printStackTrace();

            this.logError("redeem exception: %s"
                , e.getLocalizedMessage()
            );

            result = -1;

            this.pythonCall("onMarSDKRedeemResult", result, 0, "", "");
        }
    }

    @Override
    public void onResult(int code, String msg) {
        this.logMessage("marsdk.onResult code: %d msg: %s", code, msg);
        
        if (MARCode.CODE_AD_VIDEO_CALLBACK == code) {
            //play video callback msg : 1 suc 0 fail
            this.logMessage("Video callback: %s", msg);

            String watchAdTime = getCurrentTime();

            this.pythonCall("onMarSDKAdVideoCallback", msg, watchAdTime);
        }

        this.pythonCall("onMarSDKResult", code, msg);
    }

	private void submitExtraData(final int dataType) {
        this.logMessage("marsdk.submitExtraData dataType: %d", dataType);

		UserExtraData data = new UserExtraData();
		data.setDataType(dataType);
		data.setMoneyNum(100);
		data.setRoleCreateTime(System.currentTimeMillis()/1000);
		data.setRoleID("100");
		data.setRoleName("test_112");
		data.setRoleLevel("10");
		data.setRoleLevelUpTime(System.currentTimeMillis()/1000);
		data.setServerID(10);
		data.setServerName("server_10");

        this.logMessage("marsdk.submitExtraData data: %s"
            , data.toString()
        );

		MARPlatform.getInstance().submitExtraData(data);
	}
    
    public void setPropDeliveredComplete(String orderID) {
        this.logMessage("marsdk.setPropDeliveredComplete orderID: %s", orderID);

        MARPlatform.getInstance().setPropDeliveredComplete(orderID);
	}

	public void pasteCode() {
        this.logMessage("marsdk.pasteCode");

        try {
            String code = "";

            Context context = MengineActivity.getContext();

            ClipboardManager clipboard = (ClipboardManager)context.getSystemService(Context.CLIPBOARD_SERVICE);

            if (clipboard.hasPrimaryClip() == true) {
                ClipData clip = clipboard.getPrimaryClip();
                ClipData.Item item = clip.getItemAt(0);

                Object data = item.getText();

                if (data != null) {
                    code = data.toString();
                }
            }

            this.logMessage("try to paste code: %s", code);

            this.pythonCall("onMarSDKSaveClipboard", code);
        } catch (Exception e) {
            this.logError("paste code error");

            e.printStackTrace();
        }
    }

    public String getCurrentTime() {
        TimeZone tz = TimeZone.getTimeZone("Asia/Beijing");
        Calendar calendar = Calendar.getInstance(tz);
        int DATE = calendar.get(Calendar.DATE);
        int MONTH = calendar.get(Calendar.MONTH);
        int YEAR = calendar.get(Calendar.YEAR);
        String time = "" + DATE + "/" + MONTH + "/" + YEAR;
        this.logInfo("China (Beijing) time: %s", time);

        return time;
    }
}
