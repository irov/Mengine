package org.Mengine.Plugin.GooglePlayBilling;

import android.os.Bundle;
import android.os.Handler;

import androidx.annotation.MainThread;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.android.billingclient.api.AccountIdentifiers;
import com.android.billingclient.api.AcknowledgePurchaseParams;
import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.PendingPurchasesParams;
import com.android.billingclient.api.ProductDetails;
import com.android.billingclient.api.ProductDetailsResponseListener;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.QueryProductDetailsParams;
import com.android.billingclient.api.QueryPurchasesParams;
import com.android.billingclient.api.UnfetchedProduct;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineCallback;
import org.Mengine.Base.MengineFragmentInAppPurchase;
import org.Mengine.Base.MengineParamInAppProduct;
import org.Mengine.Base.MengineParamInAppPurchase;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MenginePreferences;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUI;
import org.Mengine.Base.MengineUtils;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.function.Consumer;

public class MengineGooglePlayBillingPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity {
    public static final String SERVICE_NAME = "GPlayBilling";
    public static final boolean SERVICE_EMBEDDING = true;

    private static final int ERROR_CODE_UNKNOWN = 0;
    private static final int ERROR_CODE_NOT_FOUND = 1;
    private static final int ERROR_CODE_NOT_SUPPORTED = 2;
    private static final int ERROR_CODE_NOT_INITIALIZED = 3;
    private static final int ERROR_CODE_NOT_READY = 4;
    private static final int RESTORE_QUERY_MAX_RETRIES = 2;
    private static final long RESTORE_QUERY_RETRY_DELAY_MS = 1000;
    private static final String PREFERENCE_PURCHASE_DELIVERED_TOKENS = "mengine.billing.purchase.delivered.tokens";

    private volatile BillingClient m_billingClient;
    private boolean m_billingConnecting;
    private boolean m_billingConnectedOnce;
    private boolean m_productsQueried;
    private QueryProductDetailsParams m_queryProducts;
    private MengineGooglePlayBillingBuyInApp m_buyInApp;
    private MengineGooglePlayBillingRestorePurchases m_restorePurchases;
    private final Map<String, MengineGooglePlayBillingPurchaseProcessing> m_purchaseProcessings = new HashMap<>();

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        m_billingConnecting = false;
        m_billingConnectedOnce = false;
        m_productsQueried = false;

        PurchasesUpdatedListener purchasesUpdatedListener = (billingResult, purchases) -> {
            BillingClient billingClient = m_billingClient;

            if (billingClient == null) {
                return;
            }

            this.runBillingCallback(billingClient, () -> {
                this.onPurchasesUpdated(billingResult, purchases);
            });
        };

        PendingPurchasesParams pendingPurchasesParams = PendingPurchasesParams.newBuilder()
            .enableOneTimeProducts()
            .build();

