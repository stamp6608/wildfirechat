package com.tencent.mars.proto;

import com.tencent.mars.Mars;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import cn.wildfirechat.model.ProtoChannelInfo;
import cn.wildfirechat.model.ProtoChatRoomInfo;
import cn.wildfirechat.model.ProtoChatRoomMembersInfo;
import cn.wildfirechat.model.ProtoConversationInfo;
import cn.wildfirechat.model.ProtoConversationSearchresult;
import cn.wildfirechat.model.ProtoFriendRequest;
import cn.wildfirechat.model.ProtoGroupInfo;
import cn.wildfirechat.model.ProtoGroupMember;
import cn.wildfirechat.model.ProtoGroupSearchResult;
import cn.wildfirechat.model.ProtoMessage;
import cn.wildfirechat.model.ProtoMessageContent;
import cn.wildfirechat.model.ProtoUnreadCount;
import cn.wildfirechat.model.ProtoUserInfo;

/**
 * Created by taoli on 2017/11/25.
 */

public class ProtoLogic {
    public interface IConnectionStatusCallback {
        void onConnectionStatusChanged(int status);
    }

    public interface IReceiveMessageCallback {
        void onReceiveMessage(List<ProtoMessage> messages, boolean hasMore);
        void onRecallMessage(long messageUid);
    }

    public interface ILoadRemoteMessagesCallback {
        void onSuccess(ProtoMessage[] messages);
        void onFailure(int errorCode);
    }

    public interface IGroupInfoUpdateCallback {
        void onGroupInfoUpdated(List<ProtoGroupInfo> updatedGroupInfos);
    }

    public interface IGroupMembersUpdateCallback {
        void onGroupMembersUpdated(String updatedGroupId, List<ProtoGroupMember> members);
    }

    public interface IChannelInfoUpdateCallback {
        void onChannelInfoUpdated(List<ProtoChannelInfo> updatedChannelInfos);
    }

    public interface IUserInfoUpdateCallback {
        void onUserInfoUpdated(List<ProtoUserInfo> updatedUserInfos);
    }

    public interface IFriendListUpdateCallback {
        void onFriendListUpdated(String[] updatedFriendList);
    }

    public interface IFriendRequestListUpdateCallback {
        void onFriendRequestUpdated();
    }

    public interface ISettingUpdateCallback {
        void onSettingUpdated();
    }

    public interface IGeneralCallback {
        void onSuccess();
        void onFailure(int errorCode);
    }

    public interface IGeneralCallback2 {
        void onSuccess(String retId);
        void onFailure(int errorCode);
    }

    public interface IGeneralCallback3 {
        void onSuccess(String[] retId);
        void onFailure(int errorCode);
    }

    public interface ISendMessageCallback {
        void onSuccess(long messageUid, long timestamp);
        void onFailure(int errorCode);
        void onPrepared(long messageId, long savedTime);
        void onProgress(long uploaded, long total);
        void onMediaUploaded(String remoteUrl);
    }

    public interface ISearchUserCallback {
        void onSuccess(ProtoUserInfo[] userInfos);
        void onFailure(int errorCode);
    }

    public interface IUploadMediaCallback {
        void onSuccess(String remoteUrl);
        void onProgress(long uploaded, long total);
        void onFailure(int errorCode);
    }

    public interface IGetChatRoomInfoCallback {
        void onSuccess(ProtoChatRoomInfo chatRoomInfo);
        void onFailure(int errorCode);
    }


    public interface IGetChatRoomMembersInfoCallback {
        void onSuccess(ProtoChatRoomMembersInfo chatRoomMembersInfo);
        void onFailure(int errorCode);
    }

    public interface ICreateChannelCallback {
        void onSuccess(ProtoChannelInfo channelInfo);
        void onFailure(int errorCode);
    }

    public interface ISearchChannelCallback {
        void onSuccess(ProtoChannelInfo[] channelInfos);
        void onFailure(int errorCode);
    }

