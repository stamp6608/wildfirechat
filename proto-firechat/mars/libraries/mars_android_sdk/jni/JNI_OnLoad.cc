
#include "comm/jni/util/JNI_OnLoad.h"
#include <android/log.h>
#include <pthread.h>
#include "comm/jni/util/var_cache.h"
#include "comm/jni/util/scope_jenv.h"

pthread_key_t g_env_key;

static void __DetachCurrentThread(void* a) {
    if (NULL != VarCache::Singleton()->GetJvm()) {
        VarCache::Singleton()->GetJvm()->DetachCurrentThread();
    }
}

extern "C" {


jobject g_objProtoLogic = 0;
jobject g_objConversationInfo = 0;
jobject g_objConversationSearchresult = 0;
jobject g_objGroupSearchResult = 0;
jobject g_objFriendRequest = 0;
jobject g_objGroupInfo = 0;
jobject g_objChannelInfo = 0;
jobject g_objChatRoomInfo = 0;
jobject g_objChatRoomMembersInfo = 0;
jobject g_objGroupMember = 0;
jobject g_objMessage = 0;
jobject g_objMessageContent = 0;
jobject g_objUserInfo = 0;
jobject g_objUnreadCount = 0;


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    
    if (0 != pthread_key_create(&g_env_key, __DetachCurrentThread)) {
        __android_log_print(ANDROID_LOG_ERROR, "MicroMsg", "create g_env_key fail");
        return(-1);
    }
    
    ScopeJEnv jenv(jvm);
    VarCache::Singleton()->SetJvm(jvm);


    LoadClass(jenv.GetEnv());
    LoadStaticMethod(jenv.GetEnv());
    LoadMethod(jenv.GetEnv());

    std::vector<JniOnload_t>& ref = BOOT_REGISTER_CONTAINER<JniOnload_t>() ;
    for (std::vector<JniOnload_t>::const_iterator it= ref.begin(); it!=ref.end(); ++it)
    {
    	it->func(jvm, reserved);
    }


    JNIEnv *env = jenv.GetEnv();

    jclass cls = env->FindClass("com/tencent/mars/proto/ProtoLogic");
        if (cls) {
            g_objProtoLogic = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }

    cls = env->FindClass("cn/wildfirechat/model/ProtoConversationInfo");
        if (cls) {
            g_objConversationInfo = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }

    cls = env->FindClass("cn/wildfirechat/model/ProtoUnreadCount");
        if (cls) {
            g_objUnreadCount = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }

    cls = env->FindClass("cn/wildfirechat/model/ProtoConversationSearchresult");
        if (cls) {
            g_objConversationSearchresult = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }

    cls = env->FindClass("cn/wildfirechat/model/ProtoGroupSearchResult");
        if (cls) {
            g_objGroupSearchResult = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }


    cls = env->FindClass("cn/wildfirechat/model/ProtoFriendRequest");
        if (cls) {
            g_objFriendRequest = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }
    cls = env->FindClass("cn/wildfirechat/model/ProtoGroupInfo");
        if (cls) {
            g_objGroupInfo = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }
    cls = env->FindClass("cn/wildfirechat/model/ProtoChannelInfo");
        if (cls) {
            g_objChannelInfo = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }
    cls = env->FindClass("cn/wildfirechat/model/ProtoChatRoomInfo");
        if (cls) {
            g_objChatRoomInfo = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }
    cls = env->FindClass("cn/wildfirechat/model/ProtoChatRoomMembersInfo");
        if (cls) {
            g_objChatRoomMembersInfo = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }
    cls = env->FindClass("cn/wildfirechat/model/ProtoGroupMember");
        if (cls) {
            g_objGroupMember = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }
    cls = env->FindClass("cn/wildfirechat/model/ProtoMessage");
        if (cls) {
            g_objMessage = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }
    cls = env->FindClass("cn/wildfirechat/model/ProtoMessageContent");
        if (cls) {
            g_objMessageContent = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }
    cls = env->FindClass("cn/wildfirechat/model/ProtoUserInfo");
        if (cls) {
            g_objUserInfo = env->NewGlobalRef(cls);
            env->DeleteLocalRef(cls);
        } else {
            printf("--ProtoLogic class");
            return -1;
        }
	
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
    VarCache* cache_instance = VarCache::Singleton();
    ScopeJEnv scope_jenv(cache_instance->GetJvm());
    JNIEnv *env = scope_jenv.GetEnv();

    if(g_objProtoLogic != 0) {
        env->DeleteGlobalRef(g_objProtoLogic);
        g_objProtoLogic = 0;
    }
    if(g_objConversationInfo != 0) {
        env->DeleteGlobalRef(g_objConversationInfo);
        g_objConversationInfo = 0;
    }

    if(g_objUnreadCount != 0) {
        env->DeleteGlobalRef(g_objUnreadCount);
        g_objUnreadCount = 0;
    }

    if(g_objConversationSearchresult != 0) {
        env->DeleteGlobalRef(g_objConversationSearchresult);
        g_objConversationSearchresult = 0;
    }
    if(g_objGroupSearchResult != 0) {
            env->DeleteGlobalRef(g_objGroupSearchResult);
            g_objGroupSearchResult = 0;
    }
    if(g_objFriendRequest != 0) {
        env->DeleteGlobalRef(g_objFriendRequest);
        g_objFriendRequest = 0;
    }
    if(g_objGroupInfo != 0) {
        env->DeleteGlobalRef(g_objGroupInfo);
        g_objGroupInfo = 0;
    }
    if(g_objChannelInfo != 0) {
        env->DeleteGlobalRef(g_objChannelInfo);
        g_objChannelInfo = 0;
    }
    if(g_objGroupMember != 0) {
        env->DeleteGlobalRef(g_objGroupMember);
        g_objGroupMember = 0;
    }
    if(g_objMessage != 0) {
        env->DeleteGlobalRef(g_objMessage);
        g_objMessage = 0;
    }
    if(g_objMessageContent != 0) {
        env->DeleteGlobalRef(g_objMessageContent);
        g_objMessageContent = 0;
    }
    if(g_objUserInfo != 0) {
        env->DeleteGlobalRef(g_objUserInfo);
        g_objUserInfo = 0;
    }

    VarCache::Release();
}

}
