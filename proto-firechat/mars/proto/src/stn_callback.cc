// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

/** * created on : 2012-11-28 * author : yerungui, caoshaokun
 */
#include "mars/proto/stn_callback.h"
#include "mars/comm/autobuffer.h"
#include "mars/comm/xlogger/xlogger.h"
#include <mars/baseevent/base_logic.h>
#include "mars/stn/stn.h"
#include "mars/stn/src/net_core.h"//一定要放这里，Mac os 编译
#include "libemqtt.h"
#include "mars/proto/src/Proto/notify_and_pull_message.h"
#include "mars/proto/src/Proto/version.h"
#include "mars/proto/src/Proto/get_user_setting_result.h"
#include "mars/proto/src/Proto/notify_recall_message.h"
#include "mars/proto/src/Proto/con_ack_payload.h"
#include "mars/proto/MessageDB.h"
#include "mars/proto/src/DB2.h"
#include <sstream>
#include <map>
#include <set>
#include "mars/comm/http.h"
#include "mars/app/app.h"
#include "mars/app/app_logic.h"
#include "mars/stn/stn.h"
#include <sys/time.h>
#include <time.h>

void (*shortlink_progress)(uint32_t task_id, uint32_t writed, uint32_t total) =
[](uint32_t task_id, uint32_t writed, uint32_t total) {
    mars::stn::Task task = mars::stn::GetTask(task_id);
    if(task.taskid == 0) {
        return;
    }
    //only callback when data size more than 50KB
    if(task.cmdid == UPLOAD_SEND_OUT_CMDID && total > 50 * 1024) {
		mars::stn::UploadTask *uploadTask = (mars::stn::UploadTask *)task.user_context;
		uploadTask->mCallback->onProgress(writed, total);
	}
};

namespace mars {
    namespace stn {
//        extern const std::string recallMessageTopic;
        const std::string pullMessageTopic = "MP";
        const std::string notifyMessageTopic = "MN";
        const std::string getUserSettingTopic = "UG";
      
       const std::string notifyFriendTopic = "FN";
       const std::string notifyFriendRequestTopic = "FRN";
       const std::string notifySettingTopic = "UN";
       const std::string notifyRecallMsgTopic = "RMN";
        
//        const std::string putUserSettingTopic = "UP";
        
      void setConnectionStatusCallback(ConnectionStatusCallback *callback) {
        StnCallBack::Instance()->setConnectionStatusCallback(callback);
      }
      
      void setReceiveMessageCallback(ReceiveMessageCallback *callback) {
        StnCallBack::Instance()->setReceiveMessageCallback(callback);
      }
      
        void setRefreshUserInfoCallback(GetUserInfoCallback *callback) {
            StnCallBack::Instance()->setGetUserInfoCallback(callback);
        }
        
        void setRefreshGroupInfoCallback(GetGroupInfoCallback *callback) {
            StnCallBack::Instance()->setGetGroupInfoCallback(callback);
        }
        
        void setRefreshGroupMemberCallback(GetGroupMembersCallback *callback) {
            StnCallBack::Instance()->setGetGroupMemberCallback(callback);
        }
        
        
        void setRefreshChannelInfoCallback(GetChannelInfoCallback *callback) {
            StnCallBack::Instance()->setGetChannelInfoCallback(callback);
        }

        void setRefreshFriendListCallback(GetMyFriendsCallback *callback) {
            StnCallBack::Instance()->setGetMyFriendsCallback(callback);
        }

        void setRefreshFriendRequestCallback(GetFriendRequestCallback *callback) {
            StnCallBack::Instance()->setGetFriendRequestCallback(callback);
        }
      
      void setRefreshSettingCallback(GetSettingCallback *callback) {
        StnCallBack::Instance()->setGetSettingCallback(callback);
      }
        void setDNSResult(std::vector<std::string> serverIPs) {
            StnCallBack::Instance()->setDNSResult(serverIPs);
        }
        
      ConnectionStatus getConnectionStatus() {
        return StnCallBack::Instance()->getConnectionStatus();
      }
      
StnCallBack* StnCallBack::instance_ = NULL;
        
StnCallBack* StnCallBack::Instance() {
    if(instance_ == NULL) {
        instance_ = new StnCallBack();
    }
    
    return instance_;
}
        
void StnCallBack::Release() {
    delete instance_;
    instance_ = NULL;
}

      void StnCallBack::setConnectionStatusCallback(ConnectionStatusCallback *callback) {
        m_connectionStatusCB = callback;
      }
      void StnCallBack::setReceiveMessageCallback(ReceiveMessageCallback *callback) {
        m_receiveMessageCB = callback;
      }
      
