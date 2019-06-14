    //
//  MessageDB.cc
//  stn
//
//  Created by WF Chat on 2017/8/26.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "mars/proto/MessageDB.h"
#include "mars/proto/src/DB2.h"
#include "mars/proto/stn_callback.h"
#include "mars/proto/src/Proto/conversation.h"
#include "mars/comm/thread/atomic_oper.h"
#include "mars/app/app.h"
#include <map>
#include <time.h>
#include <sstream>
#include <iostream>

namespace mars {
    namespace stn {
        std::map<int, int> *gTypeMap;
        MessageDB* MessageDB::instance_ = NULL;
        static uint32_t gs_chatroomid = 100;
        
        MessageDB* MessageDB::Instance() {
            if(instance_ == NULL) {
                instance_ = new MessageDB();
            }
            
            return instance_;
        }
        
        MessageDB::MessageDB() {
            gTypeMap = new std::map<int, int>();
        }
        
        MessageDB::~MessageDB() {
            delete gTypeMap;
            gTypeMap = NULL;
        }
    
        void MessageDB::RegisterMessageFlag(int type, int flag) {
            (*gTypeMap)[type] = flag;
        }
        
        int MessageDB::getMessageFlag(int type) {
            std::map<int, int>::iterator it = gTypeMap->find(type);
            if (it == gTypeMap->end()) {
                return 0;
            }
            return it->second;
        }
        
        long MessageDB::InsertMessage(TMessage &msg) {
            int flag = getMessageFlag(msg.content.type);
            if ((flag & 0x1) == 0) {
                return 0;
            }
            
            if (msg.conversationType == ConversationType_ChatRoom) {
                return -1 * (long)atomic_inc32(&gs_chatroomid);
            }

            MessageStatus status = msg.status;
            if (msg.direction == 1 && (status == Message_Status_Unread || status == Message_Status_Mentioned || status == Message_Status_AllMentioned)) {
                if ((flag & 0x2) == 0) {
                    status = Message_Status_Readed;
                }
            }
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
              return -1;
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_from");
            columns.push_back("_to");
            columns.push_back("_cont_type");
            columns.push_back("_cont_searchable");
            columns.push_back("_cont_push");
            columns.push_back("_cont");
            columns.push_back("_cont_data");
            columns.push_back("_cont_local");
            columns.push_back("_cont_media_type");
            columns.push_back("_cont_remote_media_url");
            columns.push_back("_cont_local_media_path");
            columns.push_back("_direction");
            columns.push_back("_status");
            columns.push_back("_uid");
            columns.push_back("_timestamp");

            std::string sql = db->GetInsertSql(MESSAGE_TABLE_NAME, columns);
#else
            std::string sql = db->GetInsertSql(MESSAGE_TABLE_NAME, {"_conv_type","_conv_target","_conv_line","_from","_to","_cont_type","_cont_searchable","_cont_push","_cont","_cont_data","_cont_local","_cont_media_type","_cont_remote_media_url","_cont_local_media_path","_direction","_status","_uid","_timestamp"});
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return -1;
            }
            
            int index = 1;
            db->Bind(statementHandle, msg.conversationType, index++);
            db->Bind(statementHandle, msg.target, index++);
            db->Bind(statementHandle, msg.line, index++);
            db->Bind(statementHandle, msg.from, index++);
            std::string toStr;
            for (std::list<std::string>::iterator it = msg.to.begin(); it != msg.to.end(); ++it) {
                if (!toStr.empty()) {
                    toStr.append(",");
                }
                toStr.append(*it);
            }
            db->Bind(statementHandle, toStr, index++);
            
            db->Bind(statementHandle, msg.content.type, index++);
            db->Bind(statementHandle, msg.content.searchableContent, index++);
            db->Bind(statementHandle, msg.content.pushContent, index++);
            db->Bind(statementHandle, msg.content.content, index++);
            db->Bind(statementHandle, (const void *)msg.content.binaryContent.c_str(), (int)msg.content.binaryContent.length(), index++);
            db->Bind(statementHandle, msg.content.localContent, index++);
            db->Bind(statementHandle, msg.content.mediaType, index++);
            db->Bind(statementHandle, msg.content.remoteMediaUrl, index++);
            db->Bind(statementHandle, msg.content.localMediaPath, index++);
            
            db->Bind(statementHandle, msg.direction, index++);
            db->Bind(statementHandle, status, index++);
            
            db->Bind(statementHandle, msg.messageUid, index++);
            db->Bind(statementHandle, msg.timestamp, index++);
            
            db->ExecuteInsert(statementHandle, &(msg.messageId));
            return msg.messageId;
        }
        
        bool MessageDB::UpdateMessageContent(long messageId, TMessageContent &content) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
  
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_cont_type");
            columns.push_back("_cont_searchable");
            columns.push_back("_cont_push");
            columns.push_back("_cont");
            columns.push_back("_cont_data");
            columns.push_back("_cont_local");
            columns.push_back("_cont_media_type");
            columns.push_back("_cont_remote_media_url");
            columns.push_back("_cont_local_media_path");
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, columns, "_id=?");
#else
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, {"_cont_type","_cont_searchable","_cont_push","_cont","_cont_data","_cont_local","_cont_media_type","_cont_remote_media_url","_cont_local_media_path"}, "_id=?");
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(statementHandle, content.type, 1);
            db->Bind(statementHandle, content.searchableContent, 2);
            db->Bind(statementHandle, content.pushContent, 3);
            db->Bind(statementHandle, content.content, 4);
            db->Bind(statementHandle, (const void *)content.binaryContent.c_str(), (int)content.binaryContent.length(), 5);
            db->Bind(statementHandle, content.localContent, 6);
            db->Bind(statementHandle, content.mediaType, 7);
            db->Bind(statementHandle, content.remoteMediaUrl, 8);
            db->Bind(statementHandle, content.localMediaPath, 9);
            db->Bind(statementHandle, (int)messageId, 10);
            
            int count = db->ExecuteUpdate(statementHandle);
            
            if (count > 0) {
                return true;
            }
            
            return false;

        }
        bool MessageDB::DeleteMessage(long messageId) {
            if (messageId < 0) {
                return false;
            }
            
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }

            std::string sql = db->GetDeleteSql(MESSAGE_TABLE_NAME, "_id=?");
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            
            statementHandle.bind(1, (int)messageId);
            
            if (db->ExecuteDelete(statementHandle) > 0) {
                return true;
            }
            
            return false;
        }
        
        bool MessageDB::UpdateMessageContentByUid(int64_t messageUid, TMessageContent &content) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_cont_type");
            columns.push_back("_cont_searchable");
            columns.push_back("_cont_push");
            columns.push_back("_cont");
            columns.push_back("_cont_data");
            columns.push_back("_cont_local");
            columns.push_back("_cont_media_type");
            columns.push_back("_cont_remote_media_url");
            columns.push_back("_cont_local_media_path");
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, columns, "_uid=?");
#else
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, {"_cont_type","_cont_searchable","_cont_push","_cont","_cont_data","_cont_local","_cont_media_type","_cont_remote_media_url","_cont_local_media_path"}, "_uid=?");
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return -1;
            }
            
            db->Bind(statementHandle, content.type, 1);
            db->Bind(statementHandle, content.searchableContent, 2);
            db->Bind(statementHandle, content.pushContent, 3);
            db->Bind(statementHandle, content.content, 4);
            db->Bind(statementHandle, (const void *)content.binaryContent.c_str(), (int)content.binaryContent.length(), 5);
            db->Bind(statementHandle, content.localContent, 6);
            db->Bind(statementHandle, content.mediaType, 7);
            db->Bind(statementHandle, content.remoteMediaUrl, 8);
            db->Bind(statementHandle, content.localMediaPath, 9);
            db->Bind(statementHandle, messageUid, 10);
            
            int count = db->ExecuteUpdate(statementHandle);
            
            if (count > 0) {
                return true;
            }
            
            return false;
        }
        bool MessageDB::DeleteMessageByUid(int64_t messageUid) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }

            std::string sql = db->GetDeleteSql(MESSAGE_TABLE_NAME, "_uid=?");
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            
            statementHandle.bind(1, messageUid);
            if (db->ExecuteDelete(statementHandle) > 0) {
                return true;
            }
            
            return false;
        }
        
        bool MessageDB::UpdateMessageTimeline(int64_t timeline) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
              return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_head");
            std::string sql = db->GetUpdateSql(TIMELINE_TABLE_NAME, columns);
#else
            std::string sql = db->GetUpdateSql(TIMELINE_TABLE_NAME, {"_head"});
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(statementHandle, timeline, 1);
            return db->ExecuteUpdate(statementHandle) > 0;
        }
        
        int64_t MessageDB::GetMessageTimeline() {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
              return -1;
            }

#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_head");
            std::string sql = db->GetSelectSql(TIMELINE_TABLE_NAME, columns);
#else
            std::string sql = db->GetSelectSql(TIMELINE_TABLE_NAME, {"_head"});
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return 0;
            }
            
            if (statementHandle.executeSelect()) {
                int64_t head = db->getBigIntValue(statementHandle, 0);
                return head;
            }
            
            return 0;
        }

        int64_t MessageDB::GetSettingVersion() {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return -1;
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("max(_update_dt)");
            std::string sql = db->GetSelectSql(USER_SETTING_TABLE_NAME, columns);
#else
            std::string sql = db->GetSelectSql(USER_SETTING_TABLE_NAME, {"max(_update_dt)"});
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return 0;
            }
            
            if (statementHandle.executeSelect()) {
                int64_t head = db->getBigIntValue(statementHandle, 0);
                return head;
            }
            
            return 0;
        }
#ifdef __ANDROID__
        extern  int myatoi(std::string s);
        extern  long long myatoll(std::string s);
#endif
        bool MessageDB::UpdateUserSettings(const std::list<TUserSettingEntry> &retList) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
            
            for (std::list<TUserSettingEntry>::const_iterator it = retList.begin(); it != retList.end(); it++) {
                const TUserSettingEntry &entry = *it;
                if(entry.scope == kUserSettingConversationSilent || entry.scope == kUserSettingConversationTop || entry.scope == kUserSettingConversationSync) {
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
                    int64_t dt = 0;
                    if(entry.scope == kUserSettingConversationSync) {
                        if (!entry.value.empty()) {
                            dt = myatoll(entry.value);
                        }
                    } else {
                        if (!entry.value.empty()) {
                            issilent = myatoi(entry.value);
                        }
                    }
                    
#else
                  int conversationType = std::stoi(type);
                  int line = std::stoi(lineStr);
                    int issilent = false;
                    int64_t dt = 0;
                    if(entry.scope == kUserSettingConversationSync) {
                        if (!entry.value.empty()) {
                            dt = std::stoll(entry.value);
                        }
                    } else {
                        if (!entry.value.empty()) {
                            issilent = std::stoi(entry.value);
                        }
                    }
                    
#endif
                    if (entry.scope == kUserSettingConversationSilent) {
                        MessageDB::Instance()->updateConversationIsSilent(conversationType, target, line, issilent);
                    } else if(entry.scope == kUserSettingConversationTop){
                        MessageDB::Instance()->updateConversationIsTop(conversationType, target, line, issilent);
                    } else if(entry.scope == kUserSettingConversationSync) {
                        MessageDB::Instance()->updateConversationRead(conversationType, target, line, dt);
                    }
                    
                }
                
                if (it->value.empty()) {
                    std::string sql = db->GetDeleteSql(USER_SETTING_TABLE_NAME,"_scope=? and _key=?");
                    int error = 0;
                    RecyclableStatement statementHandle(db, sql, error);
                    
                    db->Bind(statementHandle, it->scope, 1);
                    db->Bind(statementHandle, it->key, 1);
                    
                    db->ExecuteDelete(statementHandle);
                } else {
#ifdef __ANDROID__
                    std::list<std::string> columns;
                    columns.push_back("_scope");
                    columns.push_back("_key");
                    columns.push_back("_value");
                    columns.push_back("_update_dt");
                    std::string sql = db->GetInsertSql(USER_SETTING_TABLE_NAME, columns, true);
#else
                    std::string sql = db->GetInsertSql(USER_SETTING_TABLE_NAME, {"_scope",  "_key", "_value", "_update_dt"}, true);
#endif
                    int error = 0;
                    RecyclableStatement statementHandle(db, sql, error);
                    if (error != 0) {
                        return false;
                    }
                    
                    db->Bind(statementHandle, it->scope, 1);
                    db->Bind(statementHandle, it->key, 2);
                    db->Bind(statementHandle, it->value, 3);
                    db->Bind(statementHandle, it->updateDt, 4);
                    
                    long ret = db->ExecuteInsert(statementHandle, &ret);
                }
            }

            return true;
        }
        std::string MessageDB::GetUserSetting(int scope, const std::string &key) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return "";
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_value");
            std::string sql = db->GetSelectSql(USER_SETTING_TABLE_NAME, columns, "_scope=? and _key=?");
