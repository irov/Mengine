package org.Mengine.Base;

import android.app.AlertDialog;
import android.graphics.Typeface;
import android.text.Layout;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.AlignmentSpan;
import android.text.style.StyleSpan;
import android.widget.Button;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;
import androidx.core.content.ContextCompat;
import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;
import java.util.function.Consumer;

public class MengineUI {
    public static final MengineTag TAG = MengineTag.of("MNGUI");

    public static void showToast(@NonNull MengineActivity activity, @NonNull String format, Object ... args) {
        if (activity == null) {
            MengineLog.logError(TAG, "[ERROR] showToast activity is null format: %s args: %s"
                , format
                , Arrays.toString(args)
            );

            return;
        }

        activity.runOnUiThread(() -> {
            String message = MengineLog.buildTotalMsg(format, args);

            Toast toast = Toast.makeText(activity, message, Toast.LENGTH_SHORT);

            toast.show();
        });
    }

    public static void showToastRes(@NonNull MengineActivity activity, @StringRes int formatId, Object ... args) {
        if (activity == null) {
            MengineLog.logError(TAG, "[ERROR] showToastRes activity is null formatId: %d args: %s"
                , formatId
                , Arrays.toString(args)
            );

            return;
        }

        String format = activity.getString(formatId, args);

        MengineUI.showToast(activity, format);
    }

    public static void showOkAlertDialog(@NonNull MengineActivity activity, Runnable ok, @NonNull String title, @NonNull String format, Object ... args) {
        if (activity == null) {
            MengineLog.logError(TAG, "[ERROR] showOkAlertDialog activity is null title: %s format: %s args: %s"
                , title
                , format
                , Arrays.toString(args)
            );

            return;
        }

        activity.runOnUiThread(() -> {
            String message = MengineLog.buildTotalMsg(format, args);

            MengineLog.logMessage(TAG, "show [OK] alert dialog title: %s message: %s"
                , title
                , message
            );

            AlertDialog.Builder builder = new AlertDialog.Builder(activity);

            builder.setTitle(title);
            builder.setMessage(message);
            builder.setCancelable(false);
            builder.setPositiveButton("OK", (dialog, which) -> {
                MengineLog.logMessage(TAG, "select [OK] alert dialog OK clicked");

                if (ok != null) {
                    ok.run();
                }

                dialog.dismiss();
            });

            AlertDialog alert = builder.create();

            alert.show();
        });
    }

    public static void showOkAlertDialogRes(@NonNull MengineActivity activity, Runnable ok, @StringRes int titleId, @StringRes int formatId, Object ... args) {
        if (activity == null) {
            MengineLog.logError(TAG, "[ERROR] showOkAlertDialogRes activity is null titleId: %d formatId: %d args: %s"
                , titleId
                , formatId
                , Arrays.toString(args)
            );

            return;
        }

        String title = activity.getString(titleId);
        String message = activity.getString(formatId, args);

        MengineUI.showOkAlertDialog(activity, ok, title, message);
    }

    public static void showAllowPermissionAlertDialog(@NonNull MengineActivity activity, Runnable allow, Runnable denied, @NonNull String title, @NonNull String format, Object ... args) {
        if (activity == null) {
            MengineLog.logError(TAG, "[ERROR] showAllowPermissionAlertDialog activity is null title: %s format: %s args: %s"
                , title
                , format
                , Arrays.toString(args)
            );

            return;
        }

        activity.runOnUiThread(() -> {
            String message = MengineLog.buildTotalMsg(format, args);

            MengineLog.logMessage(TAG, "show [ALLOW PERMISSION] alert dialog title: %s message: %s"
                , title
                , message
            );

            AlertDialog.Builder builder = new AlertDialog.Builder(activity);

            builder.setTitle(title);
            builder.setMessage(message);
            builder.setCancelable(false);
            builder.setPositiveButton("ALLOW", (dialog, which) -> {
                MengineLog.logMessage(TAG, "select [ALLOW PERMISSION] alert dialog ALLOW clicked");

                if (allow != null) {
                    allow.run();
                }

                dialog.dismiss();
            });
            builder.setNegativeButton("NO THANKS", (dialog, id) -> {
                MengineLog.logMessage(TAG, "select [ALLOW PERMISSION] alert dialog NO clicked");

                if (denied != null) {
                    denied.run();
                }

                dialog.dismiss();
            });

            AlertDialog alert = builder.create();

            alert.show();
        });
    }

