// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

//
//  stn_callback.h
//  MPApp
//
//  Created by yanguoyue on 16/2/29.
//  Copyright © 2016年 tencent. All rights reserved.
//

#ifndef STNCALLBACK_h
#define STNCALLBACK_h
#include "mars/proto/src/business.h"
#include "mars/stn/stn_logic.h"
#include "mars/stn/stn.h"
#include <list>

namespace mars {
    namespace stn {
      
        class PullingMessagePublishCallback;
        class PullingMessageCallback {
            friend PullingMessagePublishCallback;
            virtual void onPullMsgSuccess(std::list<TMessage> messageList, int64_t current, int64_t head, int pullType, bool refreshSetting) = 0;
            virtual void onPullMsgFailure(int errorCode, int pullType) = 0;
        };

        
        class TGetGroupInfoCallback;
        class TGetChannelInfoCallback;
        class RefreshUserInfoCallback;
        class TLoadFriendCallback;
        class Message;
        class LoadRemoteMessagesPublishCallback;
class StnCallBack : public Callback,  PullingMessageCallback {
    
private:
    StnCallBack() : m_connectionStatus(kConnectionStatusLogout), m_connectionStatusCB(NULL), m_receiveMessageCB(NULL), m_getUserInfoCB(NULL), m_getMyFriendsCB(NULL), m_getFriendRequestCB(NULL), m_getSettingCB(NULL), m_getGroupInfoCB(NULL), m_getGroupMembersCB(NULL), m_getChannelInfoCB(NULL), deltaTime(0), nwDelay(0), mAuthed(false), pullRetryCount(0), isPullingMsg(false), isPullingChatroomMsg(false), isPullingSetting(false), currentHead(0), currentChatroomHead(0), settingHead(0) {};
    virtual ~StnCallBack() {}
    StnCallBack(StnCallBack&);
    StnCallBack& operator = (StnCallBack&);
    ConnectionStatus m_connectionStatus;
  ConnectionStatusCallback *m_connectionStatusCB;
  ReceiveMessageCallback *m_receiveMessageCB;
   GetUserInfoCallback *m_getUserInfoCB;
    GetMyFriendsCallback *m_getMyFriendsCB;
    GetFriendRequestCallback *m_getFriendRequestCB;
    GetSettingCallback *m_getSettingCB;
    GetGroupInfoCallback *m_getGroupInfoCB;
    GetGroupMembersCallback *m_getGroupMembersCB;
    GetChannelInfoCallback *m_getChannelInfoCB;
    void converProtoMessage(const Message &pmsg, TMessage &tmsg, bool saveToDb, const std::string &curUser);
  
public:
    static StnCallBack* Instance();
    static void Release();
  void setConnectionStatusCallback(ConnectionStatusCallback *callback);
  void setReceiveMessageCallback(ReceiveMessageCallback *callback);
    void setGetUserInfoCallback(GetUserInfoCallback *callback);
    void setGetMyFriendsCallback(GetMyFriendsCallback *callback);
    void setGetFriendRequestCallback(GetFriendRequestCallback *callback);
  void setGetSettingCallback(GetSettingCallback *callback);
    void setGetGroupInfoCallback(GetGroupInfoCallback *callback);
    void setGetGroupMemberCallback(GetGroupMembersCallback *callback);
    void setGetChannelInfoCallback(GetChannelInfoCallback *callback);
  ConnectionStatus getConnectionStatus() {
    return m_connectionStatus;
  }
    void updateConnectionStatus(ConnectionStatus newStatus);
    virtual bool MakesureAuthed();
    
    //流量统计
    virtual void TrafficData(ssize_t _send, ssize_t _recv);
    
    //底层询问上层该host对应的ip列表
    virtual std::vector<std::string> OnNewDns(const std::string& _host);
    //网络层收到push消息回调
    virtual void OnPush(uint64_t _channel_id, uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend);
    //底层获取task要发送的数据
    virtual bool Req2Buf(uint32_t _taskid, void* const _user_context, AutoBuffer& _outbuffer, AutoBuffer& _extend, int& _error_code, const int _channel_select);
    //底层回包返回给上层解析
    //virtual int Buf2Resp(uint32_t _taskid, void* const _user_context, const AutoBuffer& _inbuffer, const AutoBuffer& _extend, int& _error_code, const int _channel_select);
	virtual int Buf2Resp(uint32_t _taskid, void* const _user_context, const AutoBuffer& _inbuffer, const AutoBuffer& _extend, int& _error_code, const int _channel_select);
	//任务执行结束
    virtual int  OnTaskEnd(uint32_t _taskid, void* const _user_context, int _error_type, int _error_code);

    //上报网络连接状态
    //virtual void ReportConnectStatus(int _status, int longlink_status);
	virtual void ReportConnectStatus(int _status, int longlink_status);
    //长连信令校验 ECHECK_NOW, ECHECK_NEVER = 1, ECHECK_NEXT = 2
    virtual int  GetLonglinkIdentifyCheckBuffer(AutoBuffer& _identify_buffer, AutoBuffer& _buffer_hash, int32_t& _cmdid);
    //长连信令校验回包
    virtual bool OnLonglinkIdentifyResponse(const AutoBuffer& _response_buffer, const AutoBuffer& _identify_buffer_hash);
    //
    virtual void RequestSync();

    void onDBOpened();
    
    void setDNSResult(std::vector<std::string> serverIPs);

    int64_t deltaTime;
    int64_t nwDelay;
    bool mAuthed;
    friend class TGetGroupInfoCallback;
    friend class RefreshUserInfoCallback;
    friend class TLoadFriendCallback;
    friend class TLoadFriendRequestCallback;
    friend class GetUserSettingPublishCallback;
    friend class PullingMessagePublishCallback;
    friend class TGetChannelInfoCallback;
    friend class GetGroupMembersPublishCallback;
    friend class LoadRemoteMessagesPublishCallback;
private:
    static StnCallBack* instance_;
    void PullMessage(int64_t head, int type, bool retry, bool refreshSetting);
    void PullSetting(int64_t head);
    int pullRetryCount;
    bool isPullingMsg;
    bool isPullingChatroomMsg;
    bool isPullingSetting;
    void onPullMsgSuccess(std::list<TMessage> messageList, int64_t current, int64_t head, int pullType, bool refreshSetting);
    void onPullMsgFailure(int errorCode, int pullType);
    int64_t currentHead;
    int64_t currentChatroomHead;
    int64_t settingHead;
    std::vector<std::string> m_serverIPs;
    int64_t mIdentifyTime;
    
};
    }
}

#endif /* STNCALLBACK_h */