#else
            std::string sql = db->GetSelectSql(USER_SETTING_TABLE_NAME, {"_value"}, "_scope=? and _key=?");
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return "";
            }
            
            db->Bind(statementHandle, scope, 1);
            db->Bind(statementHandle, key, 2);

            std::string out;
            if (statementHandle.executeSelect()) {
                out = db->getStringValue(statementHandle, 0);
            }
            
            return out;
        }
        
        std::map<std::string, std::string> MessageDB::GetUserSettings(int scope) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return std::map<std::string, std::string>();
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_key");
            columns.push_back("_value");
            std::string sql = db->GetSelectSql(USER_SETTING_TABLE_NAME, columns, "_scope=?");
#else
            std::string sql = db->GetSelectSql(USER_SETTING_TABLE_NAME, {"_key, _value"}, "_scope=?");
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::map<std::string, std::string>();
            }
            
            db->Bind(statementHandle, scope, 1);
            
            std::map<std::string, std::string> out;
            while(statementHandle.executeSelect()) {
                std::string key = db->getStringValue(statementHandle, 0);
                std::string value = db->getStringValue(statementHandle, 1);
                out.insert(std::map<std::string, std::string>::value_type(key, value));
            }
            
            return out;
        }
        bool MessageDB::updateConversationTimestamp(int conversationType, const std::string &target, int line, int64_t timestamp) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
              return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_timestamp");
            std::string sql = db->GetUpdateSql(CONVERSATION_TABLE_NAME, columns, "_conv_type=? and _conv_target=? and _conv_line=?");
#else
            std::string sql = db->GetUpdateSql(CONVERSATION_TABLE_NAME, {"_timestamp"}, "_conv_type=? and _conv_target=? and _conv_line=?");
#endif
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(updateStatementHandle, timestamp, 1);
            db->Bind(updateStatementHandle, conversationType, 2);
            db->Bind(updateStatementHandle, target, 3);
            db->Bind(updateStatementHandle, line, 4);
            int count = db->ExecuteUpdate(updateStatementHandle);
            
            if (count > 0) {
                return true;
            }
            
            
#ifdef __ANDROID__
            columns.clear();
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_timestamp");
            sql = db->GetInsertSql(CONVERSATION_TABLE_NAME, columns, true);
#else
            sql = db->GetInsertSql(CONVERSATION_TABLE_NAME, {"_conv_type", "_conv_target", "_conv_line", "_timestamp"}, true);
#endif
            error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(statementHandle, conversationType, 1);
            db->Bind(statementHandle, target, 2);
            db->Bind(statementHandle, line, 3);
            db->Bind(statementHandle, timestamp, 4);
            return db->ExecuteInsert(statementHandle);
        }
        
        bool MessageDB::updateConversationIsTop(int conversationType, const std::string &target, int line, bool istop) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
              return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_istop");
            std::string sql = db->GetUpdateSql(CONVERSATION_TABLE_NAME, columns, "_conv_type=? and _conv_target=? and _conv_line=?");
#else
            std::string sql = db->GetUpdateSql(CONVERSATION_TABLE_NAME, {"_istop"}, "_conv_type=? and _conv_target=? and _conv_line=?");
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(statementHandle, istop, 1);
            db->Bind(statementHandle, conversationType, 2);
            db->Bind(statementHandle, target, 3);
            db->Bind(statementHandle, line, 4);
            int count = db->ExecuteUpdate(statementHandle);
            
            if (count > 0) {
                return true;
            }

#ifdef __ANDROID__
            columns.clear();
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_istop");
            std::string sql2 = db->GetInsertSql(CONVERSATION_TABLE_NAME, columns, true);
#else
            std::string sql2 = db->GetInsertSql(CONVERSATION_TABLE_NAME, {"_conv_type", "_conv_target", "_conv_line", "_istop"}, true);
#endif
            error = 0;
            RecyclableStatement statementHandle2(db, sql2, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(statementHandle2, conversationType, 1);
            db->Bind(statementHandle2, target, 2);
            db->Bind(statementHandle2, line, 3);
            db->Bind(statementHandle2, istop, 4);
            return db->ExecuteInsert(statementHandle2);
        }
        bool MessageDB::updateConversationIsSilent(int conversationType, const std::string &target, int line, bool issilent) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_issilent");
            std::string sql = db->GetUpdateSql(CONVERSATION_TABLE_NAME, columns, "_conv_type=? and _conv_target=? and _conv_line=?");
#else
            std::string sql = db->GetUpdateSql(CONVERSATION_TABLE_NAME, {"_issilent"}, "_conv_type=? and _conv_target=? and _conv_line=?");
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(statementHandle, issilent, 1);
            db->Bind(statementHandle, conversationType, 2);
            db->Bind(statementHandle, target, 3);
            db->Bind(statementHandle, line, 4);
            int count = db->ExecuteUpdate(statementHandle);
            
            if (count > 0) {
                return true;
            }
            
#ifdef __ANDROID__
            columns.clear();
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_issilent");
            std::string sql2 = db->GetInsertSql(CONVERSATION_TABLE_NAME, columns, true);
#else
            std::string sql2 = db->GetInsertSql(CONVERSATION_TABLE_NAME, {"_conv_type", "_conv_target", "_conv_line", "_issilent"}, true);
#endif
            error = 0;
            RecyclableStatement statementHandle2(db, sql2, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(statementHandle2, conversationType, 1);
            db->Bind(statementHandle2, target, 2);
            db->Bind(statementHandle2, line, 3);
            db->Bind(statementHandle2, issilent, 4);
            return db->ExecuteInsert(statementHandle2);
        }
        bool MessageDB::updateConversationDraft(int conversationType, const std::string &target, int line, const std::string &draft) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
              return false;
            }

#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_draft");
            columns.push_back("_timestamp");
            std::string sql = db->GetUpdateSql(CONVERSATION_TABLE_NAME, columns, "_conv_type=? and _conv_target=? and _conv_line=?");
#else
            std::string sql = db->GetUpdateSql(CONVERSATION_TABLE_NAME, {"_draft", "_timestamp"}, "_conv_type=? and _conv_target=? and _conv_line=?");
#endif
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(updateStatementHandle, draft, 1);
            db->Bind(updateStatementHandle, ((int64_t)time(NULL))*1000, 2);
            db->Bind(updateStatementHandle, conversationType, 3);
            db->Bind(updateStatementHandle, target, 4);
            db->Bind(updateStatementHandle, line, 5);
            int count = db->ExecuteUpdate(updateStatementHandle);
            
            if (count > 0) {
                return true;
            }
            
#ifdef __ANDROID__
            columns.clear();
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_draft");
            sql = db->GetInsertSql(CONVERSATION_TABLE_NAME, columns, true);
#else
            sql = db->GetInsertSql(CONVERSATION_TABLE_NAME, {"_conv_type", "_conv_target", "_conv_line", "_draft"}, true);
#endif
            error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(statementHandle, conversationType, 1);
            db->Bind(statementHandle, target, 2);
            db->Bind(statementHandle, line, 3);
            db->Bind(statementHandle, draft, 4);
            return db->ExecuteInsert(statementHandle);
        }
        
        bool MessageDB::GetConversationSilent(int conversationType, const std::string &target, int line) {
            char buff[64];
            memset(buff, 0, 64);
            sprintf(buff, "%d-", conversationType);
            std::string key = buff;
            memset(buff, 0, 64);
            sprintf(buff, "%d-", line);
            key += buff;
            key += target;
            std::string boolValue = GetUserSetting(kUserSettingConversationSilent, key);
            if (boolValue.length() == 1 && *(boolValue.c_str()) == '1') {
                return true;
            }
            return false;
        }
        
        std::list<TConversation> MessageDB::GetConversationList(const std::list<int> &conversationTypes, const std::list<int> &lines) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
              return std::list<TConversation>();
            }
            std::string where;
            if (conversationTypes.size() > 0) {
                where += "_conv_type in (";
                for (std::list<int>::const_iterator it = conversationTypes.begin(); it != conversationTypes.end(); it++) {
                    char str[255];
                    memset(str, 0, 255);
                    sprintf(str, "%d", *it);
                    where += str;
                    where += ",";
                }
                where = where.substr(0, where.size()-1);
                where += ") and ";
            }
            
        
            if (lines.size() > 0) {
                where += "_conv_line in (";
                for (std::list<int>::const_iterator it = lines.begin(); it != lines.end(); it++) {
                    char str[255];
                    memset(str, 0, 255);
                    sprintf(str, "%d", *it);
                    where += str;
                    where += ",";
                }
                where = where.substr(0, where.size()-1);
                where += ") and ";
            }
            
            where += " _timestamp > 0 ";
            std::string orderBy = "_istop desc, _timestamp desc";

#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_draft");
            columns.push_back("_istop");
            columns.push_back("_issilent");
            columns.push_back("_timestamp");
            std::string sql = db->GetSelectSql(CONVERSATION_TABLE_NAME, columns, where, orderBy, 500);
#else
            std::string sql = db->GetSelectSql(CONVERSATION_TABLE_NAME, {"_conv_type", "_conv_target", "_conv_line", "_draft",  "_istop", "_issilent", "_timestamp"}, where, orderBy, 500);
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<TConversation>();
            }
            
            std::list<TConversation> convs;
          while(statementHandle.executeSelect()) {
                TConversation conv;
                conv.conversationType = db->getIntValue(statementHandle, 0);
                conv.target = db->getStringValue(statementHandle, 1);
                conv.line = db->getIntValue(statementHandle, 2);
                conv.draft = db->getStringValue(statementHandle, 3);
                conv.isTop = db->getIntValue(statementHandle, 4);
                conv.isSilent = db->getIntValue(statementHandle, 5);
                conv.timestamp = db->getBigIntValue(statementHandle, 6);
                
              std::list<TMessage> lastMessages = GetMessages(conv.conversationType, conv.target, conv.line, std::list<int>(), true, 1, INT_MAX, "");
                if (lastMessages.size() > 0) {
                    conv.lastMessage = *lastMessages.begin();
                }
              
              conv.unreadCount = GetUnreadCount(conv.conversationType, conv.target, conv.line);
              
              conv.isSilent = GetConversationSilent(conv.conversationType, conv.target, conv.line);
              
                convs.push_back(conv);
            }
            return convs;
        }
        
        bool MessageDB::RemoveConversation(int conversationType, const std::string &target, int line, bool clearMessage) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
          
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_draft");
            columns.push_back("_istop");
            columns.push_back("_issilent");
            columns.push_back("_timestamp");
            std::string sql = db->GetUpdateSql(CONVERSATION_TABLE_NAME, columns, "_conv_type=? and _conv_target=? and _conv_line=?");
#else
            std::string sql = db->GetUpdateSql(CONVERSATION_TABLE_NAME, {"_draft", "_istop", "_issilent", "_timestamp"}, "_conv_type=? and _conv_target=? and _conv_line=?");
#endif
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(updateStatementHandle, "", 1);
            db->Bind(updateStatementHandle, 0, 2);
            db->Bind(updateStatementHandle, 0, 3);
            db->Bind(updateStatementHandle, 0, 4);
            db->Bind(updateStatementHandle, conversationType, 5);
            db->Bind(updateStatementHandle, target, 6);
            db->Bind(updateStatementHandle, line, 7);
            int count = db->ExecuteUpdate(updateStatementHandle);
            
            if (count > 0) {
                return true;
            }
            
