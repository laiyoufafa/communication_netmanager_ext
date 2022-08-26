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

#include "networkshare_service_stub.h"
#include "netmgr_ext_log_wrapper.h"
#include "networkshare_constants.h"

namespace OHOS {
namespace NetManagerStandard {
NetworkShareServiceStub::NetworkShareServiceStub()
{
    memberFuncMap_[static_cast<uint32_t>(INetworkShareService::MessageCode::CMD_GET_SHARING_SUPPORTED)] =
        &NetworkShareServiceStub::ReplyIsNetworkSharingSupported;
    memberFuncMap_[static_cast<uint32_t>(INetworkShareService::MessageCode::CMD_GET_IS_SHARING)] =
        &NetworkShareServiceStub::ReplyIsSharing;
    memberFuncMap_[static_cast<uint32_t>(INetworkShareService::MessageCode::CMD_START_NETWORKSHARE)] =
        &NetworkShareServiceStub::ReplyStartNetworkSharing;
    memberFuncMap_[static_cast<uint32_t>(INetworkShareService::MessageCode::CMD_STOP_NETWORKSHARE)] =
        &NetworkShareServiceStub::ReplyStopNetworkSharing;
    memberFuncMap_[static_cast<uint32_t>(INetworkShareService::MessageCode::CMD_GET_SHARABLE_REGEXS)] =
        &NetworkShareServiceStub::ReplyGetSharableRegexs;
    memberFuncMap_[static_cast<uint32_t>(INetworkShareService::MessageCode::CMD_GET_SHARING_STATE)] =
        &NetworkShareServiceStub::ReplyGetSharingState;
    memberFuncMap_[static_cast<uint32_t>(INetworkShareService::MessageCode::CMD_GET_SHARING_IFACES)] =
        &NetworkShareServiceStub::ReplyGetNetSharingIfaces;
    memberFuncMap_[static_cast<uint32_t>(INetworkShareService::MessageCode::CMD_REGISTER_EVENT_CALLBACK)] =
        &NetworkShareServiceStub::ReplyRegisterSharingEvent;
    memberFuncMap_[static_cast<uint32_t>(INetworkShareService::MessageCode::CMD_UNREGISTER_EVENT_CALLBACK)] =
        &NetworkShareServiceStub::ReplyUnregisterSharingEvent;
}

int32_t NetworkShareServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                 MessageOption &option)
{
    std::u16string myDescripter = NetworkShareServiceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        NETMGR_EXT_LOG_E("descriptor checked fail");
        return NETMANAGER_EXT_ERR_DESCRIPTOR_MISMATCH;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }

    NETMGR_EXT_LOG_I("stub default case, need check");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t NetworkShareServiceStub::ReplyIsNetworkSharingSupported(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = IsNetworkSharingSupported();
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_EXT_SUCCESS;
}

int32_t NetworkShareServiceStub::ReplyIsSharing(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = IsSharing();
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_EXT_SUCCESS;
}

int32_t NetworkShareServiceStub::ReplyStartNetworkSharing(MessageParcel &data, MessageParcel &reply)
{
    int32_t type;
    if (!data.ReadInt32(type)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    SharingIfaceType shareType = static_cast<SharingIfaceType>(type);
    int32_t ret = StartNetworkSharing(shareType);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_EXT_SUCCESS;
}

int32_t NetworkShareServiceStub::ReplyStopNetworkSharing(MessageParcel &data, MessageParcel &reply)
{
    int32_t type;
    if (!data.ReadInt32(type)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    SharingIfaceType shareType = static_cast<SharingIfaceType>(type);
    int32_t ret = StopNetworkSharing(shareType);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_EXT_SUCCESS;
}

int32_t NetworkShareServiceStub::ReplyGetSharableRegexs(MessageParcel &data, MessageParcel &reply)
{
    int32_t type;
    if (!data.ReadInt32(type)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    SharingIfaceType shareType = static_cast<SharingIfaceType>(type);
    std::vector<std::string> ifaceRegexs = GetSharableRegexs(shareType);

    if (!reply.WriteInt32(ifaceRegexs.size())) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    for (auto it = ifaceRegexs.begin(); it != ifaceRegexs.end(); ++it) {
        if (!reply.WriteString(*it)) {
            return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_EXT_SUCCESS;
}

int32_t NetworkShareServiceStub::ReplyGetSharingState(MessageParcel &data, MessageParcel &reply)
{
    int32_t type;
    if (!data.ReadInt32(type)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    SharingIfaceType shareType = static_cast<SharingIfaceType>(type);
    SharingIfaceState shareState = SharingIfaceState::SHARING_NIC_CAN_SERVER;
    int32_t ret = GetSharingState(shareType, shareState);
    if (ret != 0) {
        NETMGR_EXT_LOG_E("ReplyGetSharingState execute failed.");
        return NETMANAGER_EXT_ERROR;
    }

    if (!reply.WriteInt32(static_cast<int32_t>(shareState))) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_EXT_SUCCESS;
}

int32_t NetworkShareServiceStub::ReplyGetNetSharingIfaces(MessageParcel &data, MessageParcel &reply)
{
    int32_t state;
    if (!data.ReadInt32(state)) {
        return NETMANAGER_EXT_ERR_READ_DATA_FAIL;
    }
    SharingIfaceState shareState = static_cast<SharingIfaceState>(state);
    std::vector<std::string> ifaces = GetNetSharingIfaces(shareState);

    if (!reply.WriteInt32(ifaces.size())) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    for (auto it = ifaces.begin(); it != ifaces.end(); ++it) {
        if (!reply.WriteString(*it)) {
            return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_EXT_SUCCESS;
}

int32_t NetworkShareServiceStub::ReplyRegisterSharingEvent(MessageParcel &data, MessageParcel &reply)
{
    sptr<ISharingEventCallback> callback = iface_cast<ISharingEventCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        NETMGR_EXT_LOG_E("ReplyRegisterSharingEvent callback is null.");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }

    int32_t ret = RegisterSharingEvent(callback);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_EXT_SUCCESS;
}

int32_t NetworkShareServiceStub::ReplyUnregisterSharingEvent(MessageParcel &data, MessageParcel &reply)
{
    sptr<ISharingEventCallback> callback = iface_cast<ISharingEventCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        NETMGR_EXT_LOG_E("ReplyUnregisterSharingEvent callback is null.");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }

    int32_t ret = UnregisterSharingEvent(callback);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_EXT_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_EXT_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