        void StnCallBack::setGetUserInfoCallback(GetUserInfoCallback *callback) {
            m_getUserInfoCB = callback;
        }
        
        void StnCallBack::setGetMyFriendsCallback(GetMyFriendsCallback *callback) {
            m_getMyFriendsCB = callback;
        }

        void StnCallBack::setGetFriendRequestCallback(GetFriendRequestCallback *callback) {
            m_getFriendRequestCB = callback;
        }
      
      void StnCallBack::setGetSettingCallback(GetSettingCallback *callback) {
        m_getSettingCB = callback;
      }
      
        void StnCallBack::setGetGroupInfoCallback(GetGroupInfoCallback *callback) {
            m_getGroupInfoCB = callback;
        }
        
        void StnCallBack::setGetGroupMemberCallback(GetGroupMembersCallback *callback) {
            m_getGroupMembersCB = callback;
        }
        
        void StnCallBack::setGetChannelInfoCallback(GetChannelInfoCallback *callback) {
            m_getChannelInfoCB = callback;
        }
      
      void StnCallBack::updateConnectionStatus(ConnectionStatus newStatus) {
        m_connectionStatus = newStatus;
          if(m_connectionStatus == kConnectionStatusServerDown) {
              RequestRoute(true);
          }
        if(m_connectionStatusCB) {
          m_connectionStatusCB->onConnectionStatusChanged(m_connectionStatus);
        }
      }
      
        void StnCallBack::setDNSResult(std::vector<std::string> serverIPs) {
            m_serverIPs = serverIPs;
        }
        
bool StnCallBack::MakesureAuthed() {
    return mAuthed;
}


void StnCallBack::TrafficData(ssize_t _send, ssize_t _recv) {
    xdebug2(TSF"send:%_, recv:%_", _send, _recv);
}
extern std::string gHost;
std::vector<std::string> StnCallBack::OnNewDns(const std::string& _host) {
    if (_host != gHost) {
        return std::vector<std::string>();
    }
    
    return m_serverIPs;
}

void StnCallBack::onPullMsgSuccess(std::list<TMessage> messageList, int64_t current, int64_t head, int pullType, bool refreshSetting) {
    if(pullType != Pull_ChatRoom) {
        isPullingMsg = false;
        currentHead = current;
        pullRetryCount = 0;
    } else {
        isPullingChatroomMsg = false;
        currentChatroomHead = current;
    }
    MessageDB::Instance()->UpdateMessageTimeline(current);
    PullMessage(head, pullType, false, refreshSetting);
    
    if(m_receiveMessageCB) {
        m_receiveMessageCB->onReceiveMessage(messageList, head > current);
    }
}
void StnCallBack::onPullMsgFailure(int errorCode, int pullType) {
    if (pullType != Pull_ChatRoom) {
        isPullingMsg = false;
        pullRetryCount++;
        PullMessage(0x7FFFFFFFFFFFFFFF, pullType, true, false);
    } else {
        isPullingChatroomMsg = false;
    }
}
        void StnCallBack::converProtoMessage(const Message &pmsg, TMessage &tmsg, bool saveToDb, const std::string &curUser) {
            
            tmsg.conversationType = (int)pmsg.conversation.type;
            tmsg.line = (int)pmsg.conversation.line;
            
            tmsg.messageUid = pmsg.messageId;
            tmsg.messageId = 0;
            tmsg.timestamp = pmsg.serverTimestamp;
            tmsg.content.type = pmsg.content.type;
            tmsg.content.searchableContent = pmsg.content.searchableContent;
            tmsg.content.pushContent = pmsg.content.pushContent;
            tmsg.content.content = pmsg.content.content;
            tmsg.content.binaryContent = pmsg.content.data;
            tmsg.content.mediaType = pmsg.content.mediaType;
            tmsg.content.remoteMediaUrl = pmsg.content.remoteMediaUrl;
            tmsg.content.mentionedType = pmsg.content.mentionedType;
            tmsg.content.mentionedTargets = pmsg.content.mentionedTargets;
            
            tmsg.from = pmsg.fromUser;
            tmsg.to = pmsg.tos;
            if (tmsg.from == curUser) {
                tmsg.target = pmsg.conversation.target;
                tmsg.direction = 0;
                tmsg.status = Message_Status_Sent;
            } else {
                if(tmsg.conversationType == 0) {
                    tmsg.target = pmsg.fromUser;
                } else {
                    tmsg.target = pmsg.conversation.target;
                }
                tmsg.direction = 1;
                tmsg.status = Message_Status_Unread;
                if(tmsg.content.mentionedType == 2) {
                    tmsg.status = Message_Status_AllMentioned;
                } else if(tmsg.content.mentionedType == 1) {
                    for(std::list<std::string>::iterator it = tmsg.content.mentionedTargets.begin(); it != tmsg.content.mentionedTargets.end(); it++) {
                        if (*it == curUser) {
                            tmsg.status = Message_Status_Mentioned;
                            break;
                        }
                    }
                }
            }
            
            if (saveToDb) {
                long id = MessageDB::Instance()->InsertMessage(tmsg);
                tmsg.messageId = id;
                
                if(id > 0) {
                    MessageDB::Instance()->updateConversationTimestamp(tmsg.conversationType, tmsg.target, tmsg.line, tmsg.timestamp);
                }
            } else {
                tmsg.messageId = 0;
            }
        }
        
