#ifndef DRM_API_DEFINES_H
#define DRM_API_DEFINES_H

#include <windows.h>

#define DRM_API_VERSION_MAJOR 0
#define DRM_API_VERSION_MINOR 85

#define DRM_API_VERSION_STR "SecuROM PA GUI DLL, API Version 0.85"

// 0.83 adds DRM_API_DLL_MODE_EXPIRY
// 0.85 adds DRM_API_CMD_SET_ACTIVATION_SERVERS, DRM_API_CMD_GET_ACTIVATION_SERVERS, DRM_API_CMD_GET_SHOP_URL and DRM_API_CMD_GET_ACTIVATION_MODE

#define PID_HEADER	0x01
#define PID_DATA	0x02

#pragma pack(push, 1)

typedef struct {
	unsigned short major;
	unsigned short minor;
} drm_api_version_t;

// PID commands
typedef enum
{
	DRM_API_CMD_UNDEFINE = 0,
	DRM_API_CMD_READ_PID,
	DRM_API_CMD_READ_PROJECT_ID,
	DRM_API_CMD_GET_PID_FUNCTIONS,
	// PA commands
	DRM_API_CMD_GET_UL_REQUEST,
	DRM_API_CMD_SET_UL_CODE,
	DRM_API_CMD_EVAL_UL_CODE,
	DRM_API_CMD_SET_SERIAL,
	DRM_API_CMD_GET_SERIAL,
	DRM_API_CMD_GET_EXPIRY_INFO,
	DRM_API_CMD_GET_GRACE_INFO,

	DRM_API_CMD_IS_ONLINE,
	DRM_API_CMD_ACTIVATE_ONLINE,
	DRM_API_CMD_VERIFY_ONLINE,
	DRM_API_CMD_IS_REGISTERED,
	DRM_API_CMD_REGISTER,
	DRM_API_CMD_ENABLE_LOG,
	DRM_API_CMD_GETLOGFILENAME,
	DRM_API_CMD_GET_SERIAL_FROM_EXE,
	DRM_API_CMD_UPDATE_PLAYTIME,
	DRM_API_CMD_IS_PA_LIC,
	DRM_API_CMD_EVAL_UL_CODE_NO_UPDATE,
	DRM_API_CMD_DEL_UD,
	DRM_API_CMD_GET_LIC_ID,
	DRM_API_CMD_INVALIDATE_GRACE,
	DRM_API_CMD_INVALIDATE_EXPIRY,
	DRM_API_CMD_GET_INIT_GRACE,
	DRM_API_CMD_GET_INIT_EXPIRY,
	DRM_API_CMD_GET_TIME_CORRUPT_INFO,
	DRM_API_CMD_SET_ACTIVATION_SERVERS,
	DRM_API_CMD_GET_ACTIVATION_SERVERS,
	DRM_API_CMD_GET_SHOP_URL,
	DRM_API_CMD_GET_ACTIVATION_MODE,
} DRM_API_COMMANDS;

