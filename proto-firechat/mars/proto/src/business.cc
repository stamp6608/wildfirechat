//
//  business.cpp
//  proto
//
//  Created by WF Chat on 2017/11/4.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "business.h"

#include <sys/time.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <iterator>
#include <fstream>
#include <sstream>

#include "stn/src/net_core.h"//一定要放这里，Mac os 编译
#include "stn/src/net_source.h"

#include "mars/proto/src/Proto/group.h"
#include "mars/proto/src/Proto/conversation.h"
#include "mars/proto/src/Proto/message.h"
#include "mars/proto/src/Proto/messagecontent.h"
#include "mars/proto/src/Proto/create_group_request.h"
#include "mars/proto/src/Proto/add_group_member_request.h"
#include "mars/proto/src/Proto/remove_group_member_request.h"
#include "mars/proto/src/Proto/quit_group_request.h"
#include "mars/proto/src/Proto/upload_device_token_request.h"
#include "mars/proto/src/Proto/dismiss_group_request.h"
#include "mars/proto/src/Proto/modify_group_alias.h"
#include "mars/proto/src/Proto/modify_group_info.h"
#include "mars/proto/src/Proto/id_buf.h"
#include "mars/proto/src/Proto/int64_buf.h"
#include "mars/proto/src/Proto/id_list_buf.h"
#include "mars/proto/src/Proto/pull_group_info_result.h"
#include "mars/proto/src/Proto/pull_group_member.h"
#include "mars/proto/src/Proto/get_upload_token_result.h"
#include "mars/proto/src/Proto/transfer_group_request.h"
#include "mars/proto/src/Proto/pull_user.h"
#include "mars/proto/src/Proto/black_user_request.h"
#include "mars/proto/src/Proto/modify_my_info_request.h"
#include "mars/proto/src/Proto/add_friends_request.h"
#include "mars/proto/src/Proto/handle_friends_request.h"
#include "mars/proto/src/Proto/pull_group_member.h"
#include "mars/proto/src/Proto/modify_user_setting_request.h"
#include "mars/proto/src/Proto/route_response.h"
#include "mars/proto/src/Proto/get_chatroom_member_info_request.h"
#include "mars/proto/src/Proto/get_chatroom_member_info_result.h"
#include "mars/proto/src/Proto/get_chatroom_info_request.h"
#include "mars/proto/src/Proto/get_chatroom_info_result.h"
#include "mars/proto/src/Proto/channel_info.h"
#include "mars/proto/src/Proto/modify_channel_info.h"
#include "mars/proto/src/Proto/transfer_channel_request.h"
#include "mars/proto/src/Proto/listen_channel_request.h"
#include "mars/proto/src/Proto/pull_channel_info_request.h"
#include "mars/proto/src/Proto/search_channel_result.h"
#include "mars/proto/src/Proto/route_request.h"
#include "mars/proto/src/Proto/version.h"
#include "mars/proto/src/Proto/get_friends_result.h"
#include "mars/proto/src/Proto/friend_request.h"
#include "mars/proto/src/Proto/get_friend_request_result.h"
#include "mars/proto/src/Proto/search_user_request.h"
#include "mars/proto/src/Proto/search_user_result.h"
#include "mars/proto/src/Proto/load_remote_messages.h"
#include "mars/proto/src/Proto/notify_and_pull_message.h"
#include "mars/proto/stn_callback.h"
#include "mars/proto/MessageDB.h"
#include "mars/proto/src/DB2.h"
#include "mars/proto/src/libemqtt.h"
#include "mars/baseevent/base_logic.h"
#include "mars/app/app.h"
#include <iostream>
#include "comm/crypt/ibase64.h"

#if WFCHAT_PROTO_SERIALIZABLE
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#endif //WFCHAT_PROTO_SERIALIZABLE

extern unsigned char * decrypt_data(const unsigned char* data, unsigned int data_length, unsigned int *output_length, bool rootKey, bool checkTime);
extern unsigned char use_key[16];


namespace mars{
    namespace stn{


const std::string sendMessageTopic = "MS";
const std::string recallMessageTopic = "MR";
//const std::string pullMessageTopic = "MP"; //defined in another file
//const std::string notifyMessageTopic = "MN";
//const std::string getUserSettingTopic = "UG";
//const std::string notifyFriendTopic = "FN";
//const std::string notifyFriendRequestTopic = "FRN";
//const std::string notifySettingTopic = "SN";
const std::string putUserSettingTopic = "UP";
const std::string createGroupTopic = "GC";
const std::string addGroupMemberTopic = "GAM";
const std::string kickoffGroupMemberTopic = "GKM";
const std::string quitGroupTopic = "GQ";
const std::string dismissGroupTopic = "GD";
const std::string modifyGroupInfoTopic = "GMI";
const std::string modifyGroupAliasTopic = "GMA";
const std::string getGroupInfoTopic = "GPGI";
const std::string getUserInfoTopic = "UPUI";
const std::string getGroupMemberTopic = "GPGM";
const std::string transferGroupTopic = "GTG";
const std::string getQiniuUploadTokenTopic = "GQNUT";
const std::string modifyMyInfoTopic = "MMI";
const std::string AddFriendRequestTopic = "FAR";
const std::string HandleFriendRequestTopic = "FHR";
const std::string DeleteFriendTopic = "FDL";
const std::string SetFriendAliasTopic = "FALS";
const std::string friendRequestUnreadSyncTopic = "FRUS";
const std::string BlackListUserTopic = "BLU";
const std::string UploadDeviceTokenTopic = "UDT";
const std::string friendPullTopic = "FP";
const std::string friendRequestPullTopic = "FRP";
const std::string userSearchTopic = "US";
const std::string JoinChatroomTopic = "CRJ";
const std::string QuitChatroomTopic = "CRQ";
const std::string GetChatroomInfoTopic = "CRI";
const std::string GetChatroomMemberTopic = "CRMI";
const std::string RouteTopic = "ROUTE";
const std::string createChannelTopic = "CHC";
const std::string modifyChannelInfoTopic = "CHMI";
const std::string transferChannelInfoTopic = "CHT";
const std::string destoryChannelInfoTopic = "CHD";
const std::string channelSearchTopic = "CHS";
const std::string channelListenTopic = "CHL";
const std::string channelPullTopic = "CHP";
const std::string loadRemoteMessagesTopic = "LRM";
        
extern std::string gRouteHost;
extern int gRoutePort;
extern std::string gHost;
std::string g_chatroomId = "";

        static bool isRoutring = false;

class BusinessRouteCallback : public MQTTPublishCallback {
public:
    BusinessRouteCallback() : MQTTPublishCallback()  {}
    void onSuccess(const unsigned char* data, size_t len) {
        RouteResponse response;
        if (len > 0 && response.unserializeFromPBData((const void*)data, (int)len)) {
            std::vector<std::string> hosts;
            hosts.push_back(response.host);

            StnCallBack::Instance()->mAuthed = true;
            std::vector<uint16_t> longLinkPorts;
            longLinkPorts.push_back(response.longPort);

            NetSource::SetShortlink(response.shortPort, "");
            NetSource::SetLongLink(hosts, longLinkPorts, "");

            gHost = response.host;

            UserServerAddress us;
            us.host = gHost;
            us.longLinkPort = response.longPort;
            us.shortLinkPort = response.shortPort;

            DB2::Instance()->UpdateUserServerAddress(gUserId, us);

            MakesureLonglinkConnected();
            mars::baseevent::OnForeground(true);

            //if current use info is null that means he/she first time login, cache friend list for he/she.
            TUserInfo tuserInfo = MessageDB::Instance()->getUserInfo(mars::app::GetAccountUserName(), "", false);
            if (tuserInfo.uid.empty()) {
                MessageDB::Instance()->getMyFriendList(false);
            }
        } else {
            StnCallBack::Instance()->updateConnectionStatus(kConnectionStatusUnconnected);
        }
        delete this;
    }

    void onFalure(int errorCode) {
          ConnectionStatus newStatus = kConnectionStatusUnconnected;
        if (errorCode > 0) {
            if (errorCode == kEcServerTokenIncorrect) {
                newStatus = kConnectionStatusTokenIncorrect;
            } else if(errorCode == kEcServerUserIsBlocked) {
                newStatus = kConnectionStatusRejected;
            } else if(errorCode == kEcServerSecrectKeyMismatch) {
                newStatus = kConnectionStatusSecretKeyMismatch;
            }
        }

        StnCallBack::Instance()->updateConnectionStatus(newStatus);
        delete this;
    }
    virtual ~BusinessRouteCallback() {
        isRoutring = false;
    }
};
        void publishTask(const char* data, size_t length, MQTTPublishCallback *callback, const std::string &topic, bool orderly, bool shortOnly = false, bool isRoute = false, const std::string &host = "") {
            mars::stn::MQTTPublishTask *publishTask = new mars::stn::MQTTPublishTask(callback);
            publishTask->topic = topic;
            publishTask->length = length;
            publishTask->body = new unsigned char[publishTask->length];
            memcpy(publishTask->body, data, publishTask->length);

            if(!gToken.empty()) {
                publishTask->channel_select = ChannelType_All;
                if(!orderly) {
                    publishTask->channel_strategy = Task::kChannelFastStrategy;
                } else {
                    publishTask->channel_select = ChannelType_LongConn;
                }
                if (shortOnly) {
                    publishTask->channel_select = ChannelType_ShortConn;
                }

                if (isRoute) {
                    publishTask->need_authed = false;
                    publishTask->isRoute = true;
                    publishTask->cgi = "/route";
                    publishTask->channel_select = ChannelType_ShortConn;
                    NetSource::SetShortlink(gRoutePort, "");
                } else {
                    publishTask->cgi = "/im";
                }


                if (host.empty()) {
                    publishTask->shortlink_host_list.push_back(gHost);
                } else {
                    publishTask->shortlink_host_list.push_back(host);
                }

            }
            publishTask->limit_frequency = false;
            mars::stn::StartTask(*publishTask);
        }

        void RequestRoute(bool force) {
            if (isRoutring || gRouteHost.empty() || gToken.empty()) {
                return;
            }

            if (!force) {
                UserServerAddress us = DB2::Instance()->GetUserServerAddress(gUserId);
                if(us.host.empty() || us.longLinkPort <= 0 || us.shortLinkPort <= 0 || ((int64_t)time(NULL) - us.updateDt) > 43200) { //3600s * 12h = 43200
                    force = true;
                }
            }

            if (force) {
                isRoutring = true;
                RouteRequest request;
                app::DeviceInfo deviceInfo = app::GetDeviceInfo();
                request.app = deviceInfo.packagename;
                request.carrierName = deviceInfo.carriername;
                request.deviceName = deviceInfo.device;
                request.deviceVersion = deviceInfo.deviceversion;
                request.language = deviceInfo.language;
                request.phoneName = deviceInfo.phonename;
                request.platform = deviceInfo.platform;
                request.pushType = deviceInfo.pushtype;
                request.appversion = deviceInfo.appversion;
                request.sdkversion = deviceInfo.sdkversion;

                std::string data = request.serializeToPBData();
                publishTask(data.c_str(), data.length(), new BusinessRouteCallback(), RouteTopic, false, true, true, gRouteHost);
            }

        }

void (*Connect)(const std::string& host, uint16_t shortLinkPort)
= [](const std::string& host, uint16_t shortLinkPort) {
    gRouteHost = host;
    gRoutePort = shortLinkPort;

    NetSource::SetShortlink(shortLinkPort, "");

    UserServerAddress us = DB2::Instance()->GetUserServerAddress(gUserId);
    if(!us.host.empty() && us.longLinkPort > 0 && us.shortLinkPort > 0) {
        std::vector<std::string> hosts;
        hosts.push_back(us.host);
        StnCallBack::Instance()->mAuthed = true;
        std::vector<uint16_t> longLinkPorts;
        longLinkPorts.push_back(us.longLinkPort);

        NetSource::SetShortlink(us.shortLinkPort, "");
        NetSource::SetLongLink(hosts, longLinkPorts, "");
        gHost = us.host;

        MakesureLonglinkConnected();

        //首次连接如果离上次route超过1小时，强制刷新route信息。
        //在前后台切换或者网络切换时，超过12小时，强制刷新route信息。
        //你问我为啥是这个值，我也不知道！！！
        if(((int64_t)time(NULL) - us.updateDt) > 3600) {
            RequestRoute(true);
        }
    }

    mars::baseevent::OnForeground(true);
};

class GeneralOperationPublishCallback : public MQTTPublishCallback {
public:
    GeneralOperationPublishCallback(GeneralOperationCallback *cb) : MQTTPublishCallback(), callback(cb) {}
    GeneralOperationCallback *callback;
    void onSuccess(const unsigned char* data, size_t len) {
        if(callback)
            callback->onSuccess();
        delete this;
    };
    void onFalure(int errorCode) {
        if(callback)
            callback->onFalure(errorCode);
        delete this;
    };
    virtual ~GeneralOperationPublishCallback() {

    }
};