//
//
//            std::string sql = db->GetDeleteSql(CONVERSATION_TABLE_NAME, "_conv_type=? and _conv_target=? and _conv_line=?");
//            db->Bind(statementHandle, conversationType, 1);
//            db->Bind(statementHandle, target, 2);
//            db->Bind(statementHandle, line, 3);
//
//            if (db->ExecuteDelete(statementHandle) > 0) {
//                if (clearMessage) {
//                    RecyclableStatement statementHandle2 = db->GetDeleteStatement(MESSAGE_TABLE_NAME, "_conv_type=? and _conv_target=? and _conv_line=?");
//                    db->Bind(statementHandle2, conversationType, 1);
//                    db->Bind(statementHandle2, target, 2);
//                    db->Bind(statementHandle2, line, 3);
//                    db->ExecuteDelete(statementHandle2);
//                }
//
//                return true;
//            }
//
            return false;
        }
        
        bool MessageDB::ClearMessages(int conversationType, const std::string &target, int line) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
            
            std::string sql = db->GetDeleteSql(MESSAGE_TABLE_NAME, "_conv_type=? and _conv_line=? and _conv_target=?");
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            
            db->Bind(statementHandle, conversationType, 1);
            db->Bind(statementHandle, line, 2);
            db->Bind(statementHandle, target, 3);
            db->ExecuteDelete(statementHandle);

            return true;
        }
        
        TConversation MessageDB::GetConversation(int conversationType, const std::string &target, int line) {
            DB2 *db = DB2::Instance();
            TConversation conv;
            if (!db->isOpened()) {
              return conv;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_draft");
            columns.push_back("_istop");
            columns.push_back("_issilent");
            columns.push_back("_timestamp");
            std::string sql = db->GetSelectSql(CONVERSATION_TABLE_NAME, columns, "_conv_type=? and _conv_target=? and _conv_line=?");
#else
            std::string sql = db->GetSelectSql(CONVERSATION_TABLE_NAME, {"_draft",  "_istop", "_issilent", "_timestamp"}, "_conv_type=? and _conv_target=? and _conv_line=?"/* and _timestamp > 0"*/);
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return conv;
            }
            
            db->Bind(statementHandle, conversationType, 1);
            db->Bind(statementHandle, target, 2);
            db->Bind(statementHandle, line, 3);
            
            conv.target = target;
            conv.conversationType = conversationType;
            
            if (statementHandle.executeSelect()) {
                conv.draft = db->getStringValue(statementHandle, 0);
                conv.isTop = db->getIntValue(statementHandle, 1);
                conv.isSilent = db->getIntValue(statementHandle, 2);
                conv.timestamp = db->getBigIntValue(statementHandle, 3);
                
                std::list<TMessage> lastMessages = GetMessages(conversationType, target, line, std::list<int>(), true, 1, INT_MAX, "");
                if (lastMessages.size() > 0) {
                    conv.lastMessage = *lastMessages.begin();
                }
            }
            
            conv.unreadCount = GetUnreadCount(conversationType, target, line);
            return conv;
        }
        TUnreadCount MessageDB::GetUnreadCount(int conversationType, const std::string &target, int line) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return TUnreadCount();
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("count(*)");
            columns.push_back("sum(_status=?)");
            columns.push_back("sum(_status=?)");
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME, columns, "_conv_type=? and _conv_line=? and _conv_target=? and _status=?");
#else
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME, {"count(*), sum(_status=?), sum(_status=?)"}, "_conv_type=? and _conv_line=? and _conv_target=? and _status in (?, ?, ?)");
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return TUnreadCount();
            }
            
            int index = 1;
            db->Bind(statementHandle, Message_Status_Mentioned, index++);
            db->Bind(statementHandle, Message_Status_AllMentioned, index++);
            db->Bind(statementHandle, conversationType, index++);
            db->Bind(statementHandle, line, index++);
            db->Bind(statementHandle, target, index++);
            db->Bind(statementHandle, Message_Status_Unread, index++);
            db->Bind(statementHandle, Message_Status_Mentioned, index++);
            db->Bind(statementHandle, Message_Status_AllMentioned, index++);
            
            if (statementHandle.executeSelect()) {
                int unreadCount = db->getIntValue(statementHandle, 0);
                int unreadMetionCount = db->getIntValue(statementHandle, 1);
                int unreadMetionAllCount = db->getIntValue(statementHandle, 2);
                TUnreadCount count;
                count.unread = unreadCount;
                count.unreadMention = unreadMetionCount;
                count.unreadMentionAll = unreadMetionAllCount;
                return count;
            }
            
            return TUnreadCount();
        }
        
        TUnreadCount MessageDB::GetUnreadCount(const std::list<int> &conversationTypes, const std::list<int> lines) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return TUnreadCount();
            }
            std::string where;
            if (conversationTypes.size() > 0) {
                where += "_conv_type in (";
                for (std::list<int>::const_iterator it = conversationTypes.begin(); it != conversationTypes.end(); it++) {
                    char str[255];
                    memset(str, 0, 255);
                    sprintf(str, "%d", *it);
                    where += str;
                    where += ",";
                }
                where = where.substr(0, where.size()-1);
                where += ") and ";
            }
            
            
            if (lines.size() > 0) {
                where += "_conv_line in (";
                for (std::list<int>::const_iterator it = lines.begin(); it != lines.end(); it++) {
                    char str[255];
                    memset(str, 0, 255);
                    sprintf(str, "%d", *it);
                    where += str;
                    where += ",";
                }
                where = where.substr(0, where.size()-1);
                where += ") and ";
            }
            
            where += " _timestamp > 0 ";
            std::string orderBy = "_istop desc, _timestamp desc";
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_issilent");
            std::string sql = db->GetSelectSql(CONVERSATION_TABLE_NAME, columns, where, orderBy, 500);
#else
            std::string sql = db->GetSelectSql(CONVERSATION_TABLE_NAME, {"_conv_type", "_conv_target", "_conv_line", "_issilent"}, where, orderBy, 500);
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return TUnreadCount();
            }
            
            TUnreadCount unreadCount;
            while(statementHandle.executeSelect()) {
                TConversation conv;
                int conversationType = db->getIntValue(statementHandle, 0);
                std::string target = db->getStringValue(statementHandle, 1);
                int line = db->getIntValue(statementHandle, 2);
                bool isSilent = db->getIntValue(statementHandle, 3);
                if (!isSilent) {
                    TUnreadCount tcount = GetUnreadCount(conversationType, target, line);
                    unreadCount.unread += tcount.unread;
                    unreadCount.unreadMention += tcount.unreadMention;
                    unreadCount.unreadMentionAll += tcount.unreadMentionAll;
                }
                
            }
            return unreadCount;
        }
        
        std::list<TMessage> MessageDB::GetMessages(int conversationType, const std::string &target, int line, const std::list<int> &contentTypes, bool old, int count, long startPoint, const std::string &withUser) {
            DB2 *db = DB2::Instance();
          if (!db->isOpened()) {
            return std::list<TMessage>();
          }
            std::string where = "_conv_type=? and _conv_target=? and _conv_line=?";
            
            if (!withUser.empty()) {
                where += " and ((_direction=0 and (_to='' or _to=?)) or (_from=?))";
            }
            
            if (old) {
              if (startPoint == 0) {
                startPoint = INT_MAX;
              }
                where += " and _id < ?";
            } else {
                where += " and _id > ?";
            }
            
            if (contentTypes.size() > 0) {
                where += " and _cont_type in (";
                int count = 0;
                for (std::list<int>::const_iterator it = contentTypes.begin(); it != contentTypes.end(); ++it) {
                    char buffer[20];
                    memset(buffer, 0, 20);
                    sprintf(buffer, "%d", *it);
                    where += buffer;
                    count++;
                    if(count < contentTypes.size()) {
                        where += ",";
                    }
                    
                }
                where += ")";
            }
            
            std::string orderBy;
            if (old) {
                orderBy = "_timestamp desc, _id desc";
            } else {
                orderBy = "_timestamp asc, _id asc";
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_id");
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_from");
            columns.push_back("_to");
            columns.push_back("_cont_type");
            columns.push_back("_cont_searchable");
            columns.push_back("_cont_push");
            columns.push_back("_cont");
            columns.push_back("_cont_data");
            columns.push_back("_cont_local");
            columns.push_back("_cont_media_type");
            columns.push_back("_cont_remote_media_url");
            columns.push_back("_cont_local_media_path");
            columns.push_back("_direction");
            columns.push_back("_status");
            columns.push_back("_uid");
            columns.push_back("_timestamp");
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                                                         columns, where, orderBy, count);
#else
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                {
                    "_id",
                    "_conv_type",
                    "_conv_target",
                    "_conv_line",
                    "_from",
                    "_to",
                    "_cont_type",
                    "_cont_searchable",
                    "_cont_push",
                    "_cont",
                    "_cont_data",
                    "_cont_local",
                    "_cont_media_type",
                    "_cont_remote_media_url",
                    "_cont_local_media_path",
                    "_direction",
                    "_status",
                    "_uid",
                    "_timestamp"}, where, orderBy, count);
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<TMessage>();
            }
            
            int index = 1;
            db->Bind(statementHandle, conversationType, index++);
            db->Bind(statementHandle, target, index++);
            db->Bind(statementHandle, line, index++);
            if (!withUser.empty()) {
                db->Bind(statementHandle, withUser, index++);
                db->Bind(statementHandle, withUser, index++);
            }
            db->Bind(statementHandle, (int)startPoint, index++);
            
            std::list<TMessage> result;
            
            while (statementHandle.executeSelect()) {
                TMessage msg;
                int index = 0;
                msg.messageId = db->getIntValue(statementHandle, index++);
                msg.conversationType = db->getIntValue(statementHandle, index++);
                msg.target = db->getStringValue(statementHandle, index++);
                msg.line = db->getIntValue(statementHandle, index++);
                msg.from = db->getStringValue(statementHandle, index++);
                std::string toStr = db->getStringValue(statementHandle, index++);
                
                if (!toStr.empty()) {
                    std::istringstream f(toStr);
                    std::string s;
                    while (getline(f, s, ';')) {
                        msg.to.push_back(s);
                    }
                }
                
                
                msg.content.type = db->getIntValue(statementHandle, index++);
                msg.content.searchableContent = db->getStringValue(statementHandle, index++);
                msg.content.pushContent = db->getStringValue(statementHandle, index++);
                msg.content.content = db->getStringValue(statementHandle, index++);
                int size = 0;
                const void *p = db->getBlobValue(statementHandle, index++, size);
                msg.content.binaryContent = std::string((const char *)p, size);
                msg.content.localContent = db->getStringValue(statementHandle, index++);
                msg.content.mediaType = db->getIntValue(statementHandle, index++);
                msg.content.remoteMediaUrl = db->getStringValue(statementHandle, index++);
                msg.content.localMediaPath = db->getStringValue(statementHandle, index++);
                
                msg.direction = db->getIntValue(statementHandle, index++);
                msg.status = (MessageStatus)db->getIntValue(statementHandle, index++);
                msg.messageUid = db->getBigIntValue(statementHandle, index++);
                msg.timestamp = db->getBigIntValue(statementHandle, index++);
                
                result.push_back(msg);
            }
            if (old) {
                result.reverse();
            }
            
            return result;
        }
      
        std::list<TMessage> MessageDB::GetMessages(const std::list<int> &conversationTypes, const std::list<int> &lines, const std::list<int> &contentTypes, bool desc, int count, long startPoint, const std::string &withUser) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return std::list<TMessage>();
            }
            std::string where;
            if (conversationTypes.size() > 0) {
                where += "_conv_type in (";
                for (std::list<int>::const_iterator it = conversationTypes.begin(); it != conversationTypes.end(); it++) {
                    char str[255];
                    memset(str, 0, 255);
                    sprintf(str, "%d", *it);
                    where += str;
                    where += ",";
                }
                where = where.substr(0, where.size()-1);
                where += ") and";
            }
            
            
            if (lines.size() > 0) {
                where += " _conv_line in (";
                for (std::list<int>::const_iterator it = lines.begin(); it != lines.end(); it++) {
                    char str[255];
                    memset(str, 0, 255);
                    sprintf(str, "%d", *it);
                    where += str;
                    where += ",";
                }
                where = where.substr(0, where.size()-1);
                where += ") and";
            }
            
            
            if (!withUser.empty()) {
                where += " _from=? and";
            }
            
            if (desc) {
                if (startPoint == 0) {
                    startPoint = INT_MAX;
                }
                where += " _id < ?";
            } else {
                where += " _id > ?";
            }
            
            if (contentTypes.size() > 0) {
                where += " and _cont_type in (";
                int count = 0;
                for (std::list<int>::const_iterator it = contentTypes.begin(); it != contentTypes.end(); ++it) {
                    char buffer[20];
                    memset(buffer, 0, 20);
                    sprintf(buffer, "%d", *it);
                    where += buffer;
                    count++;
                    if(count < contentTypes.size()) {
                        where += ",";
                    }
                    
                }
                where += ")";
            }
            
            std::string orderBy;
            if (desc) {
                orderBy = "_timestamp desc, _id desc";
            } else {
                orderBy = "_timestamp asc, _id asc";
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_id");
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_from");
            columns.push_back("_to");
            columns.push_back("_cont_type");
            columns.push_back("_cont_searchable");
            columns.push_back("_cont_push");
            columns.push_back("_cont");
            columns.push_back("_cont_data");
            columns.push_back("_cont_local");
            columns.push_back("_cont_media_type");
            columns.push_back("_cont_remote_media_url");
            columns.push_back("_cont_local_media_path");
            columns.push_back("_direction");
            columns.push_back("_status");
            columns.push_back("_uid");
            columns.push_back("_timestamp");
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                               columns, where, orderBy, count);
#else
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                               {
                                                   "_id",
                                                   "_conv_type",
                                                   "_conv_target",
                                                   "_conv_line",
                                                   "_from",
                                                   "_to",
                                                   "_cont_type",
                                                   "_cont_searchable",
                                                   "_cont_push",
                                                   "_cont",
                                                   "_cont_data",
                                                   "_cont_local",
                                                   "_cont_media_type",
                                                   "_cont_remote_media_url",
                                                   "_cont_local_media_path",
                                                   "_direction",
                                                   "_status",
                                                   "_uid",
                                                   "_timestamp"}, where, orderBy, count);
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<TMessage>();
            }
            
            int index = 1;
            if (!withUser.empty()) {
                db->Bind(statementHandle, withUser, index++);
            }
            db->Bind(statementHandle, startPoint, index++);
            
            std::list<TMessage> result;
            
            while (statementHandle.executeSelect()) {
                TMessage msg;
                int index = 0;
                msg.messageId = db->getIntValue(statementHandle, index++);
                msg.conversationType = db->getIntValue(statementHandle, index++);
                msg.target = db->getStringValue(statementHandle, index++);
                msg.line = db->getIntValue(statementHandle, index++);
                msg.from = db->getStringValue(statementHandle, index++);
                std::string toStr = db->getStringValue(statementHandle, index++);
                
                if (!toStr.empty()) {
                    std::istringstream f(toStr);
                    std::string s;
                    while (getline(f, s, ';')) {
                        msg.to.push_back(s);
                    }
                }
                
                
                msg.content.type = db->getIntValue(statementHandle, index++);
                msg.content.searchableContent = db->getStringValue(statementHandle, index++);
                msg.content.pushContent = db->getStringValue(statementHandle, index++);
                msg.content.content = db->getStringValue(statementHandle, index++);
                int size = 0;
                const void *p = db->getBlobValue(statementHandle, index++, size);
                msg.content.binaryContent = std::string((const char *)p, size);
                msg.content.localContent = db->getStringValue(statementHandle, index++);
                msg.content.mediaType = db->getIntValue(statementHandle, index++);
                msg.content.remoteMediaUrl = db->getStringValue(statementHandle, index++);
                msg.content.localMediaPath = db->getStringValue(statementHandle, index++);
                
                msg.direction = db->getIntValue(statementHandle, index++);
                msg.status = (MessageStatus)db->getIntValue(statementHandle, index++);
                msg.messageUid = db->getBigIntValue(statementHandle, index++);
                msg.timestamp = db->getBigIntValue(statementHandle, index++);
                
                result.push_back(msg);
            }
            if (desc) {
                result.reverse();
            }
            
            return result;
        }
        
        std::list<TMessage> MessageDB::GetMessages(const std::list<int> &conversationTypes, const std::list<int> &lines, const int messageStatus, bool desc, int count, long startPoint, const std::string &withUser) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return std::list<TMessage>();
            }
            std::string where;
            if (conversationTypes.size() > 0) {
                where += "_conv_type in (";
                for (std::list<int>::const_iterator it = conversationTypes.begin(); it != conversationTypes.end(); it++) {
                    char str[255];
                    memset(str, 0, 255);
                    sprintf(str, "%d", *it);
                    where += str;
                    where += ",";
                }
                where = where.substr(0, where.size()-1);
                where += ") and";
            }
            
            
            if (lines.size() > 0) {
                where += " _conv_line in (";
                for (std::list<int>::const_iterator it = lines.begin(); it != lines.end(); it++) {
                    char str[255];
                    memset(str, 0, 255);
                    sprintf(str, "%d", *it);
                    where += str;
                    where += ",";
                }
                where = where.substr(0, where.size()-1);
                where += ") and";
            }
            
            where += " _status=? and";
            
            
            if (!withUser.empty()) {
                where += " _from=? and";
            }
            
            if (desc) {
                if (startPoint == 0) {
                    startPoint = INT_MAX;
                }
                where += " _id < ?";
            } else {
                where += " _id > ?";
            }
            
            std::string orderBy;
            if (desc) {
                orderBy = "_timestamp desc, _id desc";
            } else {
                orderBy = "_timestamp asc, _id asc";
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_id");
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_from");
            columns.push_back("_to");
            columns.push_back("_cont_type");
            columns.push_back("_cont_searchable");
            columns.push_back("_cont_push");
            columns.push_back("_cont");
            columns.push_back("_cont_data");
            columns.push_back("_cont_local");
            columns.push_back("_cont_media_type");
            columns.push_back("_cont_remote_media_url");
            columns.push_back("_cont_local_media_path");
            columns.push_back("_direction");
            columns.push_back("_status");
            columns.push_back("_uid");
            columns.push_back("_timestamp");
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                               columns, where, orderBy, count);
#else
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                               {
                                                   "_id",
                                                   "_conv_type",
                                                   "_conv_target",
                                                   "_conv_line",
                                                   "_from",
                                                   "_to",
                                                   "_cont_type",
                                                   "_cont_searchable",
                                                   "_cont_push",
                                                   "_cont",
                                                   "_cont_data",
                                                   "_cont_local",
                                                   "_cont_media_type",
                                                   "_cont_remote_media_url",
                                                   "_cont_local_media_path",
                                                   "_direction",
                                                   "_status",
                                                   "_uid",
                                                   "_timestamp"}, where, orderBy, count);
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<TMessage>();
            }
            
            int index = 1;
            db->Bind(statementHandle, messageStatus, index++);
            if (!withUser.empty()) {
                db->Bind(statementHandle, withUser, index++);
            }
            db->Bind(statementHandle, startPoint, index++);
            
            std::list<TMessage> result;
            
            while (statementHandle.executeSelect()) {
                TMessage msg;
                int index = 0;
                msg.messageId = db->getIntValue(statementHandle, index++);
                msg.conversationType = db->getIntValue(statementHandle, index++);
                msg.target = db->getStringValue(statementHandle, index++);
                msg.line = db->getIntValue(statementHandle, index++);
                msg.from = db->getStringValue(statementHandle, index++);
                std::string toStr = db->getStringValue(statementHandle, index++);
                
                if (!toStr.empty()) {
                    std::istringstream f(toStr);
                    std::string s;
                    while (getline(f, s, ';')) {
                        msg.to.push_back(s);
                    }
                }
                
                
                msg.content.type = db->getIntValue(statementHandle, index++);
                msg.content.searchableContent = db->getStringValue(statementHandle, index++);
                msg.content.pushContent = db->getStringValue(statementHandle, index++);
                msg.content.content = db->getStringValue(statementHandle, index++);
                int size = 0;
                const void *p = db->getBlobValue(statementHandle, index++, size);
                msg.content.binaryContent = std::string((const char *)p, size);
                msg.content.localContent = db->getStringValue(statementHandle, index++);
                msg.content.mediaType = db->getIntValue(statementHandle, index++);
                msg.content.remoteMediaUrl = db->getStringValue(statementHandle, index++);
                msg.content.localMediaPath = db->getStringValue(statementHandle, index++);
                
                msg.direction = db->getIntValue(statementHandle, index++);
                msg.status = (MessageStatus)db->getIntValue(statementHandle, index++);
                msg.messageUid = db->getBigIntValue(statementHandle, index++);
                msg.timestamp = db->getBigIntValue(statementHandle, index++);
                
                result.push_back(msg);
            }
            if (desc) {
                result.reverse();
            }
            
            return result;
        }
        
      TMessage MessageDB::GetMessageById(long messageId) {
          if (messageId < 0) {
              return TMessage();
          }
          
        DB2 *db = DB2::Instance();
        if (!db->isOpened()) {
          return TMessage();
        }
        std::string where = "_id=?";
        
#ifdef __ANDROID__
        std::list<std::string> columns;
        columns.push_back("_id");
        columns.push_back("_conv_type");
        columns.push_back("_conv_target");
        columns.push_back("_conv_line");
        columns.push_back("_from");
        columns.push_back("_to");
        columns.push_back("_cont_type");
        columns.push_back("_cont_searchable");
        columns.push_back("_cont_push");
        columns.push_back("_cont");
        columns.push_back("_cont_data");
        columns.push_back("_cont_local");
        columns.push_back("_cont_media_type");
        columns.push_back("_cont_remote_media_url");
        columns.push_back("_cont_local_media_path");
        columns.push_back("_direction");
        columns.push_back("_status");
        columns.push_back("_uid");
        columns.push_back("_timestamp");
        std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                                                     columns, where);
