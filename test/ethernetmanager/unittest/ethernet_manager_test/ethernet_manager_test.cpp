/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
 
#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "ethernet_client.h"
#include "gtest/gtest-message.h"
#include "gtest/gtest-test-part.h"
#include "gtest/hwext/gtest-ext.h"
#include "gtest/hwext/gtest-tag.h"
#include "inet_addr.h"
#include "interface_configuration.h"
#include "interface_type.h"
#include "nativetoken_kit.h"
#include "net_manager_constants.h"
#include "netmgr_ext_log_wrapper.h"
#include "refbase.h"
#include "singleton.h"
#include "static_configuration.h"
#include "token_setproc.h"

#define private public
#define protected public
#include "ethernet_client.h"
#include "ethernet_dhcp_controller.h"
#include "ethernet_service.h"
#include "ethernet_management.h"
#include "ethernet_service_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;
constexpr const char *DEV_NAME = "eth0";
constexpr const char *DEV_UP = "up";
constexpr const char *DEV_DOWN = "down";

HapInfoParams testInfoParms = {
    .userID = 1,
    .bundleName = "ethernet_manager_test",
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = true
};
PermissionDef testPermDef = {
    .permissionName = "ohos.permission.GET_NETWORK_INFO",
    .bundleName = "ethernet_manager_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test network share manager",
    .descriptionId = 1,
};
PermissionStateFull testState = {
    .permissionName = "ohos.permission.GET_NETWORK_INFO",
    .isGeneral = true,
    .resDeviceID = {"local"},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .grantFlags = {2},
};
HapPolicyParams testPolicyPrams1 = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = {testPermDef},
    .permStateList = {testState},
};

PermissionDef testPermDef2 = {
    .permissionName = "ohos.permission.CONNECTIVITY_INTERNAL",
    .bundleName = "ethernet_manager_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test network share manager",
    .descriptionId = 1,
};
PermissionStateFull testState2 = {
    .permissionName = "ohos.permission.CONNECTIVITY_INTERNAL",
    .isGeneral = true,
    .resDeviceID = {"local"},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .grantFlags = {2},
};
HapPolicyParams testPolicyPrams2 = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = {testPermDef2},
    .permStateList = {testState2},
};
std::string INFO = "info";
constexpr const char *IFACE = "iface0";
const int32_t FD = 5;
const int32_t SYSTEM_ABILITY_INVALID = 666;
constexpr uint16_t DEPENDENT_SERVICE_All = 0x0003;
const int32_t RET_ZERO = 0;
} // namespace

class AccessToken {
public:
    explicit AccessToken(HapPolicyParams &testPolicyPrams) : currentID_(GetSelfTokenID())
    {
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testInfoParms, testPolicyPrams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenIdEx.tokenIDEx);
    }
    ~AccessToken()
    {
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }

private:
    AccessTokenID currentID_;
    AccessTokenID accessID_ = 0;
};

class EthernetManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<InterfaceConfiguration> GetIfaceConfig();
    bool CheckIfaceUp(const std::string &iface);
};

void EthernetManagerTest::SetUpTestCase() {}

void EthernetManagerTest::TearDownTestCase() {}

void EthernetManagerTest::SetUp() {}

void EthernetManagerTest::TearDown() {}

sptr<InterfaceConfiguration> EthernetManagerTest::GetIfaceConfig()
{
    sptr<InterfaceConfiguration> ic = (std::make_unique<InterfaceConfiguration>()).release();
    if (!ic) {
        return ic;
    }
    ic->ipStatic_.ipAddr_.type_ = INetAddr::IPV4;
    ic->ipStatic_.ipAddr_.family_ = 0x01;
    ic->ipStatic_.ipAddr_.prefixlen_ = 0x01;
    ic->ipStatic_.ipAddr_.address_ = "172.17.5.234";
    ic->ipStatic_.ipAddr_.netMask_ = "255.255.254.0";
    ic->ipStatic_.ipAddr_.hostName_ = "netAddr";
    ic->ipStatic_.route_.type_ = INetAddr::IPV4;
    ic->ipStatic_.route_.family_ = 0x01;
    ic->ipStatic_.route_.prefixlen_ = 0x01;
    ic->ipStatic_.route_.address_ = "0.0.0.0";
    ic->ipStatic_.route_.netMask_ = "0.0.0.0";
    ic->ipStatic_.route_.hostName_ = "netAddr";
    ic->ipStatic_.gateway_.type_ = INetAddr::IPV4;
    ic->ipStatic_.gateway_.family_ = 0x01;
    ic->ipStatic_.gateway_.prefixlen_ = 0x01;
    ic->ipStatic_.gateway_.address_ = "172.17.4.1";
    ic->ipStatic_.gateway_.netMask_ = "0.0.0.0";
    ic->ipStatic_.gateway_.hostName_ = "netAddr";
    ic->ipStatic_.netMask_.type_ = INetAddr::IPV4;
    ic->ipStatic_.netMask_.family_ = 0x01;
    ic->ipStatic_.netMask_.netMask_ = "255.255.255.0";
    ic->ipStatic_.netMask_.hostName_ = "netAddr";
    INetAddr dns1;
    dns1.type_ = INetAddr::IPV4;
    dns1.family_ = 0x01;
    dns1.address_ = "8.8.8.8";
    dns1.hostName_ = "netAddr";
    INetAddr dns2;
    dns2.type_ = INetAddr::IPV4;
    dns2.family_ = 0x01;
    dns2.address_ = "114.114.114.114";
    dns2.hostName_ = "netAddr";
    ic->ipStatic_.dnsServers_.push_back(dns1);
    ic->ipStatic_.dnsServers_.push_back(dns2);
    return ic;
}

