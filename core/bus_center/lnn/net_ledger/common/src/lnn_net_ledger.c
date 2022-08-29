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

#include "lnn_net_ledger.h"

#include <string.h>
#include <securec.h>

#include "bus_center_manager.h"
#include "lnn_distributed_net_ledger.h"
#include "lnn_local_net_ledger.h"
#include "lnn_meta_node_ledger.h"
#include "softbus_errcode.h"
#include "softbus_log.h"
#include "softbus_utils.h"
#include "softbus_adapter_mem.h"

int32_t LnnInitNetLedger(void)
{
    if (LnnInitLocalLedger() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "init local net ledger fail!");
        return SOFTBUS_ERR;
    }
    if (LnnInitDistributedLedger() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "init distributed net ledger fail!");
        return SOFTBUS_ERR;
    }
    if (LnnInitMetaNodeLedger() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "init meta node ledger fail!");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t LnnInitNetLedgerDelay(void)
{
    return LnnInitLocalLedgerDelay();
}

void LnnDeinitNetLedger(void)
{
    LnnDeinitMetaNodeLedger();
    LnnDeinitDistributedLedger();
    LnnDeinitLocalLedger();
}

static int32_t LnnGetNodeKeyInfoLocal(const char *networkId, int key, uint8_t *info, uint32_t infoLen)
{
    if (networkId == NULL || info == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "params are null");
        return SOFTBUS_ERR;
    }
    switch (key) {
        case NODE_KEY_UDID:
            return LnnGetLocalStrInfo(STRING_KEY_DEV_UDID, (char *)info, infoLen);
        case NODE_KEY_UUID:
            return LnnGetLocalStrInfo(STRING_KEY_UUID, (char *)info, infoLen);
        case NODE_KEY_MASTER_UDID:
            return LnnGetLocalStrInfo(STRING_KEY_MASTER_NODE_UDID, (char *)info, infoLen);
        case NODE_KEY_BR_MAC:
            return LnnGetLocalStrInfo(STRING_KEY_BT_MAC, (char *)info, infoLen);
        case NODE_KEY_IP_ADDRESS:
            return LnnGetLocalStrInfo(STRING_KEY_WLAN_IP, (char *)info, infoLen);
        case NODE_KEY_DEV_NAME:
            return LnnGetLocalStrInfo(STRING_KEY_DEV_NAME, (char *)info, infoLen);
        case NODE_KEY_NETWORK_CAPABILITY:
            return LnnGetLocalNumInfo(NUM_KEY_NET_CAP, (int32_t *)info);
        case NODE_KEY_NETWORK_TYPE:
            return LnnGetLocalNumInfo(NUM_KEY_DISCOVERY_TYPE, (int32_t *)info);
        default:
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "invalid node key type: %d", key);
            return SOFTBUS_ERR;
    }
}

static int32_t LnnGetNodeKeyInfoRemote(const char *networkId, int key, uint8_t *info, uint32_t infoLen)
{
    if (networkId == NULL || info == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "params are null");
        return SOFTBUS_ERR;
    }
    switch (key) {
        case NODE_KEY_UDID:
            return LnnGetRemoteStrInfo(networkId, STRING_KEY_DEV_UDID, (char *)info, infoLen);
        case NODE_KEY_UUID:
            return LnnGetRemoteStrInfo(networkId, STRING_KEY_UUID, (char *)info, infoLen);
        case NODE_KEY_BR_MAC:
            return LnnGetRemoteStrInfo(networkId, STRING_KEY_BT_MAC, (char *)info, infoLen);
        case NODE_KEY_IP_ADDRESS:
            return LnnGetRemoteStrInfo(networkId, STRING_KEY_WLAN_IP, (char *)info, infoLen);
        case NODE_KEY_DEV_NAME:
            return LnnGetRemoteStrInfo(networkId, STRING_KEY_DEV_NAME, (char *)info, infoLen);
        case NODE_KEY_NETWORK_CAPABILITY:
            return LnnGetRemoteNumInfo(networkId, NUM_KEY_NET_CAP, (int32_t *)info);
        case NODE_KEY_NETWORK_TYPE:
            return LnnGetRemoteNumInfo(networkId, NUM_KEY_DISCOVERY_TYPE, (int32_t *)info);
        default:
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "invalid node key type: %d", key);
            return SOFTBUS_ERR;
    }
}