#else
        std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                                                     {
                                                                       "_id",
                                                                       "_conv_type",
                                                                       "_conv_target",
                                                                       "_conv_line",
                                                                       "_from",
                                                                       "_to",
                                                                       "_cont_type",
                                                                       "_cont_searchable",
                                                                       "_cont_push",
                                                                       "_cont",
                                                                       "_cont_data",
                                                                       "_cont_local",
                                                                       "_cont_media_type",
                                                                       "_cont_remote_media_url",
                                                                       "_cont_local_media_path",
                                                                       "_direction",
                                                                       "_status",
                                                                       "_uid",
                                                                       "_timestamp"}, where);
#endif
          
          
          int error = 0;
          RecyclableStatement statementHandle(db, sql, error);
          if (error != 0) {
              return TMessage();
          }
          
        
        db->Bind(statementHandle, messageId, 1);
        
        std::list<TMessage> result;
        TMessage msg;
        msg.messageId = -1;
        if (statementHandle.executeSelect()) {
          int index = 0;
          msg.messageId = db->getIntValue(statementHandle, index++);
          msg.conversationType = db->getIntValue(statementHandle, index++);
          msg.target = db->getStringValue(statementHandle, index++);
          msg.line = db->getIntValue(statementHandle, index++);
          msg.from = db->getStringValue(statementHandle, index++);
            std::string toStr = db->getStringValue(statementHandle, index++);
          
            if (!toStr.empty()) {
                std::istringstream f(toStr.c_str());
                std::string s;
                while (getline(f, s, ';')) {
                    msg.to.push_back(s);
                }
            }
            
          msg.content.type = db->getIntValue(statementHandle, index++);
          msg.content.searchableContent = db->getStringValue(statementHandle, index++);
          msg.content.pushContent = db->getStringValue(statementHandle, index++);
          msg.content.content = db->getStringValue(statementHandle, index++);
          int size = 0;
          const void *p = db->getBlobValue(statementHandle, index++, size);
          msg.content.binaryContent = std::string((const char *)p, size);
          msg.content.localContent = db->getStringValue(statementHandle, index++);
          msg.content.mediaType = db->getIntValue(statementHandle, index++);
          msg.content.remoteMediaUrl = db->getStringValue(statementHandle, index++);
          msg.content.localMediaPath = db->getStringValue(statementHandle, index++);
          
          msg.direction = db->getIntValue(statementHandle, index++);
          msg.status = (MessageStatus)db->getIntValue(statementHandle, index++);
          msg.messageUid = db->getBigIntValue(statementHandle, index++);
          msg.timestamp = db->getBigIntValue(statementHandle, index++);
        }
        
        return msg;
      }
      
      TMessage MessageDB::GetMessageByUid(long long messageUid) {
        DB2 *db = DB2::Instance();
        if (!db->isOpened()) {
          return TMessage();
        }
        std::string where = "_uid=?";
        
#ifdef __ANDROID__
        std::list<std::string> columns;
        columns.push_back("_id");
        columns.push_back("_conv_type");
        columns.push_back("_conv_target");
        columns.push_back("_conv_line");
        columns.push_back("_from");
        columns.push_back("_to");
        columns.push_back("_cont_type");
        columns.push_back("_cont_searchable");
        columns.push_back("_cont_push");
        columns.push_back("_cont");
        columns.push_back("_cont_data");
        columns.push_back("_cont_local");
        columns.push_back("_cont_media_type");
        columns.push_back("_cont_remote_media_url");
        columns.push_back("_cont_local_media_path");
        columns.push_back("_direction");
        columns.push_back("_status");
        columns.push_back("_uid");
        columns.push_back("_timestamp");
        std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                                                     columns, where);
#else
        std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                                                     {
                                                                       "_id",
                                                                       "_conv_type",
                                                                       "_conv_target",
                                                                       "_conv_line",
                                                                       "_from",
                                                                       "_to",
                                                                       "_cont_type",
                                                                       "_cont_searchable",
                                                                       "_cont_push",
                                                                       "_cont",
                                                                       "_cont_data",
                                                                       "_cont_local",
                                                                       "_cont_media_type",
                                                                       "_cont_remote_media_url",
                                                                       "_cont_local_media_path",
                                                                       "_direction",
                                                                       "_status",
                                                                       "_uid",
                                                                       "_timestamp"}, where);
