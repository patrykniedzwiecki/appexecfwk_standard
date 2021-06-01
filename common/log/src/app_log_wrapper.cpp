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

#include "app_log_wrapper.h"
#include <iostream>

namespace OHOS {
namespace AppExecFwk {

// initial static member object
AppLogLevel AppLogWrapper::level_ = AppLogLevel::DEBUG;

bool AppLogWrapper::JudgeLevel(const AppLogLevel &level)
{
    return (AppLogWrapper::GetLogLevel() <= level);
}

std::string AppLogWrapper::GetBriefFileName(const char *str)
{
    if (!str) {
        return std::string();
    }
    std::string fullPath(str);
    size_t pos = fullPath.find_last_of("/");
    if (pos == std::string::npos) {
        return std::string();
    }
    return fullPath.substr(pos + 1);
}

}  // namespace AppExecFwk
}  // namespace OHOS