    private static IConnectionStatusCallback connectionStatusCallback = null;
    private static IReceiveMessageCallback receiveMessageCallback = null;
    private static IGroupInfoUpdateCallback groupInfoUpdateCallback = null;
    private static IGroupMembersUpdateCallback groupMembersUpdateCallback = null;

    private static IUserInfoUpdateCallback userInfoUpdateCallback = null;
    private static IFriendListUpdateCallback friendListUpdateCallback = null;
    private static IFriendRequestListUpdateCallback friendRequestListUpdateCallback = null;
    private static ISettingUpdateCallback settingUpdateCallback = null;
    private static IChannelInfoUpdateCallback channelInfoUpdateCallback = null;

    public static void setConnectionStatusCallback(IConnectionStatusCallback connectionStatusCallback) {
        ProtoLogic.connectionStatusCallback = connectionStatusCallback;
    }

    public static void setReceiveMessageCallback(IReceiveMessageCallback receiveMessageCallback) {
        ProtoLogic.receiveMessageCallback = receiveMessageCallback;
    }

    public static void setGroupInfoUpdateCallback(IGroupInfoUpdateCallback groupInfoUpdateCallback) {
        ProtoLogic.groupInfoUpdateCallback = groupInfoUpdateCallback;
    }

    public static void setGroupMembersUpdateCallback(IGroupMembersUpdateCallback groupMembersUpdateCallback) {
        ProtoLogic.groupMembersUpdateCallback = groupMembersUpdateCallback;
    }

    public static void setChannelInfoUpdateCallback(IChannelInfoUpdateCallback channelInfoUpdateCallback) {
        ProtoLogic.channelInfoUpdateCallback = channelInfoUpdateCallback;
    }

    public static void setUserInfoUpdateCallback(IUserInfoUpdateCallback userInfoUpdateCallback) {
        ProtoLogic.userInfoUpdateCallback = userInfoUpdateCallback;
    }

    public static void setFriendListUpdateCallback(IFriendListUpdateCallback friendListUpdateCallback) {
        ProtoLogic.friendListUpdateCallback = friendListUpdateCallback;
    }

    public static void setFriendRequestListUpdateCallback(IFriendRequestListUpdateCallback friendRequestListUpdateCallback) {
        ProtoLogic.friendRequestListUpdateCallback = friendRequestListUpdateCallback;
    }

    public static void setSettingUpdateCallback(ISettingUpdateCallback settingUpdateCallback) {
        ProtoLogic.settingUpdateCallback = settingUpdateCallback;
    }

    public static final String TAG = "mars.StnLogic";

    static {
        Mars.loadDefaultMarsLibrary();
    }

    public static void onConnectionStatusChanged(int status) {
        if(connectionStatusCallback != null) {
            connectionStatusCallback.onConnectionStatusChanged(status);
        }
    }

    public static void onReceiveMessage(ProtoMessage[] messages, boolean hasMore) {
        if (receiveMessageCallback != null) {
            List<ProtoMessage> list = new ArrayList<>();
            for (ProtoMessage protoMsg: messages
                 ) {
                list.add(protoMsg);
            }

            receiveMessageCallback.onReceiveMessage(list, hasMore);
        }
    }

    public static void onRecallMessage(long messageUid) {
        if (receiveMessageCallback != null) {
            receiveMessageCallback.onRecallMessage(messageUid);
        }
    }

    public static void onGroupInfoUpdated(ProtoGroupInfo[] groupInfos) {
        if (groupInfoUpdateCallback != null) {
            List<ProtoGroupInfo> list = new ArrayList<>();
            for (ProtoGroupInfo protoUserInfo : groupInfos
                    ) {
                list.add(protoUserInfo);
            }
            groupInfoUpdateCallback.onGroupInfoUpdated(list);
        }
    }

