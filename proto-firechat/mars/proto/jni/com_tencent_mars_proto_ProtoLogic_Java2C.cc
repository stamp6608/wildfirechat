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
#include "mars/proto/proto.h"
#include "mars/proto/stn_callback.h"
#include "mars/proto/MessageDB.h"

#include "stn/src/net_core.h"
#include "stn/src/net_source.h"
#include "stn/src/signalling_keeper.h"
#include "stn/config.h"
#include "stn/proto/stnproto_logic.h"

#include <android/log.h>
 #include <string.h>


using namespace mars::stn;



DEFINE_FIND_CLASS(KProto2Java, "com/tencent/mars/proto/ProtoLogic")

#ifdef __cplusplus
extern "C" {
#endif

extern jobject   g_objProtoLogic;
extern jobject   g_objConversationInfo;
extern jobject   g_objConversationSearchresult;
extern jobject   g_objGroupSearchResult;
extern jobject   g_objFriendRequest;
extern jobject   g_objGroupInfo;
extern jobject   g_objChannelInfo;
extern jobject   g_objChatRoomInfo;
extern jobject   g_objChatRoomMembersInfo;
extern jobject   g_objGroupMember;
extern jobject   g_objMessage;
extern jobject   g_objMessageContent;
extern jobject   g_objUserInfo;
extern jobject   g_objUnreadCount;



extern void SetObjectValue_Int(JNIEnv *&env, jobject &obj, jclass &cls,
                        const char *pszSetMethod, int nVal);

extern void SetObjectValue_Bool(JNIEnv *&env, jobject &obj, jclass &cls,
                         const char *pszSetMethod, bool nVal);

extern void SetObjectValue_Long(JNIEnv *&env, jobject &obj, jclass &cls,
                         const char *pszSetMethod, long lVal);

extern void SetObjectValue_LongLong(JNIEnv *&env, jobject &obj, jclass &cls,
                             const char *pszSetMethod, long long lVal);

extern void SetObjectValue_String(JNIEnv *&env, jobject &obj, jclass &cls,
                           const char *pszSetMethod, const char *pszVal);

extern void SetObjectValue_ByteArray(JNIEnv *&env, jobject &obj, jclass &cls,
                              const char *pszSetMethod, const unsigned char *message, long nl);

extern void SetObjectValue_Object(JNIEnv *&env, jobject &obj, jclass &cls,
                           const char *pszSetMethod, jobject &jVal, const char *sign);
extern void SetObjectValue_ObjectArray(JNIEnv *&env, jobject &obj, jclass &cls,
                           const char *pszSetMethod, jobjectArray &jVal, const char *sign);
extern jstring cstring2jstring(JNIEnv *env, const char* str);
extern jobject convertProtoGroupMember(JNIEnv *env, const mars::stn::TGroupMember &tGroupMember);


DEFINE_FIND_STATIC_METHOD(KProto2Java_onConnectionStatusChanged, KProto2Java, "onConnectionStatusChanged", "(I)V")

class CSCB : public mars::stn::ConnectionStatusCallback {
public:
  void onConnectionStatusChanged(mars::stn::ConnectionStatus connectionStatus) {
    if (g_objProtoLogic) {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();

        JNU_CallStaticMethodByMethodInfo(env, KProto2Java_onConnectionStatusChanged, connectionStatus);
    }
  }
};

jobjectArray convertStringList(JNIEnv *_env, std::list<std::string> &strList) {
    jobjectArray jo_array = _env->NewObjectArray(strList.size(), _env->FindClass("java/lang/String"), 0);
    int i = 0;
    for (std::list<std::string>::iterator it = strList.begin(); it != strList.end(); it++) {
        jstring str = cstring2jstring(_env, (*it).c_str());
        _env->SetObjectArrayElement(jo_array, i++, str);
        _env->DeleteLocalRef(str);
    }
    return jo_array;
}

jobject convertProtoMessage(JNIEnv *env, const mars::stn::TMessage *tMessage) {
    jclass jmsg = (jclass)g_objMessage;
    jclass jcontent = (jclass)g_objMessageContent;

    jobject obj = env->AllocObject(jmsg);


    //ret.fromUser = [NSString stringWithUTF8String:tMessage->from.c_str()];
    SetObjectValue_String(env, obj, jmsg, "setFrom", tMessage->from.c_str());

    //ret.toUser = [NSString stringWithUTF8String:tMessage->to.c_str()];
    if(tMessage->to.size()) {
        jclass objClass = env->FindClass("java/lang/String");//定义数组中元素类型
        jobjectArray jo_array = env->NewObjectArray(tMessage->to.size(), objClass, 0);
        int i = 0;
        for (std::list<std::string>::const_iterator it = tMessage->to.begin(); it != tMessage->to.end(); it++) {
            jstring jVal = cstring2jstring(env, (*it).c_str());
            env->SetObjectArrayElement(jo_array, i++, jVal);
            env->DeleteLocalRef(jVal);
        }
        SetObjectValue_ObjectArray(env, obj, jmsg, "setTos", jo_array, "([Ljava/lang/String;)V");
        env->DeleteLocalRef(jo_array);
    }

    //ret.conversation = [[WFCCConversation alloc] init];
    //ret.conversation.type = (WFCCConversationType)tMessage->conversationType;
    SetObjectValue_Int(env, obj, jmsg, "setConversationType", tMessage->conversationType);


    //ret.conversation.target = [NSString stringWithUTF8String:tMessage->target.c_str()];
    SetObjectValue_String(env, obj, jmsg, "setTarget", tMessage->target.c_str());


    //ret.conversation.line = tMessage->line;
    SetObjectValue_Int(env, obj, jmsg, "setLine", tMessage->line);

    //ret.messageId = tMessage->messageId;
    SetObjectValue_LongLong(env, obj, jmsg, "setMessageId", tMessage->messageId);

    //ret.messageUid = tMessage->messageUid;
    SetObjectValue_LongLong(env, obj, jmsg, "setMessageUid", tMessage->messageUid);

    //ret.serverTime = tMessage->timestamp;
    SetObjectValue_LongLong(env, obj, jmsg, "setTimestamp", tMessage->timestamp);


    //ret.direction = (WFCCMessageDirection)tMessage->direction;
    SetObjectValue_Int(env, obj, jmsg, "setDirection", tMessage->direction);

    //ret.status = (WFCCMessageStatus)tMessage->status;
    SetObjectValue_Int(env, obj, jmsg, "setStatus", tMessage->status);


    //WFCCMediaMessagePayload *payload = [[WFCCMediaMessagePayload alloc] init];
    jobject objContent = env->AllocObject(jcontent);

    //payload.contentType = tMessage->content.type;
    SetObjectValue_Int(env, objContent, jcontent, "setType", tMessage->content.type);

    //payload.searchableContent = [NSString stringWithUTF8String:tMessage->content.searchableContent.c_str()];
    SetObjectValue_String(env, objContent, jcontent, "setSearchableContent", tMessage->content.searchableContent.c_str());

    //payload.pushContent = [NSString stringWithUTF8String:tMessage->content.pushContent.c_str()];
    SetObjectValue_String(env, objContent, jcontent, "setPushContent", tMessage->content.pushContent.c_str());

    //payload.content = [NSString stringWithUTF8String:tMessage->content.content.c_str()];
    SetObjectValue_String(env, objContent, jcontent, "setContent", tMessage->content.content.c_str());

    //payload.binaryContent = [NSData dataWithBytes:tMessage->content.binaryContent.c_str() length:tMessage->content.binaryContent.length()];
    SetObjectValue_ByteArray(env, objContent, jcontent, "setBinaryContent", (const unsigned char*)tMessage->content.binaryContent.c_str(), tMessage->content.binaryContent.length());


    //payload.localContent = [NSString stringWithUTF8String:tMessage->content.localContent.c_str()];
    SetObjectValue_String(env, objContent, jcontent, "setLocalContent", tMessage->content.localContent.c_str());


    //payload.mediaType = (WFCCMediaType)tMessage->content.mediaType;
    SetObjectValue_Int(env, objContent, jcontent, "setMediaType", tMessage->content.mediaType);


    //payload.remoteMediaUrl = [NSString stringWithUTF8String:tMessage->content.remoteMediaUrl.c_str()];
    SetObjectValue_String(env, objContent, jcontent, "setRemoteMediaUrl", tMessage->content.remoteMediaUrl.c_str());

    //payload.localMediaPath = [NSString stringWithUTF8String:tMessage->content.localMediaPath.c_str()];
    SetObjectValue_String(env, objContent, jcontent, "setLocalMediaPath", tMessage->content.localMediaPath.c_str());

    //payload.mentionedType = tMessage->content.mentionedType;
    SetObjectValue_Int(env, objContent, jcontent, "setMentionedType", tMessage->content.mentionedType);

    //NSMutableArray *mentionedType = [[NSMutableArray alloc] init];
    //for (std::list<std::string>::const_iterator it = tMessage->content.mentionedTargets.begin(); it != tMessage->content.mentionedTargets.end(); it++) {
    // [mentionedType addObject:[NSString stringWithUTF8String:(*it).c_str()]];
    //}
    if(tMessage->content.mentionedTargets.size()) {
        jclass objClass = env->FindClass("java/lang/String");//定义数组中元素类型
        jobjectArray jo_array = env->NewObjectArray(tMessage->content.mentionedTargets.size(), objClass, 0);
        int i = 0;
        for (std::list<std::string>::const_iterator it = tMessage->content.mentionedTargets.begin(); it != tMessage->content.mentionedTargets.end(); it++) {
            jstring jVal = cstring2jstring(env, (*it).c_str());
            env->SetObjectArrayElement(jo_array, i++, jVal);
            env->DeleteLocalRef(jVal);
        }
        SetObjectValue_ObjectArray(env, objContent, jcontent, "setMentionedTargets", jo_array, "([Ljava/lang/String;)V");
    }

    SetObjectValue_Object(env, obj, jmsg, "setContent", objContent, "(Lcn/wildfirechat/model/ProtoMessageContent;)V");

    env->DeleteLocalRef(objContent);

    return obj;
}

jobjectArray convertProtoMessageList(JNIEnv *env, const std::list<mars::stn::TMessage> &messageList) {
    jobjectArray jo_array = env->NewObjectArray(messageList.size(), (jclass) g_objMessage, 0);
    int i = 0;
    for (std::list<mars::stn::TMessage>::const_iterator it = messageList.begin(); it != messageList.end(); it++) {
        const mars::stn::TMessage &tmsg = *it;
        jobject msg = convertProtoMessage(env, &tmsg);

        env->SetObjectArrayElement(jo_array, i++, msg);
        env->DeleteLocalRef(msg);
    }
    return jo_array;
}

DEFINE_FIND_STATIC_METHOD(KProto2Java_onReceiveMessage, KProto2Java, "onReceiveMessage", "([Lcn/wildfirechat/model/ProtoMessage;Z)V")
DEFINE_FIND_STATIC_METHOD(KProto2Java_onRecallMessage, KProto2Java, "onRecallMessage", "(J)V")
class RPCB : public mars::stn::ReceiveMessageCallback {
public:
    void onReceiveMessage(const std::list<mars::stn::TMessage> &messageList, bool hasMore) {
        if (g_objProtoLogic) {
            VarCache* cache_instance = VarCache::Singleton();
            ScopeJEnv scope_jenv(cache_instance->GetJvm());
            JNIEnv *env = scope_jenv.GetEnv();

            jobjectArray jo_array = convertProtoMessageList(env, messageList);

            JNU_CallStaticMethodByMethodInfo(env, KProto2Java_onReceiveMessage, jo_array, hasMore);
            env->DeleteLocalRef(jo_array);
        }
    }
    void onRecallMessage(const std::string operatorId, long long messageUid) {
      if (g_objProtoLogic) {
          VarCache* cache_instance = VarCache::Singleton();
          ScopeJEnv scope_jenv(cache_instance->GetJvm());
          JNIEnv *env = scope_jenv.GetEnv();

          JNU_CallStaticMethodByMethodInfo(env, KProto2Java_onRecallMessage, messageUid);
      }
    }
};



jobject convertProtoUserInfo(JNIEnv *env, const mars::stn::TUserInfo &tUserInfo) {
    jclass juserInfo = (jclass)g_objUserInfo;
    jobject obj = env->AllocObject(juserInfo);

//    userInfo.userId = [NSString stringWithUTF8String:tui.uid.c_str()];
    SetObjectValue_String(env, obj, juserInfo, "setUid", tUserInfo.uid.c_str());

//    userInfo.name = [NSString stringWithUTF8String:tui.name.c_str()];
    SetObjectValue_String(env, obj, juserInfo, "setName", tUserInfo.name.c_str());

//    userInfo.portrait = [NSString stringWithUTF8String:tui.portrait.c_str()];
    SetObjectValue_String(env, obj, juserInfo, "setDisplayName", tUserInfo.displayName.c_str());
//
//    userInfo.displayName = [NSString stringWithUTF8String:tui.displayName.c_str()];
    SetObjectValue_String(env, obj, juserInfo, "setPortrait", tUserInfo.portrait.c_str());

//    userInfo.gender = tui.gender;
    SetObjectValue_Int(env, obj, juserInfo, "setGender", tUserInfo.gender);

//    userInfo.mobile = [NSString stringWithUTF8String:tui.mobile.c_str()];
    SetObjectValue_String(env, obj, juserInfo, "setMobile", tUserInfo.mobile.c_str());

//    userInfo.email = [NSString stringWithUTF8String:tui.email.c_str()];
    SetObjectValue_String(env, obj, juserInfo, "setEmail", tUserInfo.email.c_str());

//    userInfo.address = [NSString stringWithUTF8String:tui.address.c_str()];
    SetObjectValue_String(env, obj, juserInfo, "setAdress", tUserInfo.address.c_str());

//    userInfo.company = [NSString stringWithUTF8String:tui.company.c_str()];
    SetObjectValue_String(env, obj, juserInfo, "setCompany", tUserInfo.company.c_str());

//    userInfo.social = [NSString stringWithUTF8String:tui.social.c_str()];
    SetObjectValue_String(env, obj, juserInfo, "setSocial", tUserInfo.social.c_str());

//    userInfo.extra = [NSString stringWithUTF8String:tui.extra.c_str()];
    SetObjectValue_String(env, obj, juserInfo, "setExtra", tUserInfo.extra.c_str());

    SetObjectValue_String(env, obj, juserInfo, "setFriendAlias", tUserInfo.friendAlias.c_str());

    SetObjectValue_String(env, obj, juserInfo, "setGroupAlias", tUserInfo.groupAlias.c_str());

//    userInfo.updateDt = tui.updateDt;
    SetObjectValue_LongLong(env, obj, juserInfo, "setUpdateDt", tUserInfo.updateDt);

    SetObjectValue_Int(env, obj, juserInfo, "setType", tUserInfo.type);

    return obj;
}

jobjectArray convertProtoUserInfoList(JNIEnv *env, const std::list<mars::stn::TUserInfo> &users) {
    jobjectArray jo_array = env->NewObjectArray(users.size(), (jclass) g_objUserInfo, 0);
    int i = 0;
    for(std::list<mars::stn::TUserInfo>::const_iterator it = users.begin(); it != users.end(); it++) {
        const mars::stn::TUserInfo &tUserInfo = *it;
        jobject userInfo = convertProtoUserInfo(env, tUserInfo);

        env->SetObjectArrayElement(jo_array, i++, userInfo);
        env->DeleteLocalRef(userInfo);
    }
    return jo_array;
}

DEFINE_FIND_STATIC_METHOD(KProto2Java_onUserInfoUpdated, KProto2Java, "onUserInfoUpdated", "([Lcn/wildfirechat/model/ProtoUserInfo;)V")
class GUCB : public mars::stn::GetUserInfoCallback {

  void onSuccess(const std::list<mars::stn::TUserInfo> &userInfoList) {
        if (g_objProtoLogic) {
            VarCache* cache_instance = VarCache::Singleton();
            ScopeJEnv scope_jenv(cache_instance->GetJvm());
            JNIEnv *env = scope_jenv.GetEnv();

            jobjectArray jo_array = convertProtoUserInfoList(env, userInfoList);

            JNU_CallStaticMethodByMethodInfo(env, KProto2Java_onUserInfoUpdated, jo_array);
            env->DeleteLocalRef(jo_array);
        }
  }
  void onFalure(int errorCode) {

  }
};

jobject convertProtoGroupInfo(JNIEnv *env, const mars::stn::TGroupInfo &tGroupInfo) {
    jclass jgroupInfo = (jclass)g_objGroupInfo;
    jobject obj = env->AllocObject(jgroupInfo);

    //groupInfo.type = (WFCCGroupType)tgi.type;
    SetObjectValue_Int(env, obj, jgroupInfo, "setType", tGroupInfo.type);

    //groupInfo.target = [NSString stringWithUTF8String:tgi.target.c_str()];
    SetObjectValue_String(env, obj, jgroupInfo, "setTarget", tGroupInfo.target.c_str());


    //groupInfo.name = [NSString stringWithUTF8String:tgi.name.c_str()];
    SetObjectValue_String(env, obj, jgroupInfo, "setName", tGroupInfo.name.c_str());

    //groupInfo.extra = [NSData dataWithBytes:tgi.extra.c_str() length:tgi.extra.length()];
    SetObjectValue_String(env, obj, jgroupInfo, "setExtra", tGroupInfo.extra.c_str());

    //groupInfo.portrait = [NSString stringWithUTF8String:tgi.portrait.c_str()];
    SetObjectValue_String(env, obj, jgroupInfo, "setPortrait", tGroupInfo.portrait.c_str());

    //groupInfo.owner = [NSString stringWithUTF8String:tgi.owner.c_str()];
    SetObjectValue_String(env, obj, jgroupInfo, "setOwner", tGroupInfo.owner.c_str());

    //groupInfo.memberCount = tgi.memberCount;
    SetObjectValue_Int(env, obj, jgroupInfo, "setMemberCount", tGroupInfo.memberCount);

    return obj;
}

jobject convertProtoChatRoomInfo(JNIEnv *env, jstring chatroomId, const mars::stn::TChatroomInfo &tChatRoomInfo) {
    jclass jChatRoomInfo = (jclass)g_objChatRoomInfo;
    jobject obj = env->AllocObject(jChatRoomInfo);

    //chatroomInfo.chatroomId = chatroomId;
    SetObjectValue_String(env, obj, jChatRoomInfo, "setChatRoomId", ScopedJstring(env, chatroomId).GetChar());
    //chatroomInfo.title = [NSString stringWithUTF8String:info.title.c_str()];
    SetObjectValue_String(env, obj, jChatRoomInfo, "setTitle", tChatRoomInfo.title.c_str());
    //chatroomInfo.desc = [NSString stringWithUTF8String:info.desc.c_str()];
    SetObjectValue_String(env, obj, jChatRoomInfo, "setDesc", tChatRoomInfo.desc.c_str());
    //chatroomInfo.portrait = [NSString stringWithUTF8String:info.portrait.c_str()];
    SetObjectValue_String(env, obj, jChatRoomInfo, "setPortrait", tChatRoomInfo.portrait.c_str());
    //chatroomInfo.extra = [NSString stringWithUTF8String:info.extra.c_str()];
    SetObjectValue_String(env, obj, jChatRoomInfo, "setExtra", tChatRoomInfo.extra.c_str());
    //chatroomInfo.state = info.state;
    SetObjectValue_Int(env, obj, jChatRoomInfo, "setState", tChatRoomInfo.state);
    //chatroomInfo.memberCount = info.memberCount;
    SetObjectValue_Int(env, obj, jChatRoomInfo, "setMemberCount", tChatRoomInfo.memberCount);
    //chatroomInfo.createDt = info.createDt;
    SetObjectValue_LongLong(env, obj, jChatRoomInfo, "setCreateDt", tChatRoomInfo.createDt);
    //chatroomInfo.updateDt = info.updateDt;
    SetObjectValue_LongLong(env, obj, jChatRoomInfo, "setUpdateDt", tChatRoomInfo.updateDt);

    return obj;
}

jobject convertProtoChatRoomMembersInfo(JNIEnv *env, const mars::stn::TChatroomMemberInfo &info) {
/**
        memberInfo.memberCount = info.memberCount;
        NSMutableArray *members = [[NSMutableArray alloc] init];
        for (std::list<std::string>::const_iterator it = info.olderMembers.begin(); it != info.olderMembers.end(); it++) {
            [members addObject:[NSString stringWithUTF8String:it->c_str()]];
        }
        */
    jclass jChatRoomMembersInfo = (jclass)g_objChatRoomMembersInfo;
    jobject obj = env->AllocObject(jChatRoomMembersInfo);
    SetObjectValue_Int(env, obj, jChatRoomMembersInfo, "setMemberCount", info.memberCount);

    jclass list_cls = env->FindClass("java/util/ArrayList");
    jmethodID list_costruct = env->GetMethodID(list_cls, "<init>", "()V");
    jobject list_obj = env->NewObject(list_cls , list_costruct);

    jmethodID list_add  = env->GetMethodID(list_cls, "add", "(Ljava/lang/Object;)Z");

    for (std::list<std::string>::const_iterator it = info.olderMembers.begin(); it != info.olderMembers.end(); it++) {
      env->CallBooleanMethod(list_obj , list_add , ScopedJstring(env, (*it).c_str()).GetJstr());
    }
    SetObjectValue_Object(env, obj, jChatRoomMembersInfo, "setMembers", list_obj, "(Ljava/util/List;)V");

    return obj;
}

jobjectArray convertProtoGroupInfoList(JNIEnv *env, const std::list<mars::stn::TGroupInfo> &groups) {
    jobjectArray jo_array = env->NewObjectArray(groups.size(), (jclass) g_objGroupInfo, 0);
    int i = 0;
    for(std::list<mars::stn::TGroupInfo>::const_iterator it = groups.begin(); it != groups.end(); it++) {
        const mars::stn::TGroupInfo &tGroupInfo = *it;
        jobject groupInfo = convertProtoGroupInfo(env, tGroupInfo);

        env->SetObjectArrayElement(jo_array, i++, groupInfo);
        env->DeleteLocalRef(groupInfo);
    }
    return jo_array;
}

DEFINE_FIND_STATIC_METHOD(KProto2Java_onGroupInfoUpdated, KProto2Java, "onGroupInfoUpdated", "([Lcn/wildfirechat/model/ProtoGroupInfo;)V")
class GGCB : public mars::stn::GetGroupInfoCallback {
  public:

  void onSuccess(const std::list<mars::stn::TGroupInfo> &groupInfoList) {
    if (g_objProtoLogic) {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();

        jobjectArray jo_array = convertProtoGroupInfoList(env, groupInfoList);

        JNU_CallStaticMethodByMethodInfo(env, KProto2Java_onGroupInfoUpdated, jo_array);
        env->DeleteLocalRef(jo_array);
    }
  }
  void onFalure(int errorCode) {
  }
};


DEFINE_FIND_STATIC_METHOD(KProto2Java_onGroupMembersUpdated, KProto2Java, "onGroupMembersUpdated", "(Ljava/lang/String;[Lcn/wildfirechat/model/ProtoGroupMember;)V")
class GGMCB : public mars::stn::GetGroupMembersCallback {
  public:

  void onSuccess(const std::string &groupId, const std::list<mars::stn::TGroupMember> &groupMemberList) {
    if (g_objProtoLogic) {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();

        jobjectArray jo_array = env->NewObjectArray(groupMemberList.size(), (jclass) g_objGroupMember, 0);
        int i = 0;
        for(std::list<mars::stn::TGroupMember>::const_iterator it = groupMemberList.begin(); it != groupMemberList.end(); it++) {
            const mars::stn::TGroupMember &tgm = *it;
            jobject gm = convertProtoGroupMember(env, tgm);

            env->SetObjectArrayElement(jo_array, i++, gm);
            env->DeleteLocalRef(gm);
        }

        jstring jstr = cstring2jstring(env, groupId.c_str());
        JNU_CallStaticMethodByMethodInfo(env, KProto2Java_onGroupMembersUpdated, jstr, jo_array);
        env->DeleteLocalRef(jstr);
    }
  }
  void onFalure(int errorCode) {
  }
};

jobject convertProtoChannelInfo(JNIEnv *env, const mars::stn::TChannelInfo &tChannelInfo) {
    jclass jchannelInfo = (jclass)g_objChannelInfo;
    jobject obj = env->AllocObject(jchannelInfo);


    //channelInfo.channelId = [NSString stringWithUTF8String:tci.channelId.c_str()];
    SetObjectValue_String(env, obj, jchannelInfo, "setChannelId", tChannelInfo.channelId.c_str());

    //channelInfo.name = [NSString stringWithUTF8String:tci.name.c_str()];
    SetObjectValue_String(env, obj, jchannelInfo, "setName", tChannelInfo.name.c_str());

    //channelInfo.desc = [NSString stringWithUTF8String:tci.desc.c_str()];
    SetObjectValue_String(env, obj, jchannelInfo, "setDesc", tChannelInfo.desc.c_str());

    //channelInfo.extra = [NSString stringWithUTF8String:tci.extra.c_str()];
    SetObjectValue_String(env, obj, jchannelInfo, "setExtra", tChannelInfo.extra.c_str());

    //channelInfo.owner = [NSString stringWithUTF8String:tci.owner.c_str()];
    SetObjectValue_String(env, obj, jchannelInfo, "setOwner", tChannelInfo.owner.c_str());

    //channelInfo.portrait = [NSString stringWithUTF8String:tci.portrait.c_str()];
    SetObjectValue_String(env, obj, jchannelInfo, "setPortrait", tChannelInfo.portrait.c_str());

    //channelInfo.status = tci.status;
    SetObjectValue_Int(env, obj, jchannelInfo, "setStatus", tChannelInfo.status);

    //channelInfo.updateDt = tci.updateDt;
    SetObjectValue_LongLong(env, obj, jchannelInfo, "setUpdateDt", tChannelInfo.updateDt);

    return obj;
}

jobjectArray convertProtoChannelInfoList(JNIEnv *env, const std::list<mars::stn::TChannelInfo> &channels) {
    jobjectArray jo_array = env->NewObjectArray(channels.size(), (jclass) g_objChannelInfo, 0);
    int i = 0;
    for(std::list<mars::stn::TChannelInfo>::const_iterator it = channels.begin(); it != channels.end(); it++) {
        const mars::stn::TChannelInfo &tChannelInfo = *it;
        jobject channelInfo = convertProtoChannelInfo(env, tChannelInfo);

        env->SetObjectArrayElement(jo_array, i++, channelInfo);
        env->DeleteLocalRef(channelInfo);
    }
    return jo_array;
}

DEFINE_FIND_STATIC_METHOD(KProto2Java_onChannelInfoUpdated, KProto2Java, "onChannelInfoUpdated", "([Lcn/wildfirechat/model/ProtoChannelInfo;)V")
class GCHCB : public mars::stn::GetChannelInfoCallback {
public:

    void onSuccess(const std::list<mars::stn::TChannelInfo> &channelInfoList) {
        if (g_objProtoLogic) {
            VarCache* cache_instance = VarCache::Singleton();
            ScopeJEnv scope_jenv(cache_instance->GetJvm());
            JNIEnv *env = scope_jenv.GetEnv();

            jobjectArray jo_array = convertProtoChannelInfoList(env, channelInfoList);

            JNU_CallStaticMethodByMethodInfo(env, KProto2Java_onChannelInfoUpdated, jo_array);
            env->DeleteLocalRef(jo_array);
        }
    }
    void onFalure(int errorCode) {
    }
};


DEFINE_FIND_STATIC_METHOD(KProto2Java_onFriendListUpdated, KProto2Java, "onFriendListUpdated", "([Ljava/lang/String;)V")
class GFLCB : public mars::stn::GetMyFriendsCallback {
public:
    void onSuccess(std::list<std::string> friendIdList) {
        if (g_objProtoLogic) {
            VarCache* cache_instance = VarCache::Singleton();
            ScopeJEnv scope_jenv(cache_instance->GetJvm());
            JNIEnv *env = scope_jenv.GetEnv();

            if(friendIdList.size() > 0) {
                jobjectArray jFriendIdList = convertStringList(env, friendIdList);
                JNU_CallStaticMethodByMethodInfo(env, KProto2Java_onFriendListUpdated, jFriendIdList);
            }
        }
    }
    void onFalure(int errorCode) {

    }
};


DEFINE_FIND_STATIC_METHOD(KProto2Java_onFriendRequestUpdated, KProto2Java, "onFriendRequestUpdated", "()V")
class GFRCB : public mars::stn::GetFriendRequestCallback {
public:
    void onSuccess(bool hasNewRequest) {
        if(g_objProtoLogic) {
            VarCache* cache_instance = VarCache::Singleton();
            ScopeJEnv scope_jenv(cache_instance->GetJvm());
            JNIEnv *env = scope_jenv.GetEnv();

            if(hasNewRequest) {
                JNU_CallStaticMethodByMethodInfo(env, KProto2Java_onFriendRequestUpdated);
            }
        }
    }
    void onFalure(int errorCode) {

    }
};

DEFINE_FIND_STATIC_METHOD(KProto2Java_onSettingUpdated, KProto2Java, "onSettingUpdated", "()V")
class GSCB : public mars::stn::GetSettingCallback {
public:
  void onSuccess(bool hasNewRequest) {
        if(g_objProtoLogic) {
            VarCache* cache_instance = VarCache::Singleton();
            ScopeJEnv scope_jenv(cache_instance->GetJvm());
            JNIEnv *env = scope_jenv.GetEnv();

            if(hasNewRequest) {
                JNU_CallStaticMethodByMethodInfo(env, KProto2Java_onSettingUpdated);
            }
        }
  }
  void onFalure(int errorCode) {

  }
};

//CSCB gConnectionCallback();

/*
 * Class:     com_tencent_mars_stn_StnLogic
 * Method:    getLoadLibraries
 * Signature: ()jobject
 */
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getLoadLibraries
  (JNIEnv *_env, jclass clz) {

	return mars::baseevent::getLoadLibraries(_env);
}

//public static native void connect(String host, int shortPort);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_connect
		(JNIEnv *_env, jclass clz, jstring host, jint shortPort) {
	std::string ipAddress(ScopedJstring(_env, host).GetChar());
	mars::stn::Connect(ipAddress, shortPort);
}


//public static native void setAuthInfo(String userId, String token);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_setAuthInfo
		(JNIEnv *_env, jclass clz, jstring userId, jstring token) {
	std::string su(ScopedJstring(_env, userId).GetChar());
	std::string sp(ScopedJstring(_env, token).GetChar());

    mars::stn::setConnectionStatusCallback(new CSCB());
    mars::stn::setReceiveMessageCallback(new RPCB());

    mars::stn::setRefreshUserInfoCallback(new GUCB());
    mars::stn::setRefreshGroupInfoCallback(new GGCB());
    mars::stn::setRefreshGroupMemberCallback(new GGMCB());
    mars::stn::setRefreshFriendListCallback(new GFLCB());
    mars::stn::setRefreshChannelInfoCallback(new GCHCB());
    mars::stn::setRefreshFriendRequestCallback(new GFRCB());
    mars::stn::setRefreshSettingCallback(new GSCB());

	mars::stn::setAuthInfo(su, sp);
}

//public static native void disconnect(int flag);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_disconnect
		(JNIEnv *_env, jclass clz, jint flag) {
	mars::stn::Disconnect((int)flag);
}

JNIEXPORT jint JNICALL Java_com_tencent_mars_proto_ProtoLogic_getConnectionStatus
		(JNIEnv *_env, jclass clz) {
	return mars::stn::getConnectionStatus();
}

JNIEXPORT jlong JNICALL Java_com_tencent_mars_proto_ProtoLogic_getServerDeltaTime
		(JNIEnv *_env, jclass clz) {
	return (jlong)mars::stn::getServerDeltaTime();
}

std::string jstringToString(JNIEnv *env, jstring jstr) {
    if (jstr) {
        const char *cstr = env->GetStringUTFChars(jstr, NULL);
        std::string out(cstr);
        env->ReleaseStringUTFChars(jstr, cstr);
        return out;
    }
    return "";
}

std::string jarrayToString(JNIEnv *env, jbyteArray jarray) {
    if(jarray) {
	    jsize alen = env->GetArrayLength(jarray);
	    jbyte* ba = env->GetByteArrayElements(jarray, 0);
	    if (alen > 0) {
    	    std::string out((const char*)ba, (size_t)alen);
	        env->ReleaseByteArrayElements(jarray, ba, 0);
	        return out;
	    }
	}

    return "";
}

std::list<std::string> jarrayToStringList(JNIEnv *env, jobjectArray jarray) {
    std::list<std::string> out;
    if(jarray) {
	    jsize alen = env->GetArrayLength(jarray);
	    if (alen > 0) {
    	    for (int i = 0; i < alen; i++) {
              jstring jIp = (jstring)env->GetObjectArrayElement(jarray, i );
              out.insert(out.end(), (ScopedJstring(env, jIp).GetChar()));
            }
	    }
	}

    return out;
}


class IMGetChatroomInfoCallback : public mars::stn::GetChatroomInfoCallback {
private:
    jstring chatroomId;
    jobject mObj;
public:
    IMGetChatroomInfoCallback(jstring cid, jobject obj) : mars::stn::GetChatroomInfoCallback(), chatroomId(cid),  mObj(obj) {};
    void onSuccess(const mars::stn::TChatroomInfo &info) {
        JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
        jobject chatRoomInfo = convertProtoChatRoomInfo(env, chatroomId, info);
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onSuccess", "(Lcn.wildfirechat.model.ProtoChatRoomInfo;)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallObjectMethod(mObj, nMethodId, chatRoomInfo);
             }
             env->DeleteLocalRef(chatRoomInfo);
             env->DeleteLocalRef(cls);
         }
        delete this;
    }
    void onFalure(int errorCode) {
         JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onFailure", "(I)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jint)errorCode);
             }
             env->DeleteLocalRef(cls);
         }
        delete this;
    }

    virtual ~IMGetChatroomInfoCallback() {
        JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
        env->DeleteGlobalRef(mObj);
        env->DeleteGlobalRef(chatroomId);
    }
};