// Securom_DRM_API_Callback return values:
// please also update the list of error strings below when adding new codes
typedef enum
{
	DRM_API_ERROR = 0,
	DRM_API_SUCCESS,
	DRM_API_NOT_IMPLEMENTED,
	DRM_API_INVALID_COMMAND,
	DRM_API_OUT_OF_MEMORY,
	DRM_API_BUFFER_TOO_SMALL,
	DRM_API_BUFFER_TOO_LARGE,
	DRM_API_UNLOCK_INVALID_FORMAT,
	DRM_API_UNLOCK_INVALID_CPA,
	DRM_API_UNLOCK_INVALID,
	DRM_API_UNLOCK_NOT_AVAILABLE,
	DRM_API_UNLOCK_EXPIRED,
	DRM_API_UNLOCK_REVOKED,
	DRM_API_NOT_ALLOWED,
	DRM_API_USERDATA_REQUEST_FAILED,
	DRM_API_WRONG_VERSION,
	DRM_API_TIMESTAMP_EXPIRED,
	DRM_API_ACTIVATION_FAILED,
	DRM_API_SSL_ERROR,
	DRM_API_CONNECTION_ERROR,
	DRM_API_UNLOCK_CONVERT_ERROR,
	DRM_API_XML_ERROR,
	DRM_API_HTTP_SEND_ERROR,
	DRM_API_HTTP_RECEIVE_ERROR,
	DRM_API_USER_CANCEL_ERROR,
	DRM_API_EVAL_FAILED,
	DRM_API_VERIFY_FAILED,
	DRM_API_USERDATA_COMMIT_FAILED,
	DRM_API_UNLOCK_INVALID_WITHIN_GRACE,
	DRM_API_UNLOCK_GRACE_PERIOD_ENDED,
	DRM_API_PARAM_INVALID,
	DRM_API_UNLOCK_EXPIRED_WITHIN_GRACE,
	DRM_API_UNLOCK_BLACKLISTED,
	DRM_API_UNLOCK_EMPTY,
	DRM_API_SERVER_ERROR_GLOBAL_VERIFY,
	DRM_API_SERVER_ERROR_PURCHASE,
	DRM_API_SERVER_ERROR_DO_VERIFY,
	DRM_API_SERVER_ERROR_CHECK_ACTIVATIONS,
	DRM_API_SERVER_ERROR_CREATE_ACTIVATION_ENTRY,
	DRM_API_SERVER_ERROR_UPDATE_STAT,
	DRM_API_SERVER_ERROR_REGISTRATION_REQUIRED,
	DRM_API_SERVER_ERROR_LICENSE_EXPIRED,
	DRM_API_SERVER_ERROR_CPA_NOT_ACTIVE,
	DRM_API_SERVER_ERROR_PURCHASE_NOT_FOUND,
	DRM_API_SERVER_ERROR_TOO_MANY_ACT_TIMEFRAME,
	DRM_API_SERVER_ERROR_TOO_MANY_ACT_TOTAL,
	DRM_API_SERVER_ERROR_LICENSE_NOT_FOUND,
	DRM_API_SERVER_ERROR_APPLICATION_NOT_FOUND,
	DRM_API_NO_GRACE_DEFINED,
	DRM_API_NO_EXPIRY_DEFINED,
	DRM_API_NO_LOGFILE_EXISTS,
	DRM_API_UNLOCK_VALID,
	DRM_API_TIME_CORRUPT,
	DRM_API_SERVER_ERROR_INTERNAL_ERROR,
	DRM_API_SERVER_ERROR_START_DATE_NOT_REACHED,
	DRM_API_SERVER_ERROR_TOO_MANY_ACT_SAME_HWID,
	DRM_API_SERVER_ERROR_TOO_MANY_ACT_DIFF_HWID,
	DRM_API_SERVER_ERROR_UNKNOWN,
} DRM_API_ret_t;


//rc compiler can not handle the following...
#ifndef RC_INVOKED

