/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef NETWORKSHARE_CONSTANTS_H
#define NETWORKSHARE_CONSTANTS_H

#include "net_manager_ext_constants.h"

namespace OHOS {
namespace NetManagerStandard {
// Share Return
constexpr int NETWORKSHARE_IS_SUPPORTED = 1;
constexpr int NETWORKSHARE_IS_UNSUPPORTED = 0;
constexpr int NETWORKSHARE_IS_SHARING = 1;
constexpr int NETWORKSHARE_IS_UNSHARING = 0;
// Error Code
constexpr int NETWORKSHARE_SUCCESS = 0;
constexpr int NETWORKSHARE_ERROR = -1;
constexpr int NETWORKSHARE_ERROR_NO_ERROR = NETWORKSHARE_SUCCESS;
constexpr int NETWORKSHARE_ERROR_UNKNOWN_TYPE = -2;
constexpr int NETWORKSHARE_ERROR_UNKNOWN_IFACE = -3;
constexpr int NETWORKSHARE_ERROR_UNAVAIL_IFACE = -4;
constexpr int NETWORKSHARE_ERROR_SERVICE_UNAVAIL = -5;
constexpr int NETWORKSHARE_ERROR_UNSUPPORTED = -6;
constexpr int NETWORKSHARE_ERROR_INTERNAL_ERROR = -7;
constexpr int NETWORKSHARE_ERROR_TETHER_IFACE_ERROR = -8;
constexpr int NETWORKSHARE_ERROR_UNTETHER_IFACE_ERROR = -9;
constexpr int NETWORKSHARE_ERROR_ENABLE_FORWARDING_ERROR = -10;
constexpr int NETWORKSHARE_ERROR_DISABLE_FORWARDING_ERROR = -11;
constexpr int NETWORKSHARE_ERROR_IFACE_CFG_ERROR = -12;
constexpr int NETWORKSHARE_ERROR_PROVISIONING_FAILED = -13;
constexpr int NETWORKSHARE_ERROR_DHCPSERVER_ERROR = -14;
constexpr int NETWORKSHARE_ERROR_ENTITLEMENT_UNKNOWN = -15;
constexpr int NETWORKSHARE_ERROR_NO_CHANGE_TETHERING_PERMISSION = -16;
constexpr int NETWORKSHARE_ERROR_NO_ACCESS_TETHERING_PERMISSION = -17;
// Share Type
constexpr int NETWORKSHARING_INVALID = -1;
constexpr int NETWORKSHARING_WIFI = 0;
constexpr int NETWORKSHARING_USB = 1;
constexpr int NETWORKSHARING_BLUETOOTH = 2;
constexpr int NETWORKSHARING_WIFI_P2P = 3;
constexpr int NETWORKSHARING_NCM = 4;
constexpr int NETWORKSHARING_ETHERNET = 5;
constexpr int NETWORKSHARING_WIGIG = 6;
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETWORKSHARE_CONSTANTS_H