int32_t LnnGetNodeKeyInfo(const char *networkId, int key, uint8_t *info, uint32_t infoLen)
{
    bool isLocalNetworkId = false;
    char localNetworkId[NETWORK_ID_BUF_LEN] = {0};
    if (networkId == NULL || info == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "params are null");
        return SOFTBUS_ERR;
    }
    if (LnnGetLocalStrInfo(STRING_KEY_NETWORKID, localNetworkId, NETWORK_ID_BUF_LEN) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "get local network id fail");
        return SOFTBUS_ERR;
    }
    if (strncmp(localNetworkId, networkId, NETWORK_ID_BUF_LEN) == 0) {
        isLocalNetworkId = true;
    }
    if (isLocalNetworkId) {
        return LnnGetNodeKeyInfoLocal(networkId, key, info, infoLen);
    } else {
        return LnnGetNodeKeyInfoRemote(networkId, key, info, infoLen);
    }
}

int32_t LnnGetNodeKeyInfoLen(int32_t key)
{
    switch (key) {
        case NODE_KEY_UDID:
            return UDID_BUF_LEN;
        case NODE_KEY_UUID:
            return UUID_BUF_LEN;
        case NODE_KEY_MASTER_UDID:
            return UDID_BUF_LEN;
        case NODE_KEY_BR_MAC:
            return MAC_LEN;
        case NODE_KEY_IP_ADDRESS:
            return IP_LEN;
        case NODE_KEY_DEV_NAME:
            return DEVICE_NAME_BUF_LEN;
        case NODE_KEY_NETWORK_CAPABILITY:
            return NUM_BUF_SIZE;
        case NODE_KEY_NETWORK_TYPE:
            return NUM_BUF_SIZE;
        default:
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "invalid node key type: %d", key);
            return SOFTBUS_ERR;
    }
}

int32_t SoftbusDumpPrintUdid(int fd, NodeBasicInfo *nodeInfo)
{
    NodeDeviceInfoKey key;
    key = NODE_KEY_UDID;
    unsigned char udid[UDID_BUF_LEN];
    char newUdid[UDID_BUF_LEN];
    (void)memset_s(udid, sizeof(udid), 0, sizeof(udid));
    (void)memset_s(newUdid, sizeof(newUdid), 0, sizeof(newUdid));
    if (LnnGetNodeKeyInfo(nodeInfo->networkId, key, udid, UDID_BUF_LEN) != 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "LnnGetNodeKeyInfo Udid failed!");
        return SOFTBUS_ERR;
    }
    DataMasking((char *)udid, strlen((char *)udid), ID_DELIMITER, newUdid);
    SOFTBUS_DPRINTF(fd, "newUdid = %s\n", newUdid);
    return SOFTBUS_OK;
}

int32_t SoftbusDumpPrintUuid(int fd, NodeBasicInfo *nodeInfo)
{
    NodeDeviceInfoKey key;
    key = NODE_KEY_UUID;
    unsigned char uuid[UUID_BUF_LEN];
    char newUuid[UUID_BUF_LEN];
    (void)memset_s(uuid, sizeof(uuid), 0, sizeof(uuid));
    (void)memset_s(newUuid, sizeof(newUuid), 0, sizeof(newUuid));
    if (LnnGetNodeKeyInfo(nodeInfo->networkId, key, uuid, UUID_BUF_LEN) != 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "LnnGetNodeKeyInfo Uuid failed!");
        return SOFTBUS_ERR;
    }
    DataMasking((char *)uuid, strlen((char *)uuid), ID_DELIMITER, newUuid);
    SOFTBUS_DPRINTF(fd, "uuid = %s\n", newUuid);
    return SOFTBUS_OK;
}

int32_t SoftbusDumpPrintMac(int fd, NodeBasicInfo *nodeInfo)
{
    NodeDeviceInfoKey key;
    key = NODE_KEY_BR_MAC;
    unsigned char brMac[BT_MAC_LEN];
    char newBrMac[BT_MAC_LEN];
    (void)memset_s(brMac, sizeof(brMac), 0, sizeof(brMac));
    (void)memset_s(newBrMac, sizeof(newBrMac), 0, sizeof(newBrMac));
    if (LnnGetNodeKeyInfo(nodeInfo->networkId, key, brMac, BT_MAC_LEN) != 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "LnnGetNodeKeyInfo brMac failed!");
        return SOFTBUS_ERR;
    }
    DataMasking((char *)brMac, strlen((char *)brMac), MAC_DELIMITER, newBrMac);
    SOFTBUS_DPRINTF(fd, "BrMac = %s\n", newBrMac);
    return SOFTBUS_OK;
}