#define DRM_API_RET_STRING(RET) \
	(((RET) == DRM_API_ERROR) ? "an internal error occured" : \
	((RET) == DRM_API_SUCCESS) ? "success" : \
	((RET) == DRM_API_NOT_IMPLEMENTED) ? "API command not implemented" : \
	((RET) == DRM_API_INVALID_COMMAND) ? "API command is invalid " : \
	((RET) == DRM_API_OUT_OF_MEMORY) ? "out of memory" : \
	((RET) == DRM_API_BUFFER_TOO_SMALL) ? "a buffer is too small" : \
	((RET) == DRM_API_BUFFER_TOO_LARGE) ? "a buffer is too large" : \
	((RET) == DRM_API_UNLOCK_INVALID_FORMAT) ? "unlock code has invalid format" : \
	((RET) == DRM_API_UNLOCK_INVALID_CPA) ? "unlock code has invalid CPA" : \
	((RET) == DRM_API_UNLOCK_INVALID) ? "unlock code is invalid" : \
	((RET) == DRM_API_UNLOCK_NOT_AVAILABLE) ? "unlock code is not available" : \
	((RET) == DRM_API_UNLOCK_EXPIRED) ? "unlock code is expired" : \
	((RET) == DRM_API_NOT_ALLOWED) ? "API call is not allowed" : \
	((RET) == DRM_API_USERDATA_REQUEST_FAILED) ? "userdata request failed" : \
	((RET) == DRM_API_WRONG_VERSION) ? "wrong version, API update needed" : \
	((RET) == DRM_API_TIMESTAMP_EXPIRED) ? "timestamp expired" : \
	((RET) == DRM_API_ACTIVATION_FAILED) ? "activation failed" : \
	((RET) == DRM_API_SSL_ERROR) ? "a SSL error occured" : \
	((RET) == DRM_API_CONNECTION_ERROR) ? "a connection error occured" : \
	((RET) == DRM_API_UNLOCK_CONVERT_ERROR) ? "unlock convert error" : \
	((RET) == DRM_API_XML_ERROR) ? "a XML error occured" : \
	((RET) == DRM_API_HTTP_SEND_ERROR) ? "error during sending HTTP request" : \
	((RET) == DRM_API_HTTP_RECEIVE_ERROR) ? "error during receiving HTTP response" : \
	((RET) == DRM_API_USER_CANCEL_ERROR) ? "USER has canceled activation" : \
	((RET) == DRM_API_EVAL_FAILED) ? "evaluation of the unlock code failed" : \
	((RET) == DRM_API_VERIFY_FAILED) ? "verification of unlock code failed" : \
	((RET) == DRM_API_USERDATA_COMMIT_FAILED) ? "userdata commit failed" :\
	((RET) == DRM_API_UNLOCK_INVALID_WITHIN_GRACE) ? "unlock code is invalid but within grace period" :\
	((RET) == DRM_API_UNLOCK_GRACE_PERIOD_ENDED) ? "unlock code is invalid and grace period ended" :\
	((RET) == DRM_API_PARAM_INVALID) ? "invalid parameter passed to api" : \
	((RET) == DRM_API_UNLOCK_EXPIRED_WITHIN_GRACE) ? "unlock code is expired but within grace period" :\
	((RET) == DRM_API_UNLOCK_BLACKLISTED) ? "unlock code is blacklisted (already used)" :\
	((RET) == DRM_API_UNLOCK_EMPTY) ? "unlock code is empty (not set)" :\
	((RET) == DRM_API_SERVER_ERROR_GLOBAL_VERIFY) ? "server error - global verify error" :\
	((RET) == DRM_API_SERVER_ERROR_PURCHASE) ? "server error - purchase error" :\
	((RET) == DRM_API_SERVER_ERROR_DO_VERIFY) ? "server error - error during verification" :\
	((RET) == DRM_API_SERVER_ERROR_CHECK_ACTIVATIONS) ? "server error - activation check error" :\
	((RET) == DRM_API_SERVER_ERROR_CREATE_ACTIVATION_ENTRY) ? "server error - error during creating activation" :\
	((RET) == DRM_API_SERVER_ERROR_UPDATE_STAT) ? "server error - can not update statistics" :\
	((RET) == DRM_API_SERVER_ERROR_REGISTRATION_REQUIRED) ? "server error - registration required" :\
	((RET) == DRM_API_SERVER_ERROR_LICENSE_EXPIRED) ? "server error - license expired" :\
	((RET) == DRM_API_SERVER_ERROR_CPA_NOT_ACTIVE) ? "server error - project (CPA) not active" :\
	((RET) == DRM_API_SERVER_ERROR_PURCHASE_NOT_FOUND) ? "server error - purchase not found" :\
	((RET) == DRM_API_SERVER_ERROR_TOO_MANY_ACT_TIMEFRAME) ? "server error - too many activations within timeframe" :\
	((RET) == DRM_API_SERVER_ERROR_TOO_MANY_ACT_TOTAL) ? "server error - too many total activations" :\
	((RET) == DRM_API_SERVER_ERROR_LICENSE_NOT_FOUND) ? "server error - wrong/invalid serial" :\
	((RET) == DRM_API_SERVER_ERROR_APPLICATION_NOT_FOUND) ? "server error - application not found" :\
	((RET) == DRM_API_NO_GRACE_DEFINED) ? "grace period is undefined" :\
	((RET) == DRM_API_NO_EXPIRY_DEFINED) ? "expiry of unlock code is undefined" :\
	((RET) == DRM_API_NO_LOGFILE_EXISTS) ? "logfile does not exist, log something first" :\
	((RET) == DRM_API_UNLOCK_VALID) ? "unlock code is valid" :\
	((RET) == DRM_API_TIME_CORRUPT) ? "system time is corrupt" :\
	((RET) == DRM_API_SERVER_ERROR_INTERNAL_ERROR) ? "server error - internal error" :\
	((RET) == DRM_API_SERVER_ERROR_START_DATE_NOT_REACHED) ? "server error - start date not reached" :\
	((RET) == DRM_API_SERVER_ERROR_TOO_MANY_ACT_SAME_HWID) ? "server error - too many activations with same hw-id" :\
	((RET) == DRM_API_SERVER_ERROR_TOO_MANY_ACT_DIFF_HWID) ? "server error - too many activation with different hw-id" :\
	((RET) == DRM_API_SERVER_ERROR_UNKNOWN) ? "server error - unknown server error" :\
	"unknown error")

