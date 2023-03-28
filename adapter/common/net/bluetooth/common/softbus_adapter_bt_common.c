/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "softbus_adapter_bt_common.h"

#include <stdbool.h>

#include "ohos_bt_def.h"
#include "ohos_bt_gap.h"
#include "ohos_bt_gatt.h"
#include "securec.h"
#include "softbus_common.h"
#include "softbus_def.h"
#include "softbus_errcode.h"
#include "softbus_log.h"

#define STATE_LISTENER_MAX_NUM 7
#define BR_STATE_CB_TRANSPORT 1

typedef struct {
    bool isUsed;
    SoftBusBtStateListener *listener;
} StateListener;

static int ConvertBtState(int transport, int state)
{
    switch (state) {
        case OHOS_GAP_STATE_TURNING_ON:
            return (transport == BR_STATE_CB_TRANSPORT) ? SOFTBUS_BR_STATE_TURNING_ON : SOFTBUS_BT_STATE_TURNING_ON;
        case OHOS_GAP_STATE_TURN_ON:
            return (transport == BR_STATE_CB_TRANSPORT) ? SOFTBUS_BR_STATE_TURN_ON : SOFTBUS_BT_STATE_TURN_ON;
        case OHOS_GAP_STATE_TURNING_OFF:
            return (transport == BR_STATE_CB_TRANSPORT) ? SOFTBUS_BR_STATE_TURNING_OFF : SOFTBUS_BT_STATE_TURNING_OFF;
        case OHOS_GAP_STATE_TURN_OFF:
            return (transport == BR_STATE_CB_TRANSPORT) ? SOFTBUS_BR_STATE_TURN_OFF : SOFTBUS_BT_STATE_TURN_OFF;
        default:
            return -1;
    }
}

static int ConvertAclState(GapAclState state)
{
    switch (state) {
        case OHOS_GAP_ACL_STATE_CONNECTED:
            return SOFTBUS_ACL_STATE_CONNECTED;
        case OHOS_GAP_ACL_STATE_DISCONNECTED:
            return SOFTBUS_ACL_STATE_DISCONNECTED;
        case OHOS_GAP_ACL_STATE_LE_CONNECTED:
            return SOFTBUS_ACL_STATE_LE_CONNECTED;
        case OHOS_GAP_ACL_STATE_LE_DISCONNECTED:
            return SOFTBUS_ACL_STATE_LE_DISCONNECTED;
        default:
            break;
    }
    return -1;
}

static SoftBusBtAddr ConvertBtAddr(const BdAddr *bdAddr)
{
    SoftBusBtAddr btAddr = {0};
    if (memcpy_s(btAddr.addr, sizeof(btAddr.addr), bdAddr->addr, sizeof(bdAddr->addr)) != EOK) {
        CLOGE("copy bdAddr fail");
    }
    return btAddr;
}

static StateListener g_stateListener[STATE_LISTENER_MAX_NUM];
static bool g_isRegCb = false;

static void WrapperStateChangeCallback(const int transport, const int status)
{
    CLOGI("WrapperStateChangeCallback, transport=%d, status=%d", transport, status);
    int32_t listenerId;
    int32_t st = ConvertBtState(transport, status);
    for (listenerId = 0; listenerId < STATE_LISTENER_MAX_NUM; listenerId++) {
        if (g_stateListener[listenerId].isUsed &&
            g_stateListener[listenerId].listener != NULL &&
            g_stateListener[listenerId].listener->OnBtStateChanged != NULL) {
            g_stateListener[listenerId].listener->OnBtStateChanged(listenerId, st);
        }
    }
}

static void WrapperAclStateChangedCallback(const BdAddr *bdAddr, GapAclState state, unsigned int reason)
{
    if (bdAddr == NULL) {
        CLOGE("WrapperAclStateChangedCallback addr is null");
        return;
    }

    CLOGI("WrapperAclStateChangedCallback, addr:%02X:%02X:***%02X, state=%d, reason=%u",
        bdAddr->addr[MAC_FIRST_INDEX], bdAddr->addr[MAC_ONE_INDEX], bdAddr->addr[MAC_FIVE_INDEX], state, reason);
    int32_t listenerId;
    int32_t aclState = ConvertAclState(state);
    const SoftBusBtAddr btAddr = ConvertBtAddr(bdAddr);
    for (listenerId = 0; listenerId < STATE_LISTENER_MAX_NUM; listenerId++) {
        if (g_stateListener[listenerId].isUsed &&
            g_stateListener[listenerId].listener != NULL &&
            g_stateListener[listenerId].listener->OnBtAclStateChanged != NULL) {
            g_stateListener[listenerId].listener->OnBtAclStateChanged(listenerId, &btAddr, aclState);
        }
    }
}

