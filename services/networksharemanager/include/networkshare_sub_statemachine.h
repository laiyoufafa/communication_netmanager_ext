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

#ifndef NETWORKSHARE_SUB_STATEMACHINE_H
#define NETWORKSHARE_SUB_STATEMACHINE_H

#include <cstring>
#include <map>
#include <any>
#include <set>
#include <mutex>
#include <sstream>
#include <openssl/sha.h>
#include <securec.h>
#include <stdarg.h>
#include <functional>

#include "net_manager_ext_constants.h"
#include "networkshare_state_common.h"
#include "dhcp_service.h"
#include "networkshare_configuration.h"

namespace OHOS {
namespace NetManagerStandard {
class NetworkShareSubStateMachine;
struct SubSmStateTable {
    int event_;
    int curState_;
    int (NetworkShareSubStateMachine::*pEventActFun_)(const std::any &messageObj);
    int nextState_;
};

class NetworkShareSubStateMachine : public std::enable_shared_from_this<NetworkShareSubStateMachine> {
public:
    NetworkShareSubStateMachine(const std::string &ifaceName, const SharingIfaceType &interfaceType,
                                const std::shared_ptr<NetworkShareConfiguration> &configuration);
    ~NetworkShareSubStateMachine() = default;

    /**
     * get sub state machine share type
     */
    SharingIfaceType GetNetShareType() const;

    /**
     * get sub state machine interface name
     */
    const std::string &GetInterfaceName() const;

    class SubStateMachineCallback {
    public:
        virtual void OnUpdateInterfaceState(const std::shared_ptr<NetworkShareSubStateMachine> &paraSubStateMachine,
                                            int state, int lastError) = 0;
    };

    /**
     * register callback
     */
    void RegisterSubSMCallback(const std::shared_ptr<SubStateMachineCallback> &callback);

    /**
     * execute state switch
     */
    void SubSmStateSwitch(int newState);

    /**
     * execute event
     */
    void SubSmEventHandle(int eventId, const std::any &messageObj);

private:
    void CreateInitStateTable();
    void CreateSharedStateTable();
    void InitStateEnter();
    void SharedStateEnter();
    void UnavailableStateEnter();
    void InitStateExit();
    void SharedStateExit();
    void UnavailableStateExit();
    int HandleInitSharingRequest(const std::any &messageObj);
    int HandleInitInterfaceDown(const std::any &messageObj);
    int HandleSharedUnrequest(const std::any &messageObj);
    int HandleSharedInterfaceDown(const std::any &messageObj);
    int HandleSharedConnectionChange(const std::any &messageObj);
    int HandleSharedErrors(const std::any &messageObj);

    bool ConfigureShareDhcp(bool enabled);
    bool RequestIpv4Address(std::shared_ptr<INetAddr> &netAddr);
    bool StartDhcp(const std::shared_ptr<INetAddr> &netAddr);
    bool StopDhcp();
    void HandleConnectionChanged(const std::shared_ptr<UpstreamNetworkInfo> &upstreamNetInfo);
    void RemoveRoutesToLocalNetwork();
    void AddRoutesToLocalNetwork();
    void CleanupUpstreamInterface();
    bool HasChangeUpstreamIfaceSet(const std::string &newUpstreamIface);
    bool GetWifiHotspotDhcpFlag();
    bool GetBtDestinationAddr(std::string &addrStr);
    bool GetWifiApDestinationAddr(std::string &addrStr);
    bool CheckConfig(std::string &endIp, std::string &mask);

private:
    std::recursive_mutex mutex_;
    std::string ifaceName_;
    SharingIfaceType netShareType_;
    int32_t lastError_;
    std::string upstreamIfaceName_;
    std::shared_ptr<SubStateMachineCallback> trackerCallback_ = nullptr;
    std::unique_ptr<OHOS::Wifi::IDhcpService> dhcpService_ = nullptr;
    std::shared_ptr<NetworkShareConfiguration> configuration_ = nullptr;
    int curState_;
    std::vector<SubSmStateTable> stateTable_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETWORKSHARE_SUB_STATEMACHINE_H