        class RecallMessagePublishCallback : public MQTTPublishCallback {
        public:
            RecallMessagePublishCallback(GeneralOperationCallback *cb, long long messageUid) : MQTTPublishCallback(), callback(cb), uid(messageUid) {}
            GeneralOperationCallback *callback;
            long long uid;
            void onSuccess(const unsigned char* data, size_t len) {
                TMessage tmsg = MessageDB::Instance()->GetMessageByUid(uid);
                //注意：在proto层收到撤回命令或主动撤回成功会直接更新被撤回的消息，如果修改这里，需要同步修改client的encode&decode
                if(tmsg.messageId) {
                    TMessageContent tcontent;
                    tcontent.content = app::GetAccountUserName();

                    std::stringstream stream;
                    stream << uid;

                    tcontent.binaryContent = stream.str();
                    tcontent.type = 80;
                    MessageDB::Instance()->UpdateMessageContent(tmsg.messageId, tcontent);
                }

                if(callback)
                    callback->onSuccess();
                delete this;
            };
            void onFalure(int errorCode) {
                if(callback)
                    callback->onFalure(errorCode);
                delete this;
            };
            virtual ~RecallMessagePublishCallback() {

            }
        };

        class GetChatroomInfoPublishCallback : public MQTTPublishCallback {
        public:
            GetChatroomInfoPublishCallback(GetChatroomInfoCallback *cb) : MQTTPublishCallback(), callback(cb) {}
            GetChatroomInfoCallback *callback;
            void onSuccess(const unsigned char* data, size_t len) {
                if (len > 0) {
                    ChatroomInfo result;
                    if (result.unserializeFromPBData((const void*)data, (int)len)) {
                        TChatroomInfo tinfo;
                        tinfo.title = result.title;
                        tinfo.desc = result.desc;
                        tinfo.portrait = result.portrait;
                        tinfo.memberCount = result.memberCount;
                        tinfo.createDt = result.createDt;
                        tinfo.updateDt = result.updateDt;
                        tinfo.extra = result.extra;
                        tinfo.state = (int)result.state;

                        if(callback)
                            callback->onSuccess(tinfo);
                        delete this;
                        return;
                    }
                }

                if(callback)
                    callback->onFalure(kEcProtoCorruptData);
                delete this;
            };
            void onFalure(int errorCode) {
                if(callback)
                    callback->onFalure(errorCode);
                delete this;
            };
            virtual ~GetChatroomInfoPublishCallback() {

            }
        };

        class GetChatroomMemberInfoPublishCallback : public MQTTPublishCallback {
        public:
            GetChatroomMemberInfoPublishCallback(GetChatroomMemberInfoCallback *cb) : MQTTPublishCallback(), callback(cb) {}
            GetChatroomMemberInfoCallback *callback;
            void onSuccess(const unsigned char* data, size_t len) {
                if (len > 0) {
                    ChatroomMemberInfo result;
                    if (result.unserializeFromPBData((const void*)data, (int)len)) {
                        TChatroomMemberInfo tinfo;
                        tinfo.memberCount = result.memberCount;
                        tinfo.olderMembers = result.members;

                        if(callback)
                            callback->onSuccess(tinfo);
                        delete this;
                        return;
                    }
                }

                if(callback)
                    callback->onFalure(kEcProtoCorruptData);
                delete this;
            };
            void onFalure(int errorCode) {
                if(callback)
                    callback->onFalure(errorCode);
                delete this;
            };
            virtual ~GetChatroomMemberInfoPublishCallback() {

            }
        };


        class ModifyMyInfoPublishCallback : public MQTTPublishCallback {
        public:
            ModifyMyInfoPublishCallback(GeneralOperationCallback *cb, const std::list<std::pair<int, std::string>> &infos) : MQTTPublishCallback(), callback(cb), minfos(infos) {}
            GeneralOperationCallback *callback;
            const std::list<std::pair<int, std::string>> minfos;
            void onSuccess(const unsigned char* data, size_t len) {
                if(minfos.size() > 0) {
                    MessageDB::Instance()->UpdateMyInfo(minfos);
                }
                if(callback)
                    callback->onSuccess();
                delete this;
            };
            void onFalure(int errorCode) {
                if(callback)
                    callback->onFalure(errorCode);
                delete this;
            };
            virtual ~ModifyMyInfoPublishCallback() {

            }
        };


        class ModifyGroupPublishCallback : public MQTTPublishCallback {
        public:
            ModifyGroupPublishCallback(GeneralOperationCallback *cb, const std::string &groupId, int type, const std::string &newValue) : MQTTPublishCallback(), callback(cb), mGroupId(groupId), mType(type), mNewValue(newValue) {}
            GeneralOperationCallback *callback;
            const std::string mGroupId;
            int mType;
            const std::string mNewValue;
            void onSuccess(const unsigned char* data, size_t len) {
                MessageDB::Instance()->UpdateGroupInfo(mGroupId, mType, mNewValue);
                if(callback)
                    callback->onSuccess();
                delete this;
            };
            void onFalure(int errorCode) {
                if(callback)
                    callback->onFalure(errorCode);
                delete this;
            };
            virtual ~ModifyGroupPublishCallback() {

            }
        };

        class ModifyChannelPublishCallback : public MQTTPublishCallback {
        public:
            ModifyChannelPublishCallback(GeneralOperationCallback *cb, const std::string &channelId, int type, const std::string &newValue) : MQTTPublishCallback(), callback(cb), mChannelId(channelId), mType(type), mNewValue(newValue) {}
            GeneralOperationCallback *callback;
            const std::string mChannelId;
            int mType;
            const std::string mNewValue;
            void onSuccess(const unsigned char* data, size_t len) {
                if(callback)
                    callback->onSuccess();
                //Todo
//                MessageDB::Instance()->UpdateGroupInfo(mGroupId, mType, mNewValue);
                delete this;
            };
            void onFalure(int errorCode) {
                if(callback)
                    callback->onFalure(errorCode);
                delete this;
            };
            virtual ~ModifyChannelPublishCallback() {

            }
        };

        class TransferChannelPublishCallback : public MQTTPublishCallback {
        public:
            TransferChannelPublishCallback(GeneralOperationCallback *cb, const std::string &channelId, const std::string &newOwner) : MQTTPublishCallback(), callback(cb), mChannelId(channelId), mNewOwner(newOwner) {}
            GeneralOperationCallback *callback;
            const std::string mChannelId;
            const std::string mNewOwner;
            void onSuccess(const unsigned char* data, size_t len) {
                if(callback)
                    callback->onSuccess();
                //Todo
                //                MessageDB::Instance()->UpdateGroupInfo(mGroupId, mType, mNewValue);
                delete this;
            };
            void onFalure(int errorCode) {
                if(callback)
                    callback->onFalure(errorCode);
                delete this;
            };
            virtual ~TransferChannelPublishCallback() {

            }
        };

        class DestoryChannelPublishCallback : public MQTTPublishCallback {
        public:
            DestoryChannelPublishCallback(GeneralOperationCallback *cb, const std::string &channelId) : MQTTPublishCallback(), callback(cb), mChannelId(channelId) {}
            GeneralOperationCallback *callback;
            const std::string mChannelId;
            void onSuccess(const unsigned char* data, size_t len) {
                if(callback)
                    callback->onSuccess();
                //Todo
                //                MessageDB::Instance()->UpdateGroupInfo(mGroupId, mType, mNewValue);
                delete this;
            };
            void onFalure(int errorCode) {
                if(callback)
                    callback->onFalure(errorCode);
                delete this;
            };
            virtual ~DestoryChannelPublishCallback() {

            }
        };

class MessagePublishCallback : public MQTTPublishCallback {
public:
    MessagePublishCallback(long messageId, SendMsgCallback *cb) : MQTTPublishCallback(), mId(messageId), callback(cb) {}
    long mId;
	SendMsgCallback *callback;
    void onSuccess(const unsigned char* data, size_t len) {
        long long messageUId = 0;
        long long timestamp = 0;
        if (len == 16) {
            const unsigned char* p = data;
            for (int i = 0; i < 8; i++) {
                messageUId = (messageUId << 8) + *(p + i);
                timestamp = (timestamp << 8) + *(p + 8 + i);
            }
            MessageDB::Instance()->updateMessageStatus(mId, Message_Status_Sent);
            MessageDB::Instance()->updateMessageUidAndTimestamp(mId, messageUId, timestamp);
            callback->onSuccess(messageUId, timestamp);
        } else {
            MessageDB::Instance()->updateMessageStatus(mId, Message_Status_Send_Failure);
            callback->onFalure(kEcProtoCorruptData);
        }

        delete this;
    };
    void onFalure(int errorCode) {
        MessageDB::Instance()->updateMessageStatus(mId, Message_Status_Send_Failure);
        callback->onFalure(errorCode);
        delete this;
    };
    virtual ~MessagePublishCallback() {

    }
};

void publishTask(PBBase *pbData, MQTTPublishCallback *callback, const std::string &topic, bool orderly) {
    mars::stn::MQTTPublishTask *publishTask = new mars::stn::MQTTPublishTask(callback);
    publishTask->topic = topic;
    publishTask->pbData = pbData;
    if(!gToken.empty()) {
      publishTask->channel_select = ChannelType_All;
      if(!orderly) {
        publishTask->channel_strategy = Task::kChannelFastStrategy;
      } else {
        publishTask->channel_select = ChannelType_LongConn;
      }

      publishTask->cgi = "/im";
      publishTask->shortlink_host_list.push_back(gHost);
    }

    publishTask->limit_frequency = false;
    mars::stn::StartTask(*publishTask);
}

void publishTask(const std::string output, MQTTPublishCallback *callback, const std::string &topic, bool orderly = false) {
  publishTask(output.c_str(), output.length(), callback, topic, orderly);
}




void fillMessageContent(TMessageContent &tcontent, MessageContent *content, int expireDuration = 0) {
    content->type = tcontent.type;
    content->searchableContent = tcontent.searchableContent;
    content->pushContent = tcontent.pushContent;
    content->content = tcontent.content;
    content->data = std::string(tcontent.binaryContent.c_str(), tcontent.binaryContent.length());
    content->mediaType = tcontent.mediaType;
    content->remoteMediaUrl = tcontent.remoteMediaUrl;
    content->mentionedType = tcontent.mentionedType;
    content->mentionedTargets = tcontent.mentionedTargets;
    content->expireDuration = expireDuration;
    content->persistFlag = MessageDB::Instance()->getMessageFlag(content->type);
}

void fillConversation(TMessage &tmsg, Conversation *conversation) {
    conversation->type = (ConversationType)tmsg.conversationType;
    conversation->target = tmsg.target;
    conversation->line = tmsg.line;
}


void sendSavedMsg(long messageId, TMessage &tmsg, SendMsgCallback *callback, int expireDuration) {
    Message *message = new Message();

    fillConversation(tmsg, &(message->conversation));
    message->fromUser = app::GetAccountUserName();
    message->tos = tmsg.to;
    fillMessageContent(tmsg.content, &(message->content), expireDuration);

    publishTask(message, new MessagePublishCallback(messageId, callback), sendMessageTopic, true);
}

class UploadQiniuCallback : public UploadMediaCallback {
    TMessage mMsg;
    UpdateMediaCallback *mCallback;
    std::string mDomain;
    long mMid;
public:
    UploadQiniuCallback(UpdateMediaCallback *callback, std::string domain) : mCallback(callback), mDomain(domain) {

    }
    void onSuccess(std::string key) {
        std::string fileUrl = mDomain + "/" + key;
        mCallback->onSuccess(fileUrl);
        delete this;
    }
    void onFalure(int errorCode) {
        mCallback->onFalure(errorCode);
        delete this;
    }

