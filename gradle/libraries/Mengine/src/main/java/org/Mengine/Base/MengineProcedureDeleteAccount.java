package org.Mengine.Base;

import androidx.annotation.NonNull;

import java.util.List;

public class MengineProcedureDeleteAccount implements MengineProcedureInterface {
    private static final MengineTag TAG = MengineTag.of("MenginePDeleteAccount");

    public MengineProcedureDeleteAccount() {
    }

    @Override
    public boolean execute(@NonNull MengineActivity activity) {
        MengineLog.logInfo(TAG, "request delete account");

        MengineUI.showAreYouSureAlertDialogRes(activity
            , () -> { //Yes
                MengineLog.logInfo(TAG, "delete account [YES]");

                MengineAnalytics.buildEvent("mng_try_delete_account")
                    .log();

                MengineUI.showChooseOptionDialogRes(activity
                    , (option) -> {
                        MengineLog.logInfo(TAG, "select delete account [YES] option: %d"
                            , option
                        );

                        MengineAnalytics.buildEvent("mng_delete_account_option_accept")
                            .addParameterLong("option", option)
                            .log();

                        MengineApplication application = MengineApplication.INSTANCE;
                        application.removeUserData();

                        MengineUI.showOkAlertDialogRes(activity, () -> {
                            MengineLog.logInfo(TAG, "finish delete account");

                            activity.finishAndRemoveTask();
                        }, R.string.mengine_delete_account_success_title, R.string.mengine_delete_account_success_message);
                    }
                    , () -> {
                        MengineLog.logInfo(TAG, "delete account [CANCEL]");

                        MengineAnalytics.buildEvent("mng_delete_account_option_cancel")
                            .log();
                    }
                    , List.of(
                        R.string.mengine_delete_account_option_reason_1,
                        R.string.mengine_delete_account_option_reason_2,
                        R.string.mengine_delete_account_option_reason_3,
                        R.string.mengine_delete_account_option_reason_4,
                        R.string.mengine_delete_account_option_reason_5
                    )
                    , R.string.mengine_delete_account_option_title
                );
            }
            , () -> { //Cancel
                MengineLog.logInfo(TAG, "delete account [CANCEL]");

                MengineAnalytics.buildEvent("mng_delete_account_cancel")
                    .log();
            }
            , 3000
            , R.string.mengine_delete_account_try_title
            , R.string.mengine_delete_account_try_message
        );

        return true;
    }

    static {
        MengineFactoryManager.registerClazz("deleteAccount", MengineProcedureDeleteAccount.class);
    }
}