        class PullingMessagePublishCallback : public MQTTPublishCallback {
        public:
            PullingMessageCallback *cb;
            int mPullType;
            bool mRefreshSetting;
            PullingMessagePublishCallback(PullingMessageCallback *callback, int pullType, bool refreshSetting) : MQTTPublishCallback(), cb(callback), mPullType(pullType), mRefreshSetting(refreshSetting) {}
            
            void onSuccess(const unsigned char* data, size_t len) {
                std::list<TMessage> messageList;
                PullMessageResult result;
                std::string curUser = app::GetAccountUserName();
                if (result.unserializeFromPBData((const void *)data, (int)len)) {
                    bool isBegin = false;
                    if (result.messages.size() > 100 && mPullType != Pull_ChatRoom) {
                        isBegin = DB2::Instance()->BEGIN();
                    }
                    std::set<std::string> needUpdateGroup;
                    std::set<std::string> needUpdateGroupMember;
                    for (std::list<Message>::iterator it = result.messages.begin(); it != result.messages.end(); it++) {
                        TMessage tmsg;
                        StnCallBack::Instance()->converProtoMessage(*it, tmsg, true, curUser);
                        messageList.push_back(tmsg);
                        
//                        #define MESSAGE_CONTENT_TYPE_CREATE_GROUP 104
//                        #define MESSAGE_CONTENT_TYPE_ADD_GROUP_MEMBER 105
//                        #define MESSAGE_CONTENT_TYPE_KICKOF_GROUP_MEMBER 106
//                        #define MESSAGE_CONTENT_TYPE_QUIT_GROUP 107
//                        #define MESSAGE_CONTENT_TYPE_DISMISS_GROUP 108
//                        #define MESSAGE_CONTENT_TYPE_TRANSFER_GROUP_OWNER 109
//#define MESSAGE_CONTENT_TYPE_CHANGE_GROUP_NAME 110
//#define MESSAGE_CONTENT_TYPE_MODIFY_GROUP_ALIAS 111
//#define MESSAGE_CONTENT_TYPE_CHANGE_GROUP_PORTRAIT 112
                        if (tmsg.conversationType == 1) {
                            if (tmsg.content.type == MESSAGE_CONTENT_TYPE_CREATE_GROUP
                                || tmsg.content.type == MESSAGE_CONTENT_TYPE_ADD_GROUP_MEMBER
                                || tmsg.content.type == MESSAGE_CONTENT_TYPE_KICKOF_GROUP_MEMBER
                                || tmsg.content.type == MESSAGE_CONTENT_TYPE_TRANSFER_GROUP_OWNER
                                || tmsg.content.type == MESSAGE_CONTENT_TYPE_MODIFY_GROUP_ALIAS) {
                                needUpdateGroup.insert(tmsg.target);
                                needUpdateGroupMember.insert(tmsg.target);
                            } else if (tmsg.content.type == MESSAGE_CONTENT_TYPE_CHANGE_GROUP_NAME
                                       || tmsg.content.type == MESSAGE_CONTENT_TYPE_CHANGE_GROUP_PORTRAIT) {
                                needUpdateGroup.insert(tmsg.target);
                            } else if (tmsg.content.type == MESSAGE_CONTENT_TYPE_QUIT_GROUP
                                       || tmsg.content.type == MESSAGE_CONTENT_TYPE_DISMISS_GROUP) {
                                MessageDB::Instance()->RemoveGroupAndMember(tmsg.target);
                                MessageDB::Instance()->ClearUnreadStatus(tmsg.conversationType, tmsg.target, tmsg.line);
                                MessageDB::Instance()->RemoveConversation(tmsg.conversationType, tmsg.target, tmsg.line);
                            }
                        }
                    }
                    
                    for (std::set<std::string>::iterator it = needUpdateGroup.begin(); it != needUpdateGroup.end(); it++) {
                        MessageDB::Instance()->GetGroupInfo(*it, true);
                    }
                    
                    for (std::set<std::string>::iterator it = needUpdateGroupMember.begin(); it != needUpdateGroupMember.end(); it++) {
                        MessageDB::Instance()->GetGroupMembers(*it, true);
                    }
                    
                    if(isBegin) {
                        DB2::Instance()->COMMIT();
                    }
                    
                    cb->onPullMsgSuccess(messageList, result.current, result.head, mPullType, mRefreshSetting);
                    if (mRefreshSetting) {
                        StnCallBack::Instance()->PullSetting(0x7FFFFFFFFFFFFFFF);
                    }
                } else {
                    cb->onPullMsgFailure(-1, mPullType);
                }
                delete this;
            };
            void onFalure(int errorCode) {
                cb->onPullMsgFailure(errorCode, mPullType);
                delete this;
            };
            virtual ~PullingMessagePublishCallback() {
                
            }
        };
        
void StnCallBack::PullMessage(int64_t head, int type, bool retry, bool refreshSetting) {
    PullMessageRequest request;
    request.type = (PullType)type;
    
    if(type != Pull_ChatRoom) {
        if (isPullingMsg || currentHead >= head) {
            if (currentHead >= head && m_connectionStatus != kConnectionStatusConnected) {
                updateConnectionStatus(kConnectionStatusConnected);
            }
            return;
        }
        if (retry && pullRetryCount > 10) {
            updateConnectionStatus(kConnectionStatusUnconnected);
            return;
        }
        
        isPullingMsg = true;
        request.id = currentHead;
    } else  {
        if(isPullingChatroomMsg || currentChatroomHead >= head) {
            return;
        }
        isPullingChatroomMsg = true;
        request.id = currentChatroomHead;
    }
    
    std::string output = request.serializeToPBData();
    mars::stn::MQTTPublishTask *publishTask = new mars::stn::MQTTPublishTask(new PullingMessagePublishCallback(this, request.type, refreshSetting));
    publishTask->topic = pullMessageTopic;
    publishTask->length = output.length();
    publishTask->body = new unsigned char[publishTask->length];
    memcpy(publishTask->body, output.c_str(), publishTask->length);
    mars::stn::StartTask(*publishTask);
}
        class GetUserSettingPublishCallback : public MQTTPublishCallback {
        private:
            bool* pFlag;
        public:
            GetUserSettingPublishCallback(bool* gettingFlag) : MQTTPublishCallback(), pFlag(gettingFlag) {}
            
