package org.Mengine.Base;

import android.app.AlertDialog;
import android.graphics.Typeface;
import android.text.Layout;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.AlignmentSpan;
import android.text.style.StyleSpan;
import android.view.Gravity;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ProgressBar;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.StringRes;
import androidx.core.content.ContextCompat;
import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;
import java.util.function.Consumer;

public class MengineUI {
    public static final MengineTag TAG = MengineTag.of("MNGUI");

    public static void showToast(@Nullable MengineActivity activity, @NonNull String format, Object ... args) {
        if (MengineUtils.isActivityAvailable(activity) == false) {
            MengineLog.logError(TAG, "[ERROR] showToast activity is unavailable format: %s args: %s"
                , format
                , Arrays.toString(args)
            );

            return;
        }

        activity.runOnUiThread(() -> {
            if (MengineUtils.isActivityAvailable(activity) == false) {
                return;
            }

            String message = MengineLog.buildTotalMsg(format, args);

            Toast toast = Toast.makeText(activity, message, Toast.LENGTH_SHORT);

            toast.show();
        });
    }

    public static void showToastRes(@Nullable MengineActivity activity, @StringRes int formatId, Object ... args) {
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

    public static boolean showOkAlertDialog(@NonNull MengineActivity activity, Runnable ok, @NonNull String title, @NonNull String format, Object ... args) {
        if (MengineUI.showOkAlertDialog(activity, ok, null, title, format, args) == false) {
            return false;
        }

        return true;
    }

    static boolean showOkAlertDialog(@NonNull MengineActivity activity, @Nullable Runnable ok, @Nullable Runnable cancel, @NonNull String title, @NonNull String format, Object ... args) {
        if (MengineUtils.isActivityAvailable(activity) == false) {
            MengineLog.logError(TAG, "[ERROR] showOkAlertDialog activity is unavailable title: %s format: %s args: %s"
                , title
                , format
                , Arrays.toString(args)
            );

            return false;
        }

        MengineOperation<Runnable> operation = MengineUI.createDialogOperation("showOkAlertDialog");

        activity.runOnUiThread(() -> {
            if (MengineUtils.isActivityAvailable(activity) == false) {
                MengineLog.logError(TAG, "[ERROR] showOkAlertDialog activity became unavailable title: %s"
                    , title
                );

                operation.complete(cancel);

                return;
            }

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
                if (operation.isCompleted() == true) {
                    return;
                }

                MengineLog.logMessage(TAG, "select [OK] alert dialog OK clicked");

                operation.complete(ok);

                dialog.dismiss();
            });

            AlertDialog alert = builder.create();

            alert.show();
        });

        return true;
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

    public static AlertDialog showProgressAlertDialogRes(@NonNull MengineActivity activity, @StringRes int titleId, @StringRes int messageId) {
        if (MengineUtils.isActivityAvailable(activity) == false) {
            MengineLog.logError(TAG, "[ERROR] showProgressAlertDialogRes activity is unavailable titleId: %d messageId: %d"
                , titleId
                , messageId
            );

            return null;
        }

        String title = activity.getString(titleId);
        String message = activity.getString(messageId);

        int padding = Math.round(24.f * activity.getResources().getDisplayMetrics().density);

        FrameLayout container = new FrameLayout(activity);
        container.setPadding(0, padding, 0, padding);

        ProgressBar progress = new ProgressBar(activity);
        FrameLayout.LayoutParams progressParams = new FrameLayout.LayoutParams(
            ViewGroup.LayoutParams.WRAP_CONTENT,
            ViewGroup.LayoutParams.WRAP_CONTENT,
            Gravity.CENTER
        );

        container.addView(progress, progressParams);

        AlertDialog.Builder builder = new AlertDialog.Builder(activity);
        builder.setTitle(title);
        builder.setMessage(message);
        builder.setView(container);
        builder.setCancelable(false);

        AlertDialog alert = builder.create();
        alert.setCanceledOnTouchOutside(false);
        alert.show();

        return alert;
    }