#endif
        
          int error = 0;
          RecyclableStatement statementHandle(db, sql, error);
          if (error != 0) {
              return TMessage();
          }
          
        db->Bind(statementHandle, (int64_t)messageUid, 1);
        
        std::list<TMessage> result;
        TMessage msg;
        msg.messageId = -1;
        if (statementHandle.executeSelect()) {
          int index = 0;
          msg.messageId = db->getIntValue(statementHandle, index++);
          msg.conversationType = db->getIntValue(statementHandle, index++);
          msg.target = db->getStringValue(statementHandle, index++);
          msg.line = db->getIntValue(statementHandle, index++);
          msg.from = db->getStringValue(statementHandle, index++);
          std::string toStr = db->getStringValue(statementHandle, index++);
            
            if (!toStr.empty()) {
                std::istringstream f(toStr.c_str());
                std::string s;
                while (getline(f, s, ';')) {
                    msg.to.push_back(s);
                }
            }
          msg.content.type = db->getIntValue(statementHandle, index++);
          msg.content.searchableContent = db->getStringValue(statementHandle, index++);
          msg.content.pushContent = db->getStringValue(statementHandle, index++);
          msg.content.content = db->getStringValue(statementHandle, index++);
          int size = 0;
          const void *p = db->getBlobValue(statementHandle, index++, size);
          msg.content.binaryContent = std::string((const char *)p, size);
          msg.content.localContent = db->getStringValue(statementHandle, index++);
          msg.content.mediaType = db->getIntValue(statementHandle, index++);
          msg.content.remoteMediaUrl = db->getStringValue(statementHandle, index++);
          msg.content.localMediaPath = db->getStringValue(statementHandle, index++);
          
          msg.direction = db->getIntValue(statementHandle, index++);
          msg.status = (MessageStatus)db->getIntValue(statementHandle, index++);
          msg.messageUid = db->getBigIntValue(statementHandle, index++);
          msg.timestamp = db->getBigIntValue(statementHandle, index++);
        }
        
        return msg;
      }
        bool MessageDB::updateMessageStatus(long messageId, MessageStatus status) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_status");
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, columns, "_id=?");
#else
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, {"_status"}, "_id=?");
#endif
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(updateStatementHandle, status, 1);
            db->Bind(updateStatementHandle, messageId, 2);
            int count = db->ExecuteUpdate(updateStatementHandle);
            
            if (count > 0) {
                return true;
            }
            
            return false;
        }
        
        bool MessageDB::updateMessageUidAndTimestamp(long messageId, int64_t messageUid, int64_t sendTime) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_uid");
            columns.push_back("_timestamp");
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, columns, "_id=?");
#else
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, {"_uid", "_timestamp"}, "_id=?");
#endif
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(updateStatementHandle, messageUid, 1);
            db->Bind(updateStatementHandle, sendTime, 2);
            db->Bind(updateStatementHandle, messageId, 3);
            int count = db->ExecuteUpdate(updateStatementHandle);
            
            if (count > 0) {
                return true;
            }
            
            return false;
        }
        
        bool MessageDB::updateMessageRemoteMediaUrl(long messageId, const std::string &remoteMediaUrl) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_cont_remote_media_url");
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, columns, "_id=?");
#else
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, {"_cont_remote_media_url"}, "_id=?");
#endif
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(updateStatementHandle, remoteMediaUrl, 1);
            db->Bind(updateStatementHandle, messageId, 2);
            int count = db->ExecuteUpdate(updateStatementHandle);
            
            if (count > 0) {
                return true;
            }
            
            return false;
        }
        
        bool MessageDB::updateMessageLocalMediaPath(long messageId, const std::string &localMediaPath) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_cont_local_media_path");
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, columns, "_id=?");
#else
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, {"_cont_local_media_path"}, "_id=?");
#endif
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(updateStatementHandle, localMediaPath, 1);
            db->Bind(updateStatementHandle, messageId, 2);
            int count = db->ExecuteUpdate(updateStatementHandle);
            
            if (count > 0) {
                return true;
            }
            
            return false;
        }

        bool MessageDB::ClearUnreadStatus(int conversationType, const std::string &target, int line) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_status");
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, columns, "_conv_type=? and _conv_line=? and _conv_target=? and _status in (?, ?, ?)");
#else
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, {"_status"}, "_conv_type=? and _conv_line=? and _conv_target=? and _status in (?, ?, ?)");
#endif
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            
            db->Bind(updateStatementHandle, Message_Status_Readed, 1);
            db->Bind(updateStatementHandle, conversationType, 2);
            db->Bind(updateStatementHandle, line, 3);
            db->Bind(updateStatementHandle, target, 4);
            db->Bind(updateStatementHandle, Message_Status_Unread, 5);
            db->Bind(updateStatementHandle, Message_Status_Mentioned, 6);
            db->Bind(updateStatementHandle, Message_Status_AllMentioned, 7);
            int count = db->ExecuteUpdate(updateStatementHandle);
            
            if (count > 0) {
                int64_t dt = getConversationReadMaxDt(conversationType, target, line);
                if (dt > 0) {
                    syncConversationReadDt(conversationType, target, line, dt);
                }
                return true;
            }
            
            return false;
        }
        
        int64_t MessageDB::getConversationReadMaxDt(int conversationType, const std::string &target, int line) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return 0;
            }
            
             std::string where = "_conv_type=? and _conv_line=? and _conv_target=? and _direction=1";
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("max(_timestamp)");
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME, columns, where);
#else
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME, {"max(_timestamp)"}, where);
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return 0;
            }
            
            db->Bind(statementHandle, conversationType, 1);
            db->Bind(statementHandle, line, 2);
            db->Bind(statementHandle, target, 3);
            
            if(statementHandle.executeSelect()) {
                return db->getBigIntValue(statementHandle, 0);
            }
            
            return 0;
        }
        
        bool MessageDB::updateConversationRead(int conversationType, const std::string &target, int line, int64_t dt) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_status");
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, columns, "_conv_type=? and _conv_line=? and _conv_target=? and  _timestamp <= ? and _status in (?, ?, ?)");
#else
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, {"_status"}, "_conv_type=? and _conv_line=? and _conv_target=? and _timestamp <= ? and _status in (?, ?, ?)");
#endif
            
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(updateStatementHandle, Message_Status_Readed, 1);
            db->Bind(updateStatementHandle, conversationType, 2);
            db->Bind(updateStatementHandle, line, 3);
            db->Bind(updateStatementHandle, target, 4);
            db->Bind(updateStatementHandle, dt, 5);
            db->Bind(updateStatementHandle, Message_Status_Unread, 6);
            db->Bind(updateStatementHandle, Message_Status_Mentioned, 7);
            db->Bind(updateStatementHandle, Message_Status_AllMentioned, 8);
            int count = db->ExecuteUpdate(updateStatementHandle);
            
            if (count > 0) {
                return true;
            }
            
            return false;
        }
        
        bool MessageDB::ClearAllUnreadStatus() {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_status");
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, columns, "_status in (?, ?, ?)");
#else
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, {"_status"}, "_status in (?, ?, ?)");
#endif
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(updateStatementHandle, Message_Status_Readed, 1);
            db->Bind(updateStatementHandle, Message_Status_Unread, 2);
            db->Bind(updateStatementHandle, Message_Status_Mentioned, 3);
            db->Bind(updateStatementHandle, Message_Status_AllMentioned, 4);
            int count = db->ExecuteUpdate(updateStatementHandle);
            
            if (count > 0) {
                return true;
            }
            
            return false;
        }
        
        bool MessageDB::FailSendingMessages() {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_status");
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, columns, "_status=?");
#else
            std::string sql = db->GetUpdateSql(MESSAGE_TABLE_NAME, {"_status"}, "_status=?");
#endif
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(updateStatementHandle, Message_Status_Send_Failure, 1);
            db->Bind(updateStatementHandle, Message_Status_Sending, 2);
            int count = db->ExecuteUpdate(updateStatementHandle);
            
            if (count > 0) {
                return true;
            }
            
            return false;
        }
        
        std::list<TMessage> MessageDB::SearchMessages(int conversationType, const std::string &target, int line, const std::string &keyword, int limit) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened() || keyword.empty()) {
                return std::list<TMessage>();
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_id");
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_from");
            columns.push_back("_to");
            columns.push_back("_cont_type");
            columns.push_back("_cont_searchable");
            columns.push_back("_cont_push");
            columns.push_back("_cont");
            columns.push_back("_cont_data");
            columns.push_back("_cont_local");
            columns.push_back("_cont_media_type");
            columns.push_back("_cont_remote_media_url");
            columns.push_back("_cont_local_media_path");
            columns.push_back("_direction");
            columns.push_back("_status");
            columns.push_back("_uid");
            columns.push_back("_timestamp");
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                                                         columns, "_conv_type=? and _conv_line=? and _conv_target=? and _cont_searchable like ?", "_timestamp desc", limit);
#else
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                                                               {
                                                                                   "_id",
                                                                                   "_conv_type",
                                                                                   "_conv_line",
                                                                                   "_conv_target",
                                                                                   "_from",
                                                                                   "_to",
                                                                                   "_cont_type",
                                                                                   "_cont_searchable",
                                                                                   "_cont_push",
                                                                                   "_cont",
                                                                                   "_cont_data",
                                                                                   "_cont_local",
                                                                                   "_cont_media_type",
                                                                                   "_cont_remote_media_url",
                                                                                   "_cont_local_media_path",
                                                                                   "_direction",
                                                                                   "_status",
                                                                                   "_uid",
                                                                                   "_timestamp"}, "_conv_type=? and _conv_target=? and _conv_line=? and _cont_searchable like ?", "_timestamp desc", limit);
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<TMessage>();
            }
            
            db->Bind(statementHandle, conversationType, 1);
            db->Bind(statementHandle, line, 2);
            db->Bind(statementHandle, target, 3);
            db->Bind(statementHandle, "%" + keyword + "%", 4);
            
            std::list<TMessage> result;
            
            while (statementHandle.executeSelect()) {
                TMessage msg;
                int index = 0;
                msg.messageId = db->getIntValue(statementHandle, index++);
                msg.conversationType = db->getIntValue(statementHandle, index++);
                msg.target = db->getStringValue(statementHandle, index++);
                msg.line = db->getIntValue(statementHandle, index++);
                msg.from = db->getStringValue(statementHandle, index++);
                std::string toStr = db->getStringValue(statementHandle, index++);
                
                if (!toStr.empty()) {
                    std::istringstream f(toStr.c_str());
                    std::string s;
                    while (getline(f, s, ';')) {
                        msg.to.push_back(s);
                    }
                }
                msg.content.type = db->getIntValue(statementHandle, index++);
                msg.content.searchableContent = db->getStringValue(statementHandle, index++);
                msg.content.pushContent = db->getStringValue(statementHandle, index++);
                msg.content.content = db->getStringValue(statementHandle, index++);
                int size = 0;
                const void *p = db->getBlobValue(statementHandle, index++, size);
                msg.content.binaryContent = std::string((const char *)p, size);
                msg.content.localContent = db->getStringValue(statementHandle, index++);
                msg.content.mediaType = db->getIntValue(statementHandle, index++);
                msg.content.remoteMediaUrl = db->getStringValue(statementHandle, index++);
                msg.content.localMediaPath = db->getStringValue(statementHandle, index++);
                
                msg.direction = db->getIntValue(statementHandle, index++);
                msg.status = (MessageStatus)db->getIntValue(statementHandle, index++);
                msg.messageUid = db->getBigIntValue(statementHandle, index++);
                msg.timestamp = db->getBigIntValue(statementHandle, index++);
                
                result.push_back(msg);
            }
            
            
            return result;
        }
        
        std::list<TConversationSearchresult> MessageDB::SearchConversations(const std::list<int> &conversationTypes, const std::list<int> lines, const std::string &keyword, int limit) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened() || keyword.empty()) {
                return std::list<TConversationSearchresult>();
            }
            
            std::string where = "_cont_searchable like ? and ";
            if (conversationTypes.size() > 0) {
                where += "_conv_type in (";
                for (std::list<int>::const_iterator it = conversationTypes.begin(); it != conversationTypes.end(); it++) {
                    char str[255];
                    memset(str, 0, 255);
                    sprintf(str, "%d", *it);
                    where += str;
                    where += ",";
                }
                where = where.substr(0, where.size()-1);
                where += ") and ";
            }
            
            
            if (lines.size() > 0) {
                where += "_conv_line in (";
                for (std::list<int>::const_iterator it = lines.begin(); it != lines.end(); it++) {
                    char str[255];
                    memset(str, 0, 255);
                    sprintf(str, "%d", *it);
                    where += str;
                    where += ",";
                }
                where = where.substr(0, where.size()-1);
                where += ") ";
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("count(*)");
            columns.push_back("_id");
            columns.push_back("_conv_type");
            columns.push_back("_conv_target");
            columns.push_back("_conv_line");
            columns.push_back("_timestamp");
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                                                         columns, where, "_timestamp desc", limit, 0, " _conv_type, _conv_target, _conv_line");
#else
            std::string sql = db->GetSelectSql(MESSAGE_TABLE_NAME,
                                                                               {
                                                                                   "count(*)",
                                                                                   "_id",
                                                                                   "_conv_type",
                                                                                   "_conv_target",
                                                                                   "_conv_line",
                                                                                   "_timestamp"
                                                                               }, where, "_timestamp desc", limit, 0, " _conv_type, _conv_target, _conv_line");
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<TConversationSearchresult>();
            }
            
            db->Bind(statementHandle, "%" + keyword + "%", 1);
            std::list<TConversationSearchresult> results;
            
            while (statementHandle.executeSelect()) {
                int count = db->getIntValue(statementHandle, 0);
                if (count == 0) {
                    continue;
                }
                TConversationSearchresult result;
                result.marchedCount = count;
                int messageId = db->getIntValue(statementHandle, 1);
                if (count == 1) {
                    result.marchedMessage = GetMessageById(messageId);
                }
                result.conversationType = db->getIntValue(statementHandle, 2);
                result.target = db->getStringValue(statementHandle, 3);
                result.line = db->getIntValue(statementHandle, 4);
                result.timestamp = db->getBigIntValue(statementHandle, 5);
                results.push_back(result);
            }
            
            return results;
        }
        
        std::list<TUserInfo> MessageDB::SearchFriends(const std::string &keyword, int limit) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened() || keyword.empty()) {
                return std::list<TUserInfo>();
            }