    public static void onGroupMembersUpdated(String groupId, ProtoGroupMember[] members) {
        if (groupMembersUpdateCallback != null) {
            List<ProtoGroupMember> list = new ArrayList<>();
            for (ProtoGroupMember member : members) {
                list.add(member);
            }
            groupMembersUpdateCallback.onGroupMembersUpdated(groupId, list);
        }
    }

    public static void onChannelInfoUpdated(ProtoChannelInfo[] channelInfos) {
        if (channelInfoUpdateCallback != null) {
            List<ProtoChannelInfo> list = new ArrayList<>();
            for (ProtoChannelInfo protoChannelInfo : channelInfos
                    ) {
                list.add(protoChannelInfo);
            }
            channelInfoUpdateCallback.onChannelInfoUpdated(list);
        }
    }

    public static void onUserInfoUpdated(ProtoUserInfo[] userInfos) {
        if (userInfoUpdateCallback != null) {
            List<ProtoUserInfo> list = new ArrayList<>();
            for (ProtoUserInfo protoUserInfo : userInfos
                 ) {
                list.add(protoUserInfo);
            }
            userInfoUpdateCallback.onUserInfoUpdated(list);
        }
    }

    public static void onFriendListUpdated(String[] friendList) {
        if (friendListUpdateCallback != null) {
            friendListUpdateCallback.onFriendListUpdated(friendList);
        }
    }

    public static void onFriendRequestUpdated() {
        if (friendRequestListUpdateCallback != null) {
            friendRequestListUpdateCallback.onFriendRequestUpdated();
        }
    }

    public static void onSettingUpdated() {
        if (settingUpdateCallback != null) {
            settingUpdateCallback.onSettingUpdated();
        }
    }

    public static native void connect(String host, int shortPort);

    public static native void setAuthInfo(String userId, String token);

    public static native void disconnect(int flag);

    public static native int getConnectionStatus();

    public static native void sendMessage(ProtoMessage msg, int expireDuration, ISendMessageCallback callback);

    public static native void recallMessage(long messageUid, IGeneralCallback callback);

    public static native long insertMessage(ProtoMessage msg);

    public static native void updateMessageContent(ProtoMessage msg);

    public static native long getServerDeltaTime();

    public static native ProtoConversationInfo[] getConversations(int[] conversationTypes, int[] lines);

    public static native ProtoConversationInfo getConversation(int conversationType, String target, int line);

    public static native ProtoMessage[] getMessages(int conversationType, String target, int line, long fromIndex, boolean before, int count, String withUser);

//    - (void)getRemoteMessages:(WFCCConversation *)conversation
//    before:(long long)beforeMessageUid
//    count:(NSUInteger)count
//    success:(void(^)(NSArray<WFCCMessage *> *messages))successBlock
//    error:(void(^)(int error_code))errorBlock
    public static native void getRemoteMessages(int conversationType, String target, int line, long beforeMessageUid, int count, ILoadRemoteMessagesCallback callback);

    public static native ProtoMessage getMessage(long messageId);

    public static native ProtoMessage getMessageByUid(long messageUid);

    public static native ProtoUnreadCount getUnreadCount(int conversationType, String target, int line);

    public static native ProtoUnreadCount getUnreadCountEx(int[] conversationTypes, int[] lines);

    public static native void clearUnreadStatus(int conversationType, String target, int line);

    public static native void clearAllUnreadStatus();

    public static native void clearMessages(int conversationType, String target, int line);

    public static native void setMediaMessagePlayed(long messageId);

    public static native void removeConversation(int conversationType, String target, int line, boolean clearMsg);

    public static native void setConversationTop(int conversationType, String target, int line, boolean top);

    public static native void setConversationDraft(int conversationType, String target, int line, String draft);

    public static native void setConversationSilent(int conversationType, String target, int line, boolean silent);

