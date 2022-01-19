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

#include <gtest/gtest.h>

#include "bundle_installer_host.h"
#include "bundle_mgr_service.h"
#include "install_param.h"
#include "installd/installd_service.h"
#include "installd_client.h"
#include "mock_status_receiver.h"
#include "system_bundle_installer.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace OHOS::DistributedKv;
using namespace OHOS;

namespace {
std::string BUNDULE_NAME_FIRST {"bunduleName1"};
std::string BUNDULE_NAME_SECOND {"bunduleName2"};
const std::string MAIN_ABILITY {"com.ohos.distributedmusicplayer.MainAbility"};
constexpr uint32_t VERSION {5};
constexpr uint32_t VERSION_CODE {10};
std::string VERSION_NAME {"10"};
constexpr uint32_t MIN_COMPATIBLE_VERSION {8};
constexpr uint32_t TARGET_VERSION_CODE {10};
constexpr uint32_t COMPATIBLE_VERSION_CODE {10};
constexpr int32_t USER_ID {0};
constexpr int32_t USER_ID_INVALID {1000};
std::string APP_ID {"com.ohos.distributedmusicplayer_1234567890123"};
std::string NETWORK_ID_INVALID {"ffea7058b8cfcb4b74628faeaa7063ac3f1a337294176202b54311540072db42"};
}

class BmsDistributedDataStorageTest : public testing::Test {
public:
    BmsDistributedDataStorageTest();
    ~BmsDistributedDataStorageTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    const std::shared_ptr<DistributedDataStorage> GetDistributedDataStorage() const;
    DistributedBundleInfo MockDistributedBundleInfo(const std::string &bundleName) const;

private:
    std::shared_ptr<DistributedDataStorage> distributedDataStorage_ = std::make_shared<DistributedDataStorage>();
};

BmsDistributedDataStorageTest::BmsDistributedDataStorageTest()
{
}

BmsDistributedDataStorageTest::~BmsDistributedDataStorageTest()
{
}

void BmsDistributedDataStorageTest::SetUpTestCase()
{
}

void BmsDistributedDataStorageTest::TearDownTestCase()
{
}

void BmsDistributedDataStorageTest::SetUp()
{
    bool result = distributedDataStorage_->SetDeviceId();
    EXPECT_TRUE(result);
}

void BmsDistributedDataStorageTest::TearDown()
{
}

const std::shared_ptr<DistributedDataStorage> BmsDistributedDataStorageTest::GetDistributedDataStorage() const
{
    return distributedDataStorage_;
}

DistributedBundleInfo BmsDistributedDataStorageTest::MockDistributedBundleInfo(const std::string &bundleName) const
{
    DistributedBundleInfo distributedBundleInfo;
    distributedBundleInfo.name = bundleName;
    distributedBundleInfo.mainAbility = MAIN_ABILITY;
    distributedBundleInfo.minCompatibleVersion = MIN_COMPATIBLE_VERSION;
    distributedBundleInfo.targetVersionCode = TARGET_VERSION_CODE;
    distributedBundleInfo.version = VERSION;
    distributedBundleInfo.versionCode = VERSION_CODE;
    distributedBundleInfo.versionName = VERSION_NAME;
    distributedBundleInfo.appId = APP_ID;
    distributedBundleInfo.compatibleVersionCode = COMPATIBLE_VERSION_CODE;
    BundleUserInfo bundleUserInfo;
    bundleUserInfo.userId = USER_ID;
    bundleUserInfo.enabled = true;
    bundleUserInfo.abilities.emplace_back(MAIN_ABILITY);
    distributedBundleInfo.bundleUserInfos.emplace_back(bundleUserInfo);
    return distributedBundleInfo;
}

/**
 * @tc.number: SaveStorageDistributeInfo_0100
 * @tc.name: test bundle DistributedBundleInfo can be save success
 * @tc.desc: 1.save DistributedBundleInfo
 *           2.can delete DistributedBundleInfo saved
 */