    void onProgress(int current, int total) {
        mCallback->onProgress(current, total);
    }

    virtual ~UploadQiniuCallback() {

    }
};
class GetUploadTokenCallback : public MQTTPublishCallback {
public:
    GetUploadTokenCallback(UpdateMediaCallback *cb, const std::string md) : MQTTPublishCallback(), callback(cb), mediaData(md) {}
    UpdateMediaCallback *callback;
    std::string mediaData;
    long mMid;
    TMessage msg;
    void onSuccess(const unsigned char* data, size_t len) {
        GetUploadTokenResult result;
        if(result.unserializeFromPBData((const void*)data, (int)len)) {
            UploadTask *uploadTask = new UploadTask(mediaData, result.token, msg.content.mediaType, new UploadQiniuCallback(callback, result.domain));
            uploadTask->cgi = "/fs";//*/result.server();
            std::string server = result.server;
            uploadTask->shortlink_host_list.push_back(server);
            StartTask(*uploadTask);
        } else {
            callback->onFalure(kEcProtoCorruptData);
        }


        delete this;
    };
    void onFalure(int errorCode) {
        callback->onFalure(errorCode);
        delete this;
    };
    virtual ~GetUploadTokenCallback() {

    }
};

class UploadMediaForSendCallback : public UpdateMediaCallback {
public:
    TMessage mMsg;
    SendMsgCallback *mCallback;
    long mMid;
    int mExpireDuration;
    UploadMediaForSendCallback(SendMsgCallback *cb, const TMessage &tmsg, long messageId, int expireDuration) : mMsg(tmsg), mCallback(cb), mMid(messageId), mExpireDuration(expireDuration) {}

    void onSuccess(const std::string &remoteUrl) {
        MessageDB::Instance()->updateMessageRemoteMediaUrl(mMid, remoteUrl);
        mMsg.content.remoteMediaUrl = remoteUrl;
        mCallback->onMediaUploaded(remoteUrl);
        sendSavedMsg(mMid, mMsg, mCallback, mExpireDuration);
        delete this;
    }

    void onFalure(int errorCode) {
        mCallback->onFalure(errorCode);
        delete this;
    }

    void onProgress(int current, int total) {
        mCallback->onProgress(current, total);
    }