    //- (void)searchUser:(NSString *)keyword success:(void(^)(NSArray<WFCCUserInfo *> *machedUsers))successBlock error:(void(^)(int errorCode))errorBlock {
    public static native void searchUser(String keyword, ISearchUserCallback callback);

    //- (BOOL)isMyFriend:(NSString *)userId
    public static native boolean isMyFriend(String userId);

    //- (NSArray<NSString *> *)getMyFriendList:(BOOL)refresh
    public static native String[] getMyFriendList(boolean refresh);

    public static native String getFriendAlias(String userId);

    public static native void setFriendAlias(String userId, String alias, IGeneralCallback callback);

    //- (void)loadFriendRequestFromRemote
    public static native void loadFriendRequestFromRemote();

    //- (NSArray<WFCCFriendRequest *> *)getIncommingFriendRequest
    public static native ProtoFriendRequest[] getFriendRequest(boolean incomming);

    //- (void)clearUnreadFriendRequestStatus
    public static native void clearUnreadFriendRequestStatus();

    //- (int)getUnreadFriendRequestStatus
    public static native int getUnreadFriendRequestStatus();

    //- (void)removeFriend:(NSString *)userId
    //success:(void(^)())successBlock
    //error:(void(^)(int error_code))errorBlock
    public static native void removeFriend(String userId, IGeneralCallback callback);

    //- (void)sendFriendRequest:(NSString *)userId
    //                   reason:(NSString *)reason
    //                  success:(void(^)())successBlock
    //                    error:(void(^)(int error_code))errorBlock {
    public static native void sendFriendRequest(String userId, String reason, IGeneralCallback callback);


    //- (void)handleFriendRequest:(NSString *)userId
    //                     accept:(BOOL)accpet
    //                    success:(void(^)())successBlock
    //                      error:(void(^)(int error_code))errorBlock {
    public static native void handleFriendRequest(String userId, boolean accept, IGeneralCallback callback);

    //- (void)deleteFriend:(NSString *)userId
    //             success:(void(^)())successBlock
    //               error:(void(^)(int error_code))errorBlock {
    public static native void deleteFriend(String userId, IGeneralCallback callback);


//    - (BOOL)isBlackListed:(NSString *)userId;
    public static native boolean isBlackListed(String userId);

//    - (NSArray<NSString *> *)getBlackList:(BOOL)refresh;
    public static native String[] getBlackList(boolean refresh);

//- (void)setBlackList:(NSString *)userId
//    isBlackListed:(BOOL)isBlackListed
//    success:(void(^)(void))successBlock
//    error:(void(^)(int error_code))errorBlock;
    public static native void setBlackList(String userId, boolean isBlackListed, IGeneralCallback callback);

    //- (WFCCUserInfo *)getUserInfo:(NSString *)userId refresh:(BOOL)refresh
    public static native ProtoUserInfo getUserInfo(String userId, String groupId, boolean refresh);
    public static native ProtoUserInfo[] getUserInfos(String[] userIds, String groupId);

    //- (void)uploadMedia:(NSData *)mediaData mediaType:(WFCCMediaType)mediaType success:(void(^)(NSString *remoteUrl))successBlock error:(void(^)(int error_code))errorBlock
    public static native void uploadMedia(byte[] data, int mediaType, IUploadMediaCallback callback);

    //-(void)modifyMyInfo:(NSDictionary<NSNumber */*ModifyMyInfoType*/, NSString *> *)values
        //success:(void(^)())successBlock
        //error:(void(^)(int error_code))errorBlock
    public static native void modifyMyInfo(Map<Integer, String> values, IGeneralCallback callback);

    //- (BOOL)deleteMessage:(long)messageId
    public static native boolean deleteMessage(long messageId);

    //- (NSArray<WFCCConversationSearchInfo *> *)searchConversation:(NSString *)keyword
    public static native ProtoConversationSearchresult[] searchConversation(String keyword, int[] conversationTypes, int[] lines);