bool EthernetManagerTest::CheckIfaceUp(const std::string &iface)
{
    AccessToken accessToken(testPolicyPrams1);
    int32_t activeStatus = 0;
    (void)DelayedSingleton<EthernetClient>::GetInstance()->IsIfaceActive(iface, activeStatus);
    return activeStatus == 1;
}

/**
 * @tc.name: EthernetManager001
 * @tc.desc: Test EthernetManager SetIfaceConfig.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager001, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    AccessToken accessToken(testPolicyPrams2);
    sptr<InterfaceConfiguration> ic = GetIfaceConfig();
    ASSERT_EQ(DelayedSingleton<EthernetClient>::GetInstance()->SetIfaceConfig(DEV_NAME, ic), NETMANAGER_EXT_SUCCESS);
}

/**
 * @tc.name: EthernetManager002
 * @tc.desc: Test EthernetManager GetIfaceConfig.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager002, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    AccessToken accessToken(testPolicyPrams1);
    sptr<InterfaceConfiguration> ic;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->GetIfaceConfig(DEV_NAME, ic);
    ASSERT_TRUE(ic != nullptr);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}

/**
 * @tc.name: EthernetManager003
 * @tc.desc: Test EthernetManager IsIfaceActive.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager003, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    AccessToken accessToken(testPolicyPrams1);
    int32_t activeStatus = -1;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->IsIfaceActive(DEV_NAME, activeStatus);
    ASSERT_EQ(activeStatus, 1);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}

/**
 * @tc.name: EthernetManager004
 * @tc.desc: Test EthernetManager GetAllActiveIfaces.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager004, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    AccessToken accessToken(testPolicyPrams1);
    std::vector<std::string> result;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->GetAllActiveIfaces(result);
    std::vector<std::string>::iterator it = std::find(result.begin(), result.end(), DEV_NAME);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
    ASSERT_TRUE(it != result.end());
}

/**
 * @tc.name: EthernetManager005
 * @tc.desc: Test EthernetManager ResetFactory.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager005, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->ResetFactory();
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}

HWTEST_F(EthernetManagerTest, EthernetManager006, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    AccessToken accessToken(testPolicyPrams1);
    OHOS::nmd::InterfaceConfigurationParcel cfg;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->GetInterfaceConfig(DEV_NAME, cfg);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
    ASSERT_FALSE(cfg.ifName.empty());
    ASSERT_FALSE(cfg.hwAddr.empty());
}

HWTEST_F(EthernetManagerTest, SetInterfaceConfig001, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    AccessToken accessToken(testPolicyPrams2);
    OHOS::nmd::InterfaceConfigurationParcel config;
    config.ifName = "eth0";
    config.hwAddr = "";
    config.ipv4Addr = "172.17.5.234";
    config.prefixLength = 24;
    config.flags.push_back("up");
    config.flags.push_back("broadcast");
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->SetInterfaceConfig(DEV_NAME, config);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}

HWTEST_F(EthernetManagerTest, SetInterfaceConfig002, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    AccessToken accessToken(testPolicyPrams2);
    int32_t result = DelayedSingleton<EthernetClient>::GetInstance()->SetInterfaceDown(DEV_NAME);
    OHOS::nmd::InterfaceConfigurationParcel config;
    config.ifName = "eth0";
    config.hwAddr = "";
    config.ipv4Addr = "172.17.5.234";
    config.prefixLength = 24;
    config.flags.push_back("up");
    config.flags.push_back("broadcast");
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->SetInterfaceConfig(DEV_NAME, config);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
    ASSERT_TRUE(result == 0);
}

HWTEST_F(EthernetManagerTest, EthernetManager007, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    AccessToken accessToken(testPolicyPrams2);
    int32_t result = DelayedSingleton<EthernetClient>::GetInstance()->SetInterfaceUp(DEV_NAME);
    OHOS::nmd::InterfaceConfigurationParcel cfg;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->GetInterfaceConfig(DEV_NAME, cfg);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
    auto fit = std::find(cfg.flags.begin(), cfg.flags.end(), DEV_DOWN);
    ASSERT_EQ(cfg.ifName, DEV_NAME);
    ASSERT_TRUE(*fit == DEV_DOWN);
    ASSERT_TRUE(result == 0);
}

HWTEST_F(EthernetManagerTest, EthernetManager008, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    int32_t result = DelayedSingleton<EthernetClient>::GetInstance()->SetInterfaceUp(DEV_NAME);
    OHOS::nmd::InterfaceConfigurationParcel cfg;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->GetInterfaceConfig(DEV_NAME, cfg);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
    auto fit = std::find(cfg.flags.begin(), cfg.flags.end(), DEV_UP);
    ASSERT_EQ(cfg.ifName, DEV_NAME);
    ASSERT_TRUE(*fit == DEV_UP);
    ASSERT_TRUE(result == 0);

    EthernetDhcpController ethernetDhcpController;
    bool bIpv6 = true;
    ethernetDhcpController.StartDhcpClient(IFACE, bIpv6);
    ethernetDhcpController.StopDhcpClient(IFACE, bIpv6);
    OHOS::Wifi::DhcpResult dhcpResult;
    ethernetDhcpController.OnDhcpSuccess(IFACE, dhcpResult);
    ethernetDhcpController.cbObject_ = nullptr;
    ethernetDhcpController.OnDhcpSuccess(IFACE, dhcpResult);
}

HWTEST_F(EthernetManagerTest, EthernetManager009, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    EthernetService ethernetService;

    ethernetService.state_ = EthernetService::ServiceRunningState::STATE_RUNNING;
    ethernetService.OnStart();
    ethernetService.state_ = EthernetService::ServiceRunningState::STATE_STOPPED;
    ethernetService.OnStart();
    ethernetService.registerToService_ = false;
    bool initResult = ethernetService.Init();
    EXPECT_TRUE(initResult);
    ethernetService.OnStart();
    ethernetService.serviceComm_ = nullptr;
    initResult = ethernetService.Init();
    EXPECT_FALSE(initResult);
    ethernetService.OnStop();
    std::vector<std::u16string> args;
    std::u16string strU16 = u"ahaha";
    args.push_back(strU16);
    int32_t dumpRes = ethernetService.Dump(FD, args);
    EXPECT_NE(dumpRes, NETMANAGER_EXT_SUCCESS);
    ethernetService.ethManagement_ = nullptr;
    dumpRes = ethernetService.Dump(FD, args);
    EXPECT_NE(dumpRes, NETMANAGER_EXT_SUCCESS);
    ethernetService.OnAddSystemAbility(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID, DEV_NAME);
    ethernetService.OnAddSystemAbility(COMMON_EVENT_SERVICE_ID, DEV_NAME);
    ethernetService.OnAddSystemAbility(SYSTEM_ABILITY_INVALID, DEV_NAME);
    ethernetService.dependentServiceState_ = DEPENDENT_SERVICE_All;
    ethernetService.OnAddSystemAbility(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID, DEV_NAME);
    ethernetService.InitManagement();
    ethernetService.ethManagement_ = nullptr;
    ethernetService.InitManagement();
}

HWTEST_F(EthernetManagerTest, EthernetManager010, TestSize.Level1)
{
    if (!CheckIfaceUp(DEV_NAME)) {
        return;
    }
    EthernetManagement ethernetManagement;
    ethernetManagement.UpdateInterfaceState(DEV_NAME, true);
    ethernetManagement.UpdateInterfaceState(DEV_NAME, false);
    EthernetDhcpCallback::DhcpResult dhcpResult;
    int32_t ret = ethernetManagement.UpdateDevInterfaceLinkInfo(dhcpResult);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
    sptr<InterfaceConfiguration> ic;
    ethernetManagement.GetDevInterfaceCfg(IFACE, ic);
    ethernetManagement.Init();
    ethernetManagement.StartSetDevUpThd();
    ethernetManagement.DevInterfaceAdd(DEV_NAME);
    ethernetManagement.DevInterfaceRemove(DEV_NAME);
    ethernetManagement.GetDumpInfo(INFO);
    EthernetManagement::DevInterfaceStateCallback devCallback(ethernetManagement);
    ret = devCallback.OnInterfaceAdded(IFACE);
    EXPECT_EQ(ret, RET_ZERO);
    ret = devCallback.OnInterfaceRemoved(IFACE);
    EXPECT_EQ(ret, RET_ZERO);
    ret = devCallback.OnInterfaceLinkStateChanged(IFACE, true);
    EXPECT_EQ(ret, RET_ZERO);
}

HWTEST_F(EthernetManagerTest, EthernetDhcpController001, TestSize.Level1)
{
    EthernetDhcpController dhcpController;
    sptr<EthernetDhcpCallback> callback = nullptr;
    dhcpController.RegisterDhcpCallback(callback);
    const std::string iface = "eth0";
    dhcpController.StartDhcpClient(iface, true);
    dhcpController.StopDhcpClient(iface, true);

    OHOS::Wifi::DhcpResult result;
    dhcpController.OnDhcpSuccess(iface, result);

    EthernetDhcpController::EthernetDhcpControllerResultNotify ethernetDhcpControllerResultNotify(dhcpController);
    int status = 1;
    std::string ifname;
    std::string reason;
    ethernetDhcpControllerResultNotify.OnSuccess(status, ifname, result);
    ethernetDhcpControllerResultNotify.OnFailed(status, ifname, reason);
    ethernetDhcpControllerResultNotify.OnSerExitNotify(ifname); 
}

} // namespace NetManagerStandard
} // namespace OHOS