    virtual ~UploadMediaForSendCallback() {}
};

int (*sendMessage)(TMessage &tmsg, SendMsgCallback *callback, int expireDuration)
= [](TMessage &tmsg, SendMsgCallback *callback, int expireDuration) {
    tmsg.timestamp = ((int64_t)time(NULL))*1000;
    long id = MessageDB::Instance()->InsertMessage(tmsg);
    if(id > 0) {
        MessageDB::Instance()->updateConversationTimestamp(tmsg.conversationType, tmsg.target, tmsg.line, tmsg.timestamp);
    }

    callback->onPrepared(id, tmsg.timestamp);

    if(tmsg.content.mediaType > 0 && !tmsg.content.localMediaPath.empty() && tmsg.content.remoteMediaUrl.empty()) {

        char * buffer;
        long size;
        std::ifstream file (tmsg.content.localMediaPath.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
        size = file.tellg();
        file.seekg (0, std::ios::beg);
        buffer = new char [size];
        file.read (buffer, size);
        file.close();

        std::string md(buffer, size);

        delete [] buffer;

        mars::stn::MQTTPublishTask *publishTask = new mars::stn::MQTTPublishTask(new GetUploadTokenCallback(new UploadMediaForSendCallback(callback, tmsg, id, expireDuration), md));
        publishTask->topic = getQiniuUploadTokenTopic;
        publishTask->length = 1;
        publishTask->body = new unsigned char[1];
        *(publishTask->body) = (unsigned char)tmsg.content.mediaType;
        mars::stn::StartTask(*publishTask);
    } else {
        sendSavedMsg(id, tmsg, callback, expireDuration);
    }
    return 0;
};

void recallMessage(long long messageUid, GeneralOperationCallback *callback) {
    INT64Buf *buf = new INT64Buf();
    buf->id = messageUid;


    publishTask(buf, new RecallMessagePublishCallback(callback, messageUid), recallMessageTopic, true);
}

        class LoadRemoteMessagesPublishCallback : public MQTTPublishCallback {
        public:
            LoadRemoteMessagesCallback *cb;
            bool mSave2DB;
            LoadRemoteMessagesPublishCallback(LoadRemoteMessagesCallback *callback, bool save2DB) : MQTTPublishCallback(), cb(callback), mSave2DB(save2DB) {}
            
            void onSuccess(const unsigned char* data, size_t len) {
                std::list<TMessage> messageList;
                PullMessageResult result;
                std::string curUser = app::GetAccountUserName();
                if (result.unserializeFromPBData((const void *)data, (int)len)) {
                    bool isBegin = false;
                    if (result.messages.size() > 100 && mSave2DB) {
                        isBegin = DB2::Instance()->BEGIN();
                    }
                    for (std::list<Message>::iterator it = result.messages.begin(); it != result.messages.end(); it++) {
                        TMessage tmsg;
                        StnCallBack::Instance()->converProtoMessage(*it, tmsg, true, curUser);
                        messageList.push_back(tmsg);
                        
                    }
                    if(isBegin) {
                        DB2::Instance()->COMMIT();
                    }
                    
                    cb->onSuccess(messageList);
                } else {
                    cb->onSuccess(messageList);
                }
                delete this;
            };
            void onFalure(int errorCode) {
                cb->onFalure(errorCode);
                delete this;
            };
            virtual ~LoadRemoteMessagesPublishCallback() {
                
            }
        };
        
void loadRemoteMessages(const TConversation &conv, long long beforeUid, int count, LoadRemoteMessagesCallback *callback) {
    LoadRemoteMessages *request = new LoadRemoteMessages();
    request->conversation.type = (ConversationType)conv.conversationType;
    request->conversation.target = conv.target;
    request->conversation.line = conv.line;
    request->beforeUid = beforeUid;
    request->count = count;
    
    publishTask(request, new LoadRemoteMessagesPublishCallback(callback, true), loadRemoteMessagesTopic, false);
}

int uploadGeneralMedia(std::string mediaData, int mediaType, UpdateMediaCallback *callback) {
    mars::stn::MQTTPublishTask *publishTask = new mars::stn::MQTTPublishTask(new GetUploadTokenCallback(callback, mediaData));
    publishTask->topic = getQiniuUploadTokenTopic;
    publishTask->length = sizeof(int);
    publishTask->body = new unsigned char[publishTask->length];
    memcpy(publishTask->body, &mediaType, publishTask->length);
    mars::stn::StartTask(*publishTask);
    return 0;
}

int modifyMyInfo(const std::list<std::pair<int, std::string>> &infos, GeneralOperationCallback *callback) {
    ModifyMyInfoRequest *request = new ModifyMyInfoRequest();
    for (std::list<std::pair<int, std::string>>::const_iterator it = infos.begin(); it != infos.end(); it++) {
        InfoEntry entry;
        entry.type = it->first;
        entry.value = it->second;
        request->entrys.push_back(entry);
    }

    publishTask(request, new ModifyMyInfoPublishCallback(callback, infos), modifyMyInfoTopic, false);
    return 0;
}

        std::string getJoinedChatroom() {
            return g_chatroomId;
        }

        void joinChatroom(const std::string &chatroomId, GeneralOperationCallback *callback){
            if (!g_chatroomId.empty()) {
                callback->onFalure(kEcProtoInvalideParameter);
                return;
            }

            IDBuf *request = new IDBuf();
            request->id = chatroomId;
            publishTask(request, new GeneralOperationPublishCallback(callback), JoinChatroomTopic, false);
        }

        void quitChatroom(const std::string &chatroomId, GeneralOperationCallback *callback) {
            g_chatroomId.clear();

            IDBuf *request = new IDBuf();
            request->id = chatroomId;
            publishTask(request, new GeneralOperationPublishCallback(callback), QuitChatroomTopic, false);
        }

        void getChatroomInfo(const std::string &chatroomId, int64_t lastUpdateDt, GetChatroomInfoCallback *callback) {
            GetChatroomInfoRequest *request = new GetChatroomInfoRequest();
            request->chatroomId = chatroomId;
            request->updateDt = lastUpdateDt;

            publishTask(request, new GetChatroomInfoPublishCallback(callback), GetChatroomInfoTopic, false);
        }
        void getChatroomMemberInfo(const std::string &chatroomId, int maxCount, GetChatroomMemberInfoCallback *callback) {
            GetChatroomMemberInfoRequest *request = new GetChatroomMemberInfoRequest();
            request->chatroomId = chatroomId;
            if (maxCount <= 0) {
                maxCount = 10;
            }
            request->maxCount = maxCount;
            publishTask(request, new GetChatroomMemberInfoPublishCallback(callback), GetChatroomMemberTopic, false);
        }

#ifdef __ANDROID__
      extern  int myatoi(std::string s);
        extern  long long myatoll(std::string s);
      int myatoi(std::string s) {
        if(s.length() == 0) {
          return 0;
        }
        const char* p = s.c_str();
        int ret = 0;
        while(*p != 0) {
          int value = *p - '0';
          if(value < 0 || value > 9) {
            return 0;
          }
          ret = ret * 10 + value;
          p++;
        }
        return ret;
      }

        long long myatoll(std::string s) {
            if(s.length() == 0) {
                return 0;
            }
            const char* p = s.c_str();
            long long ret = 0;
            while(*p != 0) {
                int value = *p - '0';
                if(value < 0 || value > 9) {
                    return 0;
                }
                ret = ret * 10 + value;
                p++;
            }
            return ret;
        }
#endif


class ModifyUserSettingPublishCallback : public MQTTPublishCallback {
public:
    ModifyUserSettingPublishCallback(GeneralOperationCallback *cb, ModifyUserSettingReq *request) : MQTTPublishCallback(), callback(cb), mRequest(request) {}
    GeneralOperationCallback *callback;
    ModifyUserSettingReq *mRequest;
    void onSuccess(const unsigned char* data, size_t len) {
        std::list<TUserSettingEntry> entrys;
        TUserSettingEntry entry;
        entry.scope = (UserSettingScope)mRequest->scope;
        entry.key = mRequest->key;
        entry.value = mRequest->value;
        entry.updateDt = 0;
        entrys.push_back(entry);
        MessageDB::Instance()->UpdateUserSettings(entrys);
        if(entry.scope == kUserSettingConversationSilent || entry.scope == kUserSettingConversationTop) {
            std::string str(entry.key);
            unsigned long pos1 = str.find("-");
            std::string type = str.substr(0, pos1);
            unsigned long pos2 = str.find("-", pos1 + 1);
            std::string lineStr = str.substr(pos1 + 1, pos2 - pos1 - 1);

            std::string target = str.substr(pos2 + 1, str.length() - pos2 - 1);
#ifdef __ANDROID__
          int conversationType = myatoi(type);
          int line = myatoi(lineStr);
            int issilent = false;
            if (!entry.value.empty()) {
                issilent = myatoi(entry.value);
            }
#else
            int conversationType = std::stoi(type);
            int line = std::stoi(lineStr);
            int issilent = false;
            if (!entry.value.empty()) {
                issilent = std::stoi(entry.value);
            }
#endif
            if(entry.scope == kUserSettingConversationSilent) {
                MessageDB::Instance()->updateConversationIsSilent(conversationType, target, line, issilent);
            } else {
                MessageDB::Instance()->updateConversationIsTop(conversationType, target, line, issilent);
            }
        }
        
        if(callback)
            callback->onSuccess();
        delete this;
    };
    void onFalure(int errorCode) {
        if(callback)
            callback->onFalure(errorCode);
        delete this;
    };
    virtual ~ModifyUserSettingPublishCallback() {

    }
};

int modifyUserSetting(int scope, const std::string &key, const std::string &value, GeneralOperationCallback *callback) {
    ModifyUserSettingReq *request = new ModifyUserSettingReq();
    request->scope = scope;
    request->key = key;
    request->value = value;

    publishTask(request, new ModifyUserSettingPublishCallback(callback, request), putUserSettingTopic, false);
    return 0;
}

void syncConversationReadDt(int conversatinType, const std::string &target, int line, int64_t readedDt) {
    std::stringstream stream;
    stream << conversatinType;
    stream << "-";
    stream << line;
    stream << "-";
    stream << target;

    char buf[64];
    memset(buf, 0, 64);
    sprintf(buf, "%lld", readedDt);
    modifyUserSetting(kUserSettingConversationSync, stream.str(), buf, NULL);
}

class CreateGroupPublishCallback : public MQTTPublishCallback {
public:
    CreateGroupPublishCallback(CreateGroupCallback *cb) : MQTTPublishCallback(), callback(cb) {}
    CreateGroupCallback *callback;
    void onSuccess(const unsigned char* data, size_t len) {
        std::string gid((const char*)data, len);
        callback->onSuccess(gid);
        delete this;
    };
    void onFalure(int errorCode) {
        callback->onFalure(errorCode);
        delete this;
    };
    virtual ~CreateGroupPublishCallback() {

    }
};

        class CreateChannelPublishCallback : public MQTTPublishCallback {
        public:
            CreateChannelPublishCallback(CreateChannelCallback *cb, TChannelInfo tInfo) : MQTTPublishCallback(), callback(cb), channelInfo(tInfo) {}
            CreateChannelCallback *callback;
            TChannelInfo channelInfo;
            void onSuccess(const unsigned char* data, size_t len) {
                std::string cid((const char*)data, len);
                int64_t dt = 0;
                channelInfo.channelId = cid;
                channelInfo.updateDt = dt;
                stn::MessageDB::Instance()->InsertOrUpdateChannelInfo(channelInfo);
                if(callback) {
                    callback->onSuccess(channelInfo);
                }
                delete this;
            };
            void onFalure(int errorCode) {
                callback->onFalure(errorCode);
                delete this;
            };
            virtual ~CreateChannelPublishCallback() {

            }
        };

void convertUser(const User &user, TUserInfo &userInfo) {
  userInfo.uid = user.uid;
  userInfo.name = user.name;
  userInfo.displayName = user.displayName;
    userInfo.gender = user.gender;
  userInfo.portrait = user.portrait;
  userInfo.mobile = user.mobile;
  userInfo.email = user.email;
  userInfo.address = user.address;
  userInfo.company = user.company;
  userInfo.extra = user.extra;
  userInfo.updateDt = user.updateDt;
    userInfo.social = user.social;
}

        void convertChannel(const ChannelInfo &info, TChannelInfo &tInfo) {
            tInfo.channelId = info.targetId;
            tInfo.name = info.name;
            tInfo.portrait = info.portrait;
            tInfo.desc = info.desc;
            tInfo.extra = info.extra;
            tInfo.status = info.status;
            tInfo.updateDt = info.updateDt;
            tInfo.owner = info.owner;
            tInfo.secret = info.secret;
            tInfo.callback = info.callback;
            tInfo.automatic = info.automatic;
        }

class TSearchUserCallback : public MQTTPublishCallback {
private:
    SearchUserCallback *mCallback;
public:
    TSearchUserCallback(SearchUserCallback *callback) : mCallback(callback) {}

  void onSuccess(const unsigned char* data, size_t len) {
    SearchUserResult result;
    if(result.unserializeFromPBData(data, (int)len)) {
      std::list<TUserInfo> userInfos;

      for (std::list<User>::iterator it = result.entrys.begin(); it != result.entrys.end(); it++) {

        const User &u = *it;
        TUserInfo userInfo;
        convertUser(u, userInfo);
        userInfos.push_back(userInfo);
      }
      mCallback->onSuccess(userInfos, "", 0);
    } else {
      mCallback->onFalure(kEcProtoCorruptData);
    }
    delete this;
  }

    void onFalure(int errorCode) {
        mCallback->onFalure(errorCode);
        delete this;
    }

    virtual ~TSearchUserCallback() {

    }
};


void searchUser(const std::string &keyword, bool puzzy, int page, SearchUserCallback *callback) {
  SearchUserRequest *request = new SearchUserRequest();
  request->keyword = keyword;
  request->buzzy = puzzy;
  request->page = page;
  publishTask(request, new TSearchUserCallback(callback), userSearchTopic, false);
}

void sendFriendRequest(const std::string &userId, const std::string &reason, GeneralOperationCallback *callback) {
    AddFriendRequest *request = new AddFriendRequest();
    request->targetUid = userId;
    request->reason = reason;
    publishTask(request, new GeneralOperationPublishCallback(callback), AddFriendRequestTopic, false);
}

class TLoadFriendRequestCallback : public MQTTPublishCallback {
public:
    TLoadFriendRequestCallback() {}
  void onSuccess(const unsigned char* data, size_t len) {
    GetFriendRequestResult result;
  if (result.unserializeFromPBData(data, (int)len)) {
    bool newValue = false;

    for (std::list<FriendRequest>::iterator it = result.entrys.begin(); it != result.entrys.end(); it++) {

      const FriendRequest &fr = *it;
      newValue = true;
      TFriendRequest friendRequest;
      bool isSend = false;

      if (fr.fromUid == app::GetAccountUserName()) {
        isSend = true;
        friendRequest.direction = 0;
      } else {
        friendRequest.direction = 1;
        friendRequest.target = fr.fromUid;
      }


      if (isSend) {
        friendRequest.target = fr.toUid;
      }

      friendRequest.reason = fr.reason;
      friendRequest.status = fr.status;

      friendRequest.timestamp = fr.updateDt;

      if (isSend) {
        friendRequest.readStatus = fr.fromReadStatus;
      }

      if (!isSend) {
        friendRequest.readStatus = fr.toReadStatus;
      }
      MessageDB::Instance()->InsertFriendRequestOrReplace(friendRequest);
    }
    if(StnCallBack::Instance()->m_getFriendRequestCB != NULL) {
      StnCallBack::Instance()->m_getFriendRequestCB->onSuccess(newValue);
    }
  } else {
      if(StnCallBack::Instance()->m_getFriendRequestCB != NULL) {
        StnCallBack::Instance()->m_getFriendRequestCB->onFalure(kEcProtoCorruptData);
      }
    }
    delete this;
  }
  void onFalure(int errorCode) {
    if(StnCallBack::Instance()->m_getFriendRequestCB != NULL) {
      StnCallBack::Instance()->m_getFriendRequestCB->onFalure(errorCode);
    }
    delete this;
  }
    virtual ~TLoadFriendRequestCallback() {}
};
void PullFriendRequest(int64_t head) {
  int64_t maxTS = MessageDB::Instance()->getFriendRequestHead();
  if (head > maxTS) {
    loadFriendRequestFromRemote();
  }
}

void loadFriendRequestFromRemote() {
  int64_t maxTS = MessageDB::Instance()->getFriendRequestHead();
  Version *version = new Version();
  version->version = maxTS;
  publishTask(version, new TLoadFriendRequestCallback(), friendRequestPullTopic, false);
}

class TLoadFriendCallback : public MQTTPublishCallback {
public:
    TLoadFriendCallback() {}

  void onSuccess(const unsigned char* data, size_t len) {
    if (len > 0) {
      GetFriendsResult result;
      if(result.unserializeFromPBData(data, (int)len)) {
        std::list<std::string> retList;

        for (std::list<Friend>::iterator it = result.entrys.begin(); it != result.entrys.end(); it++) {
          const Friend &ff = *it;
          if(ff.state == 0) {
            retList.push_back(ff.uid);
          }

          MessageDB::Instance()->InsertFriendOrReplace(ff.uid, ff.state, ff.updateDt, ff.alias);
        }

        if(StnCallBack::Instance()->m_getMyFriendsCB) {
            StnCallBack::Instance()->m_getMyFriendsCB->onSuccess(retList);
        }
      } else {
        if(StnCallBack::Instance()->m_getMyFriendsCB) {
          StnCallBack::Instance()->m_getMyFriendsCB->onFalure(kEcProtoCorruptData);
        }
      }
    } else {
      if(StnCallBack::Instance()->m_getMyFriendsCB) {
        StnCallBack::Instance()->m_getMyFriendsCB->onFalure(kEcProtoCorruptData);
      }
    }
    delete this;
  }
  void onFalure(int errorCode) {
    delete this;
  }


    virtual ~TLoadFriendCallback() {}
};

void PullFriend(int64_t head) {
    int64_t maxTS = MessageDB::Instance()->getFriendHead();
  if (head > maxTS) {
    loadFriendFromRemote();
  }
}

void loadFriendFromRemote() {
  int64_t maxTS = MessageDB::Instance()->getFriendHead();
  Version *version = new Version();
  version->version = maxTS;
  publishTask(version, new TLoadFriendCallback(), friendPullTopic, false);
}

void handleFriendRequest(const std::string &userId, bool accept, GeneralOperationCallback *callback) {
    HandleFriendRequest *request = new HandleFriendRequest();
    request->status = accept ? 1 : 2;
    request->targetUid = userId;
    publishTask(request, new GeneralOperationPublishCallback(callback), HandleFriendRequestTopic, false);
}

void deleteFriend(const std::string &userId, GeneralOperationCallback *callback) {
    IDBuf *request = new IDBuf();
    request->id = userId;
    publishTask(request, new GeneralOperationPublishCallback(callback), DeleteFriendTopic, false);
}
        
        void setFriendAlias(const std::string &userId, const std::string &alias, GeneralOperationCallback *callback) {
            AddFriendRequest *request = new AddFriendRequest();
            request->targetUid = userId;
            request->reason = alias;
            publishTask(request, new GeneralOperationPublishCallback(callback), SetFriendAliasTopic, false);
        }

void blackListRequest(const std::string &userId, bool blacked, GeneralOperationCallback *callback) {
    BlackUserRequest *request = new BlackUserRequest();
    request->userId = userId;
    request->status = blacked ? 2 : 1; //0 friend; 1 no relation; 2 blacklist
    publishTask(request, new GeneralOperationPublishCallback(callback), BlackListUserTopic, false);
}

void (*createGroup)(const std::string &groupId, const std::string &groupName, const std::string &groupPortrait, const std::list<std::string> &groupMembers, const std::list<int> &notifyLines, TMessageContent &content, CreateGroupCallback *callback)
= [](const std::string &groupId, const std::string &groupName, const std::string &groupPortrait, const std::list<std::string> &groupMembers, const std::list<int> &notifyLines, TMessageContent &content, CreateGroupCallback *callback) {
    CreateGroupRequest *request = new CreateGroupRequest();
    request->group.groupInfo.targetId = groupId;
    request->group.groupInfo.type = GroupType_Normal;
    request->group.groupInfo.portrait = groupPortrait;
    request->group.groupInfo.name = groupName;

    for (std::list<std::string>::const_iterator it = groupMembers.begin(); it != groupMembers.end(); it++) {
        GroupMember gm;
        gm.memberId = *it;
        gm.type = GroupMemberType_Normal;
        request->group.members.push_back(gm);
    }

    for(std::list<int>::const_iterator it = notifyLines.begin(); it != notifyLines.end(); it++) {
        request->toLines.push_back(*it);
    }

    fillMessageContent(content, &(request->notifyContent));

    publishTask(request, new CreateGroupPublishCallback(callback), createGroupTopic, false);
};


void (*addMembers)(const std::string &groupId, const std::list<std::string> &members, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback)
= [](const std::string &groupId, const std::list<std::string> &members, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback) {
    AddGroupMemberRequest *request = new AddGroupMemberRequest();
    request->groupId = groupId;

    for (std::list<std::string>::const_iterator it = members.begin(); it != members.end(); it++) {
        GroupMember gm;
        gm.memberId = *it;
        gm.type = GroupMemberType_Normal;
        request->addedMembers.push_back(gm);
    }

    for(std::list<int>::const_iterator it = notifyLines.begin(); it != notifyLines.end(); it++) {
        request->toLines.push_back(*it);
    }

    fillMessageContent(content, &(request->notifyContent), 0);

    publishTask(request, new GeneralOperationPublishCallback(callback), addGroupMemberTopic, false);
};

void (*kickoffMembers)(const std::string &groupId, const std::list<std::string> &members, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback)
= [](const std::string &groupId, const std::list<std::string> &members, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback) {
    RemoveGroupMemberRequest *request = new RemoveGroupMemberRequest();
    request->groupId = groupId;

    for (std::list<std::string>::const_iterator it = members.begin(); it != members.end(); it++) {
        request->removedMembers.push_back(*it);
    }

    for(std::list<int>::const_iterator it = notifyLines.begin(); it != notifyLines.end(); it++) {
        request->toLines.push_back(*it);
    }

    fillMessageContent(content, &(request->notifyContent));

    publishTask(request, new GeneralOperationPublishCallback(callback), kickoffGroupMemberTopic, false);
};

void (*quitGroup)(const std::string &groupId, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback)
= [](const std::string &groupId, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback) {
    QuitGroupRequest *request = new QuitGroupRequest();
    request->groupId = groupId;

    for(std::list<int>::const_iterator it = notifyLines.begin(); it != notifyLines.end(); it++) {
        request->toLines.push_back(*it);
    }

    fillMessageContent(content, &(request->notifyContent));

    publishTask(request, new GeneralOperationPublishCallback(callback), quitGroupTopic, false);
};

void (*dismissGroup)(const std::string &groupId, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback)
= [](const std::string &groupId, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback) {
    DismissGroupRequest *request = new DismissGroupRequest();
    request->groupId = groupId;

    for(std::list<int>::const_iterator it = notifyLines.begin(); it != notifyLines.end(); it++) {
        request->toLines.push_back(*it);
    }

    fillMessageContent(content, &(request->notifyContent));

    publishTask(request, new GeneralOperationPublishCallback(callback), dismissGroupTopic, false);

};
class GetGroupInfoPublishCallback : public MQTTPublishCallback {
public:
    GetGroupInfoPublishCallback(GetGroupInfoCallback *cb) : MQTTPublishCallback(), callback(cb) {}
    GetGroupInfoCallback *callback;
    void onSuccess(const unsigned char* data, size_t len) {

        PullGroupInfoResult result;
        if(result.unserializeFromPBData(data, (int)len)) {
            std::list<TGroupInfo> retList;

            for (std::list<GroupInfo>::iterator it = result.infos.begin(); it != result.infos.end(); it++) {
                const ::mars::stn::GroupInfo &info = *it;
                TGroupInfo tInfo;
                tInfo.target = info.targetId;
                tInfo.name = info.name;
                tInfo.portrait = info.portrait;
                tInfo.owner = info.owner;
                tInfo.type = info.type;
                tInfo.extra = info.extra;
                tInfo.updateDt = info.updateDt;
                tInfo.memberCount = info.memberCount;
                retList.push_back(tInfo);
                MessageDB::Instance()->InsertGroupInfo(tInfo);
            }
            callback->onSuccess(retList);
        } else {
            callback->onFalure(kEcProtoCorruptData);
        }
        delete this;
    };
    void onFalure(int errorCode) {
        callback->onFalure(errorCode);
        delete this;
    };
    virtual ~GetGroupInfoPublishCallback() {

    }
};

void (*getGroupInfo)(const std::list<std::pair<std::string, int64_t>> &groupIdList, GetGroupInfoCallback *callback)
= [](const std::list<std::pair<std::string, int64_t>> &groupIdList, GetGroupInfoCallback *callback) {
    PullUserRequest *request = new PullUserRequest();
    for (std::list<std::pair<std::string, int64_t>>::const_iterator it = groupIdList.begin(); it != groupIdList.end(); it++) {
        UserRequest r;
        r.uid = it->first;
        r.updateDt = it->second;
        request->requests.push_back(r);
    }

    publishTask(request, new GetGroupInfoPublishCallback(callback), getGroupInfoTopic, false);
};

void (*modifyGroupInfo)(const std::string &groupId, int type, const std::string &newValue, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback)
= [](const std::string &groupId, int type, const std::string &newValue, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback) {
    ModifyGroupInfoRequest *request = new ModifyGroupInfoRequest();
    request->groupId = groupId;
    request->type = type;
    request->value = newValue;

    //todo
    for(std::list<int>::const_iterator it = notifyLines.begin(); it != notifyLines.end(); it++) {
        request->toLine.push_back(*it);
    }

    fillMessageContent(content, &(request->notifyContent));

    publishTask(request, new ModifyGroupPublishCallback(callback, groupId, type, newValue), modifyGroupInfoTopic, false);
};

void (*modifyGroupAlias)(const std::string &groupId, const std::string &newAlias, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback)
= [](const std::string &groupId, const std::string &newAlias, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback) {
    ModifyGroupMemberAlias *request = new ModifyGroupMemberAlias();
    request->groupId = groupId;
    request->alias = newAlias;

    for(std::list<int>::const_iterator it = notifyLines.begin(); it != notifyLines.end(); it++) {
        request->toLine.push_back(*it);
    }

    fillMessageContent(content, &(request->notifyContent));

    publishTask(request, new GeneralOperationPublishCallback(callback), modifyGroupAliasTopic, false);
};
class GetGroupMembersPublishCallback : public MQTTPublishCallback {
public:
    GetGroupMembersPublishCallback(GetGroupMembersCallback *cb, const std::string &groupId) : MQTTPublishCallback(), callback(cb), mGroupId(groupId) {}
    GetGroupMembersCallback *callback;
    std::string mGroupId;

    void onSuccess(const unsigned char* data, size_t len) {

        PullGroupMemberResult result;
        if(result.unserializeFromPBData(data, (int)len)) {
            std::list<TGroupMember> retList;

            for (std::list<GroupMember>::iterator it = result.members.begin(); it != result.members.end(); it++) {
                const GroupMember member = *it;
                TGroupMember tm;
                tm.groupId = mGroupId;
                tm.memberId = member.memberId;
                tm.alias = member.alias;
                tm.type = member.type;
                tm.updateDt = member.updateDt;
                retList.push_back(tm);
            }
            MessageDB::Instance()->UpdateGroupMember(retList);
            if(callback) {
                callback->onSuccess(mGroupId, retList);
            }

            if(StnCallBack::Instance()->m_getGroupMembersCB) {
                StnCallBack::Instance()->m_getGroupMembersCB->onSuccess(mGroupId, retList);
            }
        } else {
            if(callback)
                callback->onFalure(kEcProtoCorruptData);
        }
        delete this;
    };
    void onFalure(int errorCode) {
        if(callback)
            callback->onFalure(errorCode);
        delete this;
    };
    virtual ~GetGroupMembersPublishCallback() {

    }
};

void (*getGroupMembers)(const std::string &groupId, int64_t updateDt)
= [](const std::string &groupId, int64_t updateDt) {
    PullGroupMemberRequest *request = new PullGroupMemberRequest();
    request->target = groupId;
    request->head = updateDt;

    publishTask(request, new GetGroupMembersPublishCallback(NULL, groupId), getGroupMemberTopic, false);
};

void (*transferGroup)(const std::string &groupId, const std::string &newOwner, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback)
= [](const std::string &groupId, const std::string &newOwner, const std::list<int> &notifyLines, TMessageContent &content, GeneralOperationCallback *callback) {
    TransferGroupRequest *request = new TransferGroupRequest();
    request->groupId = groupId;
    request->newOwner = newOwner;

    for(std::list<int>::const_iterator it = notifyLines.begin(); it != notifyLines.end(); it++) {
        request->toLines.push_back(*it);
    }


    fillMessageContent(content, &(request->notifyContent));

    publishTask(request, new GeneralOperationPublishCallback(callback), transferGroupTopic, false);
};

class GetUserInfoPublishCallback : public MQTTPublishCallback {
public:
    GetUserInfoPublishCallback(GetUserInfoCallback *cb) : MQTTPublishCallback(), callback(cb) {}
    GetUserInfoCallback *callback;
    void onSuccess(const unsigned char* data, size_t len) {

        PullUserResult result;
        if(result.unserializeFromPBData(data, (int)len)) {
            std::list<TUserInfo> retList;

            for (std::list<UserResult>::iterator it = result.results.begin(); it != result.results.end(); it++) {
                UserResult &userResult = *it;
                User &info = userResult.user;

                if (userResult.code == 0) {
                    TUserInfo tInfo;

                    tInfo.uid = info.uid;
                    tInfo.name = info.name;
                    tInfo.displayName = info.displayName;
                    tInfo.gender = info.gender;
                    tInfo.portrait = info.portrait;
                    tInfo.mobile = info.mobile;
                    tInfo.email = info.email;
                    tInfo.address = info.address;
                    tInfo.company = info.company;
                    tInfo.social = info.social;
                    tInfo.extra = info.extra;
                    tInfo.updateDt = info.updateDt;

                    retList.push_back(tInfo);
                }

            }
            callback->onSuccess(retList);
        } else {
            callback->onFalure(kEcProtoCorruptData);
        }
        delete this;
    };
    void onFalure(int errorCode) {
        callback->onFalure(errorCode);
        delete this;
    };
    virtual ~GetUserInfoPublishCallback() {

    }
};


void (*getUserInfo)(const std::list<std::pair<std::string, int64_t>> &userReqList, GetUserInfoCallback *callback)
=[](const std::list<std::pair<std::string, int64_t>> &userReqList, GetUserInfoCallback *callback) {
    PullUserRequest *request = new PullUserRequest();

    for (std::list<std::pair<std::string, int64_t>>::const_iterator it = userReqList.begin(); it != userReqList.end(); it++) {
        UserRequest item;
        item.uid = ((*it).first);
        item.updateDt = ((*it).second);
        request->requests.push_back(item);
    }

    publishTask(request, new GetUserInfoPublishCallback(callback), getUserInfoTopic, false);
};

int64_t getServerDeltaTime() {
    return StnCallBack::Instance()->deltaTime;
}

void setDeviceToken(const std::string &appName, const std::string &deviceToken, int pushType) {
    UploadDeviceTokenRequest *request = new UploadDeviceTokenRequest();
  Platform type = Platform_UNSET;
  mars::app::DeviceInfo deviceInfo = mars::app::GetDeviceInfo();

    switch (deviceInfo.platform) {
        case app::PlatformType_iOS:
            type = Platform_iOS;
            break;

        case app::PlatformType_Android:
            type = Platform_Android;
            break;

        case app::PlatformType_Windows:
            type = Platform_Windows;
            break;

        case app::PlatformType_OSX:
            type = Platform_OSX;
            break;

        case app::PlatformType_WEB:
            type = Platform_WEB;
            break;
        default:
            break;
    }
    request->platform = type;
    request->appName = appName;
    request->deviceToken = deviceToken;
    request->pushType = pushType;

    publishTask(request, new GeneralOperationPublishCallback(NULL), UploadDeviceTokenTopic, false);
}

std::string gRouteHost;
int gRoutePort = 80;
std::string gHost;

std::string UrlEncode(const std::string& szToEncode)
{
    std::string src = szToEncode;
    char hex[] = "0123456789ABCDEF";
    std::string dst;

    for (size_t i = 0; i < src.size(); ++i)
    {
        unsigned char cc = src[i];
        if (isascii(cc))
        {
            if (cc == ' ')
            {
                dst += "%20";
            }
            else
                dst += cc;
        }
        else
        {
            unsigned char c = static_cast<unsigned char>(src[i]);
            dst += '%';
            dst += hex[c / 16];
            dst += hex[c % 16];
        }
    }
    return dst;
}

MQTTTask::MQTTTask(MQTT_MSG_TYPE type) : Task(), type(type) {
    user_context = this;
    channel_select = ChannelType_LongConn;
    limit_frequency = false;
    limit_flow = false;
    need_authed = true;
}
const std::string MQTTTask::description() const {
    std::stringstream ss;
    ss << Task::description();
    ss << "\n";
    ss << "type:";
    ss << type;

    ss << "topic:";
    ss << topic;
    return ss.str();
}
UploadTask::UploadTask(const std::string &data, const std::string &token, int mediaType, UploadMediaCallback *callback) : Task(), mData(data), mToken(token), mMediaType(mediaType), mCallback(callback) {
    user_context = this;
    channel_select = ChannelType_ShortConn;
    cmdid = UPLOAD_SEND_OUT_CMDID;
    limit_flow = false;
    need_authed = true;
}


MQTTPublishTask::MQTTPublishTask(MQTTPublishCallback *callback) : MQTTTask(MQTT_MSG_PUBLISH) , pbData(NULL), body(NULL), length(0), m_callback(callback) {
    cmdid = MQTT_SEND_OUT_CMDID;
}
const std::string MQTTPublishTask::description() const {
    std::stringstream ss;
    ss << MQTTTask::description();
    ss << "\n";
    ss << "body size:";
    ss << length;

    return ss.str();
}

MQTTPublishTask::~MQTTPublishTask() {
    if(pbData) {
    delete pbData;
    pbData = NULL;
    }
    if (body) {
        delete body;
        body = NULL;
        length = 0;
    }

}

MQTTSubscribeTask::MQTTSubscribeTask(MQTTGeneralCallback *callback) : MQTTTask(MQTT_MSG_SUBSCRIBE) , m_callback(callback) {
    cmdid = MQTT_SUBSCRIBE_CMDID;
}

MQTTUnsubscribeTask::MQTTUnsubscribeTask(MQTTGeneralCallback *callback) : MQTTTask(MQTT_MSG_UNSUBSCRIBE) , m_callback(callback) {
    cmdid = MQTT_UNSUBSCRIBE_CMDID;
}

MQTTPubAckTask::MQTTPubAckTask(uint16_t messageId) : MQTTTask(MQTT_MSG_PUBACK) {
    cmdid = MQTT_PUBACK_CMDID;
    taskid = messageId;
    send_only = true;
}


MQTTDisconnectTask::MQTTDisconnectTask() : MQTTTask(MQTT_MSG_DISCONNECT), flag(0) {
    cmdid = MQTT_DISCONNECT_CMDID;
    need_authed = false;
}
      std::string gUserId;
      std::string gToken;
        std::string gSecret;
        std::string gDbSecret;

        bool decodeToken(const std::string &encodetoken, std::string &token, std::string &secret, std::string &dbSecret) {
            int len2 = (int)modp_b64_decode_len(encodetoken.size());
            unsigned char * tmp = (unsigned char *)calloc( len2, sizeof(unsigned char));
            int len = (int)Comm::DecodeBase64((unsigned char*)encodetoken.c_str(), tmp, (int)encodetoken.size());
            if (len > 0) {
                unsigned int dataLen = 0;
                unsigned char* pdata = (unsigned char* )decrypt_data(tmp, len, &dataLen, true, false);
                if (dataLen == 0) {
                    free(tmp);
                    return false;
                }
                std::string str((char*)pdata);
                free(pdata);
                free(tmp);

                int i = (int)str.find("|");
                token = str.substr(0, i);
                str = str.substr(i+1);
                int j = (int)str.find("|");
                secret = str.substr(0, j);
                dbSecret = str.substr(j+1);

                const char *p = secret.c_str();
                for (int i = 0; i < 16; i++) {
                    use_key[i] = (unsigned char)(*(p+i) & 0XFF);
                }

                return true;
            }
            free(tmp);
            return false;
        }
bool setAuthInfo(const std::string &userId, const std::string &token) {
    if(!decodeToken(token, gToken, gSecret, gDbSecret)) {
        mars::stn::StnCallBack::Instance()->updateConnectionStatus(kConnectionStatusTokenIncorrect);
        return false;
    }
    mars::stn::SetCallback(StnCallBack::Instance());
    DB2::Instance()->Open(gDbSecret);
    DB2::Instance()->Upgrade();
    MessageDB::Instance()->FailSendingMessages();
    StnCallBack::Instance()->onDBOpened();
    mqtt_init(mars::app::GetDeviceInfo().clientid.c_str());
    mqtt_init_auth(userId.c_str(), gToken.c_str());
    gUserId = userId;
    isRoutring = false;
    return true;
}

void Disconnect(uint8_t flag) {
    mars::stn::MQTTDisconnectTask *disconnectTask = new mars::stn::MQTTDisconnectTask();
    disconnectTask->flag = flag;
    mars::stn::StartTask(*disconnectTask);
    DB2::Instance()->closeDB();
    gHost = "";
    gUserId = "";
    gToken = "";
    isRoutring = false;
}

class RefreshGroupInfoCallback : public GetGroupInfoCallback {

public:
    RefreshGroupInfoCallback() {};
    void onSuccess(const std::list<mars::stn::TGroupInfo> &groupInfoList) {

        for (std::list<TGroupInfo>::const_iterator it = groupInfoList.begin(); it != groupInfoList.end(); ++it) {
            MessageDB::Instance()->InsertGroupInfo(*it);
        }
        delete this;
    }
    void onFalure(int errorCode) {
        delete this;
    }

    virtual ~RefreshGroupInfoCallback() {}
};


void reloadGroupInfoFromRemote(const std::list<std::pair<std::string, int64_t>> &groupReqList) {
    getGroupInfo(groupReqList, new RefreshGroupInfoCallback());
};

class RefreshUserInfoCallback : public GetUserInfoCallback {

public:
    RefreshUserInfoCallback() {};
    void onSuccess(const std::list<TUserInfo> &userInfoList) {

        for (std::list<TUserInfo>::const_iterator it = userInfoList.begin(); it != userInfoList.end(); ++it) {
            MessageDB::Instance()->InsertUserInfoOrReplace(*it);
        }

        if(StnCallBack::Instance()->m_getUserInfoCB) {
            StnCallBack::Instance()->m_getUserInfoCB->onSuccess(userInfoList);
        }
        delete this;
    }
    void onFalure(int errorCode) {
        if(StnCallBack::Instance()->m_getUserInfoCB) {
            StnCallBack::Instance()->m_getUserInfoCB->onFalure(errorCode);
        }
        delete this;
    }

    virtual ~RefreshUserInfoCallback() {}
};


void reloadUserInfoFromRemote(const std::list<std::pair<std::string, int64_t>> &reqList) {
    getUserInfo(reqList, new RefreshUserInfoCallback());
};

void reloadGroupMembersFromRemote(const std::string &groupId, int64_t updateDt) {
    getGroupMembers(groupId, updateDt);
};

void clearFriendRequestUnread(int64_t maxDt) {
    Version *version = new Version();
    version->version = maxDt;
    publishTask(version, NULL, friendRequestUnreadSyncTopic, false);
}

void createChannel(const std::string &channelId, const std::string &channelName, const std::string &channelPortrait, int status, const std::string desc, const std::string &extra, const std::string &secret, const std::string &cb, CreateChannelCallback *callback) {
    TChannelInfo tChannelInfo;
    tChannelInfo.channelId = channelId;
    tChannelInfo.name = channelName;
    tChannelInfo.portrait = channelPortrait;
    tChannelInfo.status = status;
    tChannelInfo.owner = app::GetAccountUserName();
    tChannelInfo.desc = desc;
    tChannelInfo.extra = extra;
    tChannelInfo.secret = secret;
    tChannelInfo.callback = cb;


    ChannelInfo *request = new ChannelInfo();
    request->targetId = channelId;
    request->name = channelName;
    request->portrait = channelPortrait;
    request->status = status;
    request->desc = desc;
    request->extra = extra;
    request->secret = secret;
    request->callback = cb;
    request->automatic = 0;

    publishTask(request, new CreateChannelPublishCallback(callback, tChannelInfo), createChannelTopic, false);
}

void modifyChannelInfo(const std::string &channelId, int type, const std::string &newValue, GeneralOperationCallback *callback) {
    ModifyChannelInfo *request = new ModifyChannelInfo();
    request->channelId = channelId;
    request->type = type;
    request->value = newValue;

    publishTask(request, new ModifyChannelPublishCallback(callback, channelId, type, newValue), modifyChannelInfoTopic, false);

}

void transferChannel(const std::string &channelId, const std::string &newOwner, GeneralOperationCallback *callback) {
    TransferChannelRequest *request = new TransferChannelRequest();
    request->channelId = channelId;
    request->newOwner = newOwner;
    publishTask(request, new TransferChannelPublishCallback(callback, channelId, newOwner), transferChannelInfoTopic, false);
}

void destoryChannel(const std::string &channelId, GeneralOperationCallback *callback) {
    IDBuf *request = new IDBuf();
    request->id = channelId;
    publishTask(request, new DestoryChannelPublishCallback(callback, channelId), destoryChannelInfoTopic, false);
}

        class SearchChannelPublishCallback : public MQTTPublishCallback {
        private:
            SearchChannelCallback *mCallback;
        public:
            SearchChannelPublishCallback(SearchChannelCallback *callback) : mCallback(callback) {}

            void onSuccess(const unsigned char* data, size_t len) {
                SearchChannelResult result;
                if(result.unserializeFromPBData(data, (int)len)) {
                    std::list<TChannelInfo> userInfos;

                    for (std::list<ChannelInfo>::iterator it = result.channels.begin(); it != result.channels.end(); it++) {

                        const ChannelInfo &u = *it;
                        TChannelInfo userInfo;
                        convertChannel(u, userInfo);
                        userInfos.push_back(userInfo);
                    }
                    mCallback->onSuccess(userInfos, result.keyword);
                } else {
                    mCallback->onFalure(kEcProtoCorruptData);
                }
                delete this;
            }

            void onFalure(int errorCode) {
                mCallback->onFalure(errorCode);
                delete this;
            }

            virtual ~SearchChannelPublishCallback() {

            }
        };

void searchChannel(const std::string &keyword, bool puzzy, SearchChannelCallback *callback) {
    SearchUserRequest *request = new SearchUserRequest();
    request->keyword = keyword;
    request->buzzy = puzzy;
    request->page = 0;
    publishTask(request, new SearchChannelPublishCallback(callback), channelSearchTopic, false);
}

void listenChannel(const std::string &channelId, bool listen, GeneralOperationCallback *callback) {
    ListenChannelRequest *request = new ListenChannelRequest();
    request->channelId = channelId;
    request->listen = listen ? 1 : 0;
    publishTask(request, new GeneralOperationPublishCallback(callback), channelListenTopic, false);
}
        class LoadChannelPublishCallback : public MQTTPublishCallback {
        public:
            LoadChannelPublishCallback(GetChannelInfoCallback *cb) : MQTTPublishCallback(), callback(cb) {}
            GetChannelInfoCallback *callback;
            void onSuccess(const unsigned char* data, size_t len) {
                ChannelInfo info;
                if (info.unserializeFromPBData(data, (int)len)) {
                    TChannelInfo tInfo;
                    tInfo.channelId = info.targetId;
                    tInfo.name = info.name;
                    tInfo.portrait = info.portrait;
                    tInfo.desc = info.desc;
                    tInfo.extra = info.extra;
                    tInfo.status = info.status;
                    tInfo.updateDt = info.updateDt;
                    tInfo.owner = info.owner;
                    tInfo.secret = info.secret;
                    tInfo.callback = info.callback;
                    tInfo.automatic = info.automatic;

                    if (callback) {
                        std::list<mars::stn::TChannelInfo> channelInfoList;
                        channelInfoList.push_back(tInfo);
                        callback->onSuccess(channelInfoList);
                    }
                } else {
                if(callback)
                    callback->onFalure(kEcProtoCorruptData);
                }
                delete this;
            };
            void onFalure(int errorCode) {
                if(callback)
                    callback->onFalure(errorCode);
                delete this;
            };
            virtual ~LoadChannelPublishCallback() {

            }
        };
void reloadChannelInfoFromRemote(const std::string &channelId, int64_t updateDt, GetChannelInfoCallback *callback) {
    PullChannelInfoRequest *request = new PullChannelInfoRequest();
    request->channelId = channelId;
    request->head = updateDt;
    publishTask(request, new LoadChannelPublishCallback(callback), channelPullTopic, false);
}

#if WFCHAT_PROTO_SERIALIZABLE
        bool getValue(const Value &value, const std::string tag, int &ret) {
            if (value.HasMember(tag)) {
                const Value &v = value[tag];
                if (v.IsInt()) {
                    ret = v.GetInt();
                    return true;
                }
            }
            return false;
        }

        bool getValue(const Value &value, const std::string tag, long &ret) {
            if (value.HasMember(tag)) {
                const Value &v = value[tag];
                if (v.IsInt64()) {
                    ret = v.GetInt64();
                    return true;
                } else if (v.IsInt()) {
                    ret = v.GetInt();
                    return true;
                }
            }
            return false;
        }

        bool getValue(const Value &value, const std::string tag, int64_t &ret) {
            if (value.HasMember(tag)) {
                const Value &v = value[tag];
                if (v.IsInt64()) {
                    ret = v.GetInt64();
                    return true;
                }
            }
            return false;
        }

        bool getValue(const Value &value, const std::string tag, bool &ret) {
            if (value.HasMember(tag)) {
                const Value &v = value[tag];
                if (v.IsBool()) {
                    ret = v.GetBool();
                    return true;
                }
            }
            return false;
        }


        bool getValue(const Value &value, const std::string tag, std::string &ret) {
            if (value.HasMember(tag)) {
                const Value &v = value[tag];
                if (v.IsString()) {
                    ret = v.GetString();
                    return true;
                }
            }
            return false;
        }

        bool getValue(const Value &value, const std::string tag, TSerializable &ret) {
            if (value.HasMember(tag)) {
                const Value &v = value[tag];
                if (v.IsObject()) {
                    ret.Unserialize(v);
                    return true;
                }
            }
            return false;
        }

        bool getValue(const Value &value, const std::string tag, std::list<std::string> &ret) {
            if (value.HasMember(tag)) {
                const Value &v = value[tag];
                if (v.IsArray()) {
                    for (int i = 0; i < v.Size(); i++) {
                        const Value &t = v[i];
                        if (t.IsString()) {
                            ret.push_back(t.GetString());
                        }
                    }
                    return true;
                }
            }
            return false;
        }


        std::string base64Decode(const std::string &base64str) {
            if (base64str.empty()) {
                return std::string();
            }

            int len2 = (int)modp_b64_decode_len(base64str.size());
            char * tmp = (char *)calloc( len2, sizeof(unsigned char));
            int len = (int)Comm::DecodeBase64((unsigned char*)base64str.c_str(), (unsigned char*)tmp, (int)base64str.size());
            std::string result = std::string(tmp, len);
            free(tmp);

            return result;
        }

        std::string base64Encode(const std::string &str) {
            if (str.empty()) {
                return std::string();
            }

            unsigned int tmpLen = (unsigned int)str.length();
            const unsigned char *ptmp = (const unsigned char *)str.c_str();


            int dstlen = modp_b64_encode_len(tmpLen);

            char* dstbuf = (char*)malloc(dstlen);
            memset(dstbuf, 0, dstlen);

            int retsize = Comm::EncodeBase64(ptmp, (unsigned char*)dstbuf, tmpLen);
            dstbuf[retsize] = '\0';


            std::string result = std::string(dstbuf);
            free(dstbuf);
            dstbuf = NULL;

            return result;
        }

        void TMessageContent::Unserialize(const Value& value) {

            if (!value.IsObject()) {
                return;
            }

//            int type;
            getValue(value, "type", type);

//            std::string searchableContent;
            getValue(value, "searchableContent", searchableContent);

//            std::string pushContent;
            getValue(value, "pushContent", pushContent);

//            std::string content;
            getValue(value, "content", content);

//            std::string binaryContent;
            getValue(value, "binaryContent", binaryContent);
            binaryContent = base64Decode(binaryContent);

//            std::string localContent;
            getValue(value, "localContent", localContent);

//            int mediaType;
            getValue(value, "mediaType", mediaType);

//            std::string remoteMediaUrl;
            getValue(value, "remoteMediaUrl", remoteMediaUrl);

//            std::string localMediaPath;
            getValue(value, "localMediaPath", localMediaPath);
//
//            int mentionedType;
            getValue(value, "mentionedType", mentionedType);

//            std::list<std::string> mentionedTargets;
            getValue(value, "mentionedTargets", mentionedTargets);
        }


        void TMessageContent::Serialize(void *pwriter) const  {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

//            int type;
            writer.String("type");
            writer.Int(type);

//            std::string searchableContent;
            writer.String("searchableContent");
            writer.String(searchableContent);

//            std::string pushContent;
            writer.String("pushContent");
            writer.String(pushContent);

//            std::string content;
            writer.String("content");
            writer.String(content);

//            std::string binaryContent;
            writer.String("binaryContent");
            std::string base64edStr = base64Encode(binaryContent);
            writer.String(base64edStr);

//            std::string localContent;
            writer.String("localContent");
            writer.String(localContent);

//            int mediaType;
            writer.String("mediaType");
            writer.Int(mediaType);

//            std::string remoteMediaUrl;
            writer.String("remoteMediaUrl");
            writer.String(remoteMediaUrl);

//            std::string localMediaPath;
            writer.String("localMediaPath");
            writer.String(localMediaPath);
//
//            int mentionedType;
            writer.String("mentionedType");
            writer.Int(mentionedType);

//            std::list<std::string> mentionedTargets;
            writer.String("mentionedTargets");
            writer.StartArray();
            for (std::list<std::string>::const_iterator it = mentionedTargets.begin(); it != mentionedTargets.end(); ++it) {
                writer.String(*it);
            }
            writer.EndArray();

            writer.EndObject();
        }

        void TMessage::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }

//            int conversationType;
            getValue(value, "conversationType", conversationType);
//            std::string target;
            getValue(value, "target", target);
//            int line;
            getValue(value, "line", line);
//            std::string from;
            getValue(value, "from", from);
//            TMessageContent content;
            getValue(value, "content", content);
//            long messageId;
            getValue(value, "messageId", messageId);
//            int direction;
            getValue(value, "direction", direction);
//            MessageStatus status;
            int iStatus = 0;
            getValue(value, "status", iStatus);
            status = (MessageStatus)iStatus;

//            int64_t messageUid;
            getValue(value, "messageUid", messageUid);
//            int64_t timestamp;
            getValue(value, "timestamp", timestamp);
//            std::string to;
            getValue(value, "to", to);
        }