class IMGetChatroomMemberInfoCallback : public mars::stn::GetChatroomMemberInfoCallback {
private:
    jobject mObj;
public:
    IMGetChatroomMemberInfoCallback(jobject obj) : mars::stn::GetChatroomMemberInfoCallback(), mObj(obj) {};
    void onSuccess(const mars::stn::TChatroomMemberInfo &info) {
        JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
        jobject chatRoomMemberInfo = convertProtoChatRoomMembersInfo(env, info);
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onSuccess", "(Lcn.wildfirechat.model.ProtoChatRoomMembersInfo;)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallObjectMethod(mObj, nMethodId, chatRoomMemberInfo);
             }
             env->DeleteLocalRef(chatRoomMemberInfo);
             env->DeleteLocalRef(cls);
         }
        delete this;
    }
    void onFalure(int errorCode) {
         JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onFailure", "(I)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jint)errorCode);
             }
             env->DeleteLocalRef(cls);
         }
        delete this;
    }

    virtual ~IMGetChatroomMemberInfoCallback() {
        JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
        env->DeleteGlobalRef(mObj);
    }
};

class IMGeneralOperationCallback : public mars::stn::GeneralOperationCallback {
private:
    jobject mObj;
public:
    IMGeneralOperationCallback(jobject obj) : mars::stn::GeneralOperationCallback(), mObj(obj) {};
    void onSuccess() {
         JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onSuccess", "()V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId);
             }
             env->DeleteLocalRef(cls);
         }
        delete this;
    }
    void onFalure(int errorCode) {
         JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onFailure", "(I)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jint)errorCode);
             }
             env->DeleteLocalRef(cls);
         }
        delete this;
    }

    virtual ~IMGeneralOperationCallback() {
        JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
        env->DeleteGlobalRef(mObj);
    }
};