#ifdef __ANDROID__
            std::list<std::string> columnLeft;
            std::list<std::string> columnRight;
            columnRight.push_back("_uid");
            columnRight.push_back( "_name");
            columnRight.push_back("_display_name");
            columnRight.push_back("_portrait");
            columnRight.push_back("_gender");
            columnRight.push_back("_mobile");
            columnRight.push_back("_email");
            columnRight.push_back("_address");
            columnRight.push_back("_company");
            columnRight.push_back("_social");
            columnRight.push_back("_extra");
            columnRight.push_back("_update_dt");
            std::string sql = db->GetSelectSqlEx(FRIEND_TABLE_NAME,
                                                                         columnLeft,USER_TABLE_NAME, columnRight, "l._friend_uid = r._uid and l._state = 0 and r._display_name like ? ", "", limit);
#else
            std::string sql = db->GetSelectSqlEx(FRIEND_TABLE_NAME,
                                                                         {
                                                                         },
                                                                          USER_TABLE_NAME,
                                                                           {"_uid",  "_name", "_display_name", "_portrait", "_gender", "_mobile", "_email", "_address", "_company", "_social", "_extra", "_update_dt"},
                                                                           "l._friend_uid = r._uid and l._state = 0 and r._display_name like ? ", "", limit);
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<TUserInfo>();
            }
            
            db->Bind(statementHandle, "%" + keyword + "%", 1);
            std::list<TUserInfo> results;
            
            while (statementHandle.executeSelect()) {
                TUserInfo ui;
                ui.uid = db->getStringValue(statementHandle, 0);
                ui.name = db->getStringValue(statementHandle, 1);
                ui.displayName = db->getStringValue(statementHandle, 2);
                ui.portrait = db->getStringValue(statementHandle, 3);
                ui.gender = db->getIntValue(statementHandle, 4);
                ui.mobile = db->getStringValue(statementHandle, 5);
                ui.email = db->getStringValue(statementHandle, 6);
                ui.address = db->getStringValue(statementHandle, 7);
                ui.company = db->getStringValue(statementHandle, 8);
                ui.social = db->getStringValue(statementHandle, 9);
                ui.extra = db->getStringValue(statementHandle, 10);
                ui.updateDt = db->getBigIntValue(statementHandle, 11);
                results.push_back(ui);
            }
            
            return results;
        }
        
        std::list<TGroupSearchResult> MessageDB::SearchGroups(const std::string &keyword, int limit) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened() || keyword.empty()) {
                return std::list<TGroupSearchResult>();
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_uid");
            columns.push_back("_name");
            columns.push_back("_portrait");
            columns.push_back("_owner");
            columns.push_back("_type");
            columns.push_back("_extra");
            columns.push_back("_member_count");
            columns.push_back("_update_dt");
            std::string sql = db->GetSelectSql(GROUP_TABLE_NAME, columns, "_name like ?", "", limit);
#else
            std::string sql = db->GetSelectSql(GROUP_TABLE_NAME, {"_uid", "_name",  "_portrait", "_owner", "_type", "_extra", "_member_count", "_update_dt"}, "_name like ?", "", limit);
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<TGroupSearchResult>();
            }
            
            
            db->Bind(statementHandle, "%" + keyword + "%", 1);
            std::list<TGroupSearchResult> results;
            
            while (statementHandle.executeSelect()) {
                TGroupSearchResult result;
                result.groupInfo.target = db->getStringValue(statementHandle, 0);
                result.groupInfo.name = db->getStringValue(statementHandle, 1);
                result.groupInfo.portrait = db->getStringValue(statementHandle, 2);
                result.groupInfo.owner = db->getStringValue(statementHandle, 3);
                result.groupInfo.type = db->getIntValue(statementHandle, 4);
                result.groupInfo.extra = db->getStringValue(statementHandle, 5);
                result.groupInfo.memberCount = db->getIntValue(statementHandle, 6);
                result.groupInfo.updateDt = db->getBigIntValue(statementHandle, 7);
                result.marchedType = 0;
                results.push_back(result);
            }
            
#ifdef __ANDROID__
            std::list<std::string> columnMiddle;
            std::list<std::string> columnRight;
            columnRight.push_back("_uid");
            sql = db->GetSelectSqlEx2(GROUP_TABLE_NAME, columns, GROUP_MEMBER_TABLE_NAME, columnMiddle, USER_TABLE_NAME, columnRight, "l._uid = m._gid and m._mid = r._uid and r._name like ?", "", limit);
#else
            sql = db->GetSelectSqlEx2(GROUP_TABLE_NAME, {"_uid", "_name",  "_portrait", "_owner", "_type", "_extra", "_member_count", "_update_dt"}, GROUP_MEMBER_TABLE_NAME, {}, USER_TABLE_NAME, {"_uid"}, "l._uid = m._gid and m._mid = r._uid and r._name like ?", "", limit);
#endif
            error = 0;
            RecyclableStatement statementHandle2(db, sql, error);
            if (error != 0) {
                return std::list<TGroupSearchResult>();
            }
            
            db->Bind(statementHandle2, "%" + keyword + "%", 1);
            
            while (statementHandle2.executeSelect()) {
                bool exist = false;
                std::string groupId = db->getStringValue(statementHandle2, 0);
                for (std::list<TGroupSearchResult>::iterator it = results.begin(); it != results.end(); it++) {
                    TGroupSearchResult &tmp = *it;
                    if (tmp.groupInfo.target == groupId) {
                        tmp.marchedMemberNames.push_back(db->getStringValue(statementHandle2, 8));
                        tmp.marchedType = 2;
                        exist = true;
                        break;
                    }
                }
                
                if (!exist) {
                    TGroupSearchResult result;
                    result.groupInfo.target = groupId;
                    result.groupInfo.name = db->getStringValue(statementHandle2, 1);
                    result.groupInfo.portrait = db->getStringValue(statementHandle2, 2);
                    result.groupInfo.owner = db->getStringValue(statementHandle2, 3);
                    result.groupInfo.type = db->getIntValue(statementHandle2, 4);
                    result.groupInfo.extra = db->getStringValue(statementHandle2, 5);
                    result.groupInfo.memberCount = db->getIntValue(statementHandle2, 6);
                    result.groupInfo.updateDt = db->getBigIntValue(statementHandle2, 7);
                    result.marchedMemberNames.push_back(db->getStringValue(statementHandle2, 8));
                    result.marchedType = 1;
                    results.push_back(result);
                }
            }
            
            return results;
        }
        
        class TGetGroupInfoCallback : public GetGroupInfoCallback {
        public:
            void onSuccess(const std::list<mars::stn::TGroupInfo> &groupInfoList) {
                for (std::list<TGroupInfo>::const_iterator it = groupInfoList.begin(); it != groupInfoList.end(); it++) {
                    MessageDB::Instance()->InsertGroupInfo(*it);
                }
                if(StnCallBack::Instance()->m_getGroupInfoCB) {
                    StnCallBack::Instance()->m_getGroupInfoCB->onSuccess(groupInfoList);
                }
                delete this;
            }
            void onFalure(int errorCode) {
                if(StnCallBack::Instance()->m_getGroupInfoCB) {
                    StnCallBack::Instance()->m_getGroupInfoCB->onFalure(errorCode);
                }
                delete this;
            }
            virtual ~TGetGroupInfoCallback() {}
        };
        
        class TGetChannelInfoCallback : public GetChannelInfoCallback {
        public:
            void onSuccess(const std::list<mars::stn::TChannelInfo> &channelInfoList) {
                for (std::list<TChannelInfo>::const_iterator it = channelInfoList.begin(); it != channelInfoList.end(); it++) {
                    MessageDB::Instance()->InsertOrUpdateChannelInfo(*it);
                }
                if(StnCallBack::Instance()->m_getChannelInfoCB) {
                    StnCallBack::Instance()->m_getChannelInfoCB->onSuccess(channelInfoList);
                }
                delete this;
            }
            void onFalure(int errorCode) {
                if(StnCallBack::Instance()->m_getChannelInfoCB) {
                    StnCallBack::Instance()->m_getChannelInfoCB->onFalure(errorCode);
                }
                delete this;
            }
            virtual ~TGetChannelInfoCallback() {}
        };
        
        TGroupInfo MessageDB::GetGroupInfo(const std::string &groupId, bool refresh) {
            DB2 *db = DB2::Instance();
            
            TGroupInfo gi;
            if (!db->isOpened()) {
                return gi;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_name");
            columns.push_back("_portrait");
            columns.push_back("_owner");
            columns.push_back("_type");
            columns.push_back("_extra");
            columns.push_back("_member_count");
            columns.push_back("_update_dt");
            std::string sql = db->GetSelectSql(GROUP_TABLE_NAME, columns, "_uid=?");
#else
            std::string sql = db->GetSelectSql(GROUP_TABLE_NAME, {"_name",  "_portrait", "_owner", "_type", "_extra", "_member_count", "_update_dt"}, "_uid=?");
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return gi;
            }
            
            db->Bind(statementHandle, groupId, 1);
            gi.target = groupId;
            
            if (statementHandle.executeSelect()) {
                gi.name = db->getStringValue(statementHandle, 0);
                gi.portrait = db->getStringValue(statementHandle, 1);
                gi.owner = db->getStringValue(statementHandle, 2);
                gi.type = db->getIntValue(statementHandle, 3);
                gi.extra = db->getStringValue(statementHandle, 4);
                gi.memberCount = db->getIntValue(statementHandle, 5);
                gi.updateDt = db->getBigIntValue(statementHandle, 6);
                
            } else {
                gi.target = "";//empty
                gi.updateDt = 0;
            }
            
            if (refresh || gi.target.empty()) {
                std::list<std::pair<std::string, int64_t>> groupIdList;
                groupIdList.push_back(std::pair<std::string, int64_t>(groupId, gi.updateDt));
                getGroupInfo(groupIdList, new TGetGroupInfoCallback());
            }
            return gi;
        }
        
        long MessageDB::InsertGroupInfo(const TGroupInfo &groupInfo) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return -1;
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_uid");
            columns.push_back("_name");
            columns.push_back("_portrait");
            columns.push_back("_owner");
            columns.push_back("_type");
            columns.push_back("_extra");
            columns.push_back("_member_count");
            columns.push_back("_update_dt");
            std::string sql = db->GetInsertSql(GROUP_TABLE_NAME, columns, true);
#else
            std::string sql = db->GetInsertSql(GROUP_TABLE_NAME, {"_uid", "_name", "_portrait", "_owner", "_type", "_extra", "_member_count", "_update_dt"}, true);
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return -1;
            }
            
            db->Bind(statementHandle, groupInfo.target, 1);
            db->Bind(statementHandle, groupInfo.name, 2);
            db->Bind(statementHandle, groupInfo.portrait, 3);
            
            db->Bind(statementHandle, groupInfo.owner, 4);
            db->Bind(statementHandle, groupInfo.type, 5);
            db->Bind(statementHandle, groupInfo.extra, 6);
            db->Bind(statementHandle, groupInfo.memberCount, 7);
            db->Bind(statementHandle, groupInfo.updateDt, 8);
            long ret = 0;
            ret = db->ExecuteInsert(statementHandle, &ret);
            return ret;

        }
        
        bool MessageDB::UpdateGroupInfo(const std::string &groupId, int type, const std::string &newValue) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return -1;
            }
            
            std::list<std::string> columns;
            
            switch (type) {
                case 0:
                    columns.push_back("_name");
                    break;
                case 1:
                    columns.push_back("_portrait");
                    break;
                case 2:
                    columns.push_back("_extra");
                    break;
                default:
                    return false;
            }
            
            std::string sql = db->GetUpdateSql(GROUP_TABLE_NAME, columns, "_uid=?");

            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            int index = 1;
            db->Bind(statementHandle, newValue, index++);
            db->Bind(statementHandle, groupId, index++);
            
            return db->ExecuteUpdate(statementHandle) > 0;
        }
        
        std::list<TGroupMember> MessageDB::GetGroupMembers(const std::string &groupId, bool refresh) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return std::list<TGroupMember>();
            }

#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_gid");
            columns.push_back( "_mid");
            columns.push_back("_alias");
            columns.push_back("_type");
            columns.push_back("_update_dt");
            std::string sql = db->GetSelectSql(GROUP_MEMBER_TABLE_NAME, columns, "_gid=?");
#else
            std::string sql = db->GetSelectSql(GROUP_MEMBER_TABLE_NAME, {"_gid",  "_mid", "_alias", "_type", "_update_dt"}, "_gid=?");
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<TGroupMember>();
            }
            
            db->Bind(statementHandle, groupId, 1);
            
            std::list<TGroupMember> resultList;
            int64_t maxDt = 0;
            while (statementHandle.executeSelect()) {
                TGroupMember member;
                member.groupId = db->getStringValue(statementHandle, 0);
                member.memberId = db->getStringValue(statementHandle, 1);
                member.alias = db->getStringValue(statementHandle, 2);
                member.type = db->getIntValue(statementHandle, 3);
                member.updateDt = db->getBigIntValue(statementHandle, 4);
                resultList.push_back(member);
                if (member.updateDt > maxDt) {
                    maxDt = member.updateDt;
                }
            }
            
            if (refresh || resultList.empty()) {
                reloadGroupMembersFromRemote(groupId, maxDt);
            }
            
            return resultList;
        }
        TGroupMember MessageDB::GetGroupMember(const std::string &groupId, const std::string &memberId) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return TGroupMember();
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_gid");
            columns.push_back( "_mid");
            columns.push_back("_alias");
            columns.push_back("_type");
            columns.push_back("_update_dt");
            std::string sql = db->GetSelectSql(GROUP_MEMBER_TABLE_NAME, columns, "_gid=? and _mid=?");
