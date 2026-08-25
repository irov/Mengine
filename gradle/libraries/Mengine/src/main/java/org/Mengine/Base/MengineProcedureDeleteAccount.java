package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

import java.util.ArrayList;
import java.util.List;

public class MengineProcedureDeleteAccount implements MengineProcedureInterface {
    private static final MengineTag TAG = MengineTag.of("MNGPDeleteAccount");

    // These values are persistent analytics IDs and must match EDeleteAccountReason.
    private static final int DELETE_ACCOUNT_REASON_LOST_INTEREST = 1001;
    private static final int DELETE_ACCOUNT_REASON_START_OVER = 1002;
    private static final int DELETE_ACCOUNT_REASON_TOO_MANY_ADS = 1003;
    private static final int DELETE_ACCOUNT_REASON_SUPPORT_UNSOLVED = 1004;
    private static final int DELETE_ACCOUNT_REASON_OTHER = 1099;

    private static class DeleteAccountReason {
        final int m_id;
        @StringRes
        final int m_stringId;

        DeleteAccountReason(int id, @StringRes int stringId) {
            m_id = id;
            m_stringId = stringId;
        }
    }

    private static final List<DeleteAccountReason> DELETE_ACCOUNT_REASONS = List.of(
        new DeleteAccountReason(DELETE_ACCOUNT_REASON_LOST_INTEREST, R.string.mengine_delete_account_option_reason_1),
        new DeleteAccountReason(DELETE_ACCOUNT_REASON_START_OVER, R.string.mengine_delete_account_option_reason_2),
        new DeleteAccountReason(DELETE_ACCOUNT_REASON_TOO_MANY_ADS, R.string.mengine_delete_account_option_reason_3),
        new DeleteAccountReason(DELETE_ACCOUNT_REASON_SUPPORT_UNSOLVED, R.string.mengine_delete_account_option_reason_4),
        new DeleteAccountReason(DELETE_ACCOUNT_REASON_OTHER, R.string.mengine_delete_account_option_reason_5)
    );

    @Override
    public boolean execute(@NonNull MengineActivity activity) {
        MengineLog.logInfo(TAG, "request delete account");

        MengineUI.showAreYouSureAlertDialogRes(activity
            , () -> { //Yes
                MengineLog.logInfo(TAG, "delete account [YES]");

                MengineAnalytics.buildEvent("mng_try_delete_account")
                    .log();

                List<Integer> reasonStringIds = new ArrayList<>(DELETE_ACCOUNT_REASONS.size());

                for (DeleteAccountReason reason : DELETE_ACCOUNT_REASONS) {
                    reasonStringIds.add(reason.m_stringId);
                }

                MengineUI.showChooseOptionDialogRes(activity
                    , (optionIndex) -> {
                        DeleteAccountReason reason = DELETE_ACCOUNT_REASONS.get(optionIndex);

                        MengineLog.logInfo(TAG, "select delete account [YES] option index: %d id: %d"
                            , optionIndex
                            , reason.m_id
                        );

                        MengineAnalytics.buildEvent("mng_delete_account_option_accept")
                            .addParameterLong("option", reason.m_id)
                            .log();

                        activity.showDeleteAccountProgressDialog();
                        MengineNative.AndroidPlatform_deleteAccountAccepted();
                    }
                    , () -> {
                        MengineLog.logInfo(TAG, "delete account [CANCEL]");

                        MengineAnalytics.buildEvent("mng_delete_account_option_cancel")
                            .log();

                        activity.cancelDeleteAccountFlow();
                        MengineNative.AndroidPlatform_deleteAccountCanceled();
                    }
                    , reasonStringIds
                    , R.string.mengine_delete_account_option_title
                );
            }
            , () -> { //Cancel
                MengineLog.logInfo(TAG, "delete account [CANCEL]");

                MengineAnalytics.buildEvent("mng_delete_account_cancel")
                    .log();

                activity.cancelDeleteAccountFlow();
                MengineNative.AndroidPlatform_deleteAccountCanceled();
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