class IMLoadRemoteMessagesCallback : public mars::stn::LoadRemoteMessagesCallback {
private:
    jobject mObj;
public:
    IMLoadRemoteMessagesCallback(jobject obj) : mars::stn::LoadRemoteMessagesCallback(), mObj(obj) {};
    void onSuccess(const std::list<mars::stn::TMessage> &messageList) {
         JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onSuccess", "([Lcn/wildfirechat/model/ProtoMessage;)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                jobjectArray jo_array = convertProtoMessageList(env, messageList);
                env->CallVoidMethod(mObj, nMethodId, jo_array);
                env->DeleteLocalRef(jo_array);
             }
             env->DeleteLocalRef(cls);
         }

        delete this;
    }
    void onFalure(int errorCode) {
         JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onFailure", "(I)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jint)errorCode);
             }
             env->DeleteLocalRef(cls);
         }
        delete this;
    }

    virtual ~IMLoadRemoteMessagesCallback() {
        JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
        env->DeleteGlobalRef(mObj);
    }
};

class IMSendMessageCallback : public mars::stn::SendMsgCallback {
private:
    jobject mObj;
public:
    IMSendMessageCallback(jobject obj) : mars::stn::SendMsgCallback(), mObj(obj) {};
     void onSuccess(long long messageUid, long long timestamp) {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onSuccess", "(JJ)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jlong)messageUid, (jlong)timestamp);
             }
             env->DeleteLocalRef(cls);
         }
         delete this;
     }
    void onFalure(int errorCode) {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onFailure", "(I)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jint)errorCode);
             }
             env->DeleteLocalRef(cls);
         }
         delete this;
    }

    void onPrepared(long messageId, int64_t savedTime) {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onPrepared", "(JJ)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jlong)messageId, (jlong)savedTime);
             }
             env->DeleteLocalRef(cls);
         }
    }

    void onMediaUploaded(std::string remoteUrl) {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onMediaUploaded", "(Ljava/lang/String;)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 jstring jstr = cstring2jstring(env, remoteUrl.c_str());
                 env->CallVoidMethod(mObj, nMethodId, jstr);
                 env->DeleteLocalRef(jstr);
             }
             env->DeleteLocalRef(cls);
         }
    }

    void onProgress(int uploaded, int total) {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onProgress", "(JJ)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jlong)uploaded, (jlong)total);
             }
             env->DeleteLocalRef(cls);
         }
    }

    virtual ~IMSendMessageCallback() {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();
        env->DeleteGlobalRef(mObj);
    }
};

