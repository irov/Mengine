package org.Mengine.Plugin.GooglePlayBilling;

import android.content.Context;
import android.os.Bundle;

import androidx.annotation.AnyThread;
import androidx.annotation.NonNull;

import com.android.billingclient.api.AcknowledgePurchaseParams;
import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.PendingPurchasesParams;
import com.android.billingclient.api.ProductDetails;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.QueryProductDetailsParams;
import com.android.billingclient.api.QueryPurchasesParams;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineCallback;
import org.Mengine.Base.MengineFragment;
import org.Mengine.Base.MengineFragmentInAppPurchase;
import org.Mengine.Base.MengineInAppProductParam;
import org.Mengine.Base.MengineInAppPurchaseParam;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

public class MengineGooglePlayBillingPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity {
    public static final String SERVICE_NAME = "GPlayBilling";
    public static final boolean SERVICE_EMBEDDING = true;

    private List<ProductDetails> m_productsDetails;
    private BillingClient m_billingClient;

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        m_productsDetails = new ArrayList<>();

        PurchasesUpdatedListener purchasesUpdatedListener = (billingResult, purchases) -> {
            int responseCode = billingResult.getResponseCode();
            String debugMessage = billingResult.getDebugMessage();

            switch (responseCode) {
                case BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED: {
                    this.logWarning("onPurchasesUpdated [Feature not supported] message: %s"
                        , debugMessage
                    );

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedFeatureNotSupported");
                }break;
                case BillingClient.BillingResponseCode.SERVICE_DISCONNECTED: {
                    this.logWarning("onPurchasesUpdated [Service disconnected] message: %s"
                        , debugMessage
                    );

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedServiceDisconnected");
                }break;
                case BillingClient.BillingResponseCode.OK: {
                    this.logMessage("onPurchasesUpdated [User success the purchase]");

                    if (purchases != null) {
                        for (Purchase purchase : purchases) {
                            this.handlePurchase(purchase);
                        }
                    }

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedOk");
                }break;
                case BillingClient.BillingResponseCode.USER_CANCELED: {
                    this.logWarning("onPurchasesUpdated [User canceled the purchase] message: %s"
                        , debugMessage
                    );

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedUserCanceled");
                }break;
                case BillingClient.BillingResponseCode.SERVICE_UNAVAILABLE: {
                    this.logWarning("onPurchasesUpdated [Service Unavailable] message: %s"
                        , debugMessage
                    );

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedServiceUnavailable");
                }break;
                case BillingClient.BillingResponseCode.BILLING_UNAVAILABLE: {
                    this.logWarning("onPurchasesUpdated [Billing Unavailable] message: %s"
                        , debugMessage
                    );

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedBillingUnavailable");
                }break;
                case BillingClient.BillingResponseCode.ITEM_UNAVAILABLE: {
                    this.logWarning("onPurchasesUpdated [The user item unavailable] message: %s"
                        , debugMessage
                    );

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedItemUnavailable");
                }break;
                case BillingClient.BillingResponseCode.DEVELOPER_ERROR: {
                    this.logError(
                        "onPurchasesUpdated: Developer error means that Google Play " +
                            "does not recognize the configuration. If you are just getting started, " +
                            "make sure you have configured the application correctly in the " +
                            "Google Play Console. The SKU product ID must match and the APK you " +
                            "are using must be signed with release keys."
                    );

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedDeveloperError");
                }break;
                case BillingClient.BillingResponseCode.ERROR: {
                    this.logWarning("onPurchasesUpdated [Error] message: %s"
                        , debugMessage
                    );

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedError");
                }break;
                case BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED: {
                    this.logWarning("onPurchasesUpdated [The user already owns this item] message: %s"
                        , debugMessage
                    );

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedItemAlreadyOwned");
                }break;
                case BillingClient.BillingResponseCode.ITEM_NOT_OWNED: {
                    this.logWarning("onPurchasesUpdated [The user item not owned] message: %s"
                        , debugMessage
                    );

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedItemNotOwned");
                }break;
                default: {
                    this.logWarning("onPurchasesUpdated [Unsupported response] code: %d message: %s"
                        , responseCode
                        , debugMessage
                    );

                    this.pythonCall("onGooglePlayBillingPurchasesUpdatedUnknown", responseCode);
                }break;
            }
        };

        PendingPurchasesParams pendingPurchasesParams = PendingPurchasesParams.newBuilder()
                .enableOneTimeProducts()
                .build();

        m_billingClient = BillingClient.newBuilder(application)
            .setListener(purchasesUpdatedListener)
            .enablePendingPurchases(pendingPurchasesParams)
            .build();
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        m_productsDetails = null;

