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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_APPEXECFWK_ERRORS_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_APPEXECFWK_ERRORS_H

#include "errors.h"

namespace OHOS {
enum {
    APPEXECFWK_MODULE_COMMON = 0x00,
    APPEXECFWK_MODULE_APPMGR = 0x01,
    APPEXECFWK_MODULE_BUNDLEMGR = 0x02,
    APPEXECFWK_MODULE_FORMMGR = 0x03,
    APPEXECFWK_MODULE_APPEXECFWK = 0x08,
    // Reserved 0x03 ~ 0x0f for new modules, Event related modules start from 0x10
    APPEXECFWK_MODULE_EVENTMGR = 0x10
};

// Error code for Common
constexpr ErrCode APPEXECFWK_COMMON_ERR_OFFSET = ErrCodeOffset(SUBSYS_APPEXECFWK, APPEXECFWK_MODULE_COMMON);
enum {
    ERR_APPEXECFWK_SERVICE_NOT_READY = APPEXECFWK_COMMON_ERR_OFFSET + 1,
    ERR_APPEXECFWK_SERVICE_NOT_CONNECTED,
    ERR_APPEXECFWK_INVALID_UID,
    ERR_APPEXECFWK_INVALID_PID,
    ERR_APPEXECFWK_PARCEL_ERROR,
};

// Error code for AppMgr
constexpr ErrCode APPEXECFWK_APPMGR_ERR_OFFSET = ErrCodeOffset(SUBSYS_APPEXECFWK, APPEXECFWK_MODULE_APPMGR);
enum {
    ERR_APPEXECFWK_ASSEMBLE_START_MSG_FAILED = APPEXECFWK_APPMGR_ERR_OFFSET + 1,
    ERR_APPEXECFWK_CONNECT_APPSPAWN_FAILED,
    ERR_APPEXECFWK_BAD_APPSPAWN_CLIENT,
    ERR_APPEXECFWK_BAD_APPSPAWN_SOCKET,
    ERR_APPEXECFWK_SOCKET_READ_FAILED,
    ERR_APPEXECFWK_SOCKET_WRITE_FAILED
};

// Error code for BundleMgr
constexpr ErrCode APPEXECFWK_BUNDLEMGR_ERR_OFFSET = ErrCodeOffset(SUBSYS_APPEXECFWK, APPEXECFWK_MODULE_BUNDLEMGR);
enum {
    // the install error code from 0x0001 ~ 0x0020.
    ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR = APPEXECFWK_BUNDLEMGR_ERR_OFFSET + 0x0001,
    ERR_APPEXECFWK_INSTALL_HOST_INSTALLER_FAILED,
    ERR_APPEXECFWK_INSTALL_PARSE_FAILED,
    ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE,
    ERR_APPEXECFWK_INSTALL_VERIFICATION_FAILED,
    ERR_APPEXECFWK_INSTALL_NO_SIGNATURE_INFO,
    ERR_APPEXECFWK_INSTALL_UPDATE_INCOMPATIBLE,
    ERR_APPEXECFWK_INSTALL_PARAM_ERROR,
    ERR_APPEXECFWK_INSTALL_PERMISSION_DENIED,
    ERR_APPEXECFWK_INSTALL_ENTRY_ALREADY_EXIST,
    ERR_APPEXECFWK_INSTALL_STATE_ERROR,
    ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID,
    ERR_APPEXECFWK_INSTALL_INVALID_HAP_NAME,
    ERR_APPEXECFWK_INSTALL_INVALID_BUNDLE_FILE,
    ERR_APPEXECFWK_INSTALL_INVALID_HAP_SIZE,
    ERR_APPEXECFWK_INSTALL_GENERATE_UID_ERROR,
    ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR,
    ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR,
    ERR_APPEXECFWK_INSTALL_ALREADY_EXIST,

    ERR_APPEXECFWK_PARSE_UNEXPECTED = APPEXECFWK_BUNDLEMGR_ERR_OFFSET + 0x0021,
    ERR_APPEXECFWK_PARSE_MISSING_BUNDLE,
    ERR_APPEXECFWK_PARSE_MISSING_ABILITY,
    ERR_APPEXECFWK_PARSE_NO_PROFILE,
    ERR_APPEXECFWK_PARSE_BAD_PROFILE,
    ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR,
    ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP,
    ERR_APPEXECFWK_PARSE_PERMISSION_ERROR,
    ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR,