mars::stn::TMessage convertMessage(JNIEnv *env, jobject msg) {
    mars::stn::TMessage tMessage;
    jclass jmsg = env->GetObjectClass(msg);

    //ret.fromUser = [NSString stringWithUTF8String:tMessage->from.c_str()];
    jfieldID field = env->GetFieldID(jmsg, "from", "Ljava/lang/String;");
    jstring str = (jstring)env->GetObjectField(msg, field);
    tMessage.from = jstringToString(env, str);

    env->DeleteLocalRef(str);

    //ret.toUser = [NSString stringWithUTF8String:tMessage->to.c_str()];
    field = env->GetFieldID(jmsg, "tos", "[Ljava/lang/String;");
    jobjectArray jstringArrayTos = (jobjectArray)env->GetObjectField(msg, field);
    tMessage.to = jarrayToStringList(env, jstringArrayTos);
    env->DeleteLocalRef(jstringArrayTos);

    //ret.conversation = [[WFCCConversation alloc] init];
    //ret.conversation.type = (WFCCConversationType)tMessage->conversationType;
    field = env->GetFieldID(jmsg, "conversationType", "I");
    int i = env->GetIntField(msg, field);
    tMessage.conversationType = i;



    //ret.conversation.target = [NSString stringWithUTF8String:tMessage->target.c_str()];
    field = env->GetFieldID(jmsg, "target", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(msg, field);
    tMessage.target = jstringToString(env, str);

    env->DeleteLocalRef(str);


    //ret.conversation.line = tMessage->line;
    field = env->GetFieldID(jmsg, "line", "I");
    i = env->GetIntField(msg, field);
    tMessage.line = i;


    //ret.messageId = tMessage->messageId;
    field = env->GetFieldID(jmsg, "messageId", "J");
    long l = env->GetLongField(msg, field);
    tMessage.messageId = l;


    //ret.messageUid = tMessage->messageUid;
    field = env->GetFieldID(jmsg, "messageUid", "J");
    int64_t ll = env->GetLongField(msg, field);
    tMessage.messageUid = ll;


    //ret.serverTime = tMessage->timestamp;
    field = env->GetFieldID(jmsg, "timestamp", "J");
    ll = env->GetLongField(msg, field);
    tMessage.timestamp = ll;



    //ret.direction = (WFCCMessageDirection)tMessage->direction;
    field = env->GetFieldID(jmsg, "direction", "I");
    i = env->GetIntField(msg, field);
    tMessage.direction = i;


    //ret.status = (WFCCMessageStatus)tMessage->status;
    field = env->GetFieldID(jmsg, "status", "I");
    i = env->GetIntField(msg, field);
    tMessage.status = (mars::stn::MessageStatus)i;



    jclass jcontent = (jclass)g_objMessageContent;
    field = env->GetFieldID(jmsg, "content", "Lcn/wildfirechat/model/ProtoMessageContent;");
    jobject content = env->GetObjectField(msg, field);



    //payload.contentType = tMessage->content.type;
    field = env->GetFieldID(jcontent, "type", "I");
    i = env->GetIntField(content, field);
    tMessage.content.type = i;


    //payload.searchableContent = [NSString stringWithUTF8String:tMessage->content.searchableContent.c_str()];
    field = env->GetFieldID(jcontent, "searchableContent", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(content, field);
    tMessage.content.searchableContent = jstringToString(env, str);

    env->DeleteLocalRef(str);

    //payload.pushContent = [NSString stringWithUTF8String:tMessage->content.pushContent.c_str()];
    field = env->GetFieldID(jcontent, "pushContent", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(content, field);
    tMessage.content.pushContent = jstringToString(env, str);

    env->DeleteLocalRef(str);

    //payload.content = [NSString stringWithUTF8String:tMessage->content.content.c_str()];
    field = env->GetFieldID(jcontent, "content", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(content, field);
    tMessage.content.content = jstringToString(env, str);

    env->DeleteLocalRef(str);

    //payload.binaryContent = [NSData dataWithBytes:tMessage->content.binaryContent.c_str() length:tMessage->content.binaryContent.length()];
    field = env->GetFieldID(jcontent, "binaryContent", "[B");
    jbyteArray jarray = (jbyteArray)env->GetObjectField(content, field);
    tMessage.content.binaryContent = jarrayToString(env, jarray);

    env->DeleteLocalRef(jarray);

    //payload.localContent = [NSString stringWithUTF8String:tMessage->content.localContent.c_str()];
    field = env->GetFieldID(jcontent, "localContent", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(content, field);
    tMessage.content.localContent = jstringToString(env, str);

    env->DeleteLocalRef(str);


    //payload.mediaType = (WFCCMediaType)tMessage->content.mediaType;
    field = env->GetFieldID(jcontent, "mediaType", "I");
    i = env->GetIntField(content, field);
    tMessage.content.mediaType = i;



    //payload.remoteMediaUrl = [NSString stringWithUTF8String:tMessage->content.remoteMediaUrl.c_str()];
    field = env->GetFieldID(jcontent, "remoteMediaUrl", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(content, field);
    tMessage.content.remoteMediaUrl = jstringToString(env, str);

    env->DeleteLocalRef(str);

    //payload.localMediaPath = [NSString stringWithUTF8String:tMessage->content.localMediaPath.c_str()];
    field = env->GetFieldID(jcontent, "localMediaPath", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(content, field);
    tMessage.content.localMediaPath = jstringToString(env, str);


    //payload.mentionedType = tMessage->content.mentionedType;
    field = env->GetFieldID(jcontent, "mentionedType", "I");
    i = env->GetIntField(content, field);
    tMessage.content.mentionedType = i;


    //NSMutableArray *mentionedType = [[NSMutableArray alloc] init];
    //for (std::list<std::string>::const_iterator it = tMessage->content.mentionedTargets.begin(); it != tMessage->content.mentionedTargets.end(); it++) {
      //[mentionedType addObject:[NSString stringWithUTF8String:(*it).c_str()]];
    //}
    field = env->GetFieldID(jcontent, "mentionedTargets", "[Ljava/lang/String;");
    jobjectArray jstringArray = (jobjectArray)env->GetObjectField(content, field);
    tMessage.content.mentionedTargets = jarrayToStringList(env, jstringArray);
    env->DeleteLocalRef(jstringArray);


    env->DeleteLocalRef(str);

    return tMessage;
}

void convertMessageContent(JNIEnv *env, jobject content, mars::stn::TMessageContent &tcontent) {
    if(content == NULL) {
        return;
    }
    jclass jcontent = (jclass)g_objMessageContent;

    //payload.contentType = tMessage->content.type;
    jfieldID field = env->GetFieldID(jcontent, "type", "I");
    int i = env->GetIntField(content, field);
    tcontent.type = i;


    //payload.searchableContent = [NSString stringWithUTF8String:tMessage->content.searchableContent.c_str()];
    field = env->GetFieldID(jcontent, "searchableContent", "Ljava/lang/String;");
    jstring str = (jstring)env->GetObjectField(content, field);
    tcontent.searchableContent = jstringToString(env, str);

    env->DeleteLocalRef(str);

    //payload.pushContent = [NSString stringWithUTF8String:tMessage->content.pushContent.c_str()];
    field = env->GetFieldID(jcontent, "pushContent", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(content, field);
    tcontent.pushContent = jstringToString(env, str);

    env->DeleteLocalRef(str);

    //payload.content = [NSString stringWithUTF8String:tMessage->content.content.c_str()];
    field = env->GetFieldID(jcontent, "content", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(content, field);
    tcontent.content = jstringToString(env, str);

    env->DeleteLocalRef(str);

    //payload.binaryContent = [NSData dataWithBytes:tMessage->content.binaryContent.c_str() length:tMessage->content.binaryContent.length()];
    field = env->GetFieldID(jcontent, "binaryContent", "[B");
    jbyteArray jarray = (jbyteArray)env->GetObjectField(content, field);
    tcontent.binaryContent = jarrayToString(env, jarray);

    env->DeleteLocalRef(jarray);

    //payload.localContent = [NSString stringWithUTF8String:tMessage->content.localContent.c_str()];
    field = env->GetFieldID(jcontent, "localContent", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(content, field);
    tcontent.localContent = jstringToString(env, str);

    env->DeleteLocalRef(str);


    //payload.mediaType = (WFCCMediaType)tMessage->content.mediaType;
    field = env->GetFieldID(jcontent, "mediaType", "I");
    i = env->GetIntField(content, field);
    tcontent.mediaType = i;



    //payload.remoteMediaUrl = [NSString stringWithUTF8String:tMessage->content.remoteMediaUrl.c_str()];
    field = env->GetFieldID(jcontent, "remoteMediaUrl", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(content, field);
    tcontent.remoteMediaUrl = jstringToString(env, str);

    env->DeleteLocalRef(str);

    //payload.localMediaPath = [NSString stringWithUTF8String:tMessage->content.localMediaPath.c_str()];
    field = env->GetFieldID(jcontent, "localMediaPath", "Ljava/lang/String;");
    str = (jstring)env->GetObjectField(content, field);
    tcontent.localMediaPath = jstringToString(env, str);
    env->DeleteLocalRef(str);



    //payload.mentionedType = tMessage->content.mentionedType;
    field = env->GetFieldID(jcontent, "mentionedType", "I");
    i = env->GetIntField(content, field);
    tcontent.mentionedType = i;


    //NSMutableArray *mentionedType = [[NSMutableArray alloc] init];
    //for (std::list<std::string>::const_iterator it = tMessage->content.mentionedTargets.begin(); it != tMessage->content.mentionedTargets.end(); it++) {
      //[mentionedType addObject:[NSString stringWithUTF8String:(*it).c_str()]];
    //}
    field = env->GetFieldID(jcontent, "mentionedTargets", "[Ljava/lang/String;");
    jobjectArray jstringArray = static_cast<jobjectArray>(env->GetObjectField(content, field));
    tcontent.mentionedTargets = jarrayToStringList(env, jstringArray);
    env->DeleteLocalRef(jstringArray);
}


JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_sendMessage
		(JNIEnv *_env, jclass clz, jobject msg, jint expiredDuration, jobject callback) {
	mars::stn::TMessage tmsg;
	tmsg = convertMessage(_env, msg);

    mars::stn::sendMessage(tmsg, new IMSendMessageCallback(_env->NewGlobalRef(callback)), (int)expiredDuration);
}

// public static native void recallMessage(long messageUid, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_recallMessage
		(JNIEnv *_env, jclass clz, jlong messageUid, jobject callback) {
  mars::stn::recallMessage(messageUid, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

//public static native long insertMessage(ProtoMessage msg);
JNIEXPORT jlong JNICALL Java_com_tencent_mars_proto_ProtoLogic_insertMessage
		(JNIEnv *_env, jclass clz, jobject msg) {
	mars::stn::TMessage tmsg;
	tmsg = convertMessage(_env, msg);

    long msgId = mars::stn::MessageDB::Instance()->InsertMessage(tmsg);
    if(msgId > 0) {
        mars::stn::MessageDB::Instance()->updateConversationTimestamp(tmsg.conversationType, tmsg.target, tmsg.line, tmsg.timestamp);
    }

    return msgId;
}

//public static native void updateMessageContent(ProtoMessage msg);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_updateMessageContent
		(JNIEnv *_env, jclass clz, jobject msg) {
	mars::stn::TMessage tmsg;
	tmsg = convertMessage(_env, msg);

    mars::stn::MessageDB::Instance()->UpdateMessageContent(tmsg.messageId, tmsg.content);
}

jobject convertProtoUnreadCount(JNIEnv *env, const mars::stn::TUnreadCount &tCount) {
    jclass junread = (jclass)g_objUnreadCount;
    jobject unread = env->AllocObject(junread);
    SetObjectValue_Int(env, unread, junread, "setUnread", tCount.unread);
    SetObjectValue_Int(env, unread, junread, "setUnreadMention", tCount.unreadMention);
    SetObjectValue_Int(env, unread, junread, "setUnreadMentionAll", tCount.unreadMentionAll);
    return unread;
}

jobject convertProtoConversation(JNIEnv *env, const mars::stn::TConversation &tConv) {
    jclass jconv = (jclass)g_objConversationInfo;
    jobject obj = env->AllocObject(jconv);


    //info.conversation.type = (WFCCConversationType)tConv.conversationType;
    SetObjectValue_Int(env, obj, jconv, "setConversationType", tConv.conversationType);

    //info.conversation.target = [NSString stringWithUTF8String:tConv.target.c_str()];
    SetObjectValue_String(env, obj, jconv, "setTarget", tConv.target.c_str());

    //info.conversation.line = tConv.line;
    SetObjectValue_Int(env, obj, jconv, "setLine", tConv.line);

    //info.lastMessage = convertProtoMessage(&tConv.lastMessage);
    jobject lastMessage = convertProtoMessage(env, &tConv.lastMessage);
    SetObjectValue_Object(env, obj, jconv, "setLastMessage", lastMessage, "(Lcn/wildfirechat/model/ProtoMessage;)V");
    env->DeleteLocalRef(lastMessage);

    //info.draft = [NSString stringWithUTF8String:tConv.draft.c_str()];
    SetObjectValue_String(env, obj, jconv, "setDraft", tConv.draft.c_str());

    //info.timestamp = tConv.timestamp;
    SetObjectValue_LongLong(env, obj, jconv, "setTimestamp", tConv.timestamp);

    //info.unreadCount = [WFCCUnreadCount countOf:tConv.unreadCount.unread mention:tConv.unreadCount.unreadMention mentionAll:tConv.unreadCount.unreadMentionAll];
    jobject unread = convertProtoUnreadCount(env, tConv.unreadCount);
    SetObjectValue_Object(env, obj, jconv, "setUnreadCount", unread, "(Lcn/wildfirechat/model/ProtoUnreadCount;)V");
    env->DeleteLocalRef(unread);

    //info.isTop = tConv.isTop;
    SetObjectValue_Bool(env, obj, jconv, "setTop", tConv.isTop);

    //info.isSilent = tConv.isSilent;
    SetObjectValue_Bool(env, obj, jconv, "setSilent", tConv.isSilent);

    return obj;
}


//public static native ProtoConversationInfo[] getConversations(int[] conversationTypes, int[] lines);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getConversations
		(JNIEnv *_env, jclass clz, jintArray typeArray, jintArray lineArray) {

    std::list<int> types;
        int count = _env->GetArrayLength(typeArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return NULL;
        }

        jint *jtypes = _env->GetIntArrayElements(typeArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return NULL;
        }
        for (int i = 0; i < count; i++) {
            types.push_back((int)(jtypes[i]));
        }

    std::list<int> ls;
        count = _env->GetArrayLength(lineArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return NULL;
        }

        jtypes = _env->GetIntArrayElements(lineArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return NULL;
        }
        for (int i = 0; i < count; i++) {
            ls.push_back((int)(jtypes[i]));
        }

    std::list<mars::stn::TConversation> convers = mars::stn::MessageDB::Instance()->GetConversationList(types, ls);

    jobjectArray jo_array = _env->NewObjectArray(convers.size(), (jclass) g_objConversationInfo, 0);
    int i = 0;
    for (std::list<mars::stn::TConversation>::iterator it = convers.begin(); it != convers.end(); it++) {
        mars::stn::TConversation &tConv = *it;
        jobject conv = convertProtoConversation(_env, tConv);

        _env->SetObjectArrayElement(jo_array, i++, conv);
        _env->DeleteLocalRef(conv);
    }
    return jo_array;
}
//public static native ProtoConversationInfo getConversation(int conversationType, String target, int line);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getConversation
		(JNIEnv *_env, jclass clz, jint type, jstring target, jint line) {
    mars::stn::TConversation tConv = mars::stn::MessageDB::Instance()->GetConversation((int)type, jstringToString(_env, target), (long)line);
    jobject conv = convertProtoConversation(_env, tConv);
    return conv;
}

//public static native ProtoMessage[] getMessages(int conversationType, String target, int line, long fromIndex, boolean before, int count, String withUser);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getMessages
		(JNIEnv *_env, jclass clz, jint type, jstring target, jint line, jlong fromIndex, jboolean before, jint count, jstring withUser) {
	std::list<int> types;
    std::list<mars::stn::TMessage> messages = mars::stn::MessageDB::Instance()->GetMessages(int(type), jstringToString(_env, target), int(line), types, (bool)before, (int)count, (long)fromIndex, jstringToString(_env, withUser));
    return convertProtoMessageList(_env, messages);
}

//public static native void getRemoteMessages(int conversationType, String target, int line, long beforeMessageUid, int count, ILoadRemoteMessagesCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_getRemoteMessages
		(JNIEnv *_env, jclass clz, jint type, jstring target, jint line, jlong beforeMessageUid, jint count, jobject callback) {
	    mars::stn::TConversation conv;
        conv.target = jstringToString(_env, target);
        conv.line = line;
        conv.conversationType = type;

        mars::stn::loadRemoteMessages(conv, beforeMessageUid, count, new IMLoadRemoteMessagesCallback(_env->NewGlobalRef(callback)));
}

//public static native ProtoMessage getMessage(long messageId);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getMessage
		(JNIEnv *_env, jclass clz, jlong messageId) {
    mars::stn::TMessage message = mars::stn::MessageDB::Instance()->GetMessageById((long)messageId);
    return convertProtoMessage(_env, &message);
}
//public static native ProtoMessage getMessageByUid(long messageUid);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getMessageByUid
		(JNIEnv *_env, jclass clz, jlong messageUid) {
	mars::stn::TMessage message = mars::stn::MessageDB::Instance()->GetMessageByUid((long long)messageUid);
        return convertProtoMessage(_env, &message);
}


//public static native ProtoUnreadCount getUnreadCount(int conversationType, String target, int line);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getUnreadCount
		(JNIEnv *_env, jclass clz, jint type, jstring target, jint line) {
    mars::stn::TUnreadCount tcount = mars::stn::MessageDB::Instance()->GetUnreadCount((int)type, jstringToString(_env, target), (int)line);
    return convertProtoUnreadCount(_env, tcount);
}

//public static native ProtoUnreadCount getUnreadCountEx(int[] conversationTypes, int[] lines);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getUnreadCountEx
		(JNIEnv *_env, jclass clz, jintArray typeArray, jintArray lineArray) {
    std::list<int> types;
        int count = _env->GetArrayLength(typeArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return 0;
        }

        jint *jtypes = _env->GetIntArrayElements(typeArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return 0;
        }
        for (int i = 0; i < count; i++) {
            types.push_back((int)(jtypes[i]));
        }

    std::list<int> ls;
        count = _env->GetArrayLength(lineArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return 0;
        }

        jtypes = _env->GetIntArrayElements(lineArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return 0;
        }
        for (int i = 0; i < count; i++) {
            ls.push_back((int)(jtypes[i]));
        }

    mars::stn::TUnreadCount tcount = mars::stn::MessageDB::Instance()->GetUnreadCount(types, ls);
    return convertProtoUnreadCount(_env, tcount);
}

//public static native void clearUnreadStatus(int conversationType, String target, int line);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_clearUnreadStatus
		(JNIEnv *_env, jclass clz, jint type, jstring target, jint line) {
    mars::stn::MessageDB::Instance()->ClearUnreadStatus((int)type, jstringToString(_env, target), (int)line);
}

//public static native void clearAllUnreadStatus();
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_clearAllUnreadStatus
		(JNIEnv *_env, jclass clz) {
    mars::stn::MessageDB::Instance()->ClearAllUnreadStatus();
}

//public static native void clearMessages(int conversationType, String target, int line);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_clearMessages
        (JNIEnv *_env, jclass clz, jint type, jstring target, jint line) {
    mars::stn::MessageDB::Instance()->ClearMessages((int)type, jstringToString(_env, target), (int)line);
}

//public static native void setMediaMessagePlayed(long messageId);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_setMediaMessagePlayed
		(JNIEnv *_env, jclass clz, jlong messageId) {
    mars::stn::MessageDB::Instance()->updateMessageStatus(messageId, mars::stn::Message_Status_Played);
}
//public static native void removeConversation(int conversationType, String target, int line, boolean clearMsg);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_removeConversation
		(JNIEnv *_env, jclass clz, jint type, jstring target, jint line, jboolean clearMsg) {
    mars::stn::MessageDB::Instance()->RemoveConversation((int)type, jstringToString(_env, target), (int)line, (bool)clearMsg);
}


//public static native void setConversationTop(int conversationType, String target, int line, boolean top);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_setConversationTop
		(JNIEnv *_env, jclass clz, jint type, jstring target, jint line, jboolean top) {
    mars::stn::MessageDB::Instance()->updateConversationIsTop((int)type, jstringToString(_env, target), (int)line, (bool)top);
}

//public static native void setConversationDraft(int conversationType, String target, int line, String draft);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_setConversationDraft
		(JNIEnv *_env, jclass clz, jint type, jstring target, jint line, jstring draft) {
    mars::stn::MessageDB::Instance()->updateConversationDraft((int)type, jstringToString(_env, target), (int)line, jstringToString(_env, draft));
}

//public static native void setConversationSilent(int conversationType, String target, int line, boolean silent);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_setConversationSilent
		(JNIEnv *_env, jclass clz, jint type, jstring target, jint line, jboolean silent) {
    mars::stn::MessageDB::Instance()->updateConversationIsSilent((int)type, jstringToString(_env, target), (int)line, (bool)silent);
}

//public static native String getUserSetting(int scope, String key);
JNIEXPORT jstring JNICALL Java_com_tencent_mars_proto_ProtoLogic_getUserSetting
		(JNIEnv *_env, jclass clz, jint scope, jstring key) {
    std::string setting = mars::stn::MessageDB::Instance()->GetUserSetting((int)scope, jstringToString(_env, key));
    return cstring2jstring(_env, setting.c_str());
}

//public static native Map<String, String> getUserSettings(int scope);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getUserSettings
		(JNIEnv *_env, jclass clz, jint scope) {
    std::map<std::string, std::string> settings = mars::stn::MessageDB::Instance()->GetUserSettings(scope);
    jclass class_hashmap = _env->FindClass("java/util/HashMap");
    jmethodID hashmap_init = _env->GetMethodID(class_hashmap, "<init>", "()V");
    jobject HashMap = _env->NewObject(class_hashmap, hashmap_init, "");
    jmethodID HashMap_put = _env->GetMethodID(class_hashmap, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    for (std::map<std::string, std::string>::iterator it = settings.begin() ; it != settings.end(); it++) {
        jstring key = cstring2jstring(_env, it->first.c_str());
        jstring value = cstring2jstring(_env, it->second.c_str());
        _env->CallObjectMethod(HashMap, HashMap_put, key, value);
    }
    return HashMap;
}

//public static native void setUserSetting(int scope, String key, String value, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_setUserSetting
		(JNIEnv *_env, jclass clz, jint scope, jstring key, jstring value, jobject callback) {
    mars::stn::modifyUserSetting(scope, jstringToString(_env, key), jstringToString(_env, value), new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

//public static native void setDeviceToken(String appName, String token);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_setDeviceToken
		(JNIEnv *_env, jclass clz, jstring appName, jstring token, jint pushType) {
    mars::stn::setDeviceToken(jstringToString(_env, appName), jstringToString(_env, token), (int)pushType);
}

//public static native void setDNSResult(String[] serverIPs);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_setDNSResult
		(JNIEnv *_env, jclass clz, jobjectArray ipArray) {
    std::vector<std::string> ipList;
        int count = _env->GetArrayLength(ipArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
        }

        for (int i = 0; i < count; i++) {
            jstring jIp = (jstring)_env->GetObjectArrayElement(ipArray, i );
            ipList.push_back(ScopedJstring(_env, jIp).GetChar());
        }

    mars::stn::setDNSResult(ipList);
}

//public static native ProtoGroupInfo getGroupInfo(String groupId, boolean refresh);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getGroupInfo
		(JNIEnv *_env, jclass clz, jstring groupId, jboolean refresh) {
	mars::stn::TGroupInfo tgi = mars::stn::MessageDB::Instance()->GetGroupInfo(ScopedJstring(_env, groupId).GetChar(), refresh);
    if (!tgi.target.empty()) {
        jobject groupInfo = convertProtoGroupInfo(_env, tgi);
        return groupInfo;
    }
    return NULL;
}
//public static native void joinChatRoom(String chatRoomId, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_joinChatRoom(JNIEnv *_env, jclass clz, jstring chatroomId, jobject callback ) {
    mars::stn::joinChatroom(jstringToString(_env, chatroomId), new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}
//public static native void quitChatRoom(String chatRoomId, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_quitChatRoom(JNIEnv *_env, jclass clz, jstring chatroomId, jobject callback ) {
    mars::stn::quitChatroom(jstringToString(_env, chatroomId), new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

//public static native void getChatRoomInfo(String chatRoomId, long lastUpdateDt, IGetChatRoomInfoCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_getChatRoomInfo
		(JNIEnv *_env, jclass clz, jstring chatroomId, jlong updateDt, jobject callback ) {
    mars::stn::getChatroomInfo(jstringToString(_env, chatroomId), updateDt, new IMGetChatroomInfoCallback( (jstring)_env->NewGlobalRef(chatroomId), _env->NewGlobalRef(callback)));
}

//public static native void getChatRoomMembersInfo(String chatRoomId, int maxCount, IGetChatRoomMembersInfoCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_getChatRoomMembersInfo
		(JNIEnv *_env, jclass clz, jstring chatroomId, jint maxCount,jobject callback) {
    mars::stn::getChatroomMemberInfo(jstringToString(_env, chatroomId), maxCount, new IMGetChatroomMemberInfoCallback(_env->NewGlobalRef(callback)));
}

//public static native void registerMessageFlag(int contentType, int flag)
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_registerMessageFlag
		(JNIEnv *_env, jclass clz, jint contentType, jint flag) {
    mars::stn::MessageDB::Instance()->RegisterMessageFlag((int)contentType, (int)flag);
}


class IMSearchUserCallback : public mars::stn::SearchUserCallback {
private:
    jobject mObj;
public:
    IMSearchUserCallback(jobject obj) : mars::stn::SearchUserCallback(), mObj(obj) {}

    void onSuccess(const std::list<mars::stn::TUserInfo> &users, const std::string &keyword, int page) {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();
        jobjectArray jo_array = convertProtoUserInfoList(env, users);

        jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onSuccess", "([Lcn/wildfirechat/model/ProtoUserInfo;)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, jo_array);
             }
             env->DeleteLocalRef(cls);
         }
         env->DeleteLocalRef(jo_array);
        delete this;
    }
    void onFalure(int errorCode) {
         VarCache* cache_instance = VarCache::Singleton();
         ScopeJEnv scope_jenv(cache_instance->GetJvm());
         JNIEnv *env = scope_jenv.GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onFailure", "(I)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jint)errorCode);
             }
             env->DeleteLocalRef(cls);
         }
        delete this;
    }

    ~IMSearchUserCallback() {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();
        env->DeleteGlobalRef(mObj);
    }
};

//public static native void searchUser(String keyword, ISearchUserCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_searchUser
		(JNIEnv *_env, jclass clz, jstring keyword, jobject callback) {
    mars::stn::searchUser(ScopedJstring(_env, keyword).GetChar(), true, 0, new IMSearchUserCallback(_env->NewGlobalRef(callback)));
}

//public static native boolean isMyFriend(String userId);
JNIEXPORT jboolean JNICALL Java_com_tencent_mars_proto_ProtoLogic_isMyFriend
		(JNIEnv *_env, jclass clz, jstring userId)  {
    return mars::stn::MessageDB::Instance()->isMyFriend(ScopedJstring(_env, userId).GetChar());
}

//public static native String[] getMyFriendList(boolean refresh);
JNIEXPORT jobjectArray JNICALL Java_com_tencent_mars_proto_ProtoLogic_getMyFriendList
		(JNIEnv *_env, jclass clz, jboolean refresh) {
    std::list<std::string> friendList = mars::stn::MessageDB::Instance()->getMyFriendList(refresh);

    return convertStringList(_env, friendList);
}

//public static native boolean getFriendAlias(String userId);
JNIEXPORT jstring JNICALL Java_com_tencent_mars_proto_ProtoLogic_getFriendAlias
		(JNIEnv *_env, jclass clz, jstring userId)  {
    std::string alias =mars::stn::MessageDB::Instance()->GetFriendAlias(ScopedJstring(_env, userId).GetChar());
    return cstring2jstring(_env, alias.c_str());
}

//public static native boolean setFriendAlias(String userId, String alias, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_setFriendAlias
		(JNIEnv *_env, jclass clz, jstring userId, jstring alias, jobject callback)  {
    mars::stn::setFriendAlias(ScopedJstring(_env, userId).GetChar(),  ScopedJstring(_env, alias).GetChar(), new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

//    public static native boolean isBlackListed(String userId);
JNIEXPORT jboolean JNICALL Java_com_tencent_mars_proto_ProtoLogic_isBlackListed
		(JNIEnv *_env, jclass clz, jstring userId)  {
    return mars::stn::MessageDB::Instance()->isBlackListed(ScopedJstring(_env, userId).GetChar());
}


//    public static native String[] getBlackList(boolean refresh);
JNIEXPORT jobjectArray JNICALL Java_com_tencent_mars_proto_ProtoLogic_getBlackList
		(JNIEnv *_env, jclass clz, jboolean refresh) {
    std::list<std::string> friendList = mars::stn::MessageDB::Instance()->getBlackList(refresh);

    return convertStringList(_env, friendList);
}

//public static native void loadFriendRequestFromRemote();
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_loadFriendRequestFromRemote
		(JNIEnv *_env, jclass clz) {
    mars::stn::loadFriendRequestFromRemote();
}

jobjectArray convertProtoFriendRequest(JNIEnv *env, std::list<mars::stn::TFriendRequest> tRequests) {
    jobjectArray jo_array = env->NewObjectArray(tRequests.size(), (jclass) g_objFriendRequest, 0);
    int i = 0;
    for (std::list<mars::stn::TFriendRequest>::iterator it = tRequests.begin(); it != tRequests.end(); it++) {
        mars::stn::TFriendRequest *tRequest = &(*it);
        jclass jcls = (jclass)g_objFriendRequest;
        jobject obj = env->AllocObject(jcls);

//        request.direction = tRequest->direction;
        SetObjectValue_Int(env, obj, jcls, "setDirection", tRequest->direction);

//        request.target = [NSString stringWithUTF8String:tRequest->target.c_str()];
        SetObjectValue_String(env, obj, jcls, "setTarget", tRequest->target.c_str());

//        request.reason = [NSString stringWithUTF8String:tRequest->reason.c_str()];
        SetObjectValue_String(env, obj, jcls, "setReason", tRequest->reason.c_str());

//        request.status = tRequest->status;
        SetObjectValue_Int(env, obj, jcls, "setStatus", tRequest->status);

//        request.readStatus = tRequest->readStatus;
        SetObjectValue_Int(env, obj, jcls, "setReadStatus", tRequest->readStatus);

//        request.timestamp = tRequest->timestamp;
        SetObjectValue_Long(env, obj, jcls, "setTimestamp", tRequest->timestamp);

        env->SetObjectArrayElement(jo_array, i++, obj);
        env->DeleteLocalRef(obj);
    }
    return jo_array;
}

//public static native ProtoFriendRequest[] getFriendRequest(boolean incomming);
JNIEXPORT jobjectArray JNICALL Java_com_tencent_mars_proto_ProtoLogic_getFriendRequest
		(JNIEnv *_env, jclass clz, jboolean incomming) {
    std::list<mars::stn::TFriendRequest> tRequests = mars::stn::MessageDB::Instance()->getFriendRequest(incomming ? 1 : 0);
    return convertProtoFriendRequest(_env, tRequests);
}

//public static native void clearUnreadFriendRequestStatus();
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_clearUnreadFriendRequestStatus
		(JNIEnv *_env, jclass clz) {
    mars::stn::MessageDB::Instance()->clearUnreadFriendRequestStatus();
}

//public static native int getUnreadFriendRequestStatus();
JNIEXPORT jint JNICALL Java_com_tencent_mars_proto_ProtoLogic_getUnreadFriendRequestStatus
		(JNIEnv *_env, jclass clz) {
    return mars::stn::MessageDB::Instance()->unreadFriendRequest();
}

//public static native void removeFriend(String userId, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_removeFriend
		(JNIEnv *_env, jclass clz, jstring userId, jobject callback) {
    mars::stn::deleteFriend(ScopedJstring(_env, userId).GetChar(), new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

//public static native void sendFriendRequest(String userId, String reason, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_sendFriendRequest
		(JNIEnv *_env, jclass clz, jstring userId, jstring reason, jobject callback) {
    mars::stn::sendFriendRequest(ScopedJstring(_env, userId).GetChar(), ScopedJstring(_env, reason).GetChar(), new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

//public static native void handleFriendRequest(String userId, boolean accept, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_handleFriendRequest
		(JNIEnv *_env, jclass clz, jstring userId, jboolean accept, jobject callback) {
    mars::stn::handleFriendRequest(ScopedJstring(_env, userId).GetChar(), accept, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}


//    public static native void setBlackList(String userId, boolean isBlackListed, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_setBlackList
		(JNIEnv *_env, jclass clz, jstring userId, jboolean isBlackListed, jobject callback) {
    mars::stn::blackListRequest(ScopedJstring(_env, userId).GetChar(), isBlackListed, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}


//public static native void deleteFriend(String userId, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_deleteFriend
		(JNIEnv *_env, jclass clz, jstring userId, jobject callback) {
    mars::stn::deleteFriend(ScopedJstring(_env, userId).GetChar(), new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}


//public static native ProtoUserInfo getUserInfo(String userId, boolean refresh);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getUserInfo
		(JNIEnv *_env, jclass clz, jstring userId, jstring groupId, jboolean refresh) {
    mars::stn::TUserInfo tui = mars::stn::MessageDB::Instance()->getUserInfo(ScopedJstring(_env, userId).GetChar(), ScopedJstring(_env, groupId).GetChar(), refresh);
    if (!tui.uid.empty()) {
        jobject userInfo = convertProtoUserInfo(_env, tui);
        return userInfo;
    }
    return NULL;
}

//public static native ProtoUserInfo[] getUserInfos(List<String> userIds, String groupId);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getUserInfos
		(JNIEnv *_env, jclass clz, jobjectArray userIds, jstring groupId) {
    std::list<std::string> uids = jarrayToStringList(_env, userIds);
    std::list<mars::stn::TUserInfo> tuis = mars::stn::MessageDB::Instance()->getUserInfos(uids, ScopedJstring(_env, groupId).GetChar());

    int i = 0;
    jobjectArray jo_array = _env->NewObjectArray(tuis.size(), (jclass) g_objUserInfo, 0);
    for (std::list<mars::stn::TUserInfo>::iterator it = tuis.begin(); it != tuis.end(); it++) {
        jobject userInfo = convertProtoUserInfo(_env, *it);
        _env->SetObjectArrayElement(jo_array, i++, userInfo);
        _env->DeleteLocalRef(userInfo);
    }

    return jo_array;
}

class GeneralUpdateMediaCallback : public mars::stn::UpdateMediaCallback {
private:
    jobject mObj;
public:
  GeneralUpdateMediaCallback(jobject obj) : mars::stn::UpdateMediaCallback(), mObj(obj) {}

  void onSuccess(const std::string &remoteUrl) {
    JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
    jclass cls = env->GetObjectClass(mObj);
    if (cls != NULL) {
        jmethodID nMethodId = env->GetMethodID(cls, "onSuccess", "(Ljava/lang/String;)V");
        if (env->ExceptionCheck()) {
            printf("--%s:exception\n", __FUNCTION__);
            env->ExceptionClear();
        }
        if (nMethodId != NULL) {
            jstring jVal = cstring2jstring(env, remoteUrl.c_str());
            env->CallVoidMethod(mObj, nMethodId, jVal);
            env->DeleteLocalRef(jVal);
        }
        env->DeleteLocalRef(cls);
    }

    delete this;
  }

  void onFalure(int errorCode) {
     JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
     jclass cls = env->GetObjectClass(mObj);
     if (cls != NULL) {
        jmethodID nMethodId = env->GetMethodID(cls, "onFailure", "(I)V");
        if (env->ExceptionCheck()) {
            printf("--%s:exception\n", __FUNCTION__);
            env->ExceptionClear();
        }
        if (nMethodId != NULL) {
            env->CallVoidMethod(mObj, nMethodId, (jint)errorCode);
        }
        env->DeleteLocalRef(cls);
     }
     delete this;
  }

    void onProgress(int current, int total) {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();
        jclass cls = env->GetObjectClass(mObj);
        if (cls != NULL) {
            jmethodID nMethodId = env->GetMethodID(cls, "onProgress", "(JJ)V");
            if (env->ExceptionCheck()) {
                printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
            }
            if (nMethodId != NULL) {
                env->CallVoidMethod(mObj, nMethodId, (jlong)current, (jlong)total);
            }
            env->DeleteLocalRef(cls);
        }
    }
  ~GeneralUpdateMediaCallback() {
    JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
    env->DeleteGlobalRef(mObj);
  }
};

//public static native void uploadMedia(byte[] data, int mediaType, IUploadMediaCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_uploadMedia
		(JNIEnv *_env, jclass clz, jbyteArray jdata, jint mediaType, jobject callback) {
    jsize len  = _env->GetArrayLength(jdata);
    char* data = (char*)_env->GetByteArrayElements(jdata, 0);
    mars::stn::uploadGeneralMedia(std::string((char *)data, len), mediaType, new GeneralUpdateMediaCallback(_env->NewGlobalRef(callback)));
}

//public static native void modifyMyInfo(Map<Integer, String> values, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_modifyMyInfo
		(JNIEnv *_env, jclass clz, jobject values, jobject callback) {
    std::list<std::pair<int, std::string>> infos;

    jclass jmapclass = _env->FindClass("java/util/HashMap");
    jmethodID jkeysetmid = _env->GetMethodID(jmapclass, "keySet", "()Ljava/util/Set;");
    jmethodID jgetmid = _env->GetMethodID(jmapclass, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jobject jsetkey = _env->CallObjectMethod(values,jkeysetmid);
    jclass jsetclass = _env->FindClass("java/util/Set");
    jmethodID jtoArraymid = _env->GetMethodID(jsetclass, "toArray", "()[Ljava/lang/Object;");
    jobjectArray jobjArray = (jobjectArray)_env->CallObjectMethod(jsetkey,jtoArraymid);

    jclass jintegerclass = _env->FindClass("java/lang/Integer");
    jmethodID jintegervalue = _env->GetMethodID(jintegerclass, "intValue", "()I");

    if(jobjArray==NULL){
        printf("param is NULL");
    }

    jsize arraysize = _env->GetArrayLength(jobjArray);
    int i=0;
    for( i=0; i < arraysize; i++ ){
        jobject jkey = _env->GetObjectArrayElement(jobjArray, i );
        jstring jvalue = (jstring)_env->CallObjectMethod(values,jgetmid,jkey);
        jint ikey = _env->CallIntMethod(jkey,jintegervalue);
        infos.push_back(std::pair<int, std::string>(ikey, ScopedJstring(_env, jvalue).GetChar()));
    }

    mars::stn::modifyMyInfo(infos, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}


//public static native boolean deleteMessage(long messageId);
JNIEXPORT jboolean JNICALL Java_com_tencent_mars_proto_ProtoLogic_deleteMessage
		(JNIEnv *_env, jclass clz, jlong messageId) {
    return mars::stn::MessageDB::Instance()->DeleteMessage(messageId);
}

//public static native ProtoConversationSearchresult[] searchConversation(String keyword, int[] conversationTypes, int[] lines);
JNIEXPORT jobjectArray JNICALL Java_com_tencent_mars_proto_ProtoLogic_searchConversation
		(JNIEnv *_env, jclass clz, jstring keyword, jintArray typeArray, jintArray lineArray) {

    std::list<int> types;
        int count = _env->GetArrayLength(typeArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return 0;
        }

        jint *jtypes = _env->GetIntArrayElements(typeArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return 0;
        }
        for (int i = 0; i < count; i++) {
            types.push_back((int)(jtypes[i]));
        }

    std::list<int> ls;
        count = _env->GetArrayLength(lineArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return 0;
        }

        jtypes = _env->GetIntArrayElements(lineArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return 0;
        }
        for (int i = 0; i < count; i++) {
            ls.push_back((int)(jtypes[i]));
        }

    std::list<mars::stn::TConversationSearchresult> tresult = mars::stn::MessageDB::Instance()->SearchConversations(types, ls, ScopedJstring(_env, keyword).GetChar(), 50);
    jobjectArray jo_array = _env->NewObjectArray(tresult.size(), (jclass) g_objConversationSearchresult, 0);
    int i = 0;
    for (std::list<mars::stn::TConversationSearchresult>::iterator it = tresult.begin(); it != tresult.end(); it++) {
        jclass jcls = (jclass)g_objConversationSearchresult;
        jobject obj = _env->AllocObject(jcls);

        SetObjectValue_Int(_env, obj, jcls, "setConversationType", it->conversationType);
        SetObjectValue_String(_env, obj, jcls, "setTarget", it->target.c_str());
        SetObjectValue_Int(_env, obj, jcls, "setLine", it->line);

        jobject machedMsg = convertProtoMessage(_env, &(it->marchedMessage));
        SetObjectValue_Object(_env, obj, jcls, "setMarchedMessage", machedMsg, "(Lcn/wildfirechat/model/ProtoMessage;)V");
        _env->DeleteLocalRef(machedMsg);

        SetObjectValue_LongLong(_env, obj, jcls, "setTimestamp", it->timestamp);
        SetObjectValue_Int(_env, obj, jcls, "setMarchedCount", it->marchedCount);

        _env->SetObjectArrayElement(jo_array, i++, obj);
        _env->DeleteLocalRef(obj);
    }
    return jo_array;
}

//public static native void searchMessage(int conversationType, String target, int line, String keyword);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_searchMessage
		(JNIEnv *_env, jclass clz, jint type, jstring target, jint line, jstring keyword) {
    std::list<mars::stn::TMessage> tmessages = mars::stn::MessageDB::Instance()->SearchMessages(type, ScopedJstring(_env, target).GetChar(), line, ScopedJstring(_env, keyword).GetChar(), 500);
    return convertProtoMessageList(_env, tmessages);
}


//public static native ProtoUserInfo[] searchFriends(String keyword);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_searchFriends
		(JNIEnv *_env, jclass clz, jstring keyword) {
    std::list<mars::stn::TUserInfo> friends = mars::stn::MessageDB::Instance()->SearchFriends(ScopedJstring(_env, keyword).GetChar(), 50);
    return convertProtoUserInfoList(_env, friends);
}

//public static native ProtoGroupSearchResult[] searchGroups(String keyword);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_searchGroups
		(JNIEnv *_env, jclass clz, jstring keyword) {
    std::list<mars::stn::TGroupSearchResult> tresult = mars::stn::MessageDB::Instance()->SearchGroups(ScopedJstring(_env, keyword).GetChar(), 50);
    jobjectArray jo_array = _env->NewObjectArray(tresult.size(), (jclass) g_objGroupSearchResult, 0);
    int i = 0;
    for (std::list<mars::stn::TGroupSearchResult>::iterator it = tresult.begin(); it != tresult.end(); it++) {
        jclass jcls = (jclass)g_objGroupSearchResult;
        jobject obj = _env->AllocObject(jcls);

        jobject groupInfo = convertProtoGroupInfo(_env, it->groupInfo);
        SetObjectValue_Object(_env, obj, jcls, "setGroupInfo", groupInfo, "(Lcn/wildfirechat/model/ProtoGroupInfo;)V");
        _env->DeleteLocalRef(groupInfo);


        SetObjectValue_Int(_env, obj, jcls, "setMarchType", it->marchedType);


        jobjectArray memberlist = convertStringList(_env, it->marchedMemberNames);
        SetObjectValue_ObjectArray(_env, obj, jcls, "setMarchedMembers", memberlist, "([Ljava/lang/String;)V");
        _env->DeleteLocalRef(memberlist);

        _env->SetObjectArrayElement(jo_array, i++, obj);
        _env->DeleteLocalRef(obj);
    }
    return jo_array;
}

class IMCreateGroupCallback : public mars::stn::CreateGroupCallback {
private:
    jobject mObj;

public:
    IMCreateGroupCallback(jobject obj) : mars::stn::CreateGroupCallback(), mObj(obj) {};
    void onSuccess(std::string groupId) {
        JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
        jclass cls = env->GetObjectClass(mObj);
        if (cls != NULL) {
            jmethodID nMethodId = env->GetMethodID(cls, "onSuccess", "(Ljava/lang/String;)V");
            if (env->ExceptionCheck()) {
                printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
            }
            if (nMethodId != NULL) {
                jstring jVal = cstring2jstring(env, groupId.c_str());
                env->CallVoidMethod(mObj, nMethodId, jVal);
                env->DeleteLocalRef(jVal);
            }
            env->DeleteLocalRef(cls);
        }
        delete this;
    }

    void onFalure(int errorCode) {
         JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onFailure", "(I)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jint)errorCode);
             }
             env->DeleteLocalRef(cls);
         }
        delete this;
    }

    virtual ~IMCreateGroupCallback() {
        JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
        env->DeleteGlobalRef(mObj);
    }
};

//public static native void createGroup(String groupId, String groupName, String groupPortrait, String[] memberIds, int[] notifyLines, ProtoMessage notifyMsg, IGeneralCallback2 callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_createGroup
		(JNIEnv *_env, jclass clz, jstring groupId, jstring groupName, jstring groupPortrait, jobjectArray memberArray, jintArray lineArray, jobject notifyMsg, jobject callback) {
    std::list<std::string> memberList;
        int count = _env->GetArrayLength(memberArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return;
        }

        for (int i = 0; i < count; i++) {
            jstring jmemberId = (jstring)_env->GetObjectArrayElement(memberArray, i );
            memberList.push_back(ScopedJstring(_env, jmemberId).GetChar());
        }


    std::list<int> ls;
        count = _env->GetArrayLength(lineArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return ;
        }

        jint *jtypes = _env->GetIntArrayElements(lineArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return ;
        }
        for (int i = 0; i < count; i++) {
            ls.push_back((int)(jtypes[i]));
        }

    mars::stn::TMessageContent tcontent;
    convertMessageContent(_env, notifyMsg, tcontent);

    mars::stn::createGroup(ScopedJstring(_env, groupId).GetCharWithDefault(""), ScopedJstring(_env, groupName).GetCharWithDefault(""), ScopedJstring(_env, groupPortrait).GetCharWithDefault(""), memberList, ls, tcontent, new IMCreateGroupCallback(_env->NewGlobalRef(callback)));
}

//public static native void addMembers(String groupId, String[] memberIds, int[] notifyLines, ProtoMessage notifyMsg, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_addMembers
		(JNIEnv *_env, jclass clz, jstring groupId, jobjectArray memberArray, jintArray lineArray, jobject notifyMsg, jobject callback) {
    std::list<std::string> memberList;
        int count = _env->GetArrayLength(memberArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return;
        }

        for (int i = 0; i < count; i++) {
            jstring jmemberId = (jstring)_env->GetObjectArrayElement(memberArray, i );
            memberList.push_back(ScopedJstring(_env, jmemberId).GetChar());
        }


    std::list<int> ls;
        count = _env->GetArrayLength(lineArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return ;
        }

        jint *jtypes = _env->GetIntArrayElements(lineArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return ;
        }
        for (int i = 0; i < count; i++) {
            ls.push_back((int)(jtypes[i]));
        }

    mars::stn::TMessageContent tcontent;
    convertMessageContent(_env, notifyMsg, tcontent);

    mars::stn::addMembers(ScopedJstring(_env, groupId).GetChar(), memberList, ls, tcontent, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

//public static native void kickoffMembers(String groupId, String[] memberIds, int[] notifyLines, ProtoMessage notifyMsg, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_kickoffMembers
		(JNIEnv *_env, jclass clz, jstring groupId, jobjectArray memberArray, jintArray lineArray, jobject notifyMsg, jobject callback) {
    std::list<std::string> memberList;
        int count = _env->GetArrayLength(memberArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return;
        }

        for (int i = 0; i < count; i++) {
            jstring jmemberId = (jstring)_env->GetObjectArrayElement(memberArray, i );
            memberList.push_back(ScopedJstring(_env, jmemberId).GetChar());
        }


    std::list<int> ls;
        count = _env->GetArrayLength(lineArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return ;
        }

        jint *jtypes = _env->GetIntArrayElements(lineArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return ;
        }
        for (int i = 0; i < count; i++) {
            ls.push_back((int)(jtypes[i]));
        }

    mars::stn::TMessageContent tcontent;
    convertMessageContent(_env, notifyMsg, tcontent);

    mars::stn::kickoffMembers(ScopedJstring(_env, groupId).GetChar(), memberList, ls, tcontent, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}
//public static native void quitGroup(String groupId, int[] notifyLines, ProtoMessage notifyMsg, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_quitGroup
		(JNIEnv *_env, jclass clz, jstring groupId, jintArray lineArray, jobject notifyMsg, jobject callback) {
    std::list<int> ls;
        int count = _env->GetArrayLength(lineArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return ;
        }

        jint *jtypes = _env->GetIntArrayElements(lineArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return ;
        }
        for (int i = 0; i < count; i++) {
            ls.push_back((int)(jtypes[i]));
        }

    mars::stn::TMessageContent tcontent;
    convertMessageContent(_env, notifyMsg, tcontent);

    mars::stn::quitGroup(ScopedJstring(_env, groupId).GetChar(), ls, tcontent, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}
//public static native void dismissGroup(String groupId, int[] notifyLines, ProtoMessage notifyMsg, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_dismissGroup
		(JNIEnv *_env, jclass clz, jstring groupId, jintArray lineArray, jobject notifyMsg, jobject callback) {
    std::list<int> ls;
        int count = _env->GetArrayLength(lineArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return ;
        }

        jint *jtypes = _env->GetIntArrayElements(lineArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return ;
        }
        for (int i = 0; i < count; i++) {
            ls.push_back((int)(jtypes[i]));
        }

    mars::stn::TMessageContent tcontent;
    convertMessageContent(_env, notifyMsg, tcontent);

    mars::stn::dismissGroup(ScopedJstring(_env, groupId).GetChar(), ls, tcontent, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}
mars::stn::TGroupInfo convertGroupInfo(JNIEnv *env, jobject groupinfo) {
    mars::stn::TGroupInfo tInfo;
    jclass jcls = env->GetObjectClass(groupinfo);

    //tInfo.target = [groupInfo.target UTF8String];
    jfieldID field = env->GetFieldID(jcls, "target", "Ljava/lang/String;");
    jstring str = (jstring)env->GetObjectField(groupinfo, field);
    tInfo.target = jstringToString(env, str);
    env->DeleteLocalRef(str);


    //if (groupInfo.name) {
        //tInfo.name = [groupInfo.name UTF8String];
        field = env->GetFieldID(jcls, "name", "Ljava/lang/String;");
        str = (jstring)env->GetObjectField(groupinfo, field);
        tInfo.name = jstringToString(env, str);
        env->DeleteLocalRef(str);
    //}

    //if (groupInfo.portrait) {
        //tInfo.portrait = [groupInfo.portrait UTF8String];
        field = env->GetFieldID(jcls, "portrait", "Ljava/lang/String;");
        str = (jstring)env->GetObjectField(groupinfo, field);
        tInfo.portrait = jstringToString(env, str);
        env->DeleteLocalRef(str);
    //}
    //if (groupInfo.owner) {
        //tInfo.owner = [groupInfo.owner UTF8String];
        field = env->GetFieldID(jcls, "owner", "Ljava/lang/String;");
        str = (jstring)env->GetObjectField(groupinfo, field);
        tInfo.owner = jstringToString(env, str);
        env->DeleteLocalRef(str);
    //}
    //if (groupInfo.extra) {
        //tInfo.extra = std::string((char *)groupInfo.extra.bytes, groupInfo.extra.length);
        field = env->GetFieldID(jcls, "extra", "Ljava/lang/String;");
        str = (jstring)env->GetObjectField(groupinfo, field);
        tInfo.extra = jstringToString(env, str);
        env->DeleteLocalRef(str);
    //}

    return tInfo;
}

//public static native void modifyGroupInfo(String groupId, int modifyType, String newValue, int[] notifyLines, ProtoMessageContent notifyMsg, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_modifyGroupInfo
		(JNIEnv *_env, jclass clz, jstring groupId, jint modifyType, jstring newValue, jintArray lineArray, jobject notifyMsg, jobject callback) {
    std::list<int> ls;
        int count = _env->GetArrayLength(lineArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return ;
        }

        jint *jtypes = _env->GetIntArrayElements(lineArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return ;
        }
        for (int i = 0; i < count; i++) {
            ls.push_back((int)(jtypes[i]));
        }

    mars::stn::TMessageContent tcontent;
    convertMessageContent(_env, notifyMsg, tcontent);

    mars::stn::modifyGroupInfo(ScopedJstring(_env, groupId).GetChar(), (int)modifyType, ScopedJstring(_env, newValue).GetChar(), ls, tcontent, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

//public static native void modifyGroupAlias(String groupId, String newAlias, int[] notifyLines, ProtoMessage notifyMsg, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_modifyGroupAlias
		(JNIEnv *_env, jclass clz, jstring groupId, jstring newAlias, jintArray lineArray, jobject notifyMsg, jobject callback) {
    std::list<int> ls;
        int count = _env->GetArrayLength(lineArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return ;
        }

        jint *jtypes = _env->GetIntArrayElements(lineArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return ;
        }
        for (int i = 0; i < count; i++) {
            ls.push_back((int)(jtypes[i]));
        }

    mars::stn::TMessageContent tcontent;
    convertMessageContent(_env, notifyMsg, tcontent);

    mars::stn::modifyGroupAlias(ScopedJstring(_env, groupId).GetChar(), ScopedJstring(_env, newAlias).GetChar(), ls, tcontent, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

jobject convertProtoGroupMember(JNIEnv *env, const mars::stn::TGroupMember &tGroupMember) {
    jclass jgroupMember = (jclass)g_objGroupMember;
    jobject obj = env->AllocObject(jgroupMember);

    //member.groupId = [NSString stringWithUTF8String:it->groupId.c_str()];
    SetObjectValue_String(env, obj, jgroupMember, "setGroupId", tGroupMember.groupId.c_str());

    //member.memberId = [NSString stringWithUTF8String:it->memberId.c_str()];
    SetObjectValue_String(env, obj, jgroupMember, "setMemberId", tGroupMember.memberId.c_str());

    //member.alias = [NSString stringWithUTF8String:it->alias.c_str()];
    SetObjectValue_String(env, obj, jgroupMember, "setAlias", tGroupMember.alias.c_str());

    //member.type = (WFCCGroupMemberType)it->type;
    SetObjectValue_Int(env, obj, jgroupMember, "setType", tGroupMember.type);

    return obj;
}

//public static native ProtoGroupMember[] getGroupMembers(String groupId, boolean forceUpdate);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getGroupMembers
		(JNIEnv *_env, jclass clz, jstring groupId, jboolean forceUpdate) {
    std::list<mars::stn::TGroupMember> tmembers = mars::stn::MessageDB::Instance()->GetGroupMembers(ScopedJstring(_env, groupId).GetChar(), forceUpdate);
    jobjectArray jo_array = _env->NewObjectArray(tmembers.size(), (jclass) g_objGroupMember, 0);
    int i = 0;
    for(std::list<mars::stn::TGroupMember>::iterator it = tmembers.begin(); it != tmembers.end(); it++) {
        mars::stn::TGroupMember &tgm = *it;
        jobject gm = convertProtoGroupMember(_env, tgm);

        _env->SetObjectArrayElement(jo_array, i++, gm);
        _env->DeleteLocalRef(gm);
    }
    return jo_array;
}

//public static native ProtoGroupMember getGroupMember(String groupId, String memberId);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getGroupMember
		(JNIEnv *_env, jclass clz, jstring groupId, jstring memberId) {
    mars::stn::TGroupMember tmember = mars::stn::MessageDB::Instance()->GetGroupMember(ScopedJstring(_env, groupId).GetChar(), ScopedJstring(_env, memberId).GetChar());


        mars::stn::TGroupMember &tgm = tmember;
        jobject gm = convertProtoGroupMember(_env, tgm);


    return gm;
}

//public static native void transferGroup(String groupId, String newOwner, int[] notifyLines, ProtoMessage notifyMsg, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_transferGroup
		(JNIEnv *_env, jclass clz, jstring groupId, jstring newOwner, jintArray lineArray, jobject notifyMsg, jobject callback) {
    std::list<int> ls;
        int count = _env->GetArrayLength(lineArray);
        if (count == 0) {
            printf("--%s:idcnt", __FUNCTION__);
            return ;
        }

        jint *jtypes = _env->GetIntArrayElements(lineArray, NULL);
        if (jtypes == NULL) {
            printf("--%s:typeids", __FUNCTION__);
            return ;
        }
        for (int i = 0; i < count; i++) {
            ls.push_back((int)(jtypes[i]));
        }

    mars::stn::TMessageContent tcontent;
    convertMessageContent(_env, notifyMsg, tcontent);

    mars::stn::transferGroup(ScopedJstring(_env, groupId).GetChar(), ScopedJstring(_env, newOwner).GetChar(), ls, tcontent, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

class IMCreateChannelCallback : public mars::stn::CreateChannelCallback {
private:
    jobject mObj;

public:
    IMCreateChannelCallback(jobject obj) : mars::stn::CreateChannelCallback(), mObj(obj) {};
    void onSuccess(const mars::stn::TChannelInfo &channelInfo) {
        JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
        jclass cls = env->GetObjectClass(mObj);

        if (cls != NULL) {
            jmethodID nMethodId = env->GetMethodID(cls, "onSuccess", "(Lcn/wildfirechat/model/ProtoChannelInfo;)V");
            if (env->ExceptionCheck()) {
                printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
            }
            if (nMethodId != NULL) {
                jobject jVal = convertProtoChannelInfo(env, channelInfo);
                env->CallVoidMethod(mObj, nMethodId, jVal);
                env->DeleteLocalRef(jVal);
            }
            env->DeleteLocalRef(cls);
        }
        delete this;
    }

    void onFalure(int errorCode) {
         JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onFailure", "(I)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jint)errorCode);
             }
             env->DeleteLocalRef(cls);
         }
        delete this;
    }

    virtual ~IMCreateChannelCallback() {
        JNIEnv *env = ScopeJEnv(VarCache::Singleton()->GetJvm()).GetEnv();
        env->DeleteGlobalRef(mObj);
    }
};

//    - (void)createChannel:(NSString *)channelName
//    portrait:(NSString *)channelPortrait
//    status:(int)status
//    desc:(NSString *)desc
//    extra:(NSString *)extra
//    success:(void(^)(WFCCChannelInfo *channelInfo))successBlock
//    error:(void(^)(int error_code))errorBlock;
//    public static native void createChannel(String channelId, String channelName, String channelPortrait, int status, String desc, String extra, ICreateChannelCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_createChannel
		(JNIEnv *_env, jclass clz, jstring channelId, jstring channelName, jstring channelPortrait, jint status, jstring desc, jstring extra, jobject callback) {

//  mars::stn::createChannel("", [channelName UTF8String], [channelPortrait UTF8String], status, [desc UTF8String], [extra UTF8String], new IMCreateChannelCallback(successBlock, errorBlock));
    mars::stn::createChannel("", ScopedJstring(_env, channelName).GetCharWithDefault(""), ScopedJstring(_env, channelPortrait).GetCharWithDefault(""), status, ScopedJstring(_env, desc).GetCharWithDefault(""), ScopedJstring(_env, extra).GetCharWithDefault(""), "", "", new IMCreateChannelCallback(_env->NewGlobalRef(callback)));
}

//public static native void modifyChannelInfo(String channelId, int modifyType, String newValue, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_modifyChannelInfo
  		(JNIEnv *_env, jclass clz, jstring channelId, jint modifyType, jstring newValue, jobject callback) {

      mars::stn::modifyChannelInfo(ScopedJstring(_env, channelId).GetChar(), (int)modifyType, ScopedJstring(_env, newValue).GetChar(), new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

//- (WFCCChannelInfo *)getChannelInfo:(NSString *)channelId
//    refresh:(BOOL)refresh;
//    public static native ProtoChannelInfo getChannelInfo(String channelId, boolean refresh);
JNIEXPORT jobject JNICALL Java_com_tencent_mars_proto_ProtoLogic_getChannelInfo
		(JNIEnv *_env, jclass clz, jstring channelId, jboolean refresh) {
	mars::stn::TChannelInfo tgi = mars::stn::MessageDB::Instance()->GetChannelInfo(ScopedJstring(_env, channelId).GetChar(), refresh);
    if (!tgi.channelId.empty()) {
        jobject channelInfo = convertProtoChannelInfo(_env, tgi);
        return channelInfo;
    }
    return NULL;
}
class IMSearchChannelCallback : public mars::stn::SearchChannelCallback {
private:
    jobject mObj;
public:
    IMSearchChannelCallback(jobject obj) : mars::stn::SearchChannelCallback(), mObj(obj) {}

    void onSuccess(const std::list<mars::stn::TChannelInfo> &channels, const std::string &keyword) {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();

        jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onSuccess", "([Lcn/wildfirechat/model/ProtoChannelInfo;)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                 env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 jobjectArray jo_array = convertProtoChannelInfoList(env, channels);
                 env->CallVoidMethod(mObj, nMethodId, jo_array);
                 env->DeleteLocalRef(jo_array);
             }
             env->DeleteLocalRef(cls);
         }

        delete this;
    }
    void onFalure(int errorCode) {
         VarCache* cache_instance = VarCache::Singleton();
         ScopeJEnv scope_jenv(cache_instance->GetJvm());
         JNIEnv *env = scope_jenv.GetEnv();
         jclass cls = env->GetObjectClass(mObj);
         if (cls != NULL) {
             jmethodID nMethodId = env->GetMethodID(cls, "onFailure", "(I)V");
             if (env->ExceptionCheck()) {
                 printf("--%s:exception\n", __FUNCTION__);
                env->ExceptionClear();
             }
             if (nMethodId != NULL) {
                 env->CallVoidMethod(mObj, nMethodId, (jint)errorCode);
             }
             env->DeleteLocalRef(cls);
         }
        delete this;
    }

    ~IMSearchChannelCallback() {
        VarCache* cache_instance = VarCache::Singleton();
        ScopeJEnv scope_jenv(cache_instance->GetJvm());
        JNIEnv *env = scope_jenv.GetEnv();
        env->DeleteGlobalRef(mObj);
    }
};

//- (void)searchChannel:(NSString *)keyword success:(void(^)(NSArray<WFCCChannelInfo *> *machedChannels))successBlock error:(void(^)(int errorCode))errorBlock;
//    public static native void searchChannel(String keyword, ISearchChannelCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_searchChannel
		(JNIEnv *_env, jclass clz, jstring keyword, jobject callback) {
	//mars::stn::searchChannel([keyword UTF8String], YES, new IMSearchChannelCallback(successBlock, errorBlock));
    mars::stn::searchChannel(ScopedJstring(_env, keyword).GetChar(), true, new IMSearchChannelCallback(_env->NewGlobalRef(callback)));
}

//- (BOOL)isListened:(NSString *)channelId;
//    public static native boolean isListenedChannel(String channelId);
JNIEXPORT jboolean JNICALL Java_com_tencent_mars_proto_ProtoLogic_isListenedChannel
		(JNIEnv *_env, jclass clz, jstring channelId)  {
    std::string setting = mars::stn::MessageDB::Instance()->GetUserSetting((int)kUserSettingListenedChannels, jstringToString(_env, channelId));
    return setting == "1";
}

jobjectArray filterMap(JNIEnv *_env, std::map<std::string, std::string> settings, const std::string &value) {
    int size = 0;
    for (std::map<std::string, std::string>::iterator it = settings.begin() ; it != settings.end(); it++) {
        if(value == it->second) {
            size++;
        }
    }

    if(size == 0) {
        return NULL;
    }

    jobjectArray jo_array = _env->NewObjectArray(size, _env->FindClass("java/lang/String"), 0);
    int i = 0;

    for (std::map<std::string, std::string>::iterator it = settings.begin() ; it != settings.end(); it++) {
        if(value == it->second) {
            jstring str = cstring2jstring(_env, it->first.c_str());
            _env->SetObjectArrayElement(jo_array, i++, str);
            _env->DeleteLocalRef(str);
        }
    }

    return jo_array;
}

//
//- (void)listenChannel:(NSString *)channelId listen:(BOOL)listen success:(void(^)(void))successBlock error:(void(^)(int errorCode))errorBlock;
//    public static native void listenChannel(String channelId, boolean listen, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_listenChannel(JNIEnv *_env, jclass clz, jstring channelId, jboolean listen, jobject callback ) {
//mars::stn::listenChannel([channelId UTF8String], listen, new IMGeneralOperationCallback(successBlock, errorBlock));
    mars::stn::listenChannel(jstringToString(_env, channelId), (bool)listen, new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

//- (NSArray<NSString *> *)getMyChannels;
//    public static native String[] getMyChannels();
JNIEXPORT jobjectArray JNICALL Java_com_tencent_mars_proto_ProtoLogic_getMyChannels
		(JNIEnv *_env, jclass clz) {
    std::map<std::string, std::string> myChannelsMap = mars::stn::MessageDB::Instance()->GetUserSettings((int)kUserSettingMyChannels);

    return filterMap(_env, myChannelsMap, "1");
}

//
//- (NSArray<NSString *> *)getListenedChannels;
//    public static native String[] getListenedChannels();
JNIEXPORT jobjectArray JNICALL Java_com_tencent_mars_proto_ProtoLogic_getListenedChannels
		(JNIEnv *_env, jclass clz) {
    std::map<std::string, std::string> myChannelsMap = mars::stn::MessageDB::Instance()->GetUserSettings((int)kUserSettingListenedChannels);

    return filterMap(_env, myChannelsMap, "1");
}

//public static native void destoryChannel(String channelId, IGeneralCallback callback);
JNIEXPORT void JNICALL Java_com_tencent_mars_proto_ProtoLogic_destoryChannel(JNIEnv *_env, jclass clz, jstring channelId, jobject callback ) {
    mars::stn::destoryChannel(jstringToString(_env, channelId), new IMGeneralOperationCallback(_env->NewGlobalRef(callback)));
}

void SetObjectValue_Int(JNIEnv *&env, jobject &obj, jclass &cls,
                        const char *pszSetMethod, int nVal) {
    jmethodID methodId = env->GetMethodID(cls, pszSetMethod, "(I)V");
    if (env->ExceptionCheck()) {
        printf("--%s:exception\n", __FUNCTION__);
        env->ExceptionClear();
    }
    //if (methodId != NULL) {
        env->CallVoidMethod(obj, methodId, (jint) nVal);
    //} else
      //  printf("--method: %s not found", pszSetMethod);
}

void SetObjectValue_Bool(JNIEnv *&env, jobject &obj, jclass &cls,
                         const char *pszSetMethod, bool nVal) {
    jmethodID methodId = env->GetMethodID(cls, pszSetMethod, "(Z)V");
    if (env->ExceptionCheck()) {
        printf("--%s:exception\n", __FUNCTION__);
        env->ExceptionClear();
    }
    if (methodId != NULL) {
        env->CallVoidMethod(obj, methodId, (jboolean) nVal);
    } else
        printf("--method: %s not found", pszSetMethod);
}

void SetObjectValue_Long(JNIEnv *&env, jobject &obj, jclass &cls,
                         const char *pszSetMethod, long lVal) {
    jmethodID methodId = env->GetMethodID(cls, pszSetMethod, "(J)V");
    if (env->ExceptionCheck()) {
        printf("--%s:exception\n", __FUNCTION__);
        env->ExceptionClear();
    }
    if (methodId != NULL) {
        env->CallVoidMethod(obj, methodId, (jlong) lVal);
    } else
        printf("--method: %s not found", pszSetMethod);
}

void SetObjectValue_LongLong(JNIEnv *&env, jobject &obj, jclass &cls,
                             const char *pszSetMethod, long long lVal) {
    jmethodID methodId = env->GetMethodID(cls, pszSetMethod, "(J)V");
    if (env->ExceptionCheck()) {
        printf("--%s:exception\n", __FUNCTION__);
        env->ExceptionClear();
    }
    if (methodId != NULL) {
        env->CallVoidMethod(obj, methodId, (jlong) lVal);
    } else
        printf("--method: %s not found", pszSetMethod);
}

void SetObjectValue_String(JNIEnv *&env, jobject &obj, jclass &cls,
                           const char *pszSetMethod, const char *pszVal) {
    jmethodID methodId = env->GetMethodID(cls, pszSetMethod, "(Ljava/lang/String;)V");
    if (env->ExceptionCheck()) {
        printf("--%s:exception\n", __FUNCTION__);
        env->ExceptionClear();
    }
    if (methodId != NULL) {
        jstring jVal = cstring2jstring(env, pszVal);
        env->CallVoidMethod(obj, methodId, jVal);
        env->DeleteLocalRef(jVal);

    } else
        printf("--method: %s not found", pszSetMethod);
}

void SetObjectValue_ByteArray(JNIEnv *&env, jobject &obj, jclass &cls,
                              const char *pszSetMethod, const unsigned char *message, long nl) {
    jmethodID methodId = env->GetMethodID(cls, pszSetMethod, "([B)V");
    if (env->ExceptionCheck()) {
        printf("--%s:exception\n", __FUNCTION__);
        env->ExceptionClear();
    }
    if (methodId != NULL) {
        jbyte *by = (jbyte *) message;
        jbyteArray jMessage = env->NewByteArray(nl);
        env->SetByteArrayRegion(jMessage, 0, nl, by);
        env->CallVoidMethod(obj, methodId, jMessage);
        env->DeleteLocalRef(jMessage);
    } else
        printf("--method: %s not found", pszSetMethod);
}

void SetObjectValue_Object(JNIEnv *&env, jobject &obj, jclass &cls,
                           const char *pszSetMethod, jobject &jVal, const char *sign) {
    jmethodID methodId = env->GetMethodID(cls, pszSetMethod, sign);
    if (env->ExceptionCheck()) {
        printf("--%s:exception\n", __FUNCTION__);
        env->ExceptionClear();
    }
    if (methodId != NULL) {
        env->CallVoidMethod(obj, methodId, jVal);
    } else
        printf("--method: %s not found", pszSetMethod);
}

void SetObjectValue_ObjectArray(JNIEnv *&env, jobject &obj, jclass &cls,
                           const char *pszSetMethod, jobjectArray &jVal, const char *sign) {
    jmethodID methodId = env->GetMethodID(cls, pszSetMethod, sign);
    if (env->ExceptionCheck()) {
        printf("--%s:exception\n", __FUNCTION__);
        env->ExceptionClear();
    }
    if (methodId != NULL) {
        env->CallVoidMethod(obj, methodId, jVal);
    } else
        printf("--method: %s not found", pszSetMethod);
}

// fix bug: JNI WARNING: NewStringUTF input is not valid Modified UTF-8
// FYI: https://stackoverflow.com/questions/12127817/android-ics-4-0-ndk-newstringutf-is-crashing-down-the-app
jstring cstring2jstring(JNIEnv *env, const char* str){
//    return env->NewStringUTF(str.c_str());
    if(str == NULL){
      return NULL;
    }
    jbyteArray array = env->NewByteArray(strlen(str));
    env->SetByteArrayRegion(array, 0, strlen(str), (const jbyte*)str);
    jstring strEncode = env->NewStringUTF("UTF-8");
    jclass cls = env->FindClass("java/lang/String");
    jmethodID ctor = env->GetMethodID(cls, "<init>", "([BLjava/lang/String;)V");
    jstring object = (jstring) env->NewObject(cls, ctor, array, strEncode);

    env->DeleteLocalRef(array);
    env->DeleteLocalRef(strEncode);
    env->DeleteLocalRef(cls);

    return object;
}

#ifdef __cplusplus
}
#endif

void ExportProto() {

}