        if (m_billingClient != null) {
            m_billingClient.endConnection();
            m_billingClient = null;
        }
    }

    public void billingConnect() {
        if (m_billingClient == null) {
            this.logError("[ERROR] billingConnect billing client not created");

            return;
        }

        this.logMessage("billingConnect");

        m_billingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingServiceDisconnected() {
                MengineGooglePlayBillingPlugin.this.logMessage("Billing disconnected");

                MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingConnectServiceDisconnected");
            }

            @Override
            public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    MengineGooglePlayBillingPlugin.this.logWarning("billing invalid connection responseCode: %d message: %s"
                        , responseCode
                        , billingResult.getDebugMessage()
                    );

                    MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingConnectSetupFinishedFailed");

                    return;
                }

                MengineGooglePlayBillingPlugin.this.logMessage("billing setup finished");

                MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingConnectSetupFinishedSuccess");
            }
        });
    }

    public void queryProducts(List<String> products) {
        if (m_billingClient == null) {
            this.logError("[ERROR] queryProducts billing client not created");

            return;
        }

        this.logMessage("queryProducts products: %s"
            , products
        );

        BillingResult billingResult = m_billingClient.isFeatureSupported(BillingClient.FeatureType.PRODUCT_DETAILS);

        if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED) {
            this.makeToastLong(10000L, "GooglePlay billing asks you to update the PlayStore app");

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

        QueryProductDetailsParams.Builder params = QueryProductDetailsParams.newBuilder();
        params.setProductList(productList);

        m_billingClient.queryProductDetailsAsync(params.build(), (billingResult1, productsDetails) -> {
            int responseCode = billingResult1.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logError("[ERROR] billing invalid query product details code: %d message: %s"
                    , responseCode
                    , billingResult1.getDebugMessage()
                );

                this.pythonCall("onGooglePlayBillingQueryProductFailed");

                return;
            }

            this.logMessage("Billing query products details: %s"
                , productsDetails
            );

            m_productsDetails.clear();
            m_productsDetails.addAll(productsDetails);

            ArrayList<MengineInAppProductParam> products1 = new ArrayList<>();

            for(ProductDetails details : productsDetails) {
                MengineInAppProductParam product = new MengineInAppProductParam();

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

            for (MengineInAppProductParam product : products1) {
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

            this.logMessage("Billing response skuResponse: %s"
                , desc_products
            );

            this.pythonCall("onGooglePlayBillingQueryProductSuccess", desc_products);
        });
    }

    public void queryPurchases() {
        if (m_billingClient == null) {
            this.logError("[ERROR] queryPurchases billing client not created");

            return;
        }

        this.logMessage("queryPurchases");

        this.buildEvent("mng_billing_query_purchases")
            .log();

        QueryPurchasesParams purchasesParams = QueryPurchasesParams.newBuilder()
            .setProductType(BillingClient.ProductType.INAPP)
            .build();

        m_billingClient.queryPurchasesAsync(purchasesParams, (billingResult, purchases) -> {
            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logError("[ERROR] billing invalid query purchases responseCode: %d message: %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                this.buildEvent("mng_billing_purchases_failed")
                    .addParameterLong("error_code", responseCode)
                    .log();

                this.pythonCall("onGooglePlayBillingQueryPurchasesFailed");

                return;
            }

            this.logMessage("billing success query message: %s purchases: %s"
                , billingResult.getDebugMessage()
                , purchases
            );

            this.buildEvent("mng_billing_purchases_success")
                .log();

            for (Purchase purchase : purchases) {
                this.handlePurchase(purchase);
            }

            this.pythonCall("onGooglePlayBillingQueryPurchasesSuccess");
        });
    }

    protected ProductDetails getProductDetails(String productId) {
        for (ProductDetails product : m_productsDetails) {
            String currentProductId = product.getProductId();

            if (Objects.equals(currentProductId, productId) == false) {
                continue;
            }

            return product;
        }

        return null;
    }

    public boolean buyInApp(String productId) {
        if (m_billingClient == null) {
            this.logError("[ERROR] buyInApp billing client not created");

            return false;
        }

        ProductDetails product = this.getProductDetails(productId);

        if (product == null) {
            this.logError("[ERROR] buyInApp invalid product id: %s"
                , productId
            );

            return false;
        }

        this.logMessage("buyInApp productId: %s"
            , productId
        );

        this.buildEvent("mng_billing_buy_launch_flow")
            .addParameterString("product_id", productId)
            .log();

        List<BillingFlowParams.ProductDetailsParams> productDetailsParamsList = new ArrayList<>();

        BillingFlowParams.ProductDetailsParams productParams = BillingFlowParams.ProductDetailsParams.newBuilder()
            .setProductDetails(product)
            .build();

        productDetailsParamsList.add(productParams);

        BillingFlowParams flowParams = BillingFlowParams.newBuilder()
            .setProductDetailsParamsList(productDetailsParamsList)
            .build();

        MengineActivity activity = this.getMengineActivity();

        BillingResult billingResult = m_billingClient.launchBillingFlow(activity, flowParams);

        int responseCode = billingResult.getResponseCode();

        if (responseCode != BillingClient.BillingResponseCode.OK) {
            this.logError("[ERROR] billing invalid launch billing flow code: %d message: %s"
                , responseCode
                , billingResult.getDebugMessage()
            );

            this.buildEvent("mng_billing_buy_failed")
                .addParameterString("product_id", productId)
                .addParameterLong("error_code", responseCode)
                .log();

            this.pythonCall("onGooglePlayBillingBuyInAppFailed", productId);

            return true;
        }

        this.logMessage("buy InApp success productId: %s"
            , productId
        );

        this.buildEvent("mng_billing_buy_success")
            .addParameterString("product_id", productId)
            .log();

        this.pythonCall("onGooglePlayBillingBuyInAppSuccess", productId);

        return true;
    }

    @AnyThread
    private void handleNonConsumablePurchase(@NonNull Purchase purchase) {
        this.logMessage("handleNonConsumablePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        AcknowledgePurchaseParams.Builder acknowledgePurchaseParams = AcknowledgePurchaseParams.newBuilder()
            .setPurchaseToken(token);

        m_billingClient.acknowledgePurchase(acknowledgePurchaseParams.build(), billingResult -> {
            this.logMessage("acknowledgePurchase responseCode: %d debugMessage: %s"
                , billingResult.getResponseCode()
                , billingResult.getDebugMessage()
            );

            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logError("[ERROR] billing invalid acknowledge purchase code: %d message: %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                this.pythonCall("onGooglePlayBillingPurchasesAcknowledgeFailed", products);

                return;
            }

            this.logMessage("billing success acknowledge purchase: %s"
                , billingResult.getDebugMessage()
            );

            this.pythonCall("onGooglePlayBillingPurchasesAcknowledgeSuccess", products);
        });
    }

    @AnyThread
    private void handleConsumablePurchase(@NonNull Purchase purchase) {
        this.logMessage("handleConsumablePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        ConsumeParams consumeParams = ConsumeParams.newBuilder()
            .setPurchaseToken(token)
            .build();

        m_billingClient.consumeAsync(consumeParams, (billingResult, purchaseToken) -> {
            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logError("[ERROR] billing invalid consume code: %d message: %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                this.pythonCall("onGooglePlayBillingPurchasesOnConsumeFailed", products);

                return;
            }

            this.logMessage("billing success consume: %s"
                , billingResult.getDebugMessage()
            );

            String orderId = purchase.getOrderId();
            int quantity = purchase.getQuantity();
            boolean acknowledged = purchase.isAcknowledged();
            String originalJson = purchase.getOriginalJson();

            MengineInAppPurchaseParam purchase1 = new MengineInAppPurchaseParam();
            purchase1.INAPPPURCHASE_TRANSACTION = orderId;
            purchase1.INAPPPURCHASE_PRODUCTS = products;
            purchase1.INAPPPURCHASE_QUANTITY = quantity;
            purchase1.INAPPPURCHASE_ACKNOWLEDGED = acknowledged;
            purchase1.INAPPPURCHASE_TOKEN = purchaseToken;
            purchase1.INAPPPURCHASE_DATA = originalJson;


            MengineFragmentInAppPurchase.INSTANCE.purchaseInAppProduct(purchase1);

            this.pythonCall("onGooglePlayBillingPurchasesOnConsumeSuccess", products);
        });
    }

    @AnyThread
    private void handlePurchase(@NonNull Purchase purchase) {
        this.logMessage("handlePurchase purchase: %s"
            , purchase
        );

        int state = purchase.getPurchaseState();

        if (state != Purchase.PurchaseState.PURCHASED) {
            return;
        }

        List<String> products = purchase.getProducts();

        MengineCallback cb = (boolean successful, Map<String, Object> result) -> {
            if (successful == false) {
                this.logError("[ERROR] handlePurchase invalid isConsumable");

                return;
            }

            boolean isConsumable = (boolean)result.get("isConsumable");

            boolean acknowledged = purchase.isAcknowledged();

            this.logMessage("handlePurchase Acknowledged: %s product: %s consumable: %s"
                , acknowledged
                , products
                , isConsumable
            );

            if (acknowledged == true) {
                this.pythonCall("onGooglePlayBillingPurchaseAcknowledged", products);

                return;
            }

            if (isConsumable == true) {
                this.handleConsumablePurchase(purchase);
            } else {
                this.handleNonConsumablePurchase(purchase);
            }
        };

        this.pythonCall("onGooglePlayBillingPurchaseIsConsumable", products, cb);
    }
}