#define DRM_API_CMD_STRING(CMD) \
	(((CMD) == DRM_API_CMD_UNDEFINE) ? "CMD_UNDEFINED" : \
	((CMD) == DRM_API_CMD_READ_PID) ? "CMD_READ_PID" : \
	((CMD) == DRM_API_CMD_READ_PROJECT_ID) ? "CMD_READ_PROJECT_ID" : \
	((CMD) == DRM_API_CMD_GET_PID_FUNCTIONS) ? "CMD_GET_PID_FUNCTIONS" : \
	((CMD) == DRM_API_CMD_GET_UL_REQUEST) ? "CMD_GET_UL_REQUEST" : \
	((CMD) == DRM_API_CMD_SET_UL_CODE) ? "CMD_SET_UL_CODE" : \
	((CMD) == DRM_API_CMD_EVAL_UL_CODE) ? "CMD_EVAL_UL_CODE" : \
	((CMD) == DRM_API_CMD_SET_SERIAL) ? "CMD_SET_SERIAL" : \
	((CMD) == DRM_API_CMD_GET_SERIAL) ? "CMD_GET_SERIAL" : \
	((CMD) == DRM_API_CMD_GET_EXPIRY_INFO) ? "CMD_GET_EXPIRY_INFO" : \
	((CMD) == DRM_API_CMD_GET_GRACE_INFO) ? "CMD_GET_GRACE_INFO" : \
	((CMD) == DRM_API_CMD_IS_ONLINE) ? "CMD_IS_ONLINE" : \
	((CMD) == DRM_API_CMD_ACTIVATE_ONLINE) ? "CMD_ACTIVATE_ONLINE" : \
	((CMD) == DRM_API_CMD_VERIFY_ONLINE) ? "CMD_VERIFY_ONLINE" : \
	((CMD) == DRM_API_CMD_IS_REGISTERED) ? "CMD_IS_REGISTERED" : \
	((CMD) == DRM_API_CMD_REGISTER) ? "CMD_REGISTER" : \
	((CMD) == DRM_API_CMD_ENABLE_LOG) ? "CMD_ENABLE_LOG" : \
	((CMD) == DRM_API_CMD_GET_SERIAL_FROM_EXE) ? "CMD_GET_SERIAL_FROM_EXE" : \
	((CMD) == DRM_API_CMD_UPDATE_PLAYTIME) ? "CMD_UPDATE_PLAYTIME" : \
	((CMD) == DRM_API_CMD_GETLOGFILENAME) ? "CMD_GETLOGFILENAME" : \
	((CMD) == DRM_API_CMD_IS_PA_LIC) ? "CMD_IS_PA_LIC" : \
	((CMD) == DRM_API_CMD_EVAL_UL_CODE_NO_UPDATE) ? "CMD_EVAL_UL_CODE_NO_UPDATE" : \
	((CMD) == DRM_API_CMD_DEL_UD) ? "CMD_DEL_UD" : \
	((CMD) == DRM_API_CMD_INVALIDATE_GRACE) ? "CMD_INVALIDATE_GRACE" : \
	((CMD) == DRM_API_CMD_INVALIDATE_EXPIRY) ? "CMD_INVALIDATE_EXPIRY" : \
	((CMD) == DRM_API_CMD_GET_INIT_GRACE) ? "CMD_GET_INIT_GRACE" : \
	((CMD) == DRM_API_CMD_GET_INIT_EXPIRY) ? "CMD_GET_INIT_EXPIRY" : \
	((CMD) == DRM_API_CMD_GET_TIME_CORRUPT_INFO) ? "CMD_GET_TIME_CORRUPT_INFO" : \
	((CMD) == DRM_API_CMD_SET_ACTIVATION_SERVERS) ? "CMD_SET_ACTIVATION_SERVERS" : \
	((CMD) == DRM_API_CMD_GET_ACTIVATION_SERVERS) ? "CMD_GET_ACTIVATION_SERVERS" : \
	((CMD) == DRM_API_CMD_GET_SHOP_URL) ? "CMD_GET_SHOP_URL" : \
	((CMD) == DRM_API_CMD_GET_ACTIVATION_MODE) ? "CMD_GET_ACTIVATION_MODE" : \
	"CMD unknown")