    ERR_APPEXECFWK_INSTALLD_PARAM_ERROR,
    ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR,
    ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED,
    ERR_APPEXECFWK_INSTALLD_CREATE_DIR_EXIST,
    ERR_APPEXECFWK_INSTALLD_CHOWN_FAILED,
    ERR_APPEXECFWK_INSTALLD_REMOVE_DIR_FAILED,
    ERR_APPEXECFWK_INSTALLD_EXTRACT_FILES_FAILED,
    ERR_APPEXECFWK_INSTALLD_RNAME_DIR_FAILED,
    ERR_APPEXECFWK_INSTALLD_CLEAN_DIR_FAILED,

    ERR_APPEXECFWK_UNINSTALL_SYSTEM_APP_ERROR,
    ERR_APPEXECFWK_UNINSTALL_KILLING_APP_ERROR,
    ERR_APPEXECFWK_UNINSTALL_INVALID_NAME,
    ERR_APPEXECFWK_UNINSTALL_PARAM_ERROR,
    ERR_APPEXECFWK_UNINSTALL_PERMISSION_DENIED,
    ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR,
    ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE,
    ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_MODULE
};

// Error code for FormMgr
constexpr ErrCode APPEXECFWK_FORMMGR_ERR_OFFSET = ErrCodeOffset(SUBSYS_APPEXECFWK, APPEXECFWK_MODULE_FORMMGR);
enum {
    ERR_APPEXECFWK_FORM_COMMON_CODE = APPEXECFWK_FORMMGR_ERR_OFFSET + 1,
    ERR_APPEXECFWK_FORM_PERMISSION_DENY,    
    ERR_APPEXECFWK_FORM_GET_INFO_FAILED,
    ERR_APPEXECFWK_FORM_GET_BUNDLE_FAILED,
    ERR_APPEXECFWK_FORM_INVALID_PARAM,
    ERR_APPEXECFWK_FORM_CFG_NOT_MATCH_ID,
    ERR_APPEXECFWK_FORM_NOT_EXIST_ID,
    ERR_APPEXECFWK_FORM_BIND_PROVIDER_FAILED,
    ERR_APPEXECFWK_FORM_MAX_SYSTEM_FORMS,
    ERR_APPEXECFWK_FORM_EXCEED_INSTANCES_PER_FORM,
    ERR_APPEXECFWK_FORM_OPERATION_NOT_SELF,
    ERR_APPEXECFWK_FORM_PROVIDER_DEL_FAIL,
    ERR_APPEXECFWK_FORM_MAX_FORMS_PER_CLIENT,
    ERR_APPEXECFWK_FORM_MAX_SYSTEM_TEMP_FORMS,
    ERR_APPEXECFWK_FORM_NO_SUCH_MODULE,
    ERR_APPEXECFWK_FORM_NO_SUCH_ABILITY,
    ERR_APPEXECFWK_FORM_NO_SUCH_DIMENSION,
    ERR_APPEXECFWK_FORM_FA_NOT_INSTALLED,
    ERR_APPEXECFWK_FORM_MAX_REQUEST,
    ERR_APPEXECFWK_FORM_MAX_REFRESH,
    ERR_APPEXECFWK_FORM_GET_BMS_FAILED,

    // error code in sdk 
    ERR_APPEXECFWK_FORM_GET_FMS_FAILED,
    ERR_APPEXECFWK_FORM_SEND_FMS_MSG,    
    ERR_APPEXECFWK_FORM_FORM_DUPLICATE_ADDED,
    ERR_APPEXECFWK_FORM_IN_RECOVER,
    ERR_APPEXECFWK_FORM_GET_SYSMGR_FAILED
};
constexpr ErrCode APPEXECFWK_APPEXECFWK_ERR_OFFSET = ErrCodeOffset(SUBSYS_APPEXECFWK, APPEXECFWK_MODULE_APPEXECFWK);
enum {
    ERR_APPEXECFWK_CHECK_FAILED = APPEXECFWK_APPEXECFWK_ERR_OFFSET + 1,
    ERR_APPEXECFWK_INTERCEPT_TASK_EXECUTE_SUCCESS
};

}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_APPEXECFWK_ERRORS_H