        m_billingClient = BillingClient.newBuilder(application)
            .setListener(purchasesUpdatedListener)
            .enablePendingPurchases(pendingPurchasesParams)
            .enableAutoServiceReconnection()
            .build();
    }

    private void runBillingCallback(@NonNull BillingClient billingClient, @NonNull Runnable callback) {
        MengineUtils.performOnMainThread(() -> {
            if (m_billingClient != billingClient) {
                return;
            }

            callback.run();
        });
    }

    @MainThread
    private void onPurchasesUpdated(@NonNull BillingResult billingResult, @Nullable List<Purchase> purchases) {
        int responseCode = billingResult.getResponseCode();
        String debugMessage = billingResult.getDebugMessage();
        String productId = this.completeBuyInApp(responseCode, purchases);

        switch (responseCode) {
            case BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED: {
                this.logWarning("onPurchasesUpdated [Feature not supported] message: %s"
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedFeatureNotSupported", productId);
            }break;
            case BillingClient.BillingResponseCode.SERVICE_DISCONNECTED: {
                this.logWarning("onPurchasesUpdated [Service disconnected] message: %s"
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedServiceDisconnected", productId);
            }break;
            case BillingClient.BillingResponseCode.OK: {
                this.logInfo("onPurchasesUpdated [Purchase update received]");

                if (purchases != null) {
                    this.handlePurchases(purchases);
                }

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedOk", productId);
            }break;
            case BillingClient.BillingResponseCode.USER_CANCELED: {
                this.logInfo("onPurchasesUpdated [User canceled the purchase]");

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedUserCanceled", productId);
            }break;
            case BillingClient.BillingResponseCode.SERVICE_UNAVAILABLE: {
                this.logWarning("onPurchasesUpdated [Service Unavailable] message: %s"
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedServiceUnavailable", productId);
            }break;
            case BillingClient.BillingResponseCode.BILLING_UNAVAILABLE: {
                this.logWarning("onPurchasesUpdated [Billing Unavailable] message: %s"
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedBillingUnavailable", productId);
            }break;
            case BillingClient.BillingResponseCode.ITEM_UNAVAILABLE: {
                this.logWarning("onPurchasesUpdated [The user item unavailable] message: %s"
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedItemUnavailable", productId);
            }break;
            case BillingClient.BillingResponseCode.DEVELOPER_ERROR: {
                this.logError(
                    "onPurchasesUpdated: Developer error means that Google Play " +
                        "does not recognize the configuration. If you are just getting started, " +
                        "make sure you have configured the application correctly in the " +
                        "Google Play Console. The SKU product ID must match and the APK you " +
                        "are using must be signed with release keys."
                );

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedDeveloperError", productId);
            }break;
            case BillingClient.BillingResponseCode.ERROR: {
                this.logWarning("onPurchasesUpdated [Error] message: %s"
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedError", productId);
            }break;
            case BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED: {
                this.logWarning("onPurchasesUpdated [The user already owns this item] message: %s"
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedItemAlreadyOwned", productId);
            }break;
            case BillingClient.BillingResponseCode.ITEM_NOT_OWNED: {
                this.logWarning("onPurchasesUpdated [The user item not owned] message: %s"
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedItemNotOwned", productId);
            }break;
            case BillingClient.BillingResponseCode.NETWORK_ERROR: {
                this.logWarning("onPurchasesUpdated [Network error] message: %s"
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedNetworkError", productId);
            }break;
            default: {
                this.logWarning("onPurchasesUpdated [Unsupported response] code: %d message: %s"
                    , responseCode
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingPurchasesUpdatedUnknown", productId, responseCode);
            }break;
        }
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        BillingClient billingClient = m_billingClient;
        m_billingClient = null;

        m_billingConnecting = false;
        m_billingConnectedOnce = false;
        m_productsQueried = false;
        m_queryProducts = null;

        if (m_buyInApp != null) {
            this.completeBuyInApp(m_buyInApp);
        }

        if (m_restorePurchases != null) {
            this.completeRestorePurchases(m_restorePurchases);
        }

        List<MengineGooglePlayBillingPurchaseProcessing> purchaseProcessings = new ArrayList<>(m_purchaseProcessings.values());

        for (MengineGooglePlayBillingPurchaseProcessing processing : purchaseProcessings) {
            processing.complete(false);
        }

        if (billingClient != null) {
            billingClient.endConnection();
        }
    }

    @MainThread
    protected void billingConnect() {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            return;
        }

        if (billingClient.isReady() == true) {
            return;
        }

        if (m_billingConnecting == true) {
            return;
        }

        m_billingConnecting = true;

        billingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingServiceDisconnected() {
                MengineGooglePlayBillingPlugin.this.runBillingCallback(billingClient, () -> {
                    m_billingConnecting = false;

                    MengineGooglePlayBillingPlugin.this.logInfo("Billing disconnected");
                });
            }

            @Override
            public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
                MengineGooglePlayBillingPlugin.this.runBillingCallback(billingClient, () -> {
                    MengineGooglePlayBillingPlugin.this.onBillingSetupFinished(billingClient, billingResult);
                });
            }
        });
    }

    @MainThread
    private void onBillingSetupFinished(@NonNull BillingClient billingClient, @NonNull BillingResult billingResult) {
        m_billingConnecting = false;

        int responseCode = billingResult.getResponseCode();

        if (responseCode != BillingClient.BillingResponseCode.OK) {
            this.logInfo("billing invalid connection responseCode: %d message: %s"
                , responseCode
                , billingResult.getDebugMessage()
            );

            return;
        }

        this.logInfo("billing setup finished");

        BillingResult supportedProductDetails = billingClient.isFeatureSupported(BillingClient.FeatureType.PRODUCT_DETAILS);

        if (supportedProductDetails.getResponseCode() == BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED) {
            this.logError("[ERROR] queryProducts billing client feature PRODUCT_DETAILS is not supported");
            MengineUI.showToastRes(this.getMengineActivity(), R.string.mengine_googleplaybilling_asks_update_playstore);
        }

        boolean reconnected = m_billingConnectedOnce;
        m_billingConnectedOnce = true;

        this.activateSemaphore("GooglePlayBillingReady");

        if (reconnected == true) {
            this.nativeCall("onGooglePlayBillingReconnected");
        }

        if (m_productsQueried == true) {
            this.queryOwnedPurchases();
        }
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        this.billingConnect();
    }

    @Override
    public void onResume(@NonNull MengineActivity activity) {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            return;
        }

        if (billingClient.isReady() == false) {
            this.billingConnect();

            return;
        }

        if (m_productsQueried == false) {
            return;
        }

        this.queryOwnedPurchases();
    }

    @MainThread
    private void queryOwnedPurchases() {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            return;
        }

        if (billingClient.isReady() == false) {
            return;
        }

        QueryPurchasesParams purchasesParams = QueryPurchasesParams.newBuilder()
            .setProductType(BillingClient.ProductType.INAPP)
            .build();

        billingClient.queryPurchasesAsync(purchasesParams, (billingResult, purchases) -> {
            this.runBillingCallback(billingClient, () -> {
                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    this.logWarning("query owned purchases failed code: %d message: %s"
                        , responseCode
                        , billingResult.getDebugMessage()
                    );

                    return;
                }

                this.handlePurchases(purchases);

                Set<String> owned = new HashSet<>();

                for (Purchase purchase : purchases) {
                    if (purchase.isAcknowledged() == false) {
                        continue;
                    }

                    int state = purchase.getPurchaseState();

                    if (state != Purchase.PurchaseState.PURCHASED) {
                        continue;
                    }

                    List<String> products = purchase.getProducts();

                    owned.addAll(products);
                }

                this.logInfo("owned products: %s"
                    , owned
                );

                MengineFragmentInAppPurchase.INSTANCE.ownedInAppProducts(owned);
            });
        });
    }

    public boolean isOwnedInAppProduct(@NonNull String product) {
        boolean owned = MengineFragmentInAppPurchase.INSTANCE.isOwnedInAppProduct(product);

        return owned;
    }

    public void queryProducts(@NonNull List<String> consumableProducts, @NonNull List<String> nonConsumableProducts) {
        MengineUtils.performOnMainThread(() -> {
            this.startQueryProducts(consumableProducts, nonConsumableProducts);
        });
    }

    @MainThread
    private void startQueryProducts(@NonNull List<String> consumableProducts, @NonNull List<String> nonConsumableProducts) {
        if (consumableProducts.isEmpty() == true) {
            if (nonConsumableProducts.isEmpty() == true) {
                this.logError("[Error] queryProducts empty products list");

                this.nativeCall("onGooglePlayBillingQueryProductError", ERROR_CODE_NOT_FOUND, new IllegalArgumentException("Products list is empty"));

                return;
            }
        }

        this.logInfo("queryProducts consumableProducts: %s nonConsumableProducts: %s"
            , consumableProducts
            , nonConsumableProducts
        );

        List<String> products = new ArrayList<>();

        products.addAll(consumableProducts);
        products.addAll(nonConsumableProducts);

        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            this.logError("[ERROR] queryProducts billing client not created");

            this.nativeCall("onGooglePlayBillingQueryProductError", ERROR_CODE_NOT_INITIALIZED, new RuntimeException("Billing client not initialized"));

            return;
        }

        if (billingClient.isReady() == false) {
            this.logInfo("queryProducts billing client not ready");

            this.nativeCall("onGooglePlayBillingQueryProductFailed");

            this.billingConnect();

            return;
        }

        if (m_queryProducts != null) {
            this.logWarning("queryProducts already in progress, the request waits for its result");

            return;
        }

        List<QueryProductDetailsParams.Product> productList = new ArrayList<>();

        for (String productId : products) {
            QueryProductDetailsParams.Product product = QueryProductDetailsParams.Product.newBuilder()
                .setProductId(productId)
                .setProductType(BillingClient.ProductType.INAPP)
                .build();

            productList.add(product);
        }

        QueryProductDetailsParams.Builder builder = QueryProductDetailsParams.newBuilder();
        builder.setProductList(productList);

        QueryProductDetailsParams params = builder.build();
        m_queryProducts = params;

        ProductDetailsResponseListener listener = (billingResult, productDetailsResult) -> {
            if (m_billingClient != billingClient) {
                return;
            }

            if (m_queryProducts != params) {
                return;
            }

            m_queryProducts = null;

            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                String debugMessage = billingResult.getDebugMessage();

                if (this.isBillingResponseRetryable(responseCode) == true) {
                    this.logWarning("billing query product details temporary failure code: %d message: %s"
                        , responseCode
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingQueryProductFailed");

                    return;
                }

                this.logError("[ERROR] billing invalid query product details code: %d message: %s"
                    , responseCode
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingQueryProductError", responseCode, new RuntimeException(debugMessage));

                return;
            }

            List<ProductDetails> productsDetails = productDetailsResult.getProductDetailsList();

            this.logInfo("billing query products details: %s"
                , productsDetails
            );

            List<UnfetchedProduct> unfetchedProducts = productDetailsResult.getUnfetchedProductList();

            this.logInfo("billing query products unfetched: %s"
                , unfetchedProducts
            );

            ArrayList<MengineParamInAppProduct> products1 = new ArrayList<>();

            for(ProductDetails details : productsDetails) {
                MengineParamInAppProduct product = new MengineParamInAppProduct();

                String productId = details.getProductId();
                String productType = details.getProductType();
                String name = details.getName();
                String title = details.getTitle();
                String description = details.getDescription();

                product.INAPPPRODUCT_ID = productId;
                product.INAPPPRODUCT_TYPE = productType;
                product.INAPPPRODUCT_NAME = name;
                product.INAPPPRODUCT_TITLE = title;
                product.INAPPPRODUCT_DESCRIPTION = description;

                ProductDetails.OneTimePurchaseOfferDetails oneTimePurchaseOfferDetails = details.getOneTimePurchaseOfferDetails();

                if (oneTimePurchaseOfferDetails != null) {
                    long priceAmountMicros = oneTimePurchaseOfferDetails.getPriceAmountMicros();
                    String formattedPrice = oneTimePurchaseOfferDetails.getFormattedPrice();
                    String priceCurrencyCode = oneTimePurchaseOfferDetails.getPriceCurrencyCode();

                    product.INAPPPRODUCT_PRICE_AMOUNT_MICROS = priceAmountMicros;
                    product.INAPPPRODUCT_PRICE_FORMATTED = formattedPrice;
                    product.INAPPPRODUCT_PRICE_CURRENCY_CODE = priceCurrencyCode;
                } else {
                    product.INAPPPRODUCT_PRICE_AMOUNT_MICROS = 0L;
                    product.INAPPPRODUCT_PRICE_FORMATTED = "";
                    product.INAPPPRODUCT_PRICE_CURRENCY_CODE = "";
                }

                products1.add(product);
            }

            MengineFragmentInAppPurchase.INSTANCE.setInAppProducts(products1);

            List<Map<String, Object>> desc_products = new ArrayList<>();

            for (MengineParamInAppProduct product : products1) {
                Map<String, Object> desc_product = new HashMap<>();

                Map<String, Object> desc_offer = new HashMap<>();

                long priceAmountMicros = product.INAPPPRODUCT_PRICE_AMOUNT_MICROS;
                String formattedPrice = product.INAPPPRODUCT_PRICE_FORMATTED;
                String priceCurrencyCode = product.INAPPPRODUCT_PRICE_CURRENCY_CODE;

                desc_offer.put("priceAmountMicros", priceAmountMicros);
                desc_offer.put("formattedPrice", formattedPrice);
                desc_offer.put("priceCurrencyCode", priceCurrencyCode);

                desc_product.put("oneTimePurchaseOfferDetails", desc_offer);

                String description = product.INAPPPRODUCT_DESCRIPTION;
                String name = product.INAPPPRODUCT_NAME;
                String productId = product.INAPPPRODUCT_ID;
                String productType = product.INAPPPRODUCT_TYPE;
                String title = product.INAPPPRODUCT_TITLE;

                desc_product.put("description", description);
                desc_product.put("name", name);
                desc_product.put("productId", productId);
                desc_product.put("productType", productType);
                desc_product.put("title", title);

                desc_products.add(desc_product);
            }

            this.logInfo("billing response skuResponse: %s"
                , desc_products
            );

            m_productsQueried = true;
            this.nativeCall("onGooglePlayBillingQueryProductSuccess", desc_products);
            this.queryOwnedPurchases();
        };

        billingClient.queryProductDetailsAsync(params, (billingResult, productDetailsResult) -> {
            MengineUtils.performOnMainThread(() -> {
                listener.onProductDetailsResponse(billingResult, productDetailsResult);
            });
        });
    }

    private boolean isBillingResponseRetryable(int responseCode) {
        switch (responseCode) {
            case BillingClient.BillingResponseCode.SERVICE_DISCONNECTED:
            case BillingClient.BillingResponseCode.SERVICE_UNAVAILABLE:
            case BillingClient.BillingResponseCode.ERROR:
            case BillingClient.BillingResponseCode.NETWORK_ERROR:
                return true;
            default:
                return false;
        }
    }

    public void restorePurchases() {
        this.restorePurchases(null);
    }

    public void restorePurchases(@Nullable String accountToken) {
        this.logInfo("restorePurchases");

        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            this.logError("[ERROR] restorePurchases billing client not created");

            this.nativeCall("onGooglePlayBillingRestorePurchasesError", ERROR_CODE_NOT_INITIALIZED, new RuntimeException("Billing client not initialized"));

            return;
        }

        MengineUtils.performOnMainThread(() -> {
            this.startRestorePurchases(billingClient, accountToken);
        });
    }

    @MainThread
    private void startRestorePurchases(@NonNull BillingClient billingClient, @Nullable String accountToken) {
        if (m_billingClient != billingClient) {
            return;
        }

        MengineGooglePlayBillingRestorePurchases activeOperation = m_restorePurchases;

        if (activeOperation != null) {
            if (activeOperation.isRequest(billingClient, accountToken) == true) {
                this.logWarning("restorePurchases already in progress, the request waits for its result");

                return;
            }

            this.logWarning("restorePurchases cancels the request in progress for another account");

            this.completeRestorePurchases(activeOperation);
        }

        MengineGooglePlayBillingRestorePurchases operation = new MengineGooglePlayBillingRestorePurchases(billingClient, accountToken);
        m_restorePurchases = operation;

        this.buildEvent("mng_billing_restore_purchases")
            .log();

        this.queryRestorePurchases(operation, 0, purchases -> {
            List<Purchase> restorablePurchases = purchases;

            if (accountToken != null) {
                restorablePurchases = new ArrayList<>();

                for (Purchase purchase : purchases) {
                    if (MengineGooglePlayBillingPlugin.isPurchaseRestorable(purchase, accountToken) == false) {
                        continue;
                    }

                    restorablePurchases.add(purchase);
                }
            }

            Consumer<Boolean> completion = MengineUtils.createCountDownConsumer(restorablePurchases, successful -> {
                MengineUtils.performOnMainThread(() -> {
                    if (successful == false) {
                        this.failRestorePurchases(operation);

                        return;
                    }

                    this.queryRestorePurchases(operation, 0, ownedPurchases -> {
                        this.finishRestorePurchases(operation, ownedPurchases);
                    });
                });
            });

            for (Purchase purchase : restorablePurchases) {
                this.handlePurchase(purchase, completion);
            }
        });
    }

    @MainThread
    private void queryRestorePurchases(@NonNull MengineGooglePlayBillingRestorePurchases operation, int attempt, @NonNull Consumer<List<Purchase>> onSuccess) {
        if (operation.isCompleted() == true) {
            return;
        }

        QueryPurchasesParams purchasesParams = QueryPurchasesParams.newBuilder()
            .setProductType(BillingClient.ProductType.INAPP)
            .build();

        BillingClient billingClient = operation.getClient();

        billingClient.queryPurchasesAsync(purchasesParams, (billingResult, purchases) -> {
            MengineUtils.performOnMainThread(() -> {
                if (operation.isCompleted() == true) {
                    return;
                }

                int responseCode = billingResult.getResponseCode();

                if (responseCode == BillingClient.BillingResponseCode.OK) {
                    onSuccess.accept(purchases);

                    return;
                }

                if (this.scheduleRestorePurchasesRetry(operation, attempt, responseCode, onSuccess) == true) {
                    return;
                }

                this.logError("[ERROR] billing invalid restore purchases responseCode: %d message: %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                this.buildEvent("mng_billing_restore_purchases_failed")
                    .addParameterLong("error_code", responseCode)
                    .log();

                this.failRestorePurchases(operation);
            });
        });
    }

    @MainThread
    private boolean scheduleRestorePurchasesRetry(@NonNull MengineGooglePlayBillingRestorePurchases operation, int attempt, int responseCode, @NonNull Consumer<List<Purchase>> onSuccess) {
        if (attempt >= RESTORE_QUERY_MAX_RETRIES) {
            return false;
        }

        if (this.isBillingResponseRetryable(responseCode) == false) {
            return false;
        }

        int nextAttempt = attempt + 1;
        long delay = RESTORE_QUERY_RETRY_DELAY_MS * nextAttempt;

        this.logWarning("billing restore purchases retry: %d responseCode: %d"
            , nextAttempt
            , responseCode
        );

        Handler handler = MengineUtils.performOnMainThreadDelayed(() -> {
            this.queryRestorePurchases(operation, nextAttempt, onSuccess);
        }, delay);

        if (handler == null) {
            return false;
        }

        return true;
    }

    @MainThread
    private void completeRestorePurchases(@NonNull MengineGooglePlayBillingRestorePurchases operation) {
        operation.complete(() -> {
            if (m_restorePurchases != operation) {
                return;
            }

            m_restorePurchases = null;
        });
    }

    @MainThread
    private void finishRestorePurchases(@NonNull MengineGooglePlayBillingRestorePurchases operation, @NonNull List<Purchase> purchases) {
        if (operation.isCompleted() == true) {
            return;
        }

        Set<String> owned = new HashSet<>();

        for (Purchase purchase : purchases) {
            if (purchase.isAcknowledged() == false) {
                continue;
            }

            if (purchase.getPurchaseState() != Purchase.PurchaseState.PURCHASED) {
                continue;
            }

            owned.addAll(purchase.getProducts());
        }

        this.completeRestorePurchases(operation);

        MengineFragmentInAppPurchase.INSTANCE.ownedInAppProducts(owned);

        this.buildEvent("mng_billing_restore_purchases_success")
            .log();

        this.nativeCall("onGooglePlayBillingRestorePurchasesSuccess", owned);
        MengineUI.showToastRes(this.getMengineActivity(), R.string.mengine_googleplaybilling_restore_purchases_success);
    }

    @MainThread
    private void failRestorePurchases(@NonNull MengineGooglePlayBillingRestorePurchases operation) {
        if (operation.isCompleted() == true) {
            return;
        }

        this.completeRestorePurchases(operation);

        this.nativeCall("onGooglePlayBillingRestorePurchasesFailed");
        MengineUI.showToastRes(this.getMengineActivity(), R.string.mengine_googleplaybilling_restore_purchases_failed);
    }

    private void completeBuyInApp(@NonNull MengineGooglePlayBillingBuyInApp operation) {
        operation.complete(() -> {
            if (m_buyInApp != operation) {
                return;
            }

            m_buyInApp = null;
        });
    }

    @MainThread
    @Nullable
    private String completeBuyInApp(int responseCode, @Nullable List<Purchase> purchases) {
        MengineGooglePlayBillingBuyInApp operation = m_buyInApp;

        if (operation == null) {
            return null;
        }

        String productId = operation.getProductId();

        if (responseCode != BillingClient.BillingResponseCode.OK) {
            this.completeBuyInApp(operation);

            return productId;
        }

        if (purchases == null) {
            this.completeBuyInApp(operation);

            return productId;
        }

        if (purchases.isEmpty() == true) {
            this.completeBuyInApp(operation);

            return productId;
        }

        for (Purchase purchase : purchases) {
            List<String> products = purchase.getProducts();

            if (products.contains(productId) == false) {
                continue;
            }

            this.completeBuyInApp(operation);

            return productId;
        }

        return null;
    }

    public void buyInApp(@NonNull String productId, @NonNull String accountToken) {
        this.logInfo("buyInApp productId: %s"
            , productId
        );

        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            this.logError("[ERROR] buyInApp billing client not created");

            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_INITIALIZED, new RuntimeException("Billing client not initialized"));

            return;
        }

        this.runBillingCallback(billingClient, () -> {
            this.startBuyInApp(billingClient, productId, accountToken);
        });
    }

    @MainThread
    private void startBuyInApp(@NonNull BillingClient billingClient, @NonNull String productId, @NonNull String accountToken) {
        if (billingClient.isReady() == false) {
            this.logError("[ERROR] buyInApp billing client not ready");
            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_READY, new RuntimeException("Billing client not ready"));

            return;
        }

        if (m_buyInApp != null) {
            this.logError("[ERROR] buyInApp another purchase is in progress productId: %s"
                , m_buyInApp.getProductId()
            );

            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_READY, new RuntimeException("Another purchase is in progress"));

            return;
        }

        MengineGooglePlayBillingBuyInApp operation = new MengineGooglePlayBillingBuyInApp(productId);
        m_buyInApp = operation;

        QueryProductDetailsParams.Product queryProduct = QueryProductDetailsParams.Product.newBuilder()
            .setProductId(productId)
            .setProductType(BillingClient.ProductType.INAPP)
            .build();

        QueryProductDetailsParams queryProductDetailsParams = QueryProductDetailsParams.newBuilder()
            .setProductList(Collections.singletonList(queryProduct))
            .build();

        billingClient.queryProductDetailsAsync(queryProductDetailsParams, (billingResult, productDetailsResult) -> {
            this.runBillingCallback(billingClient, () -> {
                if (operation.isCompleted() == true) {
                    return;
                }

                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    String debugMessage = billingResult.getDebugMessage();

                    this.logError("[ERROR] buyInApp query product failed code: %d message: %s"
                        , responseCode
                        , debugMessage
                    );

                    this.completeBuyInApp(operation);

                    this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_UNKNOWN, new RuntimeException(debugMessage));

                    return;
                }

                List<ProductDetails> productDetailsList = productDetailsResult.getProductDetailsList();

                if (productDetailsList.isEmpty() == true) {
                    this.logError("[ERROR] buyInApp invalid product id: %s"
                        , productId
                    );

                    this.completeBuyInApp(operation);

                    this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_FOUND, new RuntimeException("Product not found"));

                    return;
                }

                ProductDetails productDetails = productDetailsList.get(0);

                if (accountToken.isEmpty() == true) {
                    this.launchBillingFlow(productId, productDetails);

                    return;
                }

                this.launchBillingFlow(productId, productDetails, accountToken);
            });
        });
    }

    @MainThread
    protected void launchBillingFlow(@NonNull String productId, @NonNull ProductDetails product) {
        this.launchBillingFlow(productId, product, null);
    }

    @MainThread
    private void launchBillingFlow(@NonNull String productId, @NonNull ProductDetails product, @Nullable String accountToken) {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            return;
        }

        MengineGooglePlayBillingBuyInApp operation = m_buyInApp;

        MengineActivity activity = this.getMengineActivity();

        if (MengineUtils.isActivityAvailable(activity) == false) {
            this.logError("[ERROR] buyInApp invalid activity");

            this.completeBuyInAppLaunch(operation, productId);

            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_READY, new RuntimeException("Activity not ready"));

            return;
        }

        this.buildEvent("mng_billing_buy_launch_flow")
            .addParameterString("product_id", productId)
            .log();

        List<BillingFlowParams.ProductDetailsParams> productDetailsParamsList = new ArrayList<>();

        BillingFlowParams.ProductDetailsParams productParams = BillingFlowParams.ProductDetailsParams.newBuilder()
            .setProductDetails(product)
            .build();

        productDetailsParamsList.add(productParams);

        BillingFlowParams.Builder flowParamsBuilder = BillingFlowParams.newBuilder()
            .setProductDetailsParamsList(productDetailsParamsList);

        if (accountToken != null) {
            flowParamsBuilder.setObfuscatedAccountId(accountToken);
        }

        BillingFlowParams flowParams = flowParamsBuilder.build();

        BillingResult billingResult = billingClient.launchBillingFlow(activity, flowParams);

        int responseCode = billingResult.getResponseCode();

        if (responseCode != BillingClient.BillingResponseCode.OK) {
            int subResponseCode = billingResult.getOnPurchasesUpdatedSubResponseCode();

            this.logError("[ERROR] billing invalid launch billing flow code: %d sub: %d message: %s"
                , responseCode
                , subResponseCode
                , billingResult.getDebugMessage()
            );

            this.buildEvent("mng_billing_buy_launch_flow_failed")
                .addParameterString("product_id", productId)
                .addParameterLong("error_code", responseCode)
                .addParameterLong("sub_code", subResponseCode)
                .log();

            this.completeBuyInAppLaunch(operation, productId);

            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowFailed", productId, responseCode, subResponseCode);

            return;
        }

        this.logInfo("buy InApp success productId: %s"
            , productId
        );

        this.buildEvent("mng_billing_buy_launch_flow_success")
            .addParameterString("product_id", productId)
            .log();

        this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowSuccess", productId);
    }

    @MainThread
    private void completeBuyInAppLaunch(@Nullable MengineGooglePlayBillingBuyInApp operation, @NonNull String productId) {
        if (operation == null) {
            return;
        }

        if (operation.isCompleted() == true) {
            return;
        }

        if (productId.equals(operation.getProductId()) == false) {
            return;
        }

        this.completeBuyInApp(operation);
    }

    protected void purchaseInAppProduct(@NonNull Purchase purchase, boolean isConsumable) {
        String orderId = purchase.getOrderId();
        List<String> products = purchase.getProducts();
        int quantity = purchase.getQuantity();
        boolean isAcknowledged = purchase.isAcknowledged();
        String token = purchase.getPurchaseToken();
        String originalJson = purchase.getOriginalJson();

        MengineParamInAppPurchase purchase1 = new MengineParamInAppPurchase();
        purchase1.INAPPPURCHASE_TRANSACTION = orderId;
        purchase1.INAPPPURCHASE_PRODUCTS = products;
        purchase1.INAPPPURCHASE_QUANTITY = quantity;
        purchase1.INAPPPURCHASE_ACKNOWLEDGED = isAcknowledged;
        purchase1.INAPPPURCHASE_CONSUMABLE = isConsumable;
        purchase1.INAPPPURCHASE_TOKEN = token;
        purchase1.INAPPPURCHASE_DATA = originalJson;

        MengineFragmentInAppPurchase.INSTANCE.purchaseInAppProduct(purchase1);
    }

    private boolean isPurchaseDelivered(@NonNull String token) {
        Set<String> deliveredTokens = MenginePreferences.getPreferenceStrings(PREFERENCE_PURCHASE_DELIVERED_TOKENS, null);

        if (deliveredTokens == null) {
            return false;
        }

        boolean delivered = deliveredTokens.contains(token);

        return delivered;
    }

    private void markPurchaseDelivered(@NonNull String token) {
        MenginePreferences.addPreferenceStrings(PREFERENCE_PURCHASE_DELIVERED_TOKENS, Collections.singleton(token));
    }

    private void clearPurchaseDelivered(@NonNull String token) {
        MenginePreferences.removePreferenceStrings(PREFERENCE_PURCHASE_DELIVERED_TOKENS, Collections.singleton(token));
    }

    @MainThread
    private void deliverPurchase(@NonNull MengineGooglePlayBillingPurchaseProcessing processing, @NonNull Purchase purchase, boolean isConsumable) {
        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        if (this.isPurchaseDelivered(token) == true) {
            if (isConsumable == true) {
                this.handleConsumablePurchase(processing, purchase);
            } else {
                this.handleNonConsumablePurchase(processing, purchase);
            }

            return;
        }

        MengineCallback cb = MengineCallback.onMainThread((boolean successful, Map<String, Object> result) -> {
            if (processing.isCompleted() == true) {
                return;
            }

            if (successful == false) {
                this.logError("[ERROR] purchase delivery failed products: %s", products);
                this.nativeCall("onGooglePlayBillingPurchaseDeliveryFailed", products);
                processing.complete(false);

                return;
            }

            this.markPurchaseDelivered(token);

            if (isConsumable == true) {
                this.handleConsumablePurchase(processing, purchase);
            } else {
                this.handleNonConsumablePurchase(processing, purchase);
            }
        });

        String accountToken = MengineGooglePlayBillingPlugin.getPurchaseAccountToken(purchase);

        this.nativeCall("onGooglePlayBillingPurchaseDeliver", products, token, accountToken, cb);
    }

    @Nullable
    private static String getPurchaseAccountToken(@NonNull Purchase purchase) {
        AccountIdentifiers accountIdentifiers = purchase.getAccountIdentifiers();

        if (accountIdentifiers == null) {
            return null;
        }

        String obfuscatedAccountId = accountIdentifiers.getObfuscatedAccountId();

        if ("".equals(obfuscatedAccountId) == true) {
            return null;
        }

        return obfuscatedAccountId;
    }

    private static boolean isPurchaseRestorable(@NonNull Purchase purchase, @NonNull String accountToken) {
        String owner = MengineGooglePlayBillingPlugin.getPurchaseAccountToken(purchase);

        if (owner == null) {
            return true;
        }

        if (accountToken.equalsIgnoreCase(owner) == false) {
            return false;
        }

        return true;
    }

    @MainThread
    private void handleNonConsumablePurchase(@NonNull MengineGooglePlayBillingPurchaseProcessing processing, @NonNull Purchase purchase) {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            processing.complete(false);

            return;
        }

        this.logInfo("handleNonConsumablePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        AcknowledgePurchaseParams.Builder acknowledgePurchaseParams = AcknowledgePurchaseParams.newBuilder()
            .setPurchaseToken(token);

        billingClient.acknowledgePurchase(acknowledgePurchaseParams.build(), billingResult -> {
            this.runBillingCallback(billingClient, () -> {
                if (processing.isCompleted() == true) {
                    return;
                }

                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    this.logError("[ERROR] billing invalid acknowledge purchase code: %d message: %s"
                        , responseCode
                        , billingResult.getDebugMessage()
                    );

                    this.nativeCall("onGooglePlayBillingPurchaseAcknowledgeFailed", products);
                    processing.complete(false);

                    return;
                }

                this.logInfo("billing success acknowledge purchase: %s products: %s"
                    , billingResult.getDebugMessage()
                    , products
                );

                MenginePreferences.addPreferenceStrings("mengine.billing.purchase.owned.products", products);

                this.purchaseInAppProduct(purchase, false);
                this.clearPurchaseDelivered(token);

                this.nativeCall("onGooglePlayBillingPurchaseAcknowledgeSuccess", products, token);
                processing.complete(true);
            });
        });
    }

    @MainThread
    private void handleConsumablePurchase(@NonNull MengineGooglePlayBillingPurchaseProcessing processing, @NonNull Purchase purchase) {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            processing.complete(false);

            return;
        }

        this.logInfo("handleConsumablePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        ConsumeParams consumeParams = ConsumeParams.newBuilder()
            .setPurchaseToken(token)
            .build();

        billingClient.consumeAsync(consumeParams, (billingResult, purchaseToken) -> {
            this.runBillingCallback(billingClient, () -> {
                if (processing.isCompleted() == true) {
                    return;
                }

                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    this.logError("[ERROR] billing invalid consume code: %d message: %s"
                        , responseCode
                        , billingResult.getDebugMessage()
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesOnConsumeFailed", products);
                    processing.complete(false);

                    return;
                }

                this.logInfo("billing success consume: %s products: %s"
                    , billingResult.getDebugMessage()
                    , products
                );

                this.purchaseInAppProduct(purchase, true);
                this.clearPurchaseDelivered(token);

                this.nativeCall("onGooglePlayBillingPurchasesOnConsumeSuccess", products, token);
                processing.complete(true);
            });
        });
    }

    @MainThread
    private void handlePurchases(List<? extends Purchase> purchases) {
        Consumer<Boolean> completion = successful -> {
            //Empty
        };

        for (Purchase purchase : purchases) {
            this.handlePurchase(purchase, completion);
        }
    }

    @MainThread
    private void handlePurchase(@NonNull Purchase purchase, @NonNull Consumer<Boolean> completion) {
        this.logInfo("handlePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        MengineGooglePlayBillingPurchaseProcessing activeProcessing = m_purchaseProcessings.get(token);

        if (activeProcessing != null) {
            activeProcessing.addCompletion(completion);

            return;
        }

        MengineGooglePlayBillingPurchaseProcessing processing = new MengineGooglePlayBillingPurchaseProcessing(token, () -> {
            m_purchaseProcessings.remove(token);
        });
        processing.addCompletion(completion);

        m_purchaseProcessings.put(token, processing);

        int state = purchase.getPurchaseState();

        switch (state) {
            case Purchase.PurchaseState.UNSPECIFIED_STATE: {
                this.logError("[ERROR] handlePurchase invalid purchase state: UNSPECIFIED_STATE purchase: %s"
                    , purchase
                );

                this.nativeCall("onGooglePlayBillingPurchaseUnspecifiedState", products);
                processing.complete(false);
            } break;
            case Purchase.PurchaseState.PURCHASED: {
                MengineCallback cb = MengineCallback.onMainThread((boolean successful, Map<String, Object> result) -> {
                    if (processing.isCompleted() == true) {
                        return;
                    }

                    if (result != null && Boolean.TRUE.equals(result.get("skipPurchase")) == true) {
                        this.logWarning("handlePurchase products missing from the catalog: %s", products);
                        processing.complete(true);

                        return;
                    }

                    if (successful == false) {
                        this.logError("[ERROR] handlePurchase invalid isConsumable");
                        processing.complete(false);

                        return;
                    }

                    Boolean isConsumable = (Boolean)result.get("isConsumable");

                    if (isConsumable == null) {
                        this.logError("[ERROR] handlePurchase isConsumable is null");
                        processing.complete(false);

                        return;
                    }

                    boolean acknowledged = purchase.isAcknowledged();

                    this.logInfo("handlePurchase Acknowledged: %s product: %s consumable: %s"
                        , acknowledged
                        , products
                        , isConsumable
                    );

                    if (acknowledged == true) {
                        MengineCallback acknowledgedCb = MengineCallback.onMainThread((boolean acknowledgedSuccessful, Map<String, Object> result2) -> {
                            if (processing.isCompleted() == true) {
                                return;
                            }

                            if (acknowledgedSuccessful == false) {
                                processing.complete(false);

                                return;
                            }

                            this.clearPurchaseDelivered(token);
                            processing.complete(true);
                        });

                        this.nativeCall("onGooglePlayBillingPurchaseAcknowledged", products, token, acknowledgedCb);

                        return;
                    }

                    if (isConsumable == true) {
                        this.deliverPurchase(processing, purchase, true);
                    } else {
                        this.deliverPurchase(processing, purchase, false);
                    }
                });

                this.nativeCall("onGooglePlayBillingPurchaseIsConsumable", products, cb);
            } break;
            case Purchase.PurchaseState.PENDING: {
                this.logInfo("handlePurchase [Pending] purchase: %s"
                    , purchase
                );

                this.nativeCall("onGooglePlayBillingPurchasePending", products);
                processing.complete(true);
            } break;
        }
    }
}