        void TMessage::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

            writer.String("conversation");
            writer.StartObject();
//            int conversationType;
            writer.String("conversationType");
            writer.Int(conversationType);

//            std::string target;
            writer.String("target");
            writer.String(target);

//            int line;
            writer.String("line");
            writer.Int(line);
            writer.EndObject();

//            std::string from;
            writer.String("from");
            writer.String(from);

//            TMessageContent content;
            writer.String("content");
            content.Serialize(&writer);

//            long messageId;
            writer.String("messageId");
            writer.Int64(messageId);

//            int direction;
            writer.String("direction");
            writer.Int(direction);

//            MessageStatus status;
            writer.String("status");
            writer.Int((int)status);

//            int64_t messageUid;
            writer.String("messageUid");
            writer.Int64(messageUid);

//            int64_t timestamp;
            writer.String("timestamp");
            writer.Int64(timestamp);

//            std::string to;
            writer.String("to");
            writer.StartArray();
            for (std::list<std::string>::const_iterator it = to.begin(); it != to.end(); ++it) {
                writer.String(*it);
            }
            writer.EndArray();

            writer.EndObject();
        }

        void TGroupInfo::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }
//            std::string target;
            getValue(value, "target", target);
//            std::string name;
            getValue(value, "name", name);
//            std::string portrait;
            getValue(value, "portrait", portrait);
