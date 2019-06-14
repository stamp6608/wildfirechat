// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

/**
 * created on : 2012-10-19
 * author : yanguoyue
 */


#include <jni.h>
#include <sstream>

#include "mars/comm/autobuffer.h"
#include "mars/comm/jni/util/scoped_jstring.h"
#include "mars/comm/xlogger/xlogger.h"
#include "mars/comm/jni/util/var_cache.h"
#include "mars/comm/jni/util/scope_jenv.h"
#include "mars/comm/jni/util/comm_function.h"

#include "mars/app/app.h"

DEFINE_FIND_CLASS(KC2Java, "com/tencent/mars/app/AppLogic")

namespace mars {
namespace app {

DEFINE_FIND_STATIC_METHOD(KC2Java_getAppFilePath, KC2Java, "getAppFilePath", "()Ljava/lang/String;")
std::string GetAppFilePath() {
	xverbose_function();

    VarCache* cache_instance = VarCache::Singleton();
	ScopeJEnv scope_jenv(cache_instance->GetJvm());
	JNIEnv *env = scope_jenv.GetEnv();

	jstring path = (jstring)JNU_CallStaticMethodByMethodInfo(env, KC2Java_getAppFilePath).l;
	if (NULL == path) {
		xerror2(TSF"getAppFilePath error return null");
		return "";
	}

	std::string app_path = ScopedJstring(env, path).GetChar();
	env->DeleteLocalRef(path);

	return app_path;
}

DEFINE_FIND_STATIC_METHOD(KC2Java_getAccountInfo, KC2Java, "getAccountInfo", "()Lcom/tencent/mars/app/AppLogic$AccountInfo;")
AccountInfo GetAccountInfo() {
	xverbose_function();

    VarCache* cache_instance = VarCache::Singleton();
	ScopeJEnv scope_jenv(cache_instance->GetJvm());
	JNIEnv *env = scope_jenv.GetEnv();

	AccountInfo info;
	jobject ret_obj = JNU_CallStaticMethodByMethodInfo(env, KC2Java_getAccountInfo).l;
	if (NULL == ret_obj) {
		xerror2(TSF"getAccountInfo error return null");
		return info;
	}

	jlong uin = JNU_GetField(env, ret_obj, "uin", "J").i;
	jstring username_jstr = (jstring)JNU_GetField(env, ret_obj, "userName", "Ljava/lang/String;").l;

	info.uin = (long)uin;

	if (username_jstr != NULL) {
		info.username = ScopedJstring(env, username_jstr).GetChar();
		env->DeleteLocalRef(username_jstr);
	}

	env->DeleteLocalRef(ret_obj);
	return info;
}

std::string GetAccountUserName() {
    xverbose_function();

    VarCache* cache_instance = VarCache::Singleton();
    ScopeJEnv scope_jenv(cache_instance->GetJvm());
    JNIEnv *env = scope_jenv.GetEnv();

    jobject ret_obj = JNU_CallStaticMethodByMethodInfo(env, KC2Java_getAccountInfo).l;
 	if (NULL == ret_obj) {
 		return std::string();
 	}

 	jstring username_jstr = (jstring)JNU_GetField(env, ret_obj, "userName", "Ljava/lang/String;").l;

 	env->DeleteLocalRef(ret_obj);

     if (username_jstr != NULL) {
         const char* name = env->GetStringUTFChars(username_jstr, NULL);
         std::string user_name(name);
         env->ReleaseStringUTFChars(username_jstr, name);
         env->DeleteLocalRef(username_jstr);
         return user_name;
     } else {
         return std::string();
     }
}

std::string GetRecentUserName() {
	return GetAccountUserName();
}

DEFINE_FIND_STATIC_METHOD(KC2Java_getClientVersion, KC2Java, "getClientVersion", "()I")
unsigned int GetClientVersion() {

	static unsigned int s_version = 0;
	if (0 != s_version) {
		return s_version;
	}

	VarCache* cache_instance = VarCache::Singleton();
	ScopeJEnv scope_jenv(cache_instance->GetJvm());
	JNIEnv *env = scope_jenv.GetEnv();
	s_version = (unsigned int)JNU_CallStaticMethodByMethodInfo(env, KC2Java_getClientVersion).i;

	return s_version;
}

DEFINE_FIND_STATIC_METHOD(KC2Java_getDeviceType, KC2Java, "getDeviceType", "()Lcom/tencent/mars/app/AppLogic$DeviceInfo;")
DeviceInfo GetDeviceInfo() {
	xverbose_function();

	static DeviceInfo s_info;
	if (!s_info.clientid.empty() || !s_info.device.empty()) {
		return s_info;
	}

	VarCache* cache_instance = VarCache::Singleton();
	ScopeJEnv scope_jenv(cache_instance->GetJvm());
	JNIEnv *env = scope_jenv.GetEnv();

	jobject ret_obj = JNU_CallStaticMethodByMethodInfo(env, KC2Java_getDeviceType).l;
	if (NULL == ret_obj) {
		xerror2(TSF"GetDeviceInfo error return null");
		return s_info;
	}

	jstring clientid_jstr = (jstring)JNU_GetField(env, ret_obj, "clientid", "Ljava/lang/String;").l;
	if (NULL != clientid_jstr) {
		s_info.clientid = ScopedJstring(env, clientid_jstr).GetChar();
		env->DeleteLocalRef(clientid_jstr);
	}

    s_info.platform = PlatformType_Android;

    jstring packagename_jstr = (jstring)JNU_GetField(env, ret_obj, "packagename", "Ljava/lang/String;").l;
	if (NULL != packagename_jstr) {
		s_info.packagename = ScopedJstring(env, packagename_jstr).GetChar();
		env->DeleteLocalRef(packagename_jstr);
	}

    jint pushtype_ji = (jint)JNU_GetField(env, ret_obj, "pushtype", "I").i;
	s_info.pushtype = pushtype_ji;

    jstring device_jstr = (jstring)JNU_GetField(env, ret_obj, "device", "Ljava/lang/String;").l;
	if (NULL != device_jstr) {
		s_info.device = ScopedJstring(env, device_jstr).GetChar();
		env->DeleteLocalRef(device_jstr);
	}

    jstring deviceversion_jstr = (jstring)JNU_GetField(env, ret_obj, "deviceversion", "Ljava/lang/String;").l;
	if (NULL != deviceversion_jstr) {
		s_info.deviceversion = ScopedJstring(env, deviceversion_jstr).GetChar();
		env->DeleteLocalRef(deviceversion_jstr);
	}

	jstring phonename_jstr = (jstring)JNU_GetField(env, ret_obj, "phonename", "Ljava/lang/String;").l;
  	if (NULL != phonename_jstr) {
   		s_info.phonename = ScopedJstring(env, phonename_jstr).GetChar();
   		env->DeleteLocalRef(phonename_jstr);
   	}

    jstring language_jstr = (jstring)JNU_GetField(env, ret_obj, "language", "Ljava/lang/String;").l;
	if (NULL != language_jstr) {
		s_info.language = ScopedJstring(env, language_jstr).GetChar();
		env->DeleteLocalRef(language_jstr);
	}

    jstring carriername_jstr = (jstring)JNU_GetField(env, ret_obj, "carriername", "Ljava/lang/String;").l;
	if (NULL != carriername_jstr) {
		s_info.carriername = ScopedJstring(env, carriername_jstr).GetChar();
		env->DeleteLocalRef(carriername_jstr);
	}

	return s_info;
}


mars::comm::ProxyInfo GetProxyInfo(const std::string& _host) {
    return mars::comm::ProxyInfo();
}



}}