    //- (NSArray<WFCCMessage *> *)searchMessage:(WFCCConversation *)conversation keyword:(NSString *)keyword
    public static native ProtoMessage[] searchMessage(int conversationType, String target, int line, String keyword);

    //- (NSArray<WFCCUserInfo *> *)searchFriends:(NSString *)keyword;
    public static native ProtoUserInfo[] searchFriends(String keyword);

    //- (NSArray<WFCCGroupSearchInfo *> *)searchGroups:(NSString *)keyword;
    public static native ProtoGroupSearchResult[] searchGroups(String keyword);


    //- (void)createGroup:(NSString *)groupId
//               name:(NSString *)groupName
//           portrait:(NSString *)groupPortrait
//            members:(NSArray *)groupMembers
//        notifyLines:(NSArray<NSNumber *> *)notifyLines
//      notifyContent:(WFCCMessageContent *)notifyContent
//            success:(void(^)(NSString *groupId))successBlock
//              error:(void(^)(int error_code))errorBlock;

    public static native void createGroup(String groupId, String groupName, String groupPortrait, String[] memberIds, int[] notifyLines, ProtoMessageContent notifyMsg, IGeneralCallback2 callback);
//- (void)addMembers:(NSArray *)members
//           toGroup:(NSString *)groupId
//       notifyLines:(NSArray<NSNumber *> *)notifyLines
//     notifyContent:(WFCCMessageContent *)notifyContent
//           success:(void(^)())successBlock
//             error:(void(^)(int error_code))errorBlock;
    public static native void addMembers(String groupId, String[] memberIds, int[] notifyLines, ProtoMessageContent notifyMsg, IGeneralCallback callback);
//- (void)kickoffMembers:(NSArray *)members
//             fromGroup:(NSString *)groupId
//           notifyLines:(NSArray<NSNumber *> *)notifyLines
//         notifyContent:(WFCCMessageContent *)notifyContent
//               success:(void(^)())successBlock
//                 error:(void(^)(int error_code))errorBlock;
    public static native void kickoffMembers(String groupId, String[] memberIds, int[] notifyLines, ProtoMessageContent notifyMsg, IGeneralCallback callback);
//- (void)quitGroup:(NSString *)groupId
//      notifyLines:(NSArray<NSNumber *> *)notifyLines
//    notifyContent:(WFCCMessageContent *)notifyContent
//          success:(void(^)())successBlock
//            error:(void(^)(int error_code))errorBlock;
    public static native void quitGroup(String groupId, int[] notifyLines, ProtoMessageContent notifyMsg, IGeneralCallback callback);

//- (void)dismissGroup:(NSString *)groupId
//         notifyLines:(NSArray<NSNumber *> *)notifyLines
//       notifyContent:(WFCCMessageContent *)notifyContent
//             success:(void(^)())successBlock
//               error:(void(^)(int error_code))errorBlock;
    public static native void dismissGroup(String groupId, int[] notifyLines, ProtoMessageContent notifyMsg, IGeneralCallback callback);

//- (void)modifyGroupInfo:(NSString *)groupId
//    type:(ModifyGroupInfoType)type
//    newValue:(NSString *)newValue
//    notifyLines:(NSArray<NSNumber *> *)notifyLines
//    notifyContent:(WFCCMessageContent *)notifyContent
//    success:(void(^)(void))successBlock
//    error:(void(^)(int error_code))errorBlock
    public static native void modifyGroupInfo(String groupId, int modifyType, String newValue, int[] notifyLines, ProtoMessageContent notifyMsg, IGeneralCallback callback);

//- (void)modifyGroupAlias:(NSString *)groupId
//    alias:(NSString *)newAlias
//    notifyLines:(NSArray<NSNumber *> *)notifyLines
//    notifyContent:(WFCCMessageContent *)notifyContent
//    success:(void(^)())successBlock
//    error:(void(^)(int error_code))errorBlock;
    public static native void modifyGroupAlias(String groupId, String newAlias, int[] notifyLines, ProtoMessageContent notifyMsg, IGeneralCallback callback);

//- (NSArray<WFCCGroupMember *> *)getGroupMembers:(NSString *)groupId
//                             forceUpdate:(BOOL)forceUpdate;
    public static native ProtoGroupMember[] getGroupMembers(String groupId, boolean forceUpdate);

//- (WFCCGroupMember *)getGroupMember:(NSString *)groupId
//    memberId:(NSString *)memberId;
    public static native ProtoGroupMember getGroupMember(String groupId, String memberId);

//- (void)transferGroup:(NSString *)groupId
//                   to:(NSString *)newOwner
//          notifyLines:(NSArray<NSNumber *> *)notifyLines
//        notifyContent:(WFCCMessageContent *)notifyContent
//              success:(void(^)())successBlock
//                error:(void(^)(int error_code))errorBlock;
    public static native void transferGroup(String groupId, String newOwner, int[] notifyLines, ProtoMessageContent notifyMsg, IGeneralCallback callback);

//    - (void)createChannel:(NSString *)channelName
//    portrait:(NSString *)channelPortrait
//    status:(int)status
//    desc:(NSString *)desc
//    extra:(NSString *)extra
//    success:(void(^)(WFCCChannelInfo *channelInfo))successBlock
//    error:(void(^)(int error_code))errorBlock;
    public static native void createChannel(String channelId, String channelName, String channelPortrait, int status, String desc, String extra, ICreateChannelCallback callback);

//- (WFCCChannelInfo *)getChannelInfo:(NSString *)channelId
//    refresh:(BOOL)refresh;
    public static native ProtoChannelInfo getChannelInfo(String channelId, boolean refresh);
//
//- (void)searchChannel:(NSString *)keyword success:(void(^)(NSArray<WFCCChannelInfo *> *machedChannels))successBlock error:(void(^)(int errorCode))errorBlock;
    public static native void searchChannel(String keyword, ISearchChannelCallback callback);

