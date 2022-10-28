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
 
#include <securec.h>

#include "auth_interface.h"
#include "gtest/gtest.h"
#include "softbus_app_info.h"
#include "softbus_errcode.h"
#include "softbus_json_utils.h"
#include "softbus_log.h"
#include "softbus_protocol_def.h"
#include "trans_udp_channel_manager.c"
#include "trans_udp_negotiation.c"
#include "trans_udp_negotiation_exchange.c"

using namespace testing::ext;

namespace OHOS {

class TransUdpNegoTest : public testing::Test {
public:
    TransUdpNegoTest()
    {}
    ~TransUdpNegoTest()
    {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override
    {}
    void TearDown() override
    {}
};

void TransUdpNegoTest::SetUpTestCase(void)
{}

void TransUdpNegoTest::TearDownTestCase(void)
{}

/**
 * @tc.name: TransUdpNegoTest001
 * @tc.desc: extern module active publish, stop session whitout start.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransUdpNegoTest, TransUdpNegoTest001, TestSize.Level1)
{
    int ret;
    int32_t errCode = 0;
    string msgStr = "ProcessMessage";
    ret = TransUnpackReplyErrInfo(NULL, NULL);
    EXPECT_TRUE(ret != SOFTBUS_OK);
    cJSON *msg = cJSON_Parse((char *)msgStr.c_str());
    ret = TransUnpackReplyErrInfo(msg, &errCode);
    EXPECT_TRUE(ret != SOFTBUS_OK);
}

/**
 * @tc.name: TransUdpNegoTest002
 * @tc.desc: extern module active publish, stop session whitout start.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransUdpNegoTest, TransUdpNegoTest002, TestSize.Level1)
{
    int ret;
    int32_t errCode = 0;
    ret = TransPackReplyErrInfo(NULL, errCode, NULL);
    EXPECT_TRUE(ret != SOFTBUS_OK);
}

/**
 * @tc.name: TransUdpNegoTest003
 * @tc.desc: extern module active publish, stop session whitout start.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransUdpNegoTest, TransUdpNegoTest003, TestSize.Level1)
{
    int ret;
    int64_t authId = 0;
    int64_t seq = 0;
    string msg = "ProcessMessage";
    cJSON *replyMsg = cJSON_Parse((char *)msg.c_str());

    ret = sendUdpInfo(NULL, authId, seq);
    EXPECT_TRUE(ret != SOFTBUS_OK);

    ret = sendUdpInfo(replyMsg, NULL, NULL);
    EXPECT_TRUE(ret != SOFTBUS_OK);
}

/**
 * @tc.name: TransUdpNegoTest004
 * @tc.desc: extern module active publish, stop session whitout start.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransUdpNegoTest, TransUdpNegoTest004, TestSize.Level1)
{
    int ret;
    int errCode = 0;
    string errDesc = "ProcessMessage";
    ret = SendReplyErrInfo(errCode, NULL, NULL, NULL);
    EXPECT_TRUE(ret != SOFTBUS_OK);

    ret = SendReplyErrInfo(errCode, (char *)errDesc.c_str(), NULL, NULL);
    EXPECT_TRUE(ret != SOFTBUS_OK);
}

/**
 * @tc.name: TransUdpNegoTest005
 * @tc.desc: extern module active publish, stop session whitout start.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransUdpNegoTest, TransUdpNegoTest005, TestSize.Level1)
{
    int ret;
    AppInfo appInfo;
    (void)memset_s(&appInfo, sizeof(AppInfo), 0, sizeof(AppInfo));
    appInfo.udpChannelOptType = TYPE_UDP_CHANNEL_CLOSE;
    ret = SendReplyUdpInfo(&appInfo, NULL, NULL);
    EXPECT_TRUE(ret != SOFTBUS_OK);
}

/**
 * @tc.name: TransUdpNegoTest006
 * @tc.desc: extern module active publish, stop session whitout start.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransUdpNegoTest, TransUdpNegoTest006, TestSize.Level1)
{
    int errCode = 0;
    bool needClose = true;
    AppInfo info;
    (void)memset_s(&info, sizeof(AppInfo), 0, sizeof(AppInfo));
    info.udpChannelOptType = TYPE_UDP_CHANNEL_OPEN;
    ProcessAbnormalUdpChannelState(&info, errCode, needClose);

    info.udpChannelOptType = TYPE_UDP_CHANNEL_CLOSE;
    ProcessAbnormalUdpChannelState(&info, errCode, needClose);
}

/**
 * @tc.name: TransUdpNegoTest007
 * @tc.desc: extern module active publish, stop session whitout start.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransUdpNegoTest, TransUdpNegoTest007, TestSize.Level1)
{
    int64_t seq = 0;
    string msgStr = "ProcessMessage";
    cJSON *msg = cJSON_Parse((char *)msgStr.c_str());
    TransOnExchangeUdpInfoReply(NULL, seq, msg);
}

/**
 * @tc.name: TransUdpNegoTest008
 * @tc.desc: extern module active publish, stop session whitout start.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransUdpNegoTest, TransUdpNegoTest008, TestSize.Level1)
{
    int64_t seq = 0;
    TransOnExchangeUdpInfoRequest(NULL, seq, NULL);
}

/**
 * @tc.name: TransUdpNegoTest009
 * @tc.desc: extern module active publish, stop session whitout start.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransUdpNegoTest, TransUdpNegoTest009, TestSize.Level1)
{
    int32_t ret;
    UdpChannelInfo channel;
    (void)memset_s(&channel, sizeof(UdpChannelInfo), 0, sizeof(UdpChannelInfo));
    channel.info.udpChannelOptType = TYPE_UDP_CHANNEL_OPEN;
    ret = StartExchangeUdpInfo(&channel, NULL, NULL);
    EXPECT_TRUE(ret != SOFTBUS_OK);
}

/**
 * @tc.name: TransUdpNegoTest010
 * @tc.desc: extern module active publish, stop session whitout start.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransUdpNegoTest, TransUdpNegoTest010, TestSize.Level1)
{
    int64_t authId = 0;
    AuthTransData *data;
    (void)memset_s(&data, sizeof(AuthTransData), 0, sizeof(AuthTransData));
    UdpModuleCb(authId, NULL);

    data->flag = SOFTBUS_OK;
    UdpModuleCb(authId, data);
}
}