int32_t SoftbusDumpPrintIp(int fd, NodeBasicInfo *nodeInfo)
{
    NodeDeviceInfoKey key;
    key = NODE_KEY_IP_ADDRESS;
    char ipAddr[IP_STR_MAX_LEN];
    char newIpAddr[IP_STR_MAX_LEN];
    (void)memset_s(ipAddr, sizeof(ipAddr), 0, sizeof(ipAddr));
    (void)memset_s(newIpAddr, sizeof(newIpAddr), 0, sizeof(newIpAddr));
    if (LnnGetNodeKeyInfo(nodeInfo->networkId, key, (uint8_t *)ipAddr, IP_STR_MAX_LEN) != 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "LnnGetNodeKeyInfo ipAddr failed!");
        return SOFTBUS_ERR;
    }
    DataMasking((char *)ipAddr, strlen(ipAddr), IP_DELIMITER, newIpAddr);
    SOFTBUS_DPRINTF(fd, "IpAddr = %s\n", newIpAddr);
    return SOFTBUS_OK;
}

int32_t SoftbusDumpPrintNetCapacity(int fd, NodeBasicInfo *nodeInfo)
{
    NodeDeviceInfoKey key;
    key = NODE_KEY_NETWORK_CAPABILITY;
    int32_t netCapacity = 0;
    if (LnnGetNodeKeyInfo(nodeInfo->networkId, key, (uint8_t *)&netCapacity, NUM_BUF_SIZE) != 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "LnnGetNodeKeyInfo netCapacity failed!");
        return SOFTBUS_ERR;
    }
    SOFTBUS_DPRINTF(fd, "NetCapacity = %d\n", netCapacity);
    return SOFTBUS_OK;
}

int32_t SoftbusDumpPrintNetType(int fd, NodeBasicInfo *nodeInfo)
{
    NodeDeviceInfoKey key;
    key = NODE_KEY_NETWORK_TYPE;
    int32_t netType = 0;
    if (LnnGetNodeKeyInfo(nodeInfo->networkId, key, (uint8_t *)&netType, NUM_BUF_SIZE) != 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "LnnGetNodeKeyInfo netType failed!");
        return SOFTBUS_ERR;
    }
    SOFTBUS_DPRINTF(fd, "NetType = %d\n", netType);
    return SOFTBUS_OK;
}

void SoftBusDumpBusCenterPrintInfo(int fd, NodeBasicInfo *nodeInfo)
{
    if (fd <= 0 || nodeInfo == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "param is null");
        return;
    }
    SOFTBUS_DPRINTF(fd, "DeviceName = %s\n", nodeInfo->deviceName);
    char networkId[NETWORK_ID_BUF_LEN];
    (void)memset_s(networkId, sizeof(networkId), 0, sizeof(networkId));
    DataMasking(nodeInfo->networkId, strlen(nodeInfo->networkId), ID_DELIMITER, networkId);
    SOFTBUS_DPRINTF(fd, "NetworkId = %s\n", networkId);
    if (SoftbusDumpPrintUdid(fd, nodeInfo) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "SoftbusDumpPrintUdid failed!");
        return;
    }
    if (SoftbusDumpPrintUuid(fd, nodeInfo) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "SoftbusDumpPrintUuid failed!");
        return;
    }
    if (SoftbusDumpPrintMac(fd, nodeInfo) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "SoftbusDumpPrintMac failed!");
        return;
    }
    if (SoftbusDumpPrintIp(fd, nodeInfo) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "SoftbusDumpPrintIp failed!");
        return;
    }
    if (SoftbusDumpPrintNetCapacity(fd, nodeInfo) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "SoftbusDumpPrintNetCapacity failed!");
        return;
    }
    if (SoftbusDumpPrintNetType(fd, nodeInfo) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "SoftbusDumpPrintNetType failed!");
        return;
    }
}