#else
            std::string sql = db->GetSelectSql(GROUP_MEMBER_TABLE_NAME, {"_gid",  "_mid", "_alias", "_type", "_update_dt"}, "_gid=? and _mid=?");
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return TGroupMember();
            }
            
            db->Bind(statementHandle, groupId, 1);
            db->Bind(statementHandle, memberId, 2);
            
            TGroupMember member;
            if (statementHandle.executeSelect()) {
                member.groupId = db->getStringValue(statementHandle, 0);
                member.memberId = db->getStringValue(statementHandle, 1);
                member.alias = db->getStringValue(statementHandle, 2);
                member.type = db->getIntValue(statementHandle, 3);
                member.updateDt = db->getBigIntValue(statementHandle, 4);
            }
            
            return member;
        }
        bool MessageDB::RemoveGroupAndMember(const std::string &groupId) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return false;
            }
            
            std::string sql = db->GetDeleteSql(GROUP_TABLE_NAME, "_uid=?");
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            
            db->Bind(statementHandle, groupId, 1);
            
            bool result = db->ExecuteDelete(statementHandle) > 0;
            
            std::string sql2 = db->GetDeleteSql(GROUP_MEMBER_TABLE_NAME, "_gid=?");
            
            
            error = 0;
            RecyclableStatement statementHandle2(db, sql2, error);
            db->Bind(statementHandle2, groupId, 1);
            
            result &= db->ExecuteDelete(statementHandle2) > 0;
            
            return result;
        }
        
        void MessageDB::UpdateGroupMember(const std::list<TGroupMember> &retList) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return ;
            }
            
            for (std::list<TGroupMember>::const_iterator it = retList.begin(); it != retList.end(); it++) {
                if(it->type != 4) {
#ifdef __ANDROID__
                    std::list<std::string> columns;
                    columns.push_back("_gid");
                    columns.push_back( "_mid");
                    columns.push_back("_alias");
                    columns.push_back("_type");
                    columns.push_back("_update_dt");
                    std::string sql = db->GetInsertSql(GROUP_MEMBER_TABLE_NAME, columns, true);
#else
                    std::string sql = db->GetInsertSql(GROUP_MEMBER_TABLE_NAME, {"_gid",  "_mid", "_alias", "_type", "_update_dt"}, true);
#endif
                    int error = 0;
                    RecyclableStatement statementHandle(db, sql, error);
                    if (error != 0) {
                        return ;
                    }
                    
                    db->Bind(statementHandle, it->groupId, 1);
                    db->Bind(statementHandle, it->memberId, 2);
                    db->Bind(statementHandle, it->alias, 3);
                    db->Bind(statementHandle, it->type, 4);
                    db->Bind(statementHandle, it->updateDt, 5);
                    
                    long ret = 0;
                    ret = db->ExecuteInsert(statementHandle, &ret);
                } else {
                    std::string sql = db->GetDeleteSql(GROUP_MEMBER_TABLE_NAME, "_gid=? and _mid=?");
                    int error = 0;
                    RecyclableStatement statementHandle(db, sql, error);
                    
                    db->Bind(statementHandle, it->groupId, 1);
                    db->Bind(statementHandle, it->memberId, 2);
                    
                    if (db->ExecuteDelete(statementHandle) > 0) {
                        
                    }
                }
            }
            
            return;
        }
        
        TUserInfo MessageDB::getUserInfo(const std::string &userId, const std::string &groupId, bool refresh) {
            DB2 *db = DB2::Instance();
            TUserInfo ui;
            if (!db->isOpened()) {
                return ui;
            }

            std::string sql;
            if (groupId.empty()) {
                sql = "select l._uid,l._name,l._display_name,l._portrait,l._gender,l._mobile,l._email,l._address,l._company,l._social,l._extra,l._type,l._update_dt,m._alias from t_user as l left join t_friend as m on l._uid=m._friend_uid where l._uid = ? limit 1";
            } else {
                sql = "select l._uid,l._name,l._display_name,l._portrait,l._gender,l._mobile,l._email,l._address,l._company,l._social,l._extra,l._type,l._update_dt,m._alias,r._alias from t_user as l left join t_friend as m on l._uid=m._friend_uid left join (select _mid, _alias from t_group_member where _gid = ?) as r  on l._uid=r._mid where l._uid = ? limit 1";
            }
            
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return ui;
            }
            
            int index = 1;
            if (!groupId.empty()) {
                db->Bind(statementHandle, groupId, index++);
            }
            
            db->Bind(statementHandle, userId, index++);
            
            std::list<std::pair<std::string, int64_t>> refreshReqList;
            
            if (statementHandle.executeSelect()) {
                ui.uid = db->getStringValue(statementHandle, 0);
                ui.name = db->getStringValue(statementHandle, 1);
                ui.displayName = db->getStringValue(statementHandle, 2);
                ui.portrait = db->getStringValue(statementHandle, 3);
                ui.gender = db->getIntValue(statementHandle, 4);
                ui.mobile = db->getStringValue(statementHandle, 5);
                ui.email = db->getStringValue(statementHandle, 6);
                ui.address = db->getStringValue(statementHandle, 7);
                ui.company = db->getStringValue(statementHandle, 8);
                ui.social = db->getStringValue(statementHandle, 9);
                ui.extra = db->getStringValue(statementHandle, 10);
                ui.type = db->getIntValue(statementHandle, 11);
                ui.updateDt = db->getBigIntValue(statementHandle, 12);
                ui.friendAlias = db->getStringValue(statementHandle, 13);
                if (!groupId.empty()) {
                    ui.groupAlias = db->getStringValue(statementHandle, 14);
                }
            } else {
                ui.updateDt = 0;
            }
            
            if (refresh || ui.uid.empty()) {
                std::list<std::pair<std::string, int64_t>> reqList;
                reqList.push_back(std::pair<std::string, int64_t>(userId, ui.updateDt));
                reloadUserInfoFromRemote(reqList);
            }
            return ui;
        }
        std::list<TUserInfo> MessageDB::getUserInfos(const std::list<std::string> &userIds, const std::string &groupId) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return std::list<TUserInfo>();
            }
            
            std::string sql;
            std::string uidStr;
            for (std::list<std::string>::const_iterator it = userIds.begin(); it != userIds.end();) {
                std::string userId = *it;
                uidStr += "'";
                uidStr += userId;
                uidStr += "'";
                ++it;
                if (it != userIds.end()) {
                    uidStr += ",";
                }
            }
            if (groupId.empty()) {
                sql = "select l._uid,l._name,l._display_name,l._portrait,l._gender,l._mobile,l._email,l._address,l._company,l._social,l._extra,l._type,l._update_dt,m._alias from t_user as l left join t_friend as m on l._uid=m._friend_uid where l._uid in (" + uidStr + ")";
            } else {
                sql = "select l._uid,l._name,l._display_name,l._portrait,l._gender,l._mobile,l._email,l._address,l._company,l._social,l._extra,l._type,l._update_dt,m._alias,r._alias from t_user as l left join t_friend as m on l._uid=m._friend_uid left join (select _mid, _alias from t_group_member where _gid = ?) as r  on l._uid=r._mid where l._uid in (" + uidStr + ")";
            }
            
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<TUserInfo>();;
            }
            
            int index = 1;
            if (!groupId.empty()) {
                db->Bind(statementHandle, groupId, index++);
            }
            
            std::list<TUserInfo> out;
            
            std::list<std::string> needRefreshList = userIds;
            while (statementHandle.executeSelect()) {
                TUserInfo ui;
                ui.uid = db->getStringValue(statementHandle, 0);
                ui.name = db->getStringValue(statementHandle, 1);
                ui.displayName = db->getStringValue(statementHandle, 2);
                ui.portrait = db->getStringValue(statementHandle, 3);
                ui.gender = db->getIntValue(statementHandle, 4);
                ui.mobile = db->getStringValue(statementHandle, 5);
                ui.email = db->getStringValue(statementHandle, 6);
                ui.address = db->getStringValue(statementHandle, 7);
                ui.company = db->getStringValue(statementHandle, 8);
                ui.social = db->getStringValue(statementHandle, 9);
                ui.extra = db->getStringValue(statementHandle, 10);
                ui.type = db->getIntValue(statementHandle, 11);
                ui.updateDt = db->getBigIntValue(statementHandle, 12);
                ui.friendAlias = db->getStringValue(statementHandle, 13);
                if (!groupId.empty()) {
                    ui.groupAlias = db->getStringValue(statementHandle, 14);
                }
                out.insert(out.end(), ui);
                needRefreshList.remove(ui.uid);
            }
            
            if (!needRefreshList.empty()) {
                std::list<std::pair<std::string, int64_t>> reqList;
                for (std::list<std::string>::iterator it = needRefreshList.begin(); it != needRefreshList.end(); ++it) {
                    if (reqList.size() < 500) {
                        reqList.push_back(std::pair<std::string, int64_t>(*it, 0));
                    }
                    
                    TUserInfo ui;
                    ui.uid = *it;
                    out.insert(out.end(), ui);
                }
                reloadUserInfoFromRemote(reqList);
            }
            return out;
        }
        /*
         Modify_DisplayName = 0,
         Modify_Portrait = 1,
         Modify_Gender = 2,
         Modify_Mobile = 3,
         Modify_Email = 4,
         Modify_Address = 5,
         Modify_Company = 6,
         Modify_Social = 7,
         Modify_Extra = 8,
         */
        long MessageDB::UpdateMyInfo(const std::list<std::pair<int, std::string>> &infos) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return 0L;
            }
            
            std::list<std::string> columns;
            for (std::list<std::pair<int, std::string>>::const_iterator it = infos.begin(); it != infos.end(); it++) {
                switch (it->first) {
                    case 0:
                        columns.push_back("_display_name");
                        break;
                    case 1:
                        columns.push_back("_portrait");
                        break;
                    case 2:
                        columns.push_back("_gender");
                        break;
                    case 3:
                        columns.push_back("_mobile");
                        break;
                    case 4:
                        columns.push_back("_email");
                        break;
                    case 5:
                        columns.push_back("_address");
                        break;
                    case 6:
                        columns.push_back("_company");
                        break;
                    case 7:
                        columns.push_back("_social");
                        break;
                    case 8:
                        columns.push_back("_extra");
                        break;
                    default:
                        break;
                }
            }
            
            if (columns.size() == 0) {
                return 0L;
            }
            
            std::string sql = db->GetUpdateSql(USER_TABLE_NAME, columns, "_uid=?");
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            int index = 1;
            for (std::list<std::pair<int, std::string>>::const_iterator it = infos.begin(); it != infos.end(); it++) {
                switch (it->first) {
                    case 0:
                    case 1:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        db->Bind(statementHandle, it->second, index++);
                        break;
                    case 2: {
                        int intValue = atoi(it->second.c_str());
                        db->Bind(statementHandle, intValue, index++);
                    }
                        break;
                    default:
                        break;
                }
            }
            
            db->Bind(statementHandle, app::GetAccountInfo().username, index++);
            return db->ExecuteUpdate(statementHandle);
        }
        
        long MessageDB::InsertUserInfoOrReplace(const TUserInfo &userInfo) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return 0L;
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_uid");
            columns.push_back("_name");
            columns.push_back("_display_name");
            columns.push_back("_portrait");
            columns.push_back("_gender");
            columns.push_back("_mobile");
            columns.push_back("_email");
            columns.push_back("_address");
            columns.push_back("_company");
            columns.push_back("_social");
            columns.push_back("_extra");
            columns.push_back("_type");
            columns.push_back("_update_dt");
            std::string sql = db->GetInsertSql(USER_TABLE_NAME, columns, true);
