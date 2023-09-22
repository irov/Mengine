package org.Mengine.Base;

public class MengineInAppProductParam {
    public final static MengineInAppProductParam INAPPPRODUCT_ID = new MengineInAppProductParam("INAPPPRODUCT_ID");
    public final static MengineInAppProductParam INAPPPRODUCT_TYPE = new MengineInAppProductParam("INAPPPRODUCT_TYPE");
    public final static MengineInAppProductParam INAPPPRODUCT_NAME = new MengineInAppProductParam("INAPPPRODUCT_NAME");
    public final static MengineInAppProductParam INAPPPRODUCT_TITLE = new MengineInAppProductParam("INAPPPRODUCT_TITLE");
    public final static MengineInAppProductParam INAPPPRODUCT_DESCRIPTION = new MengineInAppProductParam("INAPPPRODUCT_DESCRIPTION");
    public final static MengineInAppProductParam INAPPPRODUCT_PRICE_AMOUNT_MICROS = new MengineInAppProductParam("INAPPPRODUCT_PRICE_AMOUNT_MICROS");
    public final static MengineInAppProductParam INAPPPRODUCT_PRICE_FORMATTED = new MengineInAppProductParam("INAPPPRODUCT_PRICE_FORMATTED");
    public final static MengineInAppProductParam INAPPPRODUCT_PRICE_CURRENCY_CODE = new MengineInAppProductParam("INAPPPRODUCT_PRICE_CURRENCY_CODE");

    private final String m_name;

    MengineInAppProductParam(String name) {
        m_name = name;
    }

    public String getName() {
        return m_name;
    }
    public String toString() {
        return m_name;
    }
}