NO_SANITIZE("cfi") static void WrapperPairRequestedCallback(const BdAddr *bdAddr, int transport)
{
    if (bdAddr == NULL) {
        CLOGE("WrapperPairRequestedCallback addr is null");
        return;
    }

    CLOGI("WrapperPairRequestedCallback, addr:%02X:%02X:***%02X, transport=%d",
        bdAddr->addr[MAC_FIRST_INDEX], bdAddr->addr[MAC_ONE_INDEX], bdAddr->addr[MAC_FIVE_INDEX], transport);
    if (!PairRequestReply(bdAddr, transport, true)) {
        CLOGE("PairRequestReply error");
    }
}

NO_SANITIZE("cfi") static void WrapperPairConfiremedCallback(const BdAddr *bdAddr, int transport, int reqType,
    int number)
{
    if (bdAddr == NULL) {
        CLOGE("WrapperPairConfirmedCallback addr is null");
        return;
    }

    CLOGI("WrapperPairConfirmedCallback, addr=%02X:%02X:***%02X, transport=%d, reqType:%d, number:%d",
        bdAddr->addr[MAC_FIRST_INDEX], bdAddr->addr[MAC_ONE_INDEX], bdAddr->addr[MAC_FIVE_INDEX],
        transport, reqType, number);
    if (!SetDevicePairingConfirmation(bdAddr, transport, true)) {
        CLOGE("SetDevicePairingConfirmation error");
    }
}

static BtGapCallBacks g_softbusGapCb = {
    .stateChangeCallback = WrapperStateChangeCallback,
    .aclStateChangedCallbak = WrapperAclStateChangedCallback,
    .pairRequestedCallback = WrapperPairRequestedCallback,
    .pairConfiremedCallback = WrapperPairConfiremedCallback
};

static int RegisterListenerCallback(void)
{
    if (g_isRegCb) {
        return SOFTBUS_OK;
    }
    if (GapRegisterCallbacks(&g_softbusGapCb) != OHOS_BT_STATUS_SUCCESS) {
        return SOFTBUS_ERR;
    }
    g_isRegCb = true;
    return SOFTBUS_OK;
}

NO_SANITIZE("cfi") int SoftBusAddBtStateListener(const SoftBusBtStateListener *listener)
{
    if (listener == NULL) {
        return SOFTBUS_ERR;
    }
    if (RegisterListenerCallback() != SOFTBUS_OK) {
        return SOFTBUS_ERR;
    }
    for (int index = 0; index < STATE_LISTENER_MAX_NUM; index++) {
        if (!g_stateListener[index].isUsed) {
            g_stateListener[index].isUsed = true;
            g_stateListener[index].listener = (SoftBusBtStateListener *)listener;
            return index;
        }
    }
    return SOFTBUS_ERR;
}

NO_SANITIZE("cfi") int SoftBusRemoveBtStateListener(int listenerId)
{
    if (listenerId < 0 || listenerId >= STATE_LISTENER_MAX_NUM) {
        return SOFTBUS_INVALID_PARAM;
    }
    g_stateListener[listenerId].isUsed = false;
    g_stateListener[listenerId].listener = NULL;
    return SOFTBUS_OK;
}

NO_SANITIZE("cfi") int SoftBusEnableBt(void)
{
    if (EnableBle()) {
        return SOFTBUS_OK;
    }
    return SOFTBUS_ERR;
}

NO_SANITIZE("cfi") int SoftBusDisableBt(void)
{
    if (DisableBle()) {
        return SOFTBUS_OK;
    }
    return SOFTBUS_ERR;
}

NO_SANITIZE("cfi") int SoftBusGetBtState(void)
{
    if (IsBleEnabled()) {
        return BLE_ENABLE;
    }
    return BLE_DISABLE;
}

NO_SANITIZE("cfi") int SoftBusGetBtMacAddr(SoftBusBtAddr *mac)
{
    if (mac == NULL) {
        return SOFTBUS_ERR;
    }

    if (!GetLocalAddr(mac->addr, BT_ADDR_LEN)) {
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int SoftBusGetBtName(unsigned char *name, unsigned int *len)
{
    (void)name;
    (void)len;
    return SOFTBUS_OK;
}

NO_SANITIZE("cfi") int SoftBusSetBtName(const char *name)
{
    if (SetLocalName((unsigned char *)name, strlen(name))) {
        return SOFTBUS_OK;
    }
    return SOFTBUS_ERR;
}