    public static native void modifyChannelInfo(String channelId, int modifyType, String newValue, IGeneralCallback callback);

//- (BOOL)isListened:(NSString *)channelId;
    public static native boolean isListenedChannel(String channelId);
//
//- (void)listenChannel:(NSString *)channelId listen:(BOOL)listen success:(void(^)(void))successBlock error:(void(^)(int errorCode))errorBlock;
    public static native void listenChannel(String channelId, boolean listen, IGeneralCallback callback);
//
//- (NSArray<NSString *> *)getMyChannels;
    public static native String[] getMyChannels();
//
//- (NSArray<NSString *> *)getListenedChannels;
    public static native String[] getListenedChannels();

//- (void)destoryChannel:(NSString *)channelId success:(void(^)(void))successBlock error:(void(^)(int error_code))errorBlock
    public static native void destoryChannel(String channelId, IGeneralCallback callback);

    public static native String getUserSetting(int scope, String key);
    public static native Map<String, String> getUserSettings(int scope);
    public static native void setUserSetting(int scope, String key, String value, IGeneralCallback callback);
    public static native void setDeviceToken(String appName, String token, int pushType);
    public static native void setDNSResult(String[] serverIPs);
    public static native ProtoGroupInfo getGroupInfo(String groupId, boolean refresh);

    public static native void getChatRoomInfo(String chatRoomId, long lastUpdateDt, IGetChatRoomInfoCallback callback);
    public static native void getChatRoomMembersInfo(String chatRoomId, int maxCount, IGetChatRoomMembersInfoCallback callback);
    public static native void joinChatRoom(String chatRoomId, IGeneralCallback callback);
    public static native void quitChatRoom(String chatRoomId, IGeneralCallback callback);

    public static native void registerMessageFlag(int contentType, int flag);
        /**
         * 获取底层已加载模块
         * @return
         */
    private static native ArrayList<String> getLoadLibraries();
}