#endif //#ifndef RC_INVOKED

typedef enum
{
	DRM_API_ACTIVATION_MODE_UNDEFINED = 0,
	DRM_API_ACTIVATION_MODE_ONLINE,
	DRM_API_ACTIVATION_MODE_OFFLINE,
	DRM_API_ACTIVATION_MODE_BOTH
} DRM_API_activation_mode_t;

typedef enum
{
	DRM_API_CORRUPT_UNDEFINED = 0,
	DRM_API_CORRUPT_LAST_ACCESS,
	DRM_API_CORRUPT_SYSDATE,
} DRM_API_corrupt_reason_t;

typedef struct
{
	BOOL bSystemDateCorrupt;
	DRM_API_corrupt_reason_t reason;
} DRM_API_time_corrupt_info_t;

typedef enum
{
	INVALIDATE_UNDEFINED = 0,
	INVALIDATE_GRACE,
	INVALIDATE_EXPIRY,
} DRM_API_invalidate_type_t;

// Expiry structs:
typedef enum
{
	EXPIRY_TYPE_UNDEFINED = 0,
	EXPIRY_ENDDATE,
	EXPIRY_NUMBERDAYS,
	EXPIRY_NUMBERLAUNCHES,
	EXPIRY_PLAYTIME,
} DRM_API_expiry_type_t;

typedef union
{
	struct {
		unsigned short day,month,year;
	};
	unsigned int no_launches;
	unsigned int no_days;
	unsigned int no_playtime_minutes;

} DRM_API_expiry_data_t;

typedef struct
{
	int bExpired;
	DRM_API_expiry_type_t expiry_type;
	DRM_API_expiry_data_t expiry_data;
} DRM_API_expiry_t;

// Serial structs:

typedef enum
{
	SERIAL_UNDEFINED = 0,
	SERIAL_BINARY,
	SERIAL_ALPHANUMERIC,
	SERIAL_SECUROM,
} DRM_API_serial_format_t;

typedef struct
{
	DRM_API_serial_format_t serial_format;
	unsigned int size_serial;
	char serial[1024];
} DRM_API_serial_t;

typedef struct
{
	unsigned int size_unlock_code;
	char 	unlock_code[256];
} DRM_API_unlockcode_t;

typedef struct
{
	unsigned int size_unlock_request;
	char 	unlock_request[256];
} DRM_API_unlockrequest_t;


typedef struct
{
	int bIsOnline;
} DRM_API_is_online_ret_t;


typedef struct
{
	int bIsRegistered;
} DRM_API_is_registered_ret_t;

typedef enum
{
	ACTIVATE_STATUS_TRY_CONNECT,
	ACTIVATE_STATUS_CONNECTED,
	ACTIVATE_STATUS_SSL_ESTABLISHED,
	ACTIVATE_STATUS_REQUEST_SENT,
	ACTIVATE_STATUS_RESPONSE_RECEIVED,
	ACTIVATE_STATUS_DONE
} DRM_API_activate_status_t, DRM_API_verify_status_t;

typedef void (WINAPI *DRM_API_activate_cb_t)(DRM_API_activate_status_t, void *);
typedef void (WINAPI *DRM_API_verify_cb_t)(DRM_API_verify_status_t, void *);

typedef struct
{
	// serial must have been set before calling this
	DRM_API_activate_cb_t pCB;
	void *pData;
	HANDLE	hTerminate;
	DRM_API_unlockcode_t unlock_code;
} DRM_API_activate_online_cmd_t;

typedef struct
{
	// serial must have been set before calling this
	DRM_API_verify_cb_t pCB;
	void *pData;
	HANDLE	hTerminate;
} DRM_API_verify_online_cmd_t;

