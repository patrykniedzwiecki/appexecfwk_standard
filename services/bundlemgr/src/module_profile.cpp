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

#include "module_profile.h"

#include <sstream>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "common_profile.h"

namespace OHOS {
namespace AppExecFwk {
namespace Profile {
thread_local int32_t parseResult;
thread_local bool g_hasConfigureRemovable = false;

const std::map<std::string, DisplayOrientation> DISPLAY_ORIENTATION_MAP = {
    {"unspecified", DisplayOrientation::UNSPECIFIED},
    {"landscape", DisplayOrientation::LANDSCAPE},
    {"portrait", DisplayOrientation::PORTRAIT},
    {"followrecent", DisplayOrientation::FOLLOWRECENT}
};

const std::set<std::string> MODULE_TYPE_SET = {
    "entry",
    "feature",
    "har"
};

const std::set<std::string> DEVICE_TYPE_SET = {
    "phone",
    "tablet",
    "tv",
    "wearable",
    "liteWearable",
    "car",
    "smartVision",
    "router"
};

const std::set<std::string> VIRTUAL_MACHINE_SET = {
    "ark",
    "default"
};

const std::set<std::string> UI_SYNTAX_SET = {
    "hml",
    "ets"
};

const std::set<std::string> CONFIG_CHANGES_SET = {
    "density",
    "fontSize",
    "layout",
    "locale",
    "mcc",
    "mnc",
    "orientation",
    "size",
    "smallestSize",
    "colorMode"
};

const std::map<std::string, uint32_t> BACKGROUND_MODES_MAP = {
    {ProfileReader::KEY_DATA_TRANSFER, ProfileReader::VALUE_DATA_TRANSFER},
    {ProfileReader::KEY_AUDIO_PLAYBACK, ProfileReader::VALUE_AUDIO_PLAYBACK},
    {ProfileReader::KEY_AUDIO_RECORDING, ProfileReader::VALUE_AUDIO_RECORDING},
    {ProfileReader::KEY_LOCATION, ProfileReader::VALUE_LOCATION},
    {ProfileReader::KEY_BLUETOOTH_INTERACTION, ProfileReader::VALUE_BLUETOOTH_INTERACTION},
    {ProfileReader::KEY_MULTI_DEVICE_CONNECTION, ProfileReader::VALUE_MULTI_DEVICE_CONNECTION},
    {ProfileReader::KEY_WIFI_INTERACTION, ProfileReader::VALUE_WIFI_INTERACTION},
    {ProfileReader::KEY_VOIP, ProfileReader::VALUE_VOIP},
    {ProfileReader::KEY_TASK_KEEPING, ProfileReader::VALUE_TASK_KEEPING}
};

const std::set<std::string> EXTENSION_TYPE_SET = {
    "form",
    "workScheduler",
    "inputMethod",
    "service",
    "accessibility",
    "dataShare",
    "fileShare",
    "staticSubscriber",
    "wallpaper"
};

struct DeviceConfig {
    // pair first : if exist in module.json then true, otherwise false
    // pair second : actual value
    std::pair<bool, int32_t> apiCompatibleVersion = std::make_pair<>(false, 0);
    std::pair<bool, bool> distributedNotificationEnabled = std::make_pair<>(false, false);
    std::pair<bool, bool> keepAlive = std::make_pair<>(false, false);
    std::pair<bool, bool> removable = std::make_pair<>(false, true);
    std::pair<bool, bool> singleUser = std::make_pair<>(false, false);
    std::pair<bool, bool> clearUserData = std::make_pair<>(false, true);
};

struct Metadata {
    std::string name;
    std::string value;
    std::string resource;
};

struct Ability {
    std::string name;
    std::string srcEntrance;
    std::string description;
    int32_t descriptionId = 0;
    std::string icon;
    int32_t iconId = 0;
    std::string label;
    int32_t labelId = 0;
    std::vector<std::string> permissions;
    std::vector<Metadata> metadata;
    std::string orientation = ABILITY_ORIENTATION_DEFAULT_VALUE;
    bool visible = false;
    std::vector<Skill> skills;
    std::vector<std::string> configChanges;
    std::vector<std::string> backgroundModes;
};

struct Extension {
    std::string name;
    std::string srcEntrance;
    std::string icon;
    int32_t iconId = 0;
    std::string label;
    int32_t labelId = 0;
    std::string description;
    int32_t descriptionId = 0;
    std::string type;
    std::vector<std::string> permissions;
    bool visible = false;
    std::vector<Skill> skills;
    std::vector<Metadata> metadata;
};

struct App {
    std::string bundleName;
    bool debug = false;
    std::string icon;
    int32_t iconId = 0;
    std::string label;
    int32_t labelId = 0;
    std::string description;
    int32_t descriptionId = 0;
    std::string vendor;
    int32_t versionCode = 0;
    std::string versionName;
    int32_t minCompatibleVersionCode = -1;
    int32_t apiCompatibleVersion = -1;
    int32_t apiTargetVersion = -1;
    std::string apiReleaseType = APP_API_RELEASETYPE_DEFAULT_VALUE;
    bool distributedNotificationEnabled = false;
    std::string entityType = APP_ENTITY_TYPE_DEFAULT_VALUE;
    bool keepAlive = false;
    std::pair<bool, bool> removable = std::make_pair<>(false, true);
    bool singleUser = false;
    bool clearUserData = true;
    std::map<std::string, DeviceConfig> deviceConfigs;
};

struct Module {
    std::string name;
    std::string type;
    std::string srcEntrance;
    std::string description;
    int32_t descriptionId = 0;
    std::string process;
    std::string mainElement;
    std::vector<std::string> deviceTypes;
    bool deliveryWithInstall = true;
    bool installationFree = true;
    std::string virtualMachine;
    std::string uiSyntax;
    std::string pages;
    std::vector<Metadata> metadata;
    std::vector<Ability> abilities;
    std::vector<Extension> extensionAbilities;
    std::vector<RequestPermission> requestPermissions;
    std::vector<DefinePermission> definePermissions;
};

struct ModuleJson {
    App app;
    Module module;
};

void from_json(const nlohmann::json &jsonObject, Metadata &metadata)
{
    APP_LOGD("read metadata tag from module.json");
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        META_DATA_NAME,
        metadata.name,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        META_DATA_VALUE,
        metadata.value,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        META_DATA_RESOURCE,
        metadata.resource,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Ability &ability)
{
    APP_LOGD("read ability tag from module.json");
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ABILITY_NAME,
        ability.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        SRC_ENTRANCE,
        ability.srcEntrance,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION,
        ability.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION_ID,
        ability.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ICON,
        ability.icon,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ICON_ID,
        ability.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        LABEL,
        ability.label,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        LABEL_ID,
        ability.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        PERMISSIONS,
        ability.permissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<Metadata>>(jsonObject,
        jsonObjectEnd,
        META_DATA,
        ability.metadata,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ABILITY_ORIENTATION,
        ability.orientation,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        VISIBLE,
        ability.visible,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Skill>>(jsonObject,
        jsonObjectEnd,
        SKILLS,
        ability.skills,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        ABILITY_CONFIGCHANGES,
        ability.configChanges,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        ABILITY_BACKGROUNDMODES,
        ability.backgroundModes,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
}

void from_json(const nlohmann::json &jsonObject, Extension &extension)
{
    APP_LOGD("read extension tag from module.json");
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        EXTENSION_ABILITY_NAME,
        extension.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        SRC_ENTRANCE,
        extension.srcEntrance,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ICON,
        extension.icon,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ICON_ID,
        extension.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        LABEL,
        extension.label,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        LABEL_ID,
        extension.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION,
        extension.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION_ID,
        extension.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        EXTENSION_ABILITY_TYPE,
        extension.type,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        PERMISSIONS,
        extension.permissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        VISIBLE,
        extension.visible,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Skill>>(jsonObject,
        jsonObjectEnd,
        SKILLS,
        extension.skills,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<Metadata>>(jsonObject,
        jsonObjectEnd,
        META_DATA,
        extension.metadata,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
}

void from_json(const nlohmann::json &jsonObject, DeviceConfig &deviceConfig)
{
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(DEVICE_CONFIG_API_COMPATIBLE_VERSION) != jsonObjectEnd) {
        deviceConfig.apiCompatibleVersion.first = true;
        GetValueIfFindKey<int32_t>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_API_COMPATIBLE_VERSION,
            deviceConfig.apiCompatibleVersion.second,
            JsonType::NUMBER,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    if (jsonObject.find(DEVICE_CONFIG_DISTRIBUTED_NOTIFICATION_ENABLED) != jsonObjectEnd) {
        deviceConfig.distributedNotificationEnabled.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_DISTRIBUTED_NOTIFICATION_ENABLED,
            deviceConfig.distributedNotificationEnabled.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    if (jsonObject.find(DEVICE_CONFIG_KEEP_ALIVE) != jsonObjectEnd) {
        deviceConfig.keepAlive.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_KEEP_ALIVE,
            deviceConfig.keepAlive.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    if (jsonObject.find(DEVICE_CONFIG_REMOVABLE) != jsonObjectEnd) {
        deviceConfig.removable.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_REMOVABLE,
            deviceConfig.removable.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    if (jsonObject.find(DEVICE_CONFIG_SINGLE_USER) != jsonObjectEnd) {
        deviceConfig.singleUser.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_SINGLE_USER,
            deviceConfig.singleUser.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    if (jsonObject.find(DEVICE_CONFIG_CLEAR_USER_DATA) != jsonObjectEnd) {
        deviceConfig.clearUserData.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_CLEAR_USER_DATA,
            deviceConfig.clearUserData.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
}

void from_json(const nlohmann::json &jsonObject, App &app)
{
    APP_LOGD("read app tag from module.json");
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APP_BUNDLE_NAME,
        app.bundleName,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APP_DEBUG,
        app.debug,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ICON,
        app.icon,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ICON_ID,
        app.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        LABEL,
        app.label,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        LABEL_ID,
        app.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION,
        app.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION_ID,
        app.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APP_VENDOR,
        app.vendor,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        APP_VERSION_CODE,
        app.versionCode,
        JsonType::NUMBER,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APP_VERSION_NAME,
        app.versionName,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        APP_MIN_COMPATIBLE_VERSION_CODE,
        app.minCompatibleVersionCode,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        APP_API_COMPATIBLE_VERSION,
        app.apiCompatibleVersion,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        APP_API_TARGET_VERSION,
        app.apiTargetVersion,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APP_API_RELEASETYPE,
        app.apiReleaseType,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APP_DISTRIBUTED_NOTIFICATION_ENABLED,
        app.distributedNotificationEnabled,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APP_ENTITY_TYPE,
        app.entityType,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APP_KEEP_ALIVE,
        app.keepAlive,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (jsonObject.find(APP_REMOVABLE) != jsonObject.end()) {
        app.removable.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            APP_REMOVABLE,
            app.removable.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APP_SINGLE_USER,
        app.singleUser,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APP_CLEAR_USER_DATA,
        app.clearUserData,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (jsonObject.find(APP_PHONE) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_PHONE,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_PHONE] = deviceConfig;
    }
    if (jsonObject.find(APP_TABLET) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_TABLET,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_TABLET] = deviceConfig;
    }
    if (jsonObject.find(APP_TV) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_TV,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_TV] = deviceConfig;
    }
    if (jsonObject.find(APP_WEARABLE) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_WEARABLE,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_WEARABLE] = deviceConfig;
    }
    if (jsonObject.find(APP_LITE_WEARABLE) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_LITE_WEARABLE,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_LITE_WEARABLE] = deviceConfig;
    }
    if (jsonObject.find(APP_CAR) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_CAR,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_CAR] = deviceConfig;
    }
    if (jsonObject.find(APP_SMART_VISION) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_SMART_VISION,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_SMART_VISION] = deviceConfig;
    }
    if (jsonObject.find(APP_ROUTER) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_ROUTER,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_ROUTER] = deviceConfig;
    }
}

void from_json(const nlohmann::json &jsonObject, Module &module)
{
    APP_LOGD("read module tag from module.json");
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_NAME,
        module.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_TYPE,
        module.type,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        SRC_ENTRANCE,
        module.srcEntrance,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION,
        module.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION_ID,
        module.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_PROCESS,
        module.process,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_MAIN_ELEMENT,
        module.mainElement,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        MODULE_DEVICE_TYPES,
        module.deviceTypes,
        JsonType::ARRAY,
        true,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        MODULE_DELIVERY_WITH_INSTALL,
        module.deliveryWithInstall,
        JsonType::BOOLEAN,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        MODULE_INSTALLATION_FREE,
        module.installationFree,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_VIRTUAL_MACHINE,
        module.virtualMachine,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_UI_SYNTAX,
        module.uiSyntax,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_PAGES,
        module.pages,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Metadata>>(jsonObject,
        jsonObjectEnd,
        META_DATA,
        module.metadata,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<Ability>>(jsonObject,
        jsonObjectEnd,
        MODULE_ABILITIES,
        module.abilities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<Extension>>(jsonObject,
        jsonObjectEnd,
        MODULE_EXTENSION_ABILITIES,
        module.extensionAbilities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<RequestPermission>>(jsonObject,
        jsonObjectEnd,
        MODULE_REQUEST_PERMISSIONS,
        module.requestPermissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<DefinePermission>>(jsonObject,
        jsonObjectEnd,
        MODULE_DEFINE_PERMISSIONS,
        module.definePermissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
}

void from_json(const nlohmann::json &jsonObject, ModuleJson &moduleJson)
{
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<App>(jsonObject,
        jsonObjectEnd,
        APP,
        moduleJson.app,
        JsonType::OBJECT,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Module>(jsonObject,
        jsonObjectEnd,
        MODULE,
        moduleJson.module,
        JsonType::OBJECT,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
}
}  // namespace Profile

namespace {
void GetMetadata(std::vector<Metadata> &metadata, const std::vector<Profile::Metadata> &profileMetadata)
{
    for (const Profile::Metadata &item : profileMetadata) {
        Metadata tmpMetadata;
        tmpMetadata.name = item.name;
        tmpMetadata.value = item.value;
        tmpMetadata.resource = item.resource;
        metadata.emplace_back(tmpMetadata);
    }
}

bool CheckBundleNameIsValid(const std::string &bundleName)
{
    if (bundleName.empty()) {
        return false;
    }
    if (bundleName.size() < Constants::MIN_BUNDLE_NAME || bundleName.size() > Constants::MAX_BUNDLE_NAME) {
        return false;
    }
    char head = bundleName.at(0);
    if (head < 'A' || (head > 'Z' && head < 'a') || head > 'z') {
        return false;
    }
    for (const auto &c : bundleName) {
        if (c < '.' || c == '/' || (c > '9' && c < 'A') || (c > 'Z' && c < '_') || c == '`' || c > 'z') {
            return false;
        }
    }
    return true;
}

bool ToApplicationInfo(const Profile::ModuleJson &moduleJson, ApplicationInfo &applicationInfo)
{
    APP_LOGD("transform ModuleJson to ApplicationInfo");
    applicationInfo.name = moduleJson.app.bundleName;
    applicationInfo.bundleName = moduleJson.app.bundleName;
    applicationInfo.debug = moduleJson.app.debug;
    applicationInfo.icon = moduleJson.app.icon;
    applicationInfo.iconPath = moduleJson.app.icon;
    applicationInfo.iconId = moduleJson.app.iconId;
    applicationInfo.label = moduleJson.app.label;
    applicationInfo.labelId = moduleJson.app.labelId;
    applicationInfo.description = moduleJson.app.description;
    applicationInfo.descriptionId = moduleJson.app.descriptionId;
    applicationInfo.vendor = moduleJson.app.vendor;
    applicationInfo.versionCode = moduleJson.app.versionCode;
    applicationInfo.versionName = moduleJson.app.versionName;
    if (moduleJson.app.minCompatibleVersionCode != -1) {
        applicationInfo.minCompatibleVersionCode = moduleJson.app.minCompatibleVersionCode;
    } else {
        // default equal to versionCode
        applicationInfo.minCompatibleVersionCode = moduleJson.app.versionCode;
    }
    if (moduleJson.app.apiCompatibleVersion != -1) {
        applicationInfo.apiCompatibleVersion = moduleJson.app.apiCompatibleVersion;
    } else {
        // default equal to current device API version
        applicationInfo.apiCompatibleVersion = 0; // to do, get device API
    }
    if (moduleJson.app.apiTargetVersion != -1) {
        applicationInfo.apiTargetVersion = moduleJson.app.apiTargetVersion;
    } else {
        // default equal to current device API version
        applicationInfo.apiTargetVersion = 0; // to do, get device API
    }
    applicationInfo.apiReleaseType = moduleJson.app.apiReleaseType;
    applicationInfo.distributedNotificationEnabled = moduleJson.app.distributedNotificationEnabled;
    applicationInfo.entityType = moduleJson.app.entityType;
    applicationInfo.deviceId = Constants::CURRENT_DEVICE_ID; // to do

    if (applicationInfo.isSystemApp) {
        applicationInfo.keepAlive = moduleJson.app.keepAlive;
        if (moduleJson.app.removable.first) {
            Profile::g_hasConfigureRemovable = true;
            applicationInfo.removable = moduleJson.app.removable.second;
        }
        applicationInfo.singleUser = moduleJson.app.singleUser;
        applicationInfo.clearUserData = moduleJson.app.clearUserData;
    }

    // device adapt
    std::string deviceType = "phone";
    if (moduleJson.app.deviceConfigs.find(deviceType) != moduleJson.app.deviceConfigs.end()) {
        Profile::DeviceConfig deviceConfig = moduleJson.app.deviceConfigs.at(deviceType);
        if (deviceConfig.apiCompatibleVersion.first) {
            applicationInfo.apiCompatibleVersion = deviceConfig.apiCompatibleVersion.second;
        }
        if (deviceConfig.distributedNotificationEnabled.first) {
            applicationInfo.distributedNotificationEnabled = deviceConfig.distributedNotificationEnabled.second;
        }
        if (applicationInfo.isSystemApp) {
            if (deviceConfig.keepAlive.first) {
                applicationInfo.keepAlive = deviceConfig.keepAlive.second;
            }
            if (deviceConfig.removable.first) {
                Profile::g_hasConfigureRemovable = true;
                applicationInfo.removable = deviceConfig.removable.second;
            }
            if (deviceConfig.singleUser.first) {
                applicationInfo.singleUser = deviceConfig.singleUser.second;
            }
            if (deviceConfig.clearUserData.first) {
                applicationInfo.clearUserData = deviceConfig.clearUserData.second;
            }
        }
    }

    applicationInfo.enabled = true;
    return true;
}

bool ToBundleInfo(const ApplicationInfo &applicationInfo, BundleInfo &bundleInfo)
{
    bundleInfo.name = applicationInfo.bundleName;
    bundleInfo.vendor = applicationInfo.vendor;
    bundleInfo.versionCode = static_cast<uint32_t>(applicationInfo.versionCode);
    bundleInfo.versionName = applicationInfo.versionName;
    bundleInfo.compatibleVersion = static_cast<uint32_t>(applicationInfo.apiCompatibleVersion);
    bundleInfo.targetVersion = static_cast<uint32_t>(applicationInfo.apiTargetVersion);
    bundleInfo.releaseType = applicationInfo.apiReleaseType;
    bundleInfo.isKeepAlive = applicationInfo.keepAlive;
    bundleInfo.label = applicationInfo.label;
    bundleInfo.description = applicationInfo.description;
    return true;
}

uint32_t GetBackgroundModes(const std::vector<std::string> &backgroundModes)
{
    uint32_t backgroundMode = 0;
    for (const std::string &item : backgroundModes) {
        if (Profile::BACKGROUND_MODES_MAP.find(item) != Profile::BACKGROUND_MODES_MAP.end()) {
            backgroundMode |= Profile::BACKGROUND_MODES_MAP.at(item);
        }
    }
    return backgroundMode;
}

bool ToAbilityInfo(const Profile::ModuleJson &moduleJson, const Profile::Ability &ability, AbilityInfo &abilityInfo)
{
    APP_LOGD("transform ModuleJson to AbilityInfo");
    abilityInfo.name = ability.name;
    abilityInfo.srcEntrance = ability.srcEntrance;
    abilityInfo.description = ability.description;
    abilityInfo.descriptionId = ability.descriptionId;
    abilityInfo.iconPath = ability.icon;
    abilityInfo.iconId = ability.iconId;
    abilityInfo.label = ability.label;
    abilityInfo.labelId = ability.labelId;
    abilityInfo.permissions = ability.permissions;

    auto iterOrientation = std::find_if(std::begin(Profile::DISPLAY_ORIENTATION_MAP),
        std::end(Profile::DISPLAY_ORIENTATION_MAP),
        [&ability](const auto &item) { return item.first == ability.orientation; });
    if (iterOrientation != Profile::DISPLAY_ORIENTATION_MAP.end()) {
        abilityInfo.orientation = iterOrientation->second;
    }
    abilityInfo.visible = ability.visible;
    for (const std::string &configChange : ability.configChanges) {
        if (Profile::CONFIG_CHANGES_SET.find(configChange) != Profile::CONFIG_CHANGES_SET.end()) {
            abilityInfo.configChanges.emplace_back(configChange);
        }
    }
    abilityInfo.backgroundModes = GetBackgroundModes(ability.backgroundModes);
    GetMetadata(abilityInfo.metadata, ability.metadata);
    abilityInfo.package = moduleJson.module.name;
    abilityInfo.bundleName = moduleJson.app.bundleName;
    abilityInfo.moduleName = moduleJson.module.name;
    abilityInfo.applicationName = moduleJson.app.bundleName;
    return true;
}

bool ToExtensionInfo(const Profile::ModuleJson &moduleJson,
    const Profile::Extension &extension, ExtensionInfo &extensionInfo)
{
    APP_LOGD("transform ModuleJson to ExtensionInfo");
    extensionInfo.name = extension.name;
    extensionInfo.srcEntrance = extension.srcEntrance;
    extensionInfo.icon = extension.icon;
    extensionInfo.iconId = extension.iconId;
    extensionInfo.label = extension.label;
    extensionInfo.labelId = extension.labelId;
    extensionInfo.description = extension.description;
    extensionInfo.descriptionId = extension.descriptionId;
    if (Profile::EXTENSION_TYPE_SET.find(extension.type) != Profile::EXTENSION_TYPE_SET.end()) {
        extensionInfo.type = extension.type;
    }
    extensionInfo.permissions = extension.permissions;
    extensionInfo.visible = extension.visible;
    GetMetadata(extensionInfo.metadata, extension.metadata);
    extensionInfo.bundleName = moduleJson.app.bundleName;
    extensionInfo.moduleName = moduleJson.module.name;
    return true;
}

bool ToInnerModuleInfo(const Profile::ModuleJson &moduleJson, InnerModuleInfo &innerModuleInfo)
{
    APP_LOGD("transform ModuleJson to InnerModuleInfo");
    innerModuleInfo.modulePackage = moduleJson.module.name;
    innerModuleInfo.moduleName = moduleJson.module.name;
    innerModuleInfo.description = moduleJson.module.description;
    innerModuleInfo.descriptionId = moduleJson.module.descriptionId;
    GetMetadata(innerModuleInfo.metadata, moduleJson.module.metadata);
    innerModuleInfo.distro.deliveryWithInstall = moduleJson.module.deliveryWithInstall;
    innerModuleInfo.distro.installationFree = moduleJson.module.installationFree;
    innerModuleInfo.distro.moduleName = moduleJson.module.name;
    if (Profile::MODULE_TYPE_SET.find(moduleJson.module.type) != Profile::MODULE_TYPE_SET.end()) {
        innerModuleInfo.distro.moduleType = moduleJson.module.type;
    }
    // to do, verify and systemApp
    innerModuleInfo.definePermissions = moduleJson.module.definePermissions;
    // to do, verify
    innerModuleInfo.requestPermissions = moduleJson.module.requestPermissions;
    innerModuleInfo.mainAbility = moduleJson.module.mainElement;
    innerModuleInfo.srcPath = moduleJson.module.srcEntrance;
    if (!moduleJson.module.process.empty()) {
        innerModuleInfo.process = moduleJson.module.process;
    } else {
        std::string processName;
        processName.append(moduleJson.app.bundleName).append(".").append(moduleJson.module.name);
        innerModuleInfo.process = processName;
    }
    for (const std::string &deviceType : moduleJson.module.deviceTypes) {
        if (Profile::DEVICE_TYPE_SET.find(deviceType) != Profile::DEVICE_TYPE_SET.end()) {
            innerModuleInfo.deviceTypes.emplace_back(deviceType);
        }
    }
    if (Profile::VIRTUAL_MACHINE_SET.find(moduleJson.module.virtualMachine) != Profile::VIRTUAL_MACHINE_SET.end()) {
        innerModuleInfo.virtualMachine = moduleJson.module.virtualMachine;
    }
    if (Profile::UI_SYNTAX_SET.find(moduleJson.module.uiSyntax) != Profile::UI_SYNTAX_SET.end()) {
        innerModuleInfo.uiSyntax = moduleJson.module.uiSyntax;
    }
    innerModuleInfo.pages = moduleJson.module.pages;
    // abilities and extensionAbilities store in InnerBundleInfo
    return true;
}

bool ToInnerBundleInfo(const Profile::ModuleJson &moduleJson, InnerBundleInfo &innerBundleInfo)
{
    APP_LOGD("transform ModuleJson to InnerBundleInfo");
    if (!CheckBundleNameIsValid(moduleJson.app.bundleName)) {
        APP_LOGE("bundle name is invalid");
        return false;
    }

    ApplicationInfo applicationInfo;
    applicationInfo.isSystemApp = innerBundleInfo.GetAppType() == Constants::AppType::SYSTEM_APP; // modify GetAppType
    ToApplicationInfo(moduleJson, applicationInfo);

    innerBundleInfo.SetHasConfigureRemovable(Profile::g_hasConfigureRemovable);
    Profile::g_hasConfigureRemovable = false;

    BundleInfo bundleInfo;
    ToBundleInfo(applicationInfo, bundleInfo);
    bundleInfo.applicationInfo.removable = applicationInfo.removable;

    InnerModuleInfo innerModuleInfo;
    ToInnerModuleInfo(moduleJson, innerModuleInfo);

    // handle abilities
    bool findEntry = false;
    for (const Profile::Ability &ability : moduleJson.module.abilities) {
        AbilityInfo abilityInfo;
        if (!ToAbilityInfo(moduleJson, ability, abilityInfo)) {
            APP_LOGE("ToAbilityInfo error");
            return false;
        }
        std::string key;
        key.append(moduleJson.app.bundleName).append(".")
            .append(moduleJson.module.name).append(".").append(abilityInfo.name);
        innerModuleInfo.abilityKeys.emplace_back(key);
        innerModuleInfo.skillKeys.emplace_back(key);
        innerBundleInfo.InsertSkillInfo(key, ability.skills);
        innerBundleInfo.InsertAbilitiesInfo(key, abilityInfo);
        if (findEntry) {
            continue;
        }
        // get entry ability
        for (const auto &skill : ability.skills) {
            bool isEntryAction = std::find(skill.actions.begin(), skill.actions.end(),
                Constants::INTENT_ACTION_HOME) != skill.actions.end();
            bool isEntryEntity = std::find(skill.entities.begin(), skill.entities.end(),
                Constants::INTENT_ENTITY_HOME) != skill.entities.end();
            if (isEntryAction && isEntryEntity) {
                innerBundleInfo.SetMainAbility(key);
                innerBundleInfo.SetMainAbilityName(ability.name);
                innerModuleInfo.label = ability.label;
                innerModuleInfo.labelId = ability.labelId;
                // get launcher application and ability
                bool isLauncherEntity = std::find(skill.entities.begin(), skill.entities.end(),
                    Constants::FLAG_HOME_INTENT_FROM_SYSTEM) != skill.entities.end();
                if (isLauncherEntity) {
                    applicationInfo.isLauncherApp = true;
                    abilityInfo.isLauncherAbility = true;
                }
                findEntry = true;
                break;
            }
        }
    }

    // handle extensionAbilities
    for (const Profile::Extension &extension : moduleJson.module.extensionAbilities) {
        ExtensionInfo extensionInfo;
        if (!ToExtensionInfo(moduleJson, extension, extensionInfo)) {
            APP_LOGE("ToExtensionInfo error");
            return false;
        }
        std::string key;
        key.append(moduleJson.app.bundleName).append(".")
            .append(moduleJson.module.name).append(".").append(extension.name);
        innerModuleInfo.extensionKeys.emplace_back(key);
        innerModuleInfo.extensionSkillKeys.emplace_back(key);
        innerBundleInfo.InsertExtensionSkillInfo(key, extension.skills);
        innerBundleInfo.InsertExtensionInfo(key, extensionInfo);
    }

    if (moduleJson.module.type == Profile::MODULE_TYPE_ENTRY) {
        innerBundleInfo.SetHasEntry(true);
        innerModuleInfo.isEntry = true;
    }
    innerBundleInfo.SetCurrentModulePackage(moduleJson.module.name);
    innerBundleInfo.SetBaseApplicationInfo(applicationInfo);
    innerBundleInfo.SetBaseBundleInfo(bundleInfo);
    innerBundleInfo.InsertInnerModuleInfo(moduleJson.module.name, innerModuleInfo);
    return true;
}
}  // namespace

ErrCode ModuleProfile::TransformTo(const std::ostringstream &source, InnerBundleInfo &innerBundleInfo) const
{
    APP_LOGD("transform module.json stream to InnerBundleInfo");
    Profile::ModuleJson moduleJson;
    nlohmann::json jsonObject = nlohmann::json::parse(source.str(), nullptr, false);
    if (jsonObject.is_discarded()) {
        APP_LOGE("bad profile");
        return ERR_APPEXECFWK_PARSE_BAD_PROFILE;
    }
    moduleJson = jsonObject.get<Profile::ModuleJson>();
    if (Profile::parseResult != ERR_OK) {
        APP_LOGE("parseResult is %{public}d", Profile::parseResult);
        int32_t ret = Profile::parseResult;
        // need recover parse result to ERR_OK
        Profile::parseResult = ERR_OK;
        Profile::g_hasConfigureRemovable = false;
        return ret;
    }
    if (!ToInnerBundleInfo(moduleJson, innerBundleInfo)) {
        return ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR;
    }
    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS