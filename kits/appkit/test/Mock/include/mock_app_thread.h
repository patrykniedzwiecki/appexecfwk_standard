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

#ifndef FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_APP_THREAD_H
#define FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_APP_THREAD_H

#include <gtest/gtest.h>
#include "event_handler.h"
#include "refbase.h"
#include "ohos_application.h"
#include "ability_local_record.h"

namespace OHOS {
namespace AppExecFwk {

class MockHandler : public EventHandler {
public:
    explicit MockHandler(const std::shared_ptr<EventRunner> &runner);
    ~MockHandler()
    {}
    void ProcessEvent(const InnerEvent::Pointer &event) override;
};

}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_KITS_APPKIT_TEST_MOCK_APP_THREAD_H