    public static void showAllowPermissionAlertDialog(@NonNull MengineActivity activity, Runnable allow, Runnable denied, @NonNull String title, @NonNull String format, Object ... args) {
        MengineOperation<Runnable> operation = MengineUI.createDialogOperation("showAllowPermissionAlertDialog");

        if (MengineUtils.isActivityAvailable(activity) == false) {
            MengineLog.logError(TAG, "[ERROR] showAllowPermissionAlertDialog activity is unavailable title: %s format: %s args: %s"
                , title
                , format
                , Arrays.toString(args)
            );

            operation.complete(denied);

            return;
        }

        activity.runOnUiThread(() -> {
            if (MengineUtils.isActivityAvailable(activity) == false) {
                MengineLog.logError(TAG, "[ERROR] showAllowPermissionAlertDialog activity became unavailable title: %s"
                    , title
                );

                operation.complete(denied);

                return;
            }

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
                if (operation.isCompleted() == true) {
                    return;
                }

                MengineLog.logMessage(TAG, "select [ALLOW PERMISSION] alert dialog ALLOW clicked");

                operation.complete(allow);

                dialog.dismiss();
            });
            builder.setNegativeButton("NO THANKS", (dialog, id) -> {
                if (operation.isCompleted() == true) {
                    return;
                }

                MengineLog.logMessage(TAG, "select [ALLOW PERMISSION] alert dialog NO clicked");

                operation.complete(denied);

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

            MengineUtils.performOnMainThread(() -> {
                MengineUI.runOutcome(denied);
            });

            return;
        }

        String title = activity.getString(titleId);
        String message = activity.getString(messageId, args);

        MengineUI.showAllowPermissionAlertDialog(activity, allow, denied, title, message);
    }

    public static void showAreYouSureAlertDialog(@NonNull MengineActivity activity, Runnable yes, Runnable cancel, long delayMillis, @NonNull String title, @NonNull String format, Object ... args) {
        MengineOperation<Runnable> operation = MengineUI.createDialogOperation("showAreYouSureAlertDialog");

        if (MengineUtils.isActivityAvailable(activity) == false) {
            MengineLog.logError(TAG, "[ERROR] showAreYouSureAlertDialog activity is unavailable title: %s format: %s args: %s"
                , title
                , format
                , Arrays.toString(args)
            );

            operation.complete(cancel);

            return;
        }

        activity.runOnUiThread(() -> {
            if (MengineUtils.isActivityAvailable(activity) == false) {
                MengineLog.logError(TAG, "[ERROR] showAreYouSureAlertDialog activity became unavailable title: %s"
                    , title
                );

                operation.complete(cancel);

                return;
            }

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
                if (operation.isCompleted() == true) {
                    return;
                }

                MengineLog.logMessage(TAG, "select [ARE YOU SURE] alert dialog YES clicked");

                operation.complete(yes);

                dialog.dismiss();
            });
            builder.setNegativeButton("CANCEL", (dialog, id) -> {
                if (operation.isCompleted() == true) {
                    return;
                }

                MengineLog.logMessage(TAG, "select [ARE YOU SURE] alert dialog CANCEL clicked");

                operation.complete(cancel);

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

            MengineUtils.performOnMainThread(() -> {
                MengineUI.runOutcome(cancel);
            });

            return;
        }

        String title = activity.getString(titleId);
        String message = activity.getString(formatId, args);

        MengineUI.showAreYouSureAlertDialog(activity, yes, cancel, delayMillis, title, message);
    }

    public static void showChooseOptionDialog(@NonNull MengineActivity activity, @NonNull Consumer<Integer> accept, @NonNull Runnable cancel, @NonNull List<String> options, @NonNull String title) {
        MengineOperation<Runnable> operation = MengineUI.createDialogOperation("showChooseOptionDialog");

        if (MengineUtils.isActivityAvailable(activity) == false) {
            MengineLog.logError(TAG, "[ERROR] showChooseOptionDialog activity is unavailable title: %s options: %s"
                , title
                , options
            );

            operation.complete(cancel);

            return;
        }

        activity.runOnUiThread(() -> {
            if (MengineUtils.isActivityAvailable(activity) == false) {
                MengineLog.logError(TAG, "[ERROR] showChooseOptionDialog activity became unavailable title: %s"
                    , title
                );

                operation.complete(cancel);

                return;
            }

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
                if (operation.isCompleted() == true) {
                    return;
                }

                Integer option = selectedPos[0];

                MengineLog.logMessage(TAG,"click [CHOOSE OPTION] accept option: %d"
                    , option
                );

                operation.complete(() -> {
                    if (accept == null) {
                        return;
                    }

                    accept.accept(option);
                });

                dialog.dismiss();
            });

            builder.setNegativeButton("CANCEL", (dialog, id) -> {
                if (operation.isCompleted() == true) {
                    return;
                }

                MengineLog.logMessage(TAG, "click [CHOOSE OPTION] cancel");

                operation.complete(cancel);

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

            MengineUtils.performOnMainThread(() -> {
                MengineUI.runOutcome(cancel);
            });

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

    private static MengineOperation<Runnable> createDialogOperation(@NonNull String name) {
        return new MengineOperation<>(name) {
            @Override
            protected void onComplete(@Nullable Runnable action) {
                MengineUI.runOutcome(action);
            }
        };
    }

    private static void runOutcome(@Nullable Runnable outcome) {
        if (outcome == null) {
            return;
        }

        outcome.run();
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
