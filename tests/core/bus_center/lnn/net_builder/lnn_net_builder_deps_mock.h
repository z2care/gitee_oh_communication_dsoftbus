/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef LNN_NET_BUILDER_DEPS_MOCK_H
#define LNN_NET_BUILDER_DEPS_MOCK_H

#include <gmock/gmock.h>
#include <mutex>

#include "auth_interface.h"
#include "lnn_distributed_net_ledger.h"
#include "lnn_settingdata_event_monitor.h"
#include "softbus_bus_center.h"

namespace OHOS {
class NetBuilderDepsInterface {
public:
    NetBuilderDepsInterface() {};
    virtual ~NetBuilderDepsInterface() {};

    virtual int32_t LnnGetSettingDeviceName(char *deviceName, uint32_t len) = 0;
    virtual int32_t LnnGetAllOnlineAndMetaNodeInfo(NodeBasicInfo **info, int32_t *infoNum) = 0;
    virtual int32_t AuthGetDeviceUuid(int64_t authId, char *uuid, uint16_t size) = 0;
    virtual NodeInfo *LnnGetNodeInfoById(const char *id, IdCategory type) = 0;
    virtual int32_t LnnDeleteMetaInfo(const char *udid, ConnectionAddrType type) = 0;
    virtual int32_t TransGetConnByChanId(int32_t channelId, int32_t channelType, int32_t* connId) = 0;
    virtual int32_t AuthMetaStartVerify(uint32_t connectionId, const uint8_t *key, uint32_t keyLen,
        uint32_t requestId, const AuthVerifyCallback *callBack) = 0;
};
class NetBuilderDepsInterfaceMock : public NetBuilderDepsInterface {
public:
    NetBuilderDepsInterfaceMock();
    ~NetBuilderDepsInterfaceMock() override;
    MOCK_METHOD2(LnnGetSettingDeviceName, int32_t (char *, uint32_t));
    MOCK_METHOD2(LnnGetAllOnlineAndMetaNodeInfo, int32_t (NodeBasicInfo **, int32_t *));
    MOCK_METHOD3(AuthGetDeviceUuid, int32_t (int64_t, char*, uint16_t));
    MOCK_METHOD2(LnnGetNodeInfoById, NodeInfo * (const char *, IdCategory));
    MOCK_METHOD2(LnnDeleteMetaInfo, int32_t (const char *, ConnectionAddrType));
    MOCK_METHOD3(TransGetConnByChanId, int32_t (int32_t, int32_t, int32_t *));
    MOCK_METHOD5(AuthMetaStartVerify, int32_t (uint32_t, const uint8_t *,
        uint32_t, uint32_t, const AuthVerifyCallback *));
};
} // namespace OHOS
#endif // LNN_NET_BUILDER_DEPS_MOCK_H