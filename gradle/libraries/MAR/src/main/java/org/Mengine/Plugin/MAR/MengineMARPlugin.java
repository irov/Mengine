package org.Mengine.Plugin.MAR;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.ThreadUtil;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;

import android.content.Intent;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;

import com.mar.sdk.MARCallBack;
import com.mar.sdk.SDKParams;
import com.mar.sdk.IAction;
import com.mar.sdk.MARCode;
import com.mar.sdk.MARSDK;
import com.mar.sdk.PayParams;
import com.mar.sdk.UserExtraData;
import com.mar.sdk.gg.MARGgPlatform;
import com.mar.sdk.gg.control.MggControl;
import com.mar.sdk.platform.MARExitListener;
import com.mar.sdk.platform.MARInitListener;
import com.mar.sdk.platform.MARPlatform;
import com.mar.sdk.plugin.MARUser;
import com.mar.sdk.utils.ResourceHelper;
import com.mar.sdk.verify.UToken;

import org.json.JSONObject;

import android.view.KeyEvent;

import java.util.Calendar;
import java.util.TimeZone;

public class MengineMARPlugin extends MenginePlugin implements MARInitListener {
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("MarSDK");
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        MARPlatform.getInstance().init(activity, this);
    }

    public void login() {
        MengineActivity activity = this.getActivity();

        MARPlatform.getInstance().login(activity);
    }

    public void loginCustom(final String loginType) {
        MengineActivity activity = this.getActivity();

        MARPlatform.getInstance().loginCustom(activity, loginType);
    }

    public void pay(int coinNum, String productID, String productName, String productDesc, int price) {
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

        // params.setPayNotifyUrl("http://www.game.com/pay/callback");

        MARPlatform.getInstance().pay(activity, params);
    }

    public void redeemCode(String code) {
        this.log("try redeem code %s", code);

        MARPlatform.getInstance().exchangeGift(code);
    }

    public void showAd() {
        if (MARGgPlatform.getInstance().getVideoFlag() == true) {
            MARGgPlatform.getInstance().showVideo();
        }
    }

    public void updateData(String json_str) {
        MARPlatform.getInstance().updateGameArchive(json_str,1);
    }

    public void marSDKGetData(int serialNumber) {
        MengineActivity activity = this.getActivity();

        MARPlatform.getInstance().getGameArchive(serialNumber, new MARCallBack() {
            @Override
            public void onCallBack(String var1) {
                MengineMARPlugin.this.log("get data from server, start...");

                MengineMARPlugin.this.pythonCall("onMarSDKGetData", var1);
            }
        });
    }

    @Override
    public void onAppCreate(MengineApplication application) {
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

        if( keyCode == KeyEvent.KEYCODE_BACK && action == KeyEvent.ACTION_DOWN ) {
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
                            public void onClick(DialogInterface dialog,
                                                int whichButton) {
                                //这里什么都不用做
                            }
                        });

                    builder.setNeutralButton("一会再玩",
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog,
                                                int whichButton) {
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
        this.log("marsdk.onInitResult code: %d msg: %s", code, msg);

        switch(code) {
            case MARCode.CODE_INIT_SUCCESS:
                this.log("marsdk init success");

                this.pythonCall("onMarSDKInitSuccess");
                break;
            case MARCode.CODE_INIT_FAIL:
                this.log("marsdk init fail");

                this.pythonCall("onMarSDKInitFail");
                break;
        }
    }

    @Override
    public void onLoginResult(int code, UToken uToken) {
        this.log("marsdk.onLoginResult code: %d uToken: %s", code, uToken);

        switch(code){
            case MARCode.CODE_LOGIN_SUCCESS:
                this.log("marsdk login success");

                //get control info
                int gameType = MARSDK.getInstance().getGameType();

                this.log("marsdk login game type: %d", gameType);
                
                if ((gameType == 1 || gameType == 3) && !MggControl.getInstance().getFreeFlag()){
                    MggControl.getInstance().reqAdControlInfo();
                }

                this.submitExtraData(UserExtraData.TYPE_ENTER_GAME);

                this.pythonCall("onMarSDKLoginSuccess");

                //m_activity.getData(1);
                break;
            case MARCode.CODE_LOGIN_FAIL:
                this.log("marsdk login fail");

                if (MARSDK.getInstance().getGameType() == 1){
                    MARPlatform.getInstance().visitorLogin();
                }

                this.pythonCall("onMarSDKLoginFail");
                break;
        }
    }
    
    @Override
    public void onSwitchAccount(UToken uToken) {
        this.log("marsdk.onSwitchAccount uToken: %s", uToken);

        if(uToken != null){
            this.pythonCall("onMarSDKSwitchAccount");
        }
    }
    
    @Override
    public void onLogout() {
        this.log("marsdk.onLogout");

        this.pythonCall("onMarSDKLogout");
    }    
    
    @Override
    public void onPayResult(int code, String msg) {
        this.log("marsdk.onPayResult code: %d msg: %s", code, msg);
        
        try {
            JSONObject json = new JSONObject(msg);

            String productId = json.getString("productId");

            if (json.getInt("payResult") == 0){
                this.log("pay complete orderId: %s", json.getString("orderId"));

                this.setPropDeliveredComplete(json.getString("orderId"));

                this.pythonCall("onMarSDKPaySuccess", productId);
            } else {
                this.log("pay fail");

                this.pythonCall("onMarSDKPayFail", productId);
            }
        } catch (Exception e){
            this.log("pay error");

            e.printStackTrace();
        }
    }

    @Override
    public void onRedeemResult(String msg) {
        this.log("marsdk.onRedeemResult msg: %s", msg);

        int result;
        try {
            result = 0;

            JSONObject json = new JSONObject(msg);

            int propNumber = json.getInt("propNumber");
            String propType = json.getString("propType");
            String message = json.getString("msg");

            this.pythonCall("onMarSDKRedeemResult", result, propNumber, propType, message);
        } catch(Exception e) {
            this.log("redeem error");

            result = -1;

            this.pythonCall("onMarSDKRedeemResult", result, 0, "", "");
            e.printStackTrace();
        }
    }

    @Override
    public void onResult(int code, String msg) {
        this.log("marsdk.onResult code: %d msg: %s", code, msg);
        
        if(MARCode.CODE_AD_VIDEO_CALLBACK == code){
            //play video callback msg : 1 suc 0 fail
            this.log("Video callback: %s", msg);
            String watchAdTime = getCurrentTime();

            this.pythonCall("onMarSDKAdVideoCallback", msg, watchAdTime);
        }

        this.pythonCall("onMarSDKResult", code, msg);
    }

	private void submitExtraData(final int dataType){
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
		MARPlatform.getInstance().submitExtraData(data);
		// queryRealNameInfo();
	}
    
    public void setPropDeliveredComplete(String orderID){
		MARPlatform.getInstance().setPropDeliveredComplete(orderID);
	}

	public void pasteCode() {
        try {
            String code = null;

            MengineActivity activity = this.getActivity();
            Context context = activity.getContext();

            ClipboardManager clipboard = (ClipboardManager)context.getSystemService(Context.CLIPBOARD_SERVICE);

            if (clipboard.hasPrimaryClip() == true) {
                ClipData clip = clipboard.getPrimaryClip();
                ClipData.Item item = clip.getItemAt(0);

                Object data = item.getText();

                if (data == null) {
                    code = null;
                } else
                    code = data.toString();
            }

            this.log("try to paste code: \"%s\"", code);

            this.pythonCall("onMarSDKSaveClipboard", code);
        }
        catch (Exception e)
        {
            this.log("paste code error");

            e.printStackTrace();
        }
    }

    public String getCurrentTime()
    {
        TimeZone tz = TimeZone.getTimeZone("Asia/Beijing");
        Calendar calendar = Calendar.getInstance(tz);
        String time = String.valueOf(
                calendar.get(Calendar.DATE)) + "/" +
                String.valueOf(calendar.get(Calendar.MONTH)) + "/" +
                String.valueOf(calendar.get(Calendar.YEAR));

        this.log("China (Beijing) time: %s", time);

        return time;
    }
}