            void onSuccess(const unsigned char* data, size_t len) {
                std::list<TUserSettingEntry> retList;
                GetUserSettingResult result;
                std::string curUser = app::GetAccountUserName();
                
                if (result.unserializeFromPBData((const void *)data, (int)len)) {
                    int64_t maxDt = 0;
                    for (std::list<UserSettingEntry>::iterator it = result.entrys.begin(); it != result.entrys.end(); it++) {
                        UserSettingEntry &entry = *it;
                        TUserSettingEntry tentry;
                        tentry.scope = (UserSettingScope)entry.scope;
                        tentry.key = entry.key;
                        tentry.value = entry.value;
                        tentry.updateDt = entry.updateDt;
                        retList.push_back(tentry);
                        if (tentry.updateDt > maxDt) {
                            maxDt = tentry.updateDt;
                        }
                    }
                    if (maxDt > 0) {
                        StnCallBack::Instance()->settingHead = maxDt;
                    }
                  if(StnCallBack::Instance()->m_getSettingCB != NULL) {
                    StnCallBack::Instance()->m_getSettingCB->onSuccess(retList.size() > 0);
                  }
                    MessageDB::Instance()->UpdateUserSettings(retList);
                } else {
                  if(StnCallBack::Instance()->m_getSettingCB != NULL) {
                    StnCallBack::Instance()->m_getSettingCB->onFalure(-1);
                  }
                }
                delete this;
            };
            void onFalure(int errorCode) {
              if(StnCallBack::Instance()->m_getSettingCB != NULL) {
                StnCallBack::Instance()->m_getSettingCB->onFalure(errorCode);
              }
                delete this;
            };
            virtual ~GetUserSettingPublishCallback() {
                if(pFlag)
                *pFlag = false;
                pFlag = NULL;
            }
        };
void StnCallBack::PullSetting(int64_t head) {
    if (isPullingSetting || settingHead >= head) {
        return;
    }
    isPullingSetting = true;
    Version request;
    request.version = settingHead;
    
    std::string output = request.serializeToPBData();
    mars::stn::MQTTPublishTask *publishTask = new mars::stn::MQTTPublishTask(new GetUserSettingPublishCallback(&isPullingSetting));
    publishTask->topic = getUserSettingTopic;
    publishTask->length = output.length();
    publishTask->body = new unsigned char[publishTask->length];
    memcpy(publishTask->body, output.c_str(), publishTask->length);
    mars::stn::StartTask(*publishTask);
}
static int64_t GetLongLongFromBuffer(unsigned char *buffer) {
    int64_t ret = 0;
    for (int i = 0; i < 8; i++) {
        ret <<= 8;
        ret += *(buffer + i);
    }
    return ret;
}
void StnCallBack::OnPush(uint64_t _channel_id, uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend) {
    std::string topic = (char *)(_body.Ptr());
    if (topic.compare(notifyMessageTopic) == 0) {
        NotifyMessage notifyMessage;
        if (notifyMessage.unserializeFromPBData(_extend.Ptr(), (int)_extend.Length())) {
            PullMessage(notifyMessage.head, (int)notifyMessage.type, false, false);
        }
    } else if (topic.compare(notifyFriendTopic) == 0) {
      if (_extend.Length() == 8) {
        PullFriend(GetLongLongFromBuffer((unsigned char*)_extend.Ptr()));
      }
    } else if(topic.compare(notifyFriendRequestTopic) == 0) {
      if (_extend.Length() == 8) {
        PullFriendRequest(GetLongLongFromBuffer((unsigned char*)_extend.Ptr()));
      }
    } else if(topic.compare(notifySettingTopic) == 0) {
      if (_extend.Length() == 8) {
        PullSetting(GetLongLongFromBuffer((unsigned char*)_extend.Ptr()));
      }
    } else if(topic.compare(notifyRecallMsgTopic) == 0) {
        NotifyRecallMessage recall;
        if (recall.unserializeFromPBData(_extend.Ptr(), (int)_extend.Length())) {
            TMessage tmsg = MessageDB::Instance()->GetMessageByUid(recall.messageUid);
            if(tmsg.messageId == 0) {
                return;
            }
            
            //注意：在proto层收到撤回命令或主动撤回成功会直接更新被撤回的消息，如果修改这里，需要同步修改client的encode&decode
            TMessageContent tcontent;
            tcontent.content = recall.operatorId;
            
            std::stringstream stream;
            stream << recall.messageUid;
            
            tcontent.binaryContent = stream.str();
            tcontent.type = 80;
            MessageDB::Instance()->UpdateMessageContent(tmsg.messageId, tcontent);
            
            if(m_receiveMessageCB) {
                m_receiveMessageCB->onRecallMessage(recall.operatorId, recall.messageUid);
            }
        }
    } else if(topic.compare("MS") == 0) {
        Message message;
        if (message.unserializeFromPBData(_extend.Ptr(), (int)_extend.Length())) {
            std::list<TMessage> messageList;
            TMessage tmsg;
            std::string curUser = app::GetAccountUserName();
            StnCallBack::Instance()->converProtoMessage(message, tmsg, false, curUser);
            messageList.push_back(tmsg);
            m_receiveMessageCB->onReceiveMessage(messageList, false);
        }
    }
}
        
 
        std::string mapToString(const std::map<std::string, std::string>& m)
        {
            std::string str="";
            typename std::map<std::string, std::string>::const_iterator it = m.begin();
            for(;it != m.end();it++)
            {
                str += "{\"";
                str += it->first + "\":\"" + it->second;
                str += "\"}";
            }
            return str;
        }

        
static const std::string UploadBoundary = "--727f6ee7446cbf7263";

void packageUploadMediaData(const std::string &data, AutoBuffer& _out_buff, AutoBuffer& _extend, unsigned char mediaType, const std::string &uploadToken) {
    
    std::string fileName;
    std::stringstream ss;
    ss << mars::app::GetAccountUserName();
    ss << "-";
    ss << time(NULL);
    ss << "-";
    ss << rand()%10000;
    ss >> fileName;
    
    std::string mimeType;
    if (mediaType == 3) {
        mimeType = "image_jpeg";
    } else if(mediaType == 2) {
        mimeType = "audio_amr";
    } else {
        mimeType = "application_octet-stream";
    }
    
    std::string firstBody = "--" + UploadBoundary + "\r\nContent-Disposition: form-data; name=\"token\"\r\n\r\n"
    + uploadToken + "\r\n--" + UploadBoundary + "\r\nContent-Disposition: form-data; name=\"key\"\r\n\r\n" + fileName + "\r\n--"
    + UploadBoundary + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"" + fileName + "\"\r\nContent-Type: " + mimeType + "\r\n\r\n";
    
    std::string lastBody =  "\r\n--" + UploadBoundary + "--";
    
    unsigned int dataLen = (unsigned int)(data.size() + firstBody.length() + lastBody.length());
    char len_str[32] = {0};
    snprintf(len_str, sizeof(len_str), "%u", dataLen);
    
    _out_buff.AllocWrite(dataLen);
    _out_buff.Write(firstBody.c_str(), firstBody.length());
    _out_buff.Write(data.c_str(), data.length());
    _out_buff.Write(lastBody.c_str(), lastBody.length());
    
    std::map<std::string, std::string> paramMap;
    paramMap["method"] = "POST";
    paramMap[http::HeaderFields::KStringContentType] = "multipart/form-data; boundary=" + UploadBoundary;
    paramMap[http::HeaderFields::KStringContentLength] = len_str;
    
    
    std::string mapStr = mapToString(paramMap);
    _extend.AllocWrite(mapStr.size());
    _extend.Write(mapStr.c_str(), mapStr.size());
}


        
bool StnCallBack::Req2Buf(uint32_t _taskid, void* const _user_context, AutoBuffer& _outbuffer, AutoBuffer& _extend, int& _error_code, const int _channel_select) {
    Task *task = (Task *)_user_context;
    if(task->cmdid == UPLOAD_SEND_OUT_CMDID) {
        UploadTask *uploadTask = (UploadTask *)_user_context;
        packageUploadMediaData(uploadTask->mData, _outbuffer, _extend, uploadTask->mMediaType, uploadTask->mToken);;
    } else { //MQTT tasks
      const MQTTTask *mqttTask = (const MQTTTask *)_user_context;
      if (mqttTask->type == MQTT_MSG_PUBLISH) {
        const MQTTPublishTask *publishTask = (const MQTTPublishTask *)_user_context;
          if(publishTask->pbData) {
              const std::string data = publishTask->pbData->serializeToPBData();
              _extend.AllocWrite(data.length());
              _extend.Write(data.c_str(), data.length());
          } else {
              _extend.AllocWrite(publishTask->length);
              _extend.Write(publishTask->body, publishTask->length);
          }
        
        _outbuffer.AllocWrite(publishTask->topic.length());
        _outbuffer.Write(publishTask->topic.c_str(), publishTask->topic.length());
      }  else if (mqttTask->type == MQTT_MSG_DISCONNECT) {
        const MQTTDisconnectTask *disconnectTask = (const MQTTDisconnectTask *)_user_context;
        _outbuffer.AllocWrite(1);
        _outbuffer.Write(&(disconnectTask->flag), 1);
      } else if (mqttTask->type == MQTT_MSG_SUBSCRIBE){
        const MQTTSubscribeTask *subscribeTask = (const MQTTSubscribeTask *)_user_context;
        _outbuffer.AllocWrite(subscribeTask->topic.length());
        _outbuffer.Write(subscribeTask->topic.c_str(), subscribeTask->topic.length());
      } else if (mqttTask->type == MQTT_MSG_UNSUBSCRIBE){
        const MQTTUnsubscribeTask *unsubscribeTask = (const MQTTUnsubscribeTask *)_user_context;
        _outbuffer.AllocWrite(unsubscribeTask->topic.length());
        _outbuffer.Write(unsubscribeTask->topic.c_str(), unsubscribeTask->topic.length());
      }
    }
    
    xinfo2(TSF"PROTO -> TASK after package:%0", task->description());

  return true;
}

        
int StnCallBack::Buf2Resp(uint32_t _taskid, void* const _user_context, const AutoBuffer& _inbuffer, const AutoBuffer& _extend, int& _error_code, const int _channel_select) {
    
    Task *task = (Task *)_user_context;
    std::string className = typeid(_user_context).name();
    
    if(task->cmdid == UPLOAD_SEND_OUT_CMDID) {
        UploadTask *uploadTask = (UploadTask *)_user_context;

        std::string result((char *)_inbuffer.Ptr(), _inbuffer.Length());
        long index = result.find("\"key\":\"");
        if (index > 0 && index < INT_MAX) {
            std::string rest = result.substr(index + 7);
            index = rest.find("\"");
            if (index > 0 && index < INT_MAX) {
                std::string key = rest.substr(0, index);
                if (!key.empty()) {
                    xinfo2(TSF"PROTO -> Upload success:%0", key);
                    uploadTask->mCallback->onSuccess(key);
                    return mars::stn::kTaskFailHandleNormal;
                }
            }
        }
        xinfo2(TSF"PROTO -> Upload failure:%0", result);
        uploadTask->mCallback->onFalure(-1);
        return mars::stn::kTaskFailHandleNormal;
    }
    
  const MQTTTask *mqttTask = (const MQTTTask *)_user_context;
  if (mqttTask->type == MQTT_MSG_PUBLISH) {
      xinfo2(TSF"PROTO -> TASK(%0) has response", task->description());
      xinfo2(TSF"PROTO -> TASK errorcode:%0", _error_code);
    const MQTTPublishTask *publishTask = (const MQTTPublishTask *)_user_context;
      if (_error_code == 0) {
          if (_inbuffer.Length() < 1) {
              if(publishTask->m_callback)
                  publishTask->m_callback->onFalure(kEcProtoCorruptData);
          } else {
              unsigned char *p = (unsigned char *)_inbuffer.Ptr();
              xinfo2(TSF"PROTO -> TASK business code:%0(0success, otherwise failure)", *p);
              if (*p == 0) {
                  if(publishTask->m_callback)
                  publishTask->m_callback->onSuccess((const unsigned char *)(_inbuffer.Ptr()) + 1, (unsigned int)(_inbuffer.Length()-1));
              } else {
                  if(publishTask->m_callback)
                      publishTask->m_callback->onFalure(*p);
              }
          }
      }
    else if(publishTask->m_callback)
      publishTask->m_callback->onFalure(_error_code);
  }  else if (mqttTask->type == MQTT_MSG_DISCONNECT) {
    //const MQTTDisconnectTask *disconnectTask = (const MQTTDisconnectTask *)_user_context;
    //disconnect task no response
  } else if (mqttTask->type == MQTT_MSG_SUBSCRIBE){
    const MQTTSubscribeTask *subscribeTask = (const MQTTSubscribeTask *)_user_context;
    if (_error_code == 0)
      subscribeTask->m_callback->onSuccess();
    else
      subscribeTask->m_callback->onFalure(_error_code);
  } else if (mqttTask->type == MQTT_MSG_UNSUBSCRIBE){
    const MQTTUnsubscribeTask *unsubscribeTask = (const MQTTUnsubscribeTask *)_user_context;
    if (_error_code == 0)
      unsubscribeTask->m_callback->onSuccess();
    else
      unsubscribeTask->m_callback->onFalure(_error_code);
  } else {
    
  }
    int handle_type = mars::stn::kTaskFailHandleNormal;
  
    return handle_type;
}

int StnCallBack::OnTaskEnd(uint32_t _taskid, void* const _user_context, int _error_type, int _error_code) {
    Task *task = (Task *)_user_context;
    if(task->cmdid == UPLOAD_SEND_OUT_CMDID) {
        UploadTask *uploadTask = (UploadTask *)_user_context;
        if (_error_code != 0) {
            xinfo2(TSF"PROTO -> Upload TASK %0 end with error code:%1", task->description(), _error_code);
            uploadTask->mCallback->onFalure(_error_code);
        }
        delete uploadTask;
        return 0;
    }

    if(task->cmdid == MQTT_SEND_OUT_CMDID) {
        const MQTTTask *mqttTask = (const MQTTTask *)_user_context;
        if (mqttTask->type == MQTT_MSG_PUBLISH) {
            const MQTTPublishTask *publishTask = (const MQTTPublishTask *)_user_context;
            if (_error_code != 0) {
                xinfo2(TSF"PROTO -> TASK DESCRIPTIONS:%0 end with orror:%1", task->description(), _error_code);
                if(publishTask->m_callback)
                    publishTask->m_callback->onFalure(_error_code);
            }
        } else if (mqttTask->type == MQTT_MSG_SUBSCRIBE){
            const MQTTSubscribeTask *subscribeTask = (const MQTTSubscribeTask *)_user_context;
            if (_error_code != 0) {
                subscribeTask->m_callback->onFalure(_error_code);
            }
        } else if (mqttTask->type == MQTT_MSG_UNSUBSCRIBE){
            const MQTTUnsubscribeTask *unsubscribeTask = (const MQTTUnsubscribeTask *)_user_context;
            if (_error_code != 0) {
                unsubscribeTask->m_callback->onFalure(_error_code);
            }
        } else if (mqttTask->type == MQTT_MSG_PUBACK){
            //const MQTTPubAckTask *ackTask = (const MQTTPubAckTask *)_user_context;
            if (_error_code != 0) {
                //ack failure
            }
            
        } else {
            
        }
        delete mqttTask;
    }
  return 0;

}


      
void StnCallBack::ReportConnectStatus(int _status, int longlink_status) {
    switch (longlink_status) {
        case mars::stn::kServerDown:
            updateConnectionStatus(kConnectionStatusServerDown);
            break;
        case mars::stn::kServerFailed:
        case mars::stn::kGateWayFailed:
            updateConnectionStatus(kConnectionStatusUnconnected);
            break;
        case mars::stn::kConnecting:
            updateConnectionStatus(kConnectionStatusConnecting);
            break;
        case mars::stn::kNetworkUnkown:
            updateConnectionStatus(kConnectionStatusUnconnected);
            return;
        default:
            return;
    }
}

int  StnCallBack::GetLonglinkIdentifyCheckBuffer(AutoBuffer& _identify_buffer, AutoBuffer& _buffer_hash, int32_t& _cmdid) {
	_cmdid = MQTT_CONNECT_CMDID;
    struct timeval tv;
    if(gettimeofday(&tv, NULL) != -1) {
        mIdentifyTime = (int64_t)tv.tv_sec*1000 + tv.tv_usec/1000;
    } else {
        mIdentifyTime = ((int64_t)time(NULL) + 1)*1000;
    }
    return IdentifyMode::kCheckNow;
}
        
extern void RequestRoute(bool force);
        
bool StnCallBack::OnLonglinkIdentifyResponse(const AutoBuffer& _response_buffer, const AutoBuffer& _identify_buffer_hash) {
  unsigned char * _packed = ( unsigned char *)_response_buffer.Ptr();
  
    /*
     CONNECTION_ACCEPTED((byte) 0x00),
     CONNECTION_REFUSED_UNACCEPTABLE_PROTOCOL_VERSION((byte) 0X01),
     CONNECTION_REFUSED_IDENTIFIER_REJECTED((byte) 0x02),
     CONNECTION_REFUSED_SERVER_UNAVAILABLE((byte) 0x03),
     CONNECTION_REFUSED_BAD_USER_NAME_OR_PASSWORD((byte) 0x04),
     CONNECTION_REFUSED_NOT_AUTHORIZED((byte) 0x05),
     CONNECTION_REFUSED_UNEXPECT_NODE((byte) 0X06);
     CONNECTION_REFUSED_SESSION_NOT_EXIST((byte) 0X07);
     */
  unsigned char ackCode = *(_packed + 1);
  ConnectAckPayload ackPayload;
  if (_response_buffer.Length() > 2) {
    ackPayload.unserializeFromPBData(_packed + 2, (int)_response_buffer.Length() - 2);
  }
  if (ackCode == 0) {
      if(ackPayload.server_time > 0) {
          int64_t cur = ((int64_t)time(NULL) + 1)*1000;
          struct timeval tv;
          if(gettimeofday(&tv, NULL) != -1) {
              cur = (int64_t)tv.tv_sec*1000 + tv.tv_usec/1000;
          }
          nwDelay = (cur - mIdentifyTime)/2;
          deltaTime = ackPayload.server_time - cur - nwDelay;
      }
      mAuthed = true;
      updateConnectionStatus(kConnectionStatusReceiving);
      PullFriend(ackPayload.friend_head);
      PullFriendRequest(ackPayload.friend_rq_head);
      PullMessage(0x7FFFFFFFFFFFFFFF, Pull_Normal, false, ackPayload.setting_head > settingHead);
  } else if(ackCode == 0x06) {
      mAuthed = true;
      int nodePort = ackPayload.node_port;
      std::vector<uint16_t> ports;
      ports.push_back(nodePort >> 16);
      RequestRoute(true);
      mars::stn::MakesureLonglinkConnected();
      
    updateConnectionStatus(kConnectionStatusUnconnected);
    return false;
  } else if(ackCode == 0x07) {
      mAuthed = false;
      RequestRoute(true);
      updateConnectionStatus(kConnectionStatusUnconnected);
      return false;
  } else if(ackCode == 0x02) {
      mAuthed = false;
      updateConnectionStatus(kConnectionStatusRejected);
  } else {
    mAuthed = false;
    RequestRoute(true);
    updateConnectionStatus(kConnectionStatusUnconnected);
    return false;
  }
    return true;
}
//
void StnCallBack::RequestSync() {

}
        void StnCallBack::onDBOpened() {
            currentHead = MessageDB::Instance()->GetMessageTimeline();
            settingHead = MessageDB::Instance()->GetSettingVersion();
        }
        
        
    }
}