    public static void showAllowPermissionAlertDialogRes(@NonNull MengineActivity activity, Runnable allow, Runnable denied, @StringRes int titleId, @StringRes int messageId, Object ... args) {
        if (activity ==  null){
            MengineLog.logError(TAG, "[ERROR] showAllowPermissionAlertDialogRes activity is null titleId: %d messageId: %d args: %s"
                , titleId
                , messageId
                , Arrays.toString(args)
            );

            return;
        }

        String title = activity.getString(titleId);
        String message = activity.getString(messageId, args);

        MengineUI.showAllowPermissionAlertDialog(activity, allow, denied, title, message);
    }

    public static void showAreYouSureAlertDialog(@NonNull MengineActivity activity, Runnable yes, Runnable cancel, long delayMillis, @NonNull String title, @NonNull String format, Object ... args) {
        if (activity == null) {
            MengineLog.logError(TAG, "[ERROR] showAreYouSureAlertDialog activity is null title: %s format: %s args: %s"
                , title
                , format
                , Arrays.toString(args)
            );

            return;
        }

        activity.runOnUiThread(() -> {
            String message = MengineLog.buildTotalMsg(format, args);

            MengineLog.logMessage(TAG, "show [ARE YOU SURE] alert dialog title: %s message: %s"
                , title
                , message
            );

            String AreYouSureText = "\n\nAre you sure?";

            SpannableString spannableMessage = new SpannableString(message + AreYouSureText);

            spannableMessage.setSpan(new StyleSpan(Typeface.BOLD), message.length(), (message + AreYouSureText).length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
            spannableMessage.setSpan(new AlignmentSpan.Standard(Layout.Alignment.ALIGN_OPPOSITE), message.length(), spannableMessage.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);

            AlertDialog.Builder builder = new AlertDialog.Builder(activity);

            builder.setTitle(title);
            builder.setMessage(spannableMessage);
            builder.setCancelable(false);
            builder.setPositiveButton("YES", (dialog, which) -> {
                MengineLog.logMessage(TAG, "select [ARE YOU SURE] alert dialog YES clicked");

                if (yes != null) {
                    yes.run();
                }

                dialog.dismiss();
            });
            builder.setNegativeButton("CANCEL", (dialog, id) -> {
                MengineLog.logMessage(TAG, "select [ARE YOU SURE] alert dialog CANCEL clicked");

                if (cancel != null) {
                    cancel.run();
                }

                dialog.dismiss();
            });

            AlertDialog alert = builder.create();

            alert.show();

            int darker_gray = ContextCompat.getColor(activity, android.R.color.darker_gray);
            int holo_red_light = ContextCompat.getColor(activity, android.R.color.holo_red_light);

            if (delayMillis > 0) {
                alert.getButton(AlertDialog.BUTTON_POSITIVE).setTextColor(darker_gray);
                alert.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(false);

                MengineUtils.performOnMainThreadDelayed(() -> {
                    alert.getButton(AlertDialog.BUTTON_POSITIVE).setTextColor(holo_red_light);
                    alert.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(true);
                }, delayMillis);
            } else {
                alert.getButton(AlertDialog.BUTTON_POSITIVE).setTextColor(holo_red_light);
            }
        });
    }

    public static void showAreYouSureAlertDialogRes(@NonNull MengineActivity activity, Runnable yes, Runnable cancel, long delayMillis, @StringRes int titleId, @StringRes int formatId, Object ... args) {
        if (activity == null) {
            MengineLog.logError(TAG, "[ERROR] showAreYouSureAlertDialogRes activity is null titleId: %d formatId: %d args: %s"
                , titleId
                , formatId
                , Arrays.toString(args)
            );

            return;
        }

        String title = activity.getString(titleId);
        String message = activity.getString(formatId, args);

        MengineUI.showAreYouSureAlertDialog(activity, yes, cancel, delayMillis, title, message);
    }

    public static void showChooseOptionDialog(@NonNull MengineActivity activity, @NonNull Consumer<Integer> accept, @NonNull Runnable cancel, @NonNull List<String> options, @NonNull String title) {
        if (activity == null) {
            MengineLog.logError(TAG, "[ERROR] showChooseOptionDialog activity is null title: %s options: %s"
                , title
                , options
            );

            return;
        }

        activity.runOnUiThread(() -> {
            MengineLog.logMessage(TAG, "show [CHOOSE OPTION] title: %s"
                , title
            );

            int darker_gray = ContextCompat.getColor(activity, android.R.color.darker_gray);
            int holo_red_light = ContextCompat.getColor(activity, android.R.color.holo_red_light);

            AlertDialog.Builder builder = new AlertDialog.Builder(activity);
            builder.setTitle(title);

            CharSequence[] items = options.toArray(new CharSequence[0]);

            final Integer [] selectedPos = { -1 };

            builder.setSingleChoiceItems(items, selectedPos[0], (dialog, which) -> {
                selectedPos[0] = which;

                Button positive = ((AlertDialog) dialog).getButton(AlertDialog.BUTTON_POSITIVE);
                positive.setEnabled(true);
                positive.setTextColor(holo_red_light);
            });

            builder.setCancelable(false);

            builder.setPositiveButton("ACCEPT", (dialog, which) -> {
                int option = selectedPos[0];

                MengineLog.logMessage(TAG,"click [CHOOSE OPTION] accept option: %d"
                    , option
                );

                if (accept != null) {
                    accept.accept(option);
                }

                dialog.dismiss();
            });

            builder.setNegativeButton("CANCEL", (dialog, id) -> {
                MengineLog.logMessage(TAG, "click [CHOOSE OPTION] cancel");

                if (cancel != null) {
                    cancel.run();
                }

                dialog.dismiss();
            });

            AlertDialog alert = builder.create();

            alert.show();

            alert.getButton(AlertDialog.BUTTON_POSITIVE).setTextColor(darker_gray);
            alert.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(false);
        });
    }

    public static void showChooseOptionDialogRes(@NonNull MengineActivity activity, @NonNull Consumer<Integer> accept, @NonNull Runnable cancel, @NonNull List<Integer> optionIds, @StringRes int titleId) {
        if (activity == null) {
            MengineLog.logError(TAG, "[ERROR] showChooseOptionDialogRes activity is null titleId: %d optionIds: %s"
                , titleId
                , optionIds
            );

            return;
        }

        String title = activity.getString(titleId);

        List<String> options = new ArrayList<>(optionIds.size());
        for (@StringRes int optionId : optionIds) {
            String option = activity.getString(optionId);
            options.add(option);
        }

        MengineUI.showChooseOptionDialog(activity, accept, cancel, options, title);
    }

    public static void finishActivityWithAlertDialog(@NonNull MengineActivity activity, @NonNull String title, @NonNull String format, Object ... args) {
        if (activity == null) {
            MengineLog.logError(TAG, "[ERROR] finishActivityWithAlertDialog activity is null title: %s format: %s args: %s"
                , title
                , format
                , Arrays.toString(args)
            );

            return;
        }

        MengineLog.logError(TAG, format, args);

        MengineUtils.debugBreak();

        MengineUI.showOkAlertDialog(activity, () -> {
            activity.finishAndRemoveTask();
        }, title, format, args);
    }

    public static void finishActivityWithAlertDialogRes(@NonNull MengineActivity activity, @StringRes int titleId, @StringRes int messageId, Object ... args) {
        if (activity == null) {
            MengineLog.logError(TAG, "[ERROR] finishActivityWithAlertDialogRes activity is null titleId: %d messageId: %d args: %s"
                , titleId
                , messageId
                , Arrays.toString(args)
            );

            return;
        }

        String title = activity.getString(titleId);
        String message = activity.getString(messageId, args);

        MengineUI.finishActivityWithAlertDialog(activity, title, message);
    }
}