typedef struct
{
	DRM_API_ret_t ret;
	int remaining_activations;
} DRM_API_activate_online_ret_t, DRM_API_verify_online_ret_t;

#define DRM_API_CALL_IN_OUT(CMD, DATA_IN, TYPE_IN, SIZE_DATA_IN, DATA_OUT, TYPE_OUT, SIZE_DATA_OUT, RESULT) \
	{ \
		static VirtualMachine_t SecuROM_VM; \
		\
		SecuROM_VM.drm_api.cmd = CMD; \
		SecuROM_VM.code_ptr = DRM_API; \
		SecuROM_VM.drm_api.result = RESULT; \
		SecuROM_VM.drm_api.version.major = DRM_API_VERSION_MAJOR; \
		SecuROM_VM.drm_api.version.minor = DRM_API_VERSION_MINOR; \
		if (SecuROM_VM.drm_api.result) *SecuROM_VM.drm_api.result = DRM_API_ERROR; \
		\
		SecuROM_VM.drm_api.data_in = (PBYTE) (TYPE_IN *) DATA_IN; \
		SecuROM_VM.drm_api.size_data_in = SIZE_DATA_IN; \
		\
		SecuROM_VM.drm_api.data_out = (PBYTE) (TYPE_OUT *) DATA_OUT; \
		SecuROM_VM.drm_api.size_data_out = (int *) SIZE_DATA_OUT; \
		\
		SECUROM_MARKER_VM_CALL(0x42424242, DRM_API, SecuROM_VM); \
		if (SecuROM_VM.drm_api.result && RESULT) *RESULT = (DRM_API_ret_t) *SecuROM_VM.drm_api.result; \
	}

#define DRM_API_CALL_IN(CMD, DATA_IN, TYPE_IN, SIZE_DATA_IN, RESULT) \
	DRM_API_CALL_IN_OUT(CMD, DATA_IN, TYPE_IN, SIZE_DATA_IN, NULL, void, 0, RESULT)

#define DRM_API_CALL_OUT(CMD, DATA_OUT, TYPE_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_IN_OUT(CMD, NULL, void, 0, DATA_OUT, TYPE_OUT, SIZE_DATA_OUT, RESULT)

#define DRM_API_GET_UL_REQUEST(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_UL_REQUEST, DATA_OUT, DRM_API_unlockrequest_t, SIZE_DATA_OUT, RESULT)

#define DRM_API_SET_UL_CODE(DATA_IN, SIZE_DATA_IN, RESULT) \
	DRM_API_CALL_IN(DRM_API_CMD_SET_UL_CODE, DATA_IN, DRM_API_unlockcode_t, SIZE_DATA_IN, RESULT)

#define DRM_API_EVAL_UL_CODE(DATA_IN, SIZE_DATA_IN, RESULT) \
	DRM_API_CALL_IN(DRM_API_CMD_EVAL_UL_CODE, DATA_IN, DRM_API_unlockcode_t, SIZE_DATA_IN, RESULT)

#define DRM_API_GET_SERIAL(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_SERIAL, DATA_OUT, DRM_API_serial_t, SIZE_DATA_OUT, RESULT)

#define DRM_API_SET_SERIAL(DATA_IN, SIZE_DATA_IN, RESULT) \
	DRM_API_CALL_IN(DRM_API_CMD_SET_SERIAL, DATA_IN, DRM_API_serial_t, SIZE_DATA_IN, RESULT)

#define DRM_API_IS_ONLINE(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_IS_ONLINE, DATA_OUT, DRM_API_is_online_ret_t, SIZE_DATA_OUT, RESULT)

#define DRM_API_ACTIVATE_ONLINE(DATA_IN, DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_IN_OUT(DRM_API_CMD_ACTIVATE_ONLINE, DATA_IN, DRM_API_activate_online_cmd_t, sizeof(DRM_API_activate_online_cmd_t), DATA_OUT, DRM_API_activate_online_ret_t, SIZE_DATA_OUT, RESULT)

#define DRM_API_VERIFY_ONLINE(DATA_IN, DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_IN_OUT(DRM_API_CMD_VERIFY_ONLINE, DATA_IN, DRM_API_verify_online_cmd_t, sizeof(DRM_API_verify_online_cmd_t), DATA_OUT, DRM_API_activate_online_ret_t, SIZE_DATA_OUT, RESULT)