//            std::string owner;
            getValue(value, "owner", owner);
//            int type;
            getValue(value, "type", type);
//            int memberCount;
            getValue(value, "memberCount", memberCount);
//            std::string extra;
            getValue(value, "extra", extra);
//            int64_t updateDt;
            getValue(value, "updateDt", updateDt);
        }

        void TGroupInfo::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();
//
//            std::string target;
            writer.String("target");
            writer.String(target);
//            std::string name;
            writer.String("name");
            writer.String(name);
//            std::string portrait;
            writer.String("portrait");
            writer.String(portrait);
//            std::string owner;
            writer.String("owner");
            writer.String(owner);
//            int type;
            writer.String("type");
            writer.Int(type);
//            int memberCount;
            writer.String("memberCount");
            writer.Int(memberCount);
//            std::string extra;
            writer.String("extra");
            writer.String(extra);
//            int64_t updateDt;
            writer.String("updateDt");
            writer.Int64(updateDt);

            writer.EndObject();
        }

        void TGroupMember::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }
//            std::string groupId;
            getValue(value, "groupId", groupId);
//            std::string memberId;
            getValue(value, "memberId", memberId);
//            std::string alias;
            getValue(value, "alias", alias);
//            int type;
            getValue(value, "type", type);
//            int64_t updateDt;
            getValue(value, "updateDt", updateDt);
        }

        void TGroupMember::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