#else
            std::string sql = db->GetInsertSql(USER_TABLE_NAME, {"_uid",  "_name", "_display_name", "_portrait", "_gender", "_mobile", "_email", "_address", "_company", "_social", "_extra", "_type", "_update_dt"}, true);
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            db->Bind(statementHandle, userInfo.uid, 1);
            db->Bind(statementHandle, userInfo.name, 2);
            db->Bind(statementHandle, userInfo.displayName, 3);
            db->Bind(statementHandle, userInfo.portrait, 4);
            db->Bind(statementHandle, userInfo.gender, 5);
            
            db->Bind(statementHandle, userInfo.mobile, 6);
            db->Bind(statementHandle, userInfo.email, 7);
            db->Bind(statementHandle, userInfo.address, 8);
            db->Bind(statementHandle, userInfo.company, 9);
            db->Bind(statementHandle, userInfo.social, 10);

            db->Bind(statementHandle, userInfo.extra, 11);
            db->Bind(statementHandle, userInfo.type, 12);
            db->Bind(statementHandle, userInfo.updateDt, 13);
            
            long ret = 0;
            ret = db->ExecuteInsert(statementHandle, &ret);
            return ret;
        }
        bool MessageDB::isMyFriend(const std::string &userId) {
            DB2 *db = DB2::Instance();

            if (!db->isOpened()) {
                return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_id");
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, columns, "_friend_uid=? and _state=?");
#else
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, {"_id"}, "_friend_uid=? and _state=?");
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(statementHandle, userId, 1);
            db->Bind(statementHandle, 0, 2);
            std::list<std::pair<std::string, int64_t>> refreshReqList;
            
            if (statementHandle.executeSelect()) {
                return true;
            } else {
                return false;
            }
        }
        
        bool MessageDB::isBlackListed(const std::string &userId) {
            DB2 *db = DB2::Instance();
            
            if (!db->isOpened()) {
                return false;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_id");
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, columns, "_friend_uid=? and _state=2");
#else
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, {"_id"}, "_friend_uid=? and _state=2");
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return false;
            }
            
            db->Bind(statementHandle, userId, 1);
            std::list<std::pair<std::string, int64_t>> refreshReqList;
            
            if (statementHandle.executeSelect()) {
                return true;
            } else {
                return false;
            }
        }
        
        std::list<std::string> MessageDB::getMyFriendList(bool refresh) {
            DB2 *db = DB2::Instance();

            if (!db->isOpened()) {
                return std::list<std::string>();
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_friend_uid");
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, columns, "_state=?");
#else
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, {"_friend_uid"}, "_state=?");
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<std::string>();
            }
            
            db->Bind(statementHandle, 0, 1);
            std::list<std::string> result;
            
            while(statementHandle.executeSelect()) {
                result.push_back(db->getStringValue(statementHandle, 0));
            }
            
            if (result.empty() || refresh) {
                loadFriendFromRemote();
            }
            return result;
        }
        
        std::list<std::string> MessageDB::getBlackList(bool refresh) {
            DB2 *db = DB2::Instance();
            
            if (!db->isOpened()) {
                return std::list<std::string>();
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_friend_uid");
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, columns, "_state=2");
#else
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, {"_friend_uid"}, "_state=2");
#endif

            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::list<std::string>();
            }
            
            std::list<std::string> result;            
            while(statementHandle.executeSelect()) {
                result.push_back(db->getStringValue(statementHandle, 0));
            }
            
            if (result.empty() || refresh) {
                loadFriendFromRemote();
            }
            return result;
        }
        
        std::string MessageDB::GetFriendAlias(const std::string &friendId) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return std::string();
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_alias");
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, columns, "_friend_uid=?");
#else
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, {"_alias"}, "_friend_uid=?");
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return std::string();
            }
            
            std::string alias;
            db->Bind(statementHandle, friendId, 1);
            if(statementHandle.executeSelect()) {
                alias = db->getStringValue(statementHandle, 0);
            }
            
            return alias;
        }
        
        int64_t MessageDB::getFriendRequestHead() {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return INT_MAX;
            }
            
            int64_t maxTS = 0;
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("max(_update_dt)");
            std::string sql = db->GetSelectSql(FRIEND_REQUEST_TABLE_NAME, columns);
#else
            std::string sql = db->GetSelectSql(FRIEND_REQUEST_TABLE_NAME, {"max(_update_dt)"});
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return INT_MAX;
            }
            
            
            while(statementHandle.executeSelect()) {
                maxTS = db->getBigIntValue(statementHandle, 0);
            }
            
            return maxTS;
        }
        
        int64_t MessageDB::getFriendHead() {
            DB2 *db = DB2::Instance();

            
            if (!db->isOpened()) {
                return INT_MAX;
            }
            
            int64_t maxTS = 0;
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("max(_update_dt)");
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, columns);
#else
            std::string sql = db->GetSelectSql(FRIEND_TABLE_NAME, {"max(_update_dt)"});
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return INT_MAX;
            }
            
            while(statementHandle.executeSelect()) {
                maxTS = db->getBigIntValue(statementHandle, 0);
            }
            
            return maxTS;

        }
        long MessageDB::InsertFriendRequestOrReplace(const TFriendRequest &friendRequest) {
            DB2 *db = DB2::Instance();

            if (!db->isOpened()) {
                return 0L;
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_direction");
            columns.push_back("_target_uid");
            columns.push_back("_reason");
            columns.push_back("_status");
            columns.push_back("_read_status");
            columns.push_back("_update_dt");
            std::string sql = db->GetInsertSql(FRIEND_REQUEST_TABLE_NAME, columns, true);
#else
            std::string sql = db->GetInsertSql(FRIEND_REQUEST_TABLE_NAME, {"_direction", "_target_uid", "_reason", "_status", "_read_status", "_update_dt"}, true);
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return 0L;
            }
            
            db->Bind(statementHandle, friendRequest.direction, 1);
            db->Bind(statementHandle, friendRequest.target, 2);
            db->Bind(statementHandle, friendRequest.reason, 3);
            db->Bind(statementHandle, friendRequest.status, 4);
            
            db->Bind(statementHandle, friendRequest.readStatus, 5);
            db->Bind(statementHandle, friendRequest.timestamp, 6);
            
            long ret = 0;
            ret = db->ExecuteInsert(statementHandle, &ret);
            return ret;
        }
        
        long MessageDB::InsertFriendOrReplace(const std::string &friendUid, int state, int64_t timestamp, const std::string &alias) {
            DB2 *db = DB2::Instance();

            if (!db->isOpened()) {
                return 0L;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_friend_uid");
            columns.push_back("_state");
            columns.push_back("_update_dt");
            columns.push_back("_alias");
            std::string sql = db->GetInsertSql(FRIEND_TABLE_NAME, columns, true);
#else
            std::string sql = db->GetInsertSql(FRIEND_TABLE_NAME, {"_friend_uid", "_state", "_update_dt", "_alias"}, true);
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return 0L;
            }
            
            db->Bind(statementHandle, friendUid, 1);
            db->Bind(statementHandle, state, 2);
            db->Bind(statementHandle, timestamp, 3);
            db->Bind(statementHandle, alias, 4);
            
            long ret = 0;
            ret = db->ExecuteInsert(statementHandle, &ret);
            return ret;
        }
        
        std::list<TFriendRequest> MessageDB::getFriendRequest(int direction) {
            std::list<TFriendRequest> requests;
            
            DB2 *db = DB2::Instance();

            if (!db->isOpened()) {
                return requests;
            }

#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_target_uid");
            columns.push_back("_reason");
            columns.push_back("_status");
            columns.push_back("_read_status");
            columns.push_back("_update_dt");
            std::string sql = db->GetSelectSql(FRIEND_REQUEST_TABLE_NAME, columns, "_direction=?", "_id desc");
#else
            std::string sql = db->GetSelectSql(FRIEND_REQUEST_TABLE_NAME, {"_target_uid", "_reason", "_status", "_read_status", "_update_dt"}, "_direction=?", "_id desc");
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return requests;
            }
            
            db->Bind(statementHandle, direction, 1);
            std::list<std::string> result;
            
            while(statementHandle.executeSelect()) {
                TFriendRequest request;
                request.target = db->getStringValue(statementHandle, 0);
                request.reason = db->getStringValue(statementHandle, 1);
                request.status = db->getIntValue(statementHandle, 2);
                request.readStatus = db->getIntValue(statementHandle, 3);
                request.timestamp = db->getBigIntValue(statementHandle, 4);
                requests.push_back(request);
            }
            
            return requests;
        }
        
        int MessageDB::unreadFriendRequest() {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return 0;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("count(*)");
            std::string sql = db->GetSelectSql(FRIEND_REQUEST_TABLE_NAME, columns, "_direction=1 and _read_status = 0");
#else
            std::string sql = db->GetSelectSql(FRIEND_REQUEST_TABLE_NAME, {"count(*)"}, "_direction=1 and _read_status = 0");
#endif
            

            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return 0;
            }
            
            if(statementHandle.executeSelect()) {
                return db->getIntValue(statementHandle, 0);
            }

            return 0;
        }

        int64_t MessageDB::getUnreadFriendRequestMaxDt() {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return 0;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("max(_update_dt)");
            std::string sql = db->GetSelectSql(FRIEND_REQUEST_TABLE_NAME, columns, "_direction=1 and _read_status = 0");
#else
            std::string sql = db->GetSelectSql(FRIEND_REQUEST_TABLE_NAME, {"max(_update_dt)"}, "_direction=1 and _read_status = 0");
#endif
            
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return 0;
            }
            
            
            if(statementHandle.executeSelect()) {
                return db->getBigIntValue(statementHandle, 0);
            }
            
            return 0;
        }
        void MessageDB::clearUnreadFriendRequestStatus() {
            DB2 *db = DB2::Instance();

            if (!db->isOpened()) {
                return;
            }
            int64_t maxDt = getUnreadFriendRequestMaxDt();
            if (maxDt == 0) {
                return;
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_read_status");
            std::string sql = db->GetUpdateSql(FRIEND_REQUEST_TABLE_NAME, columns, "_direction=1 and _read_status=0");
#else
            std::string sql = db->GetUpdateSql(FRIEND_REQUEST_TABLE_NAME, {"_read_status"}, "_direction=1 and _read_status=0");
#endif
            
            int error = 0;
            RecyclableStatement updateStatementHandle(db, sql, error);
            if (error != 0) {
                return;
            }
            
            db->Bind(updateStatementHandle, 1, 1);
            int count = db->ExecuteUpdate(updateStatementHandle);
            if (count > 0) {
                clearFriendRequestUnread(maxDt);
            }
            return;
        }
        
        extern void reloadChannelInfoFromRemote(const std::string &channelId, int64_t updateDt, GetChannelInfoCallback *callback);
        TChannelInfo MessageDB::GetChannelInfo(const std::string &channelId, bool refresh) {
            DB2 *db = DB2::Instance();
            
            TChannelInfo ci;
            if (!db->isOpened()) {
                return ci;
            }
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_name");
            columns.push_back("_portrait");
            columns.push_back("_owner");
            columns.push_back("_status");
            columns.push_back("_desc");
            columns.push_back("_extra");
            columns.push_back("_secret");
            columns.push_back("_callback");
            columns.push_back("_update_dt");
            std::string sql = db->GetSelectSql(CHANNEL_TABLE_NAME, columns, "_uid=?");
#else
            std::string sql = db->GetSelectSql(CHANNEL_TABLE_NAME, {"_name",  "_portrait", "_owner", "_status", "_desc", "_extra", "_secret", "_callback", "_update_dt"}, "_uid=?");
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return ci;
            }
            
            db->Bind(statementHandle, channelId, 1);
            ci.channelId = channelId;
            
            if (statementHandle.executeSelect()) {
                ci.name = db->getStringValue(statementHandle, 0);
                ci.portrait = db->getStringValue(statementHandle, 1);
                ci.owner = db->getStringValue(statementHandle, 2);
                ci.status = db->getIntValue(statementHandle, 3);
                ci.desc = db->getStringValue(statementHandle, 4);
                ci.extra = db->getStringValue(statementHandle, 5);
                ci.secret = db->getStringValue(statementHandle, 6);
                ci.callback = db->getStringValue(statementHandle, 7);
                ci.updateDt = db->getBigIntValue(statementHandle, 8);
                
            } else {
                ci.channelId = "";//empty
                ci.updateDt = 0;
            }
            
            if (refresh || ci.channelId.empty()) {
                reloadChannelInfoFromRemote(channelId, ci.updateDt, new TGetChannelInfoCallback());
            }
            return ci;
        }
        
        long MessageDB::InsertOrUpdateChannelInfo(const TChannelInfo &channelInfo) {
            DB2 *db = DB2::Instance();
            if (!db->isOpened()) {
                return 0L;
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_uid");
            columns.push_back("_name");
            columns.push_back("_portrait");
            columns.push_back("_owner");
            columns.push_back("_status");
            columns.push_back("_desc");
            columns.push_back("_extra");
            columns.push_back("_secret");
            columns.push_back("_callback");
            columns.push_back("_update_dt");
            std::string sql = db->GetInsertSql(CHANNEL_TABLE_NAME, columns, true);
#else
            std::string sql = db->GetInsertSql(CHANNEL_TABLE_NAME, {"_uid",  "_name",  "_portrait", "_owner", "_status", "_desc", "_extra", "_secret", "_callback", "_update_dt"}, true);
#endif
            int error = 0;
            RecyclableStatement statementHandle(db, sql, error);
            if (error != 0) {
                return 0L;
            }
            
            db->Bind(statementHandle, channelInfo.channelId, 1);
            db->Bind(statementHandle, channelInfo.name, 2);
            db->Bind(statementHandle, channelInfo.portrait, 3);
            db->Bind(statementHandle, channelInfo.owner, 4);
            db->Bind(statementHandle, channelInfo.status, 5);
            db->Bind(statementHandle, channelInfo.desc, 6);
            db->Bind(statementHandle, channelInfo.extra, 7);
            db->Bind(statementHandle, channelInfo.secret, 8);
            db->Bind(statementHandle, channelInfo.callback, 9);
            db->Bind(statementHandle, channelInfo.updateDt, 10);
            
            long ret = 0;
            return db->ExecuteInsert(statementHandle, &ret);
        }
    }
}
