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
 * created on : 2012-07-19
 * author : yanguoyue
 */

#include <jni.h>
#include <string>
#include <map>

#include "mars/baseevent/active_logic.h"
#include "mars/baseevent/baseprjevent.h"
#include "mars/baseevent/baseevent.h"
#include "mars/comm/xlogger/xlogger.h"
#include "mars/comm/singleton.h"
#include "mars/comm/strutil.h"
#include "mars/comm/bootrun.h"
#include "mars/comm/jni/util/var_cache.h"
#include "mars/comm/jni/util/scope_jenv.h"
#include "mars/comm/jni/util/comm_function.h"
#include "mars/comm/jni/util/var_cache.h"
#include "mars/comm/jni/util/scoped_jstring.h"
//#include "mars/log/appender.h"
#include "mars/stn/stn_logic.h"

#include "stn/src/net_core.h"
#include "stn/src/net_source.h"
#include "stn/src/signalling_keeper.h"
#include "stn/config.h"
#include "stn/proto/stnproto_logic.h"

#include <android/log.h>

using namespace mars::stn;

DEFINE_FIND_CLASS(KNetJava2C, "com/tencent/mars/stn/StnLogic")

extern "C" {

/*
 * Class:     com_tencent_mars_stn_StnLogic
 * Method:    getLoadLibraries
 * Signature: ()jobject
 */
JNIEXPORT jobject JNICALL Java_com_tencent_mars_stn_StnLogic_getLoadLibraries
  (JNIEnv *_env, jclass clz) {

	return mars::baseevent::getLoadLibraries(_env);
}


/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    setSignallingStrategy
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_setSignallingStrategy
  (JNIEnv *_env, jclass, jlong _period, jlong _keep_time) {
	xverbose_function();

	SetSignallingStrategy((long)_period, (long)_keep_time);
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    keepSignalling
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_keepSignalling
  (JNIEnv *_env, jclass) {
	xverbose_function();

	KeepSignalling();
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    stopSignalling
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_stopSignalling
  (JNIEnv *_env, jclass) {
	xverbose_function();

	StopSignalling();
}

JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_setClientVersion
  (JNIEnv *_env, jclass, jint _client_version) {
	//mars::stn::SetClientVersion(_client_version);
}

}

void ExportSTN() {

}