//            std::string groupId;
            writer.String("groupId");
            writer.String(groupId);
//            std::string memberId;
            writer.String("memberId");
            writer.String(memberId);
//            std::string alias;
            writer.String("alias");
            writer.String(alias);
//            int type;
            writer.String("type");
            writer.Int(type);
//            int64_t updateDt;
            writer.String("updateDt");
            writer.Int64(updateDt);
//
            writer.EndObject();
        }


        void TUserInfo::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }

//            std::string uid;
            getValue(value, "uid", uid);
//            std::string name;
            getValue(value, "name", name);
//            std::string displayName;
            getValue(value, "displayName", displayName);
//            int gender;
            getValue(value, "gender", gender);
//            std::string portrait;
            getValue(value, "portrait", portrait);
//            std::string mobile;
            getValue(value, "mobile", mobile);
//            std::string email;
            getValue(value, "email", email);
//            std::string address;
            getValue(value, "address", address);
//            std::string company;
            getValue(value, "company", company);
//            std::string social;
            getValue(value, "social", social);
//            std::string extra;
            getValue(value, "extra", extra);
//            //0 normal; 1 robot; 2 thing;
//            int type;
            getValue(value, "type", type);
//            int64_t updateDt;
            getValue(value, "updateDt", updateDt);
            getValue(value, "friendAlias", friendAlias);
            getValue(value, "groupAlias", groupAlias);
        }

        void TUserInfo::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