#define DRM_API_GET_GRACE_INFO(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_GRACE_INFO, DATA_OUT, DRM_API_expiry_t, SIZE_DATA_OUT, RESULT)

#define DRM_API_GET_EXPIRY_INFO(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_EXPIRY_INFO, DATA_OUT, DRM_API_expiry_t, SIZE_DATA_OUT, RESULT)

#define DRM_API_ENABLE_LOGGING(RESULT) \
	DRM_API_CALL_IN(DRM_API_CMD_ENABLE_LOG, NULL, void, 0, RESULT)

#define DRM_API_GETLOGFILENAME(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GETLOGFILENAME, DATA_OUT, char, SIZE_DATA_OUT, RESULT)

#define DRM_API_GET_SERIAL_FROM_EXE(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_SERIAL_FROM_EXE, DATA_OUT, char, SIZE_DATA_OUT, RESULT)

#define DRM_API_DEL_UD(RESULT) \
	DRM_API_CALL_IN(DRM_API_CMD_DEL_UD, NULL, void, 0, RESULT)

#define DRM_API_GET_LIC_ID(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_LIC_ID, DATA_OUT, char, SIZE_DATA_OUT, RESULT)

#define DRM_API_INVALIDATE_GRACE(RESULT) \
	DRM_API_CALL_IN(DRM_API_CMD_INVALIDATE_GRACE, NULL, void, 0, RESULT)

#define DRM_API_INVALIDATE_EXPIRY(RESULT) \
	DRM_API_CALL_IN(DRM_API_CMD_INVALIDATE_EXPIRY, NULL, void, 0, RESULT)

#define DRM_API_GET_INIT_GRACE(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_INIT_GRACE, DATA_OUT, DRM_API_expiry_t, SIZE_DATA_OUT, RESULT)

#define DRM_API_GET_INIT_EXPIRY(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_INIT_EXPIRY, DATA_OUT, DRM_API_expiry_t, SIZE_DATA_OUT, RESULT)

#define DRM_API_GET_TIME_CORRUPT_INFO(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_TIME_CORRUPT_INFO, DATA_OUT, DRM_API_time_corrupt_info_t, SIZE_DATA_OUT, RESULT)

#define DRM_API_SET_ACTIVATION_SERVERS(DATA_IN, SIZE_DATA_IN, RESULT) \
	DRM_API_CALL_IN(DRM_API_CMD_SET_ACTIVATION_SERVERS, DATA_IN, char, SIZE_DATA_IN, RESULT)

#define DRM_API_GET_ACTIVATION_SERVERS(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_ACTIVATION_SERVERS, DATA_OUT, char, SIZE_DATA_OUT, RESULT)

#define DRM_API_GET_SHOP_URL(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_SHOP_URL, DATA_OUT, char, SIZE_DATA_OUT, RESULT)

#define DRM_API_GET_ACTIVATION_MODE(DATA_OUT, SIZE_DATA_OUT, RESULT) \
	DRM_API_CALL_OUT(DRM_API_CMD_GET_ACTIVATION_MODE, DATA_OUT, DRM_API_activation_mode_t, SIZE_DATA_OUT, RESULT)

typedef struct SECUROM_PID {
	int Pid_error;
	unsigned long Pid_version;

	union {
		unsigned char Pid_buf[264];

		struct {
			unsigned char Pid_reserved[3];
			unsigned char Pid_type;
			unsigned long Pid_length;
			unsigned char Pid_data[256];
		};
	};
} tSECUROM_PID;

typedef struct SECUROM_PID_FUNCS {
	char Pid_drive[4];
	int  (__stdcall * Pid_init)(int option);
	int (__stdcall * Pid_close)(void);
	int (__stdcall * Pid_query_pid)(char *drive, BYTE *key, BYTE *dest, BYTE type);
	int (__stdcall * Pid_query_pid_specify)(char *drv, DWORD lba, BYTE *key, BYTE *dest);
	int (__stdcall * Pid_get_inquiry_string)(char *drive, char *buf);
	DWORD (__stdcall * Pid_version)(void);
} tSECUROM_PID_FUNCS;

#pragma pack(pop)

#endif