HWTEST_F(BmsDistributedDataStorageTest, SaveStorageDistributeInfo_0100, Function | SmallTest | Level0)
{
    auto distributedBundleInfo = MockDistributedBundleInfo(BUNDULE_NAME_FIRST);
    auto dataStorage =  GetDistributedDataStorage();
    bool result = dataStorage->SaveStorageDistributeInfo(distributedBundleInfo);
    EXPECT_TRUE(result);

    result = dataStorage->DeleteStorageDistributeInfo(BUNDULE_NAME_FIRST);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: DeleteStorageDistributeInfo_0100
 * @tc.name: test bundle DistributedBundleInfo
 * @tc.desc: 1.can not delete DistributedBundleInfo does not exist but ret true
 *
 */
HWTEST_F(BmsDistributedDataStorageTest, DeleteStorageDistributeInfo_0100, Function | SmallTest | Level0)
{
    auto dataStorage =  GetDistributedDataStorage();
    bool result = dataStorage->DeleteStorageDistributeInfo(BUNDULE_NAME_SECOND);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: QueryStroageDistributeInfo_0100
 * @tc.name: test bundle DistributedBundleInfo can be query
 * @tc.desc: 1.can not query DistributedBundleInfo does not exist
 *
 */
HWTEST_F(BmsDistributedDataStorageTest, QueryStroageDistributeInfo_0100, Function | SmallTest | Level0)
{
    auto dataStorage =  GetDistributedDataStorage();
    DistributedBundleInfo distributedBundleInfo;
    bool result = dataStorage->QueryStroageDistributeInfo(
        BUNDULE_NAME_SECOND, USER_ID_INVALID, NETWORK_ID_INVALID, distributedBundleInfo);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: QueryStroageDistributeInfo_0200
 * @tc.name: test bundle DistributedBundleInfo can be query
 * @tc.desc: 1.save DistributedBundleInfo
 *           2.can query DistributedBundleInfo saved success
 *
 */
HWTEST_F(BmsDistributedDataStorageTest, QueryStroageDistributeInfo_0200, Function | SmallTest | Level0)
{
    auto dataStorage =  GetDistributedDataStorage();
    auto distributedBundleInfo = MockDistributedBundleInfo(BUNDULE_NAME_FIRST);
    bool result = dataStorage->SaveStorageDistributeInfo(distributedBundleInfo);
    EXPECT_TRUE(result);

    DistributedKv::DistributedKvDataManager dataManager;
    DeviceInfo deviceInfo;
    dataManager.GetLocalDevice(deviceInfo);

    DistributedBundleInfo getInfo;
    result = dataStorage->QueryStroageDistributeInfo(BUNDULE_NAME_FIRST, USER_ID, deviceInfo.deviceId, getInfo);
    EXPECT_TRUE(result);

    EXPECT_EQ(getInfo.name, distributedBundleInfo.name);

    result = dataStorage->DeleteStorageDistributeInfo(BUNDULE_NAME_FIRST);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: QueryStroageDistributeInfo_0300
 * @tc.name: test bundle DistributedBundleInfo can be query
 * @tc.desc: 1.save DistributedBundleInfo
 *           2.can not query DistributedBundleInfo UserInfo with invalid UserId
 *
 */
HWTEST_F(BmsDistributedDataStorageTest, QueryStroageDistributeInfo_0300, Function | SmallTest | Level0)
{
    auto dataStorage =  GetDistributedDataStorage();
    auto distributedBundleInfo = MockDistributedBundleInfo(BUNDULE_NAME_FIRST);
    bool result = dataStorage->SaveStorageDistributeInfo(distributedBundleInfo);
    EXPECT_TRUE(result);

    DistributedKv::DistributedKvDataManager dataManager;
    DeviceInfo deviceInfo;
    dataManager.GetLocalDevice(deviceInfo);

    DistributedBundleInfo getInfo;
    result = dataStorage->QueryStroageDistributeInfo(BUNDULE_NAME_FIRST, USER_ID_INVALID, deviceInfo.deviceId, getInfo);
    EXPECT_TRUE(result);

    EXPECT_EQ(getInfo.bundleUserInfos.size(), 0);

    result = dataStorage->DeleteStorageDistributeInfo(BUNDULE_NAME_FIRST);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: QueryStroageDistributeInfo_0400
 * @tc.name: test bundle DistributedBundleInfo can be query
 * @tc.desc: 1.save DistributedBundleInfo
 *           2.can not query DistributedBundleInfo with invalid BundleName
 *
 */
HWTEST_F(BmsDistributedDataStorageTest, QueryStroageDistributeInfo_0400, Function | SmallTest | Level0)
{
    auto dataStorage =  GetDistributedDataStorage();
    auto distributedBundleInfo = MockDistributedBundleInfo(BUNDULE_NAME_FIRST);
    bool result = dataStorage->SaveStorageDistributeInfo(distributedBundleInfo);
    EXPECT_TRUE(result);

    DistributedKv::DistributedKvDataManager dataManager;
    DeviceInfo deviceInfo;
    dataManager.GetLocalDevice(deviceInfo);

    DistributedBundleInfo getInfo;
    result = dataStorage->QueryStroageDistributeInfo(BUNDULE_NAME_SECOND, USER_ID, deviceInfo.deviceId, getInfo);
    EXPECT_FALSE(result);

    result = dataStorage->DeleteStorageDistributeInfo(BUNDULE_NAME_FIRST);
    EXPECT_TRUE(result);
}

/**
 * @tc.number: QueryStroageDistributeInfo_0500
 * @tc.name: test bundle DistributedBundleInfo can be query
 * @tc.desc: 1.save DistributedBundleInfo
 *           2.can not query DistributedBundleInfo with invalid NetworkId
 *
 */
HWTEST_F(BmsDistributedDataStorageTest, QueryStroageDistributeInfo_0500, Function | SmallTest | Level0)
{
    auto dataStorage =  GetDistributedDataStorage();
    auto distributedBundleInfo = MockDistributedBundleInfo(BUNDULE_NAME_FIRST);
    bool result = dataStorage->SaveStorageDistributeInfo(distributedBundleInfo);
    EXPECT_TRUE(result);

    DistributedBundleInfo getInfo;
    result = dataStorage->QueryStroageDistributeInfo(BUNDULE_NAME_SECOND, USER_ID, NETWORK_ID_INVALID, getInfo);
    EXPECT_FALSE(result);

    result = dataStorage->DeleteStorageDistributeInfo(BUNDULE_NAME_FIRST);
    EXPECT_TRUE(result);
}