//            std::string uid;
            writer.String("uid");
            writer.String(uid);
//            std::string name;
            writer.String("name");
            writer.String(name);
//            std::string displayName;
            writer.String("displayName");
            writer.String(displayName);
//            int gender;
            writer.String("gender");
            writer.Int(gender);
//            std::string portrait;
            writer.String("portrait");
            writer.String(portrait);
//            std::string mobile;
            writer.String("mobile");
            writer.String(mobile);
//            std::string email;
            writer.String("email");
            writer.String(email);
//            std::string address;
            writer.String("address");
            writer.String(address);
//            std::string company;
            writer.String("company");
            writer.String(company);
//            std::string social;
            writer.String("social");
            writer.String(social);
//            std::string extra;
            writer.String("extra");
            writer.String(extra);
            
            writer.String("friendAlias");
            writer.String(friendAlias);
            
            writer.String("groupAlias");
            writer.String(groupAlias);
//            //0 normal; 1 robot; 2 thing;
//            int type;
            writer.String("type");
            writer.Int(type);
//            int64_t updateDt;
            writer.String("updateDt");
            writer.Int64(updateDt);

            writer.EndObject();
        }


        void TChatroomInfo::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }
//            std::string title;
            getValue(value, "title", title);
//            std::string desc;
            getValue(value, "desc", desc);
//            std::string portrait;
            getValue(value, "portrait", portrait);
//            int memberCount;
            getValue(value, "memberCount", memberCount);
//            int64_t createDt;
            getValue(value, "createDt", createDt);
//            int64_t updateDt;
            getValue(value, "updateDt", updateDt);
//            std::string extra;
            getValue(value, "extra", extra);

            
//            //0 normal; 1 not started; 2 end
//            int state;
            getValue(value, "state", state);
        }

        void TChatroomInfo::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

//            std::string title;
            writer.String("title");
            writer.String(title);
//            std::string desc;
            writer.String("desc");
            writer.String(desc);
//            std::string portrait;
            writer.String("portrait");
            writer.String(portrait);
//            int memberCount;
            writer.String("memberCount");
            writer.Int(memberCount);
//            int64_t createDt;
            writer.String("createDt");
            writer.Int64(createDt);
//            int64_t updateDt;
            writer.String("updateDt");
            writer.Int64(updateDt);
//            std::string extra;
            writer.String("extra");
            writer.String(extra);
//            //0 normal; 1 not started; 2 end
//            int state;
            writer.String("state");
            writer.Int(state);

            writer.EndObject();
        }

        void TChatroomMemberInfo::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }

//            int memberCount;
            getValue(value, "memberCount", memberCount);
//            std::list<std::string> olderMembers;
            getValue(value, "olderMembers", olderMembers);
        }

        void TChatroomMemberInfo::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

//            int memberCount;
//            std::list<std::string> olderMembers;

            writer.EndObject();
        }

        void TChannelInfo::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }

//            std::string channelId;
            getValue(value, "channelId", channelId);
//            std::string name;
            getValue(value, "name", name);
//            std::string portrait;
            getValue(value, "portrait", portrait);
//            std::string owner;
            getValue(value, "owner", owner);
//            int status;
            getValue(value, "status", status);
//            std::string desc;
            getValue(value, "desc", desc);
//            std::string extra;
            getValue(value, "extra", extra);
//            std::string secret;
            getValue(value, "secret", secret);
//            std::string callback;
            getValue(value, "callback", callback);
//            int64_t updateDt;
            getValue(value, "updateDt", updateDt);
//            int automatic;
            getValue(value, "automatic", automatic);
        }

        void TChannelInfo::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

//            std::string channelId;
            writer.String("channelId");
            writer.String(channelId);
//            std::string name;
            writer.String("name");
            writer.String(name);
//            std::string portrait;
            writer.String("portrait");
            writer.String(portrait);
//            std::string owner;
            writer.String("owner");
            writer.String(owner);
//            int status;
            writer.String("status");
            writer.Int(status);
//            std::string desc;
            writer.String("desc");
            writer.String(desc);
//            std::string extra;
            writer.String("extra");
            writer.String(extra);
//            std::string secret;
            writer.String("secret");
            writer.String(secret);
//            std::string callback;
            writer.String("callback");
            writer.String(callback);
//            int64_t updateDt;
            writer.String("updateDt");
            writer.Int64(updateDt);
//            int automatic;
            writer.String("automatic");
            writer.Int(automatic);

            writer.EndObject();
        }


        void TUnreadCount::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }

//            int unread;
            getValue(value, "unread", unread);
//            int unreadMention;
            getValue(value, "unreadMention", unreadMention);
//            int unreadMentionAll;
            getValue(value, "unreadMentionAll", unreadMentionAll);
        }

        void TUnreadCount::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

            writer.String("unread");
            writer.Int(unread);
            writer.String("unreadMention");
            writer.Int(unreadMention);
            writer.String("unreadMentionAll");
            writer.Int(unreadMentionAll);

            writer.EndObject();
        }


        void TConversation::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }

//            int conversationType;
            getValue(value, "conversationType", conversationType);
//            std::string target;
            getValue(value, "target", target);
//            int line;
            getValue(value, "line", line);
//            TMessage lastMessage;
            getValue(value, "lastMessage", lastMessage);
//            int64_t timestamp;
            getValue(value, "timestamp", timestamp);
//            std::string draft;
            getValue(value, "draft", draft);
//            TUnreadCount unreadCount;
            getValue(value, "unreadCount", unreadCount);
//            bool isTop;
            getValue(value, "isTop", isTop);
//            bool isSilent;
            getValue(value, "isSilent", isSilent);
        }

        void TConversation::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

//            int conversationType;
            writer.String("conversationType");
            writer.Int(conversationType);
//            std::string target;
            writer.String("target");
            writer.String(target);
//            int line;
            writer.String("line");
            writer.Int(line);
//            TMessage lastMessage;
            writer.String("lastMessage");
            lastMessage.Serialize(pwriter);
//            int64_t timestamp;
            writer.String("timestamp");
            writer.Int64(timestamp);
//            std::string draft;
            writer.String("draft");
            writer.String(draft);
//            TUnreadCount unreadCount;
            writer.String("unreadCount");
            unreadCount.Serialize(pwriter);
//            bool isTop;
            writer.String("isTop");
            writer.Bool(isTop);
//            bool isSilent;
            writer.String("isSilent");
            writer.Bool(isSilent);

            writer.EndObject();
        }




        void TConversationSearchresult::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }

//            int conversationType;
            getValue(value, "conversationType", conversationType);
//            std::string target;
            getValue(value, "target", target);
//            int line;
            getValue(value, "line", line);
//            //only marchedCount == 1, load the message
//            TMessage marchedMessage;
            getValue(value, "marchedMessage", marchedMessage);
//            int64_t timestamp;
            getValue(value, "timestamp", timestamp);
//            int marchedCount;
            getValue(value, "marchedCount", marchedCount);
        }

        void TConversationSearchresult::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

//            int conversationType;
            writer.String("conversationType");
            writer.Int(conversationType);
//            std::string target;
            writer.String("target");
            writer.String(target);
//            int line;
            writer.String("line");
            writer.Int(line);
//            //only marchedCount == 1, load the message
//            TMessage marchedMessage;
            writer.String("marchedMessage");
            marchedMessage.Serialize(&writer);
//            int64_t timestamp;
            writer.String("timestamp");
            writer.Int64(timestamp);
//            int marchedCount;
            writer.String("marchedCount");
            writer.Int(marchedCount);
            writer.EndObject();
        }


        void TGroupSearchResult::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }

//            TGroupInfo groupInfo;
            getValue(value, "groupInfo", groupInfo);
//            int marchedType;  //0 march name, 1 march group member, 2 both
            getValue(value, "marchedType", marchedType);
//            std::list<std::string> marchedMemberNames;
            getValue(value, "marchedMemberNames", marchedMemberNames);
        }

        void TGroupSearchResult::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

//            TGroupInfo groupInfo;
            writer.String("groupInfo");
            groupInfo.Serialize(&writer);

//            int marchedType;  //0 march name, 1 march group member, 2 both
            writer.String("marchedType");
            writer.Int(marchedType);
//            std::list<std::string> marchedMemberNames;
            writer.String("marchedMemberNames");
            writer.StartArray();
            for (std::list<std::string>::const_iterator it = marchedMemberNames.begin(); it != marchedMemberNames.end(); ++it) {
                writer.String(*it);
            }
            writer.EndArray();

            writer.EndObject();
        }

        void TFriendRequest::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }

//            int direction;
            getValue(value, "direction", direction);
//            std::string target;
            getValue(value, "target", target);
//            std::string reason;
            getValue(value, "reason", reason);
//            int status;
            getValue(value, "status", status);
//            int readStatus;
            getValue(value, "readStatus", readStatus);
//            int64_t timestamp;
            getValue(value, "timestamp", timestamp);
        }

        void TFriendRequest::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

//            int direction;
            writer.String("direction");
            writer.Int(direction);
//            std::string target;
            writer.String("target");
            writer.String(target);
//            std::string reason;
            writer.String("reason");
            writer.String(reason);
//            int status;
            writer.String("status");
            writer.Int(status);
//            int readStatus;
            writer.String("readStatus");
            writer.Int(readStatus);
//            int64_t timestamp;
            writer.String("timestamp");
            writer.Int64(timestamp);

            writer.EndObject();
        }



        void TUserSettingEntry::Unserialize(const Value& value) {
            if (!value.IsObject()) {
                return;
            }

//            UserSettingScope scope;
            int iscope = -1;
            getValue(value, "scope", iscope);
            if (iscope != -1) {
                scope = (UserSettingScope)iscope;
            }

//            std::string key;
            getValue(value, "key", key);
//            std::string value;
            getValue(value, "value", this->value);
//            int64_t updateDt;
            getValue(value, "updateDt", updateDt);
        }

        void TUserSettingEntry::Serialize(void *pwriter) const {
            Writer<StringBuffer> &writer = *((Writer<StringBuffer> *)pwriter);

            writer.StartObject();

//            UserSettingScope scope;
            writer.String("scope");
            writer.Int((int)scope);
//            std::string key;
            writer.String("key");
            writer.String(key);
//            std::string value;
            writer.String("value");
            writer.String(value);
//            int64_t updateDt;
            writer.String("updateDt");
            writer.Int64(updateDt);
//
            writer.EndObject();
        }

        bool TSerializable::fromJson(std::string jsonStr) {
            Document document;
            if (document.Parse(jsonStr).HasParseError()) {
                printf("\nParsing to document failure(%s).\n", jsonStr.c_str());
                return false;
            }
            Unserialize(document);
            return true;
        }
        std::string TSerializable::toJson() const {
            StringBuffer sb;
            PrettyWriter<StringBuffer> writer(sb);
            Serialize(&writer);
            return sb.GetString();
        }

        std::string TSerializable::list2Json(std::list<std::string> &strs) {
            StringBuffer sb;
            PrettyWriter<StringBuffer> writer(sb);
            writer.StartArray();
            for (std::list<std::string>::const_iterator it = strs.begin(); it != strs.end(); it++) {
                const std::string &o = *it;
                writer.String(o);
            }
            writer.EndArray();

            return sb.GetString();
        }

        std::string TSerializable::list2Json(std::list<int> &is) {
            StringBuffer sb;
            PrettyWriter<StringBuffer> writer(sb);
            writer.StartArray();
            for (std::list<int>::const_iterator it = is.begin(); it != is.end(); it++) {
                const int &o = *it;
                writer.Int(o);
            }
            writer.EndArray();

            return sb.GetString();
        }
#endif

}}
