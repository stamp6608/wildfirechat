//
//  DB2.cpp
//  proto
//
//  Created by WF Chat on 2017/11/18.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "DB2.h"
#include "mars/app/app.h"
#include "mars/comm/xlogger/xlogger.h"
#include "business.h"
#include "boost/filesystem.hpp"

namespace mars {
    namespace stn {
        
        const char* getError(sqlite3 *db) {
            if (db) {
                return sqlite3_errmsg(db);
            }
            return "";
        }
        RecyclableStatement::RecyclableStatement(DB2 *db2, const std::string &sql, int &error) : m_db(db2->m_db), m_stmt(NULL), m_sql(sql) {
            if (m_db == NULL) {
                error = -1;
                return;
            }
            
            error = sqlite3_prepare_v2(m_db, m_sql.c_str(), -1, &m_stmt, NULL);
            if (error != SQLITE_OK)
            {
                xerror2("prepare db error:%d info:%s sql:%s", error, getError(m_db), m_sql.c_str());
                error = sqlite3_finalize(m_stmt);
                m_stmt = NULL;
                return ;
            }
            error = 0;
        }
        
        RecyclableStatement::RecyclableStatement(sqlite3 *db, const std::string &sql, int &error) : m_db(db), m_stmt(NULL), m_sql(sql) {
            if (m_db == NULL) {
                error = -1;
                return;
            }
            
            error = sqlite3_prepare_v2(m_db, m_sql.c_str(), -1, &m_stmt, NULL);
            if (error != SQLITE_OK)
            {
                xerror2("prepare db error:%d info:%s sql:%s", error, getError(m_db), m_sql.c_str());
                error = sqlite3_finalize(m_stmt);
                m_stmt = NULL;
                return ;
            }
            error = 0;
        }

            bool RecyclableStatement::executeSelect() {
                int error = sqlite3_step(m_stmt);
                if (error == SQLITE_ROW) {
                    return true;
                }
                if (error != SQLITE_OK && error != SQLITE_DONE) {
                    xerror2("sql select error:%d, errorInfo:%s", error, getError(m_db));
                }
                return false;
            }
            
            bool RecyclableStatement::executeInsert(long *rowId) {
                int error = sqlite3_step(m_stmt);
                long lid = (long)sqlite3_last_insert_rowid(m_db);
                if (rowId) {
                    *rowId = lid;
                }
                if (error != SQLITE_DONE) {
                    xerror2("sql select error:%d, errorInfo:%s", error, getError(m_db));
                }

                return error == SQLITE_DONE;
            }
            
            bool RecyclableStatement::executeDelete(long *changes) {
                int error = sqlite3_step(m_stmt);
                long lid = sqlite3_changes(m_db);
                if (changes) {
                    *changes = lid;
                }
                if (error != SQLITE_DONE) {
                    xerror2("sql select error:%d, errorInfo:%s", error, getError(m_db));
                }

                return error == SQLITE_DONE;
            }
            
            bool RecyclableStatement::executeUpdate(long *changes) {
                int error = sqlite3_step(m_stmt);
                long lid = sqlite3_changes(m_db);
                if (changes) {
                    *changes = lid;
                }
                
                if (error != SQLITE_DONE) {
                    xerror2("sql select error:%d, errorInfo:%s", error, getError(m_db));
                }

                return error == SQLITE_DONE;
            }
            
            int RecyclableStatement::bind(int icol, int value)
            {
                if (!m_stmt)
                {
                    return -1;
                }
                return sqlite3_bind_int(m_stmt, icol, value);
            }
#if !ANDROID || !__LP64__
            int RecyclableStatement::bind(int icol, long value)
            {
                if (!m_stmt)
                {
                    return -1;
                }
                return sqlite3_bind_int(m_stmt, icol, (int)value);
            }
#endif
            int RecyclableStatement::bind(int icol, int64_t value) {
                if (!m_stmt) {
                    return -1;
                }
                
                return sqlite3_bind_int64(m_stmt, icol, value);
            }
            
            int RecyclableStatement::bind(int icol, float value) {
                if (!m_stmt) {
                    return -1;
                }
                
                return sqlite3_bind_double(m_stmt, icol, value);
            }
            
            
            int RecyclableStatement::bind(int icol, const char* value) {
                if (!m_stmt)
                {
                    return -1;
                }
                if (value)
                    return sqlite3_bind_text(m_stmt, icol, value, -1, SQLITE_STATIC);
                else
                    return sqlite3_bind_null(m_stmt, icol);
            }
            
            int RecyclableStatement::bind(int icol, const std::string& value) {
                if (!m_stmt)
                {
                    return -1;
                }
                return sqlite3_bind_text(m_stmt, icol, value.c_str(), -1, SQLITE_STATIC);
            }
            
            int RecyclableStatement::bind(int icol, const void* value, int size) {
                if (!m_stmt)
                {
                    return -1;
                }
                return sqlite3_bind_blob(m_stmt, icol, value, size, SQLITE_STATIC);
            }
            
            float RecyclableStatement::getFloatValue(int icol)  const {
                if (!m_stmt) {
                    return -1;
                }
                return sqlite3_column_double(m_stmt, icol);
            }
            
            int RecyclableStatement::getIntValue(int icol)  const {
                if (!m_stmt) {
                    return -1;
                }
                return sqlite3_column_int(m_stmt, icol);
            }
            
            int64_t RecyclableStatement::getInt64Value(int icol)  const {
                if (!m_stmt) {
                    return -1;
                }
                return sqlite3_column_int64(m_stmt, icol);
            }
            
            std::string RecyclableStatement::getStringValue(int icol)  const {
                if (!m_stmt) {
                    return "";
                }
                char *str = (char *)sqlite3_column_text(m_stmt, icol);
                if (str == NULL) {
                    return "";
                }
                return std::string(str);
            }
            
            const void * RecyclableStatement::getBlobValue(int icol, int &size)  const {
                if (!m_stmt) {
                    size = 0;
                    return NULL;
                }
                size = sqlite3_column_bytes(m_stmt, icol);
                return sqlite3_column_blob(m_stmt, icol);
            }
            
            RecyclableStatement::~RecyclableStatement() {
                if (m_stmt) {
                    int rc = SQLITE_OK;
                    if((rc = sqlite3_finalize(m_stmt)) != SQLITE_OK) {
                        xerror2("finalize_db err:%s, code:%d", getError(m_db), rc);
                    }
                    m_stmt = NULL;
                }
            }
        
        static const std::string DB2_NAME = "data";
        
        const std::string VERSION_TABLE_NAME = "t_version";
        const std::string VERSION_COLUMN_VERSION = "_version";
        
        const std::string MESSAGE_TABLE_NAME = "t_message";
        const std::string USER_TABLE_NAME = "t_user";
        const std::string GROUP_TABLE_NAME = "t_group";
        const std::string CHANNEL_TABLE_NAME = "t_channel";
        const std::string GROUP_MEMBER_TABLE_NAME = "t_group_member";
        const std::string TIMELINE_TABLE_NAME = "t_timeline";
        const std::string USER_SETTING_TABLE_NAME = "t_user_setting";
        const std::string CONVERSATION_TABLE_NAME = "t_conversation";
        
        const std::string FRIEND_TABLE_NAME = "t_friend";
        const std::string FRIEND_REQUEST_TABLE_NAME = "t_friend_request";
        
        DB2* DB2::instance_ = NULL;
        
        DB2* DB2::Instance() {
            if(instance_ == NULL) {
                instance_ = new DB2();
            }
            
            return instance_;
        }
        
        DB2::DB2() : opened(false), m_db(NULL) {
            
        }
        
        DB2::~DB2() {
            
        }
        
        bool DB2::isOpened() {
            return opened;
        }
        
        void DB2::Open(const std::string &sec) {
            secret = sec;
            
            std::string path = app::GetAppFilePath() + "/" + app::GetDeviceInfo().clientid;
            std::string DB2Path = path + "/" + DB2_NAME;
            if(!boost::filesystem::exists(path)) {
                boost::filesystem::create_directories(path);
                std::string oldDB = app::GetAppFilePath() + "/" + DB2_NAME;
                if (boost::filesystem::exists(oldDB)) {
                    boost::filesystem::copy(oldDB, DB2Path);
                    boost::filesystem::remove(oldDB);
                }
            }
            
            xerror2("open db %s",DB2Path.c_str());
            
            closeDB();
            sqlite3_shutdown();
            int rc = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
            if (rc != SQLITE_OK)
            {
                xerror2("config db error %d,ver:%s",rc,sqlite3_libversion());
                return;
            }
            rc = sqlite3_config(SQLITE_CONFIG_MEMSTATUS, 0);
            if(rc != SQLITE_OK)
            {
                xerror2("config db error %d,ver:%s",rc,sqlite3_libversion());
                return;
            }
            
            rc = sqlite3_open_v2(DB2Path.c_str(), &m_db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_CREATE, NULL);
            if(rc != SQLITE_OK)
            {
                xerror2("open error:%s error:%d",getError(m_db), rc);
                closeDB();
                return;
            }
            rc = sqlite3_key(m_db, sec.c_str(), int(sec.size()));
            if (rc != SQLITE_OK) {
                xerror2("open error:%s error:%d",getError(m_db), rc);
                closeDB();
                return;
            }
            
            xerror2("open db done");
            opened = true;
        }
        
        void DB2::closeDB() {
            if (m_db)
            {
                if (sqlite3_close(m_db) != SQLITE_OK)
                {
                    xerror2("close error:%s",getError(m_db));
                }
                m_db = NULL;
            }
        }
      
#ifdef __ANDROID__
      std::string DB2::GetSelectSqlEx(const std::string &tableNameLeft, const std::list<std::string> &columnsLeft, const std::string &tableNameRight, const std::list<std::string> &columnsRight, const std::string &where, const std::string &orderBy, int limit, int offset, const std::string &groupBy)
#else
      std::string DB2::GetSelectSqlEx(const std::string &tableNameLeft, const std::initializer_list<std::string> &columnsLeft, const std::string &tableNameRight, const std::initializer_list<std::string> &columnsRight, const std::string &where, const std::string &orderBy, int limit, int offset, const std::string &groupBy)
#endif
      {
        
        std::string sql = "select ";
#ifdef __ANDROID__
        for (std::list<std::string>::const_iterator it = columnsLeft.begin(); it != columnsLeft.end(); ++it)
#else
          for (std::initializer_list<std::string>::const_iterator it = columnsLeft.begin(); it != columnsLeft.end(); ++it)
#endif
          {
            sql += "l.";
            sql += *it;
            sql += ",";
          }
        
        
#ifdef __ANDROID__
        for (std::list<std::string>::const_iterator it = columnsRight.begin(); it != columnsRight.end(); ++it)
#else
          for (std::initializer_list<std::string>::const_iterator it = columnsRight.begin(); it != columnsRight.end(); ++it)
#endif
          {
            sql += "r.";
            sql += *it;
            sql += ",";
          }
        
          sql = sql.substr(0, sql.length() - 1); //remove last ","
        
        
        sql += " from ";
        sql += tableNameLeft;
        sql += " l, ";
        sql += tableNameRight;
        sql += " r ";
        
        if (where.size()) {
          sql += " where ";
          sql += where;
        }
        
        
        if (groupBy.length()) {
          sql += " group by ";
          sql += groupBy;
        }
        
        if (orderBy.size()) {
          sql += " order by ";
          sql += orderBy;
        }
        
        if (offset > 0) {
          sql += " offset ";
          sql += offset;
        }
        
        if (limit > 0) {
          sql += " limit ";
          char str[256];
          memset(str, 0, 256);
          sprintf(str, "%d", limit);
          sql += str;
        }
        
          return sql;
      }

#ifdef __ANDROID__
        std::string DB2::GetSelectSqlEx2(const std::string &tableNameLeft, const std::list<std::string> &columnsLeft, const std::string &tableNameMiddle, const std::list<std::string> &columnsMiddle, const std::string &tableNameRight, const std::list<std::string> &columnsRight, const std::string &where,const std::string &orderBy, int limit, int offset, const std::string &groupBy)
#else
        std::string DB2::GetSelectSqlEx2(const std::string &tableNameLeft, const std::initializer_list<std::string> &columnsLeft, const std::string &tableNameMiddle, const std::initializer_list<std::string> &columnsMiddle, const std::string &tableNameRight, const std::initializer_list<std::string> &columnsRight, const std::string &where, const std::string &orderBy, int limit, int offset, const std::string &groupBy)
#endif
        {
            
            std::string sql = "select ";
#ifdef __ANDROID__
            for (std::list<std::string>::const_iterator it = columnsLeft.begin(); it != columnsLeft.end(); ++it)
#else
                for (std::initializer_list<std::string>::const_iterator it = columnsLeft.begin(); it != columnsLeft.end(); ++it)
#endif
                {
                    sql += "l.";
                    sql += *it;
                    sql += ",";
                }
   
#ifdef __ANDROID__
            for (std::list<std::string>::const_iterator it = columnsMiddle.begin(); it != columnsMiddle.end(); ++it)
#else
                for (std::initializer_list<std::string>::const_iterator it = columnsMiddle.begin(); it != columnsMiddle.end(); ++it)
#endif
                {
                    sql += "m.";
                    sql += *it;
                    sql += ",";
                }
            
            
            if (!tableNameRight.empty()) {
#ifdef __ANDROID__
                for (std::list<std::string>::const_iterator it = columnsRight.begin(); it != columnsRight.end(); ++it) {
#else
                for (std::initializer_list<std::string>::const_iterator it = columnsRight.begin(); it != columnsRight.end(); ++it) {
#endif
                    sql += "r.";
                    sql += *it;
                    sql += ",";
                }
            }
            
            sql = sql.substr(0, sql.length() - 1); //remove last ","
            
            
            sql += " from ";
            sql += tableNameLeft;
            sql += " l, ";
            sql += tableNameMiddle;
            sql += " m";
                
            if (!tableNameRight.empty()) {
                sql += ", ";
                sql += tableNameRight;
                sql += " r";
            }
            
            if (where.size()) {
                sql += " where ";
                sql += where;
            }
            
            
            if (groupBy.length()) {
                sql += " group by ";
                sql += groupBy;
            }
            
            if (orderBy.size()) {
                sql += " order by ";
                sql += orderBy;
            }
            
            if (offset > 0) {
                sql += " offset ";
                sql += offset;
            }
            
            if (limit > 0) {
                sql += " limit ";
                char str[256];
                memset(str, 0, 256);
                sprintf(str, "%d", limit);
                sql += str;
            }
            
            return sql;
        }

#ifdef __ANDROID__
        std::string DB2::GetSelectSql(const std::string &tableName, const std::list<std::string> &columns, const std::string &where, const std::string &orderBy, int limit, int offset, const std::string &groupBy)
#else
        std::string DB2::GetSelectSql(const std::string &tableName, const std::initializer_list<std::string> &columns, const std::string &where, const std::string &orderBy, int limit, int offset, const std::string &groupBy)
#endif
        {
            std::string sql = "select ";
#ifdef __ANDROID__
            for (std::list<std::string>::const_iterator it = columns.begin(); it != columns.end(); ++it)
#else
                for (std::initializer_list<std::string>::const_iterator it = columns.begin(); it != columns.end(); ++it)
#endif
                {
                    sql += *it;
                    sql += ",";
                }
            
            sql = sql.substr(0, sql.length() - 1); //remove last ","
            
            sql += " from ";
            sql += tableName;
            
            if (where.size()) {
                sql += " where ";
                sql += where;
            }
            
            
            if (groupBy.length()) {
                sql += " group by ";
                sql += groupBy;
            }
            
            if (orderBy.size()) {
                sql += " order by ";
                sql += orderBy;
            }
            
            if (offset > 0) {
                sql += " offset ";
                sql += offset;
            }
            
            if (limit > 0) {
                sql += " limit ";
                char str[256];
                memset(str, 0, 256);
                sprintf(str, "%d", limit);
                sql += str;
            }
            return sql;
        }
        
#ifdef __ANDROID__
        std::string DB2::GetInsertSql(const std::string &table, const std::list<std::string> &columns, bool replace)
#else
        std::string DB2::GetInsertSql(const std::string &table, const std::initializer_list<std::string> &columns, bool replace)
#endif
        {
            
            std::string sql = "";
            if (replace) {
                sql = "replace into ";
            } else {
                sql = "insert into ";
            }
            
            sql += table;
            
            sql += "(";
#ifdef __ANDROID__
            for (std::list<std::string>::const_iterator it = columns.begin(); it != columns.end(); ++it)
#else
            for (std::initializer_list<std::string>::const_iterator it = columns.begin(); it != columns.end(); ++it)
#endif
            {
                sql += *it;
                sql += ",";
            }

            sql = sql.substr(0, sql.length() - 1); //remove last ","
            sql += ")";
            sql += " values(";
            
#ifdef __ANDROID__
            for (std::list<std::string>::const_iterator it = columns.begin(); it != columns.end(); ++it)
#else
            for (std::initializer_list<std::string>::const_iterator it = columns.begin(); it != columns.end(); ++it)
#endif
            {
                sql += "?";
                sql += ",";
            }
            
            sql = sql.substr(0, sql.length() - 1); //remove last ","
            sql += ")";
            
            return sql;
        }
        
        bool DB2::ExecuteInsert(RecyclableStatement &statementHandle, long *rowId) {
            return statementHandle.executeInsert(rowId);
        }
        
        std::string DB2::GetDeleteSql(const std::string &table, const std::string &where) {
            
            std::string sql = "delete from ";
            sql += table;
            
            if (where.length()) {
                sql += " where ";
                sql += where;
            }
            
            return sql;
        }
        
        int DB2::ExecuteDelete(RecyclableStatement &statementHandle) {
            long changes = 0;
            statementHandle.executeDelete(&changes);
            return (int)changes;
        }
#ifdef __ANDROID__
        std::string DB2::GetUpdateSql(const std::string &table, const std::list<std::string> &columns, const std::string &where)
#else
        std::string DB2::GetUpdateSql(const std::string &table, const std::initializer_list<std::string> &columns, const std::string &where)
#endif
        {
            
            std::string sql = "update ";
            
            sql += table;
            
            sql += " set ";
#ifdef __ANDROID__
            for (std::list<std::string>::const_iterator it = columns.begin(); it != columns.end(); ++it)
#else
            for (std::initializer_list<std::string>::const_iterator it = columns.begin(); it != columns.end(); ++it)
#endif
            {
                sql += *it;
                sql += "=?,";
            }
            
            sql = sql.substr(0, sql.length() - 1); //remove last ","
            
            if (where.length()) {
                sql += " where ";
                sql += where;
            }
            
            return sql;
        }
        
#ifndef __ANDROID__
        std::string DB2::GetUpdateSql(const std::string &table, const std::list<std::string> &columns, const std::string &where)
        {
            
            std::string sql = "update ";
            
            sql += table;
            
            sql += " set ";
            for (std::list<std::string>::const_iterator it = columns.begin(); it != columns.end(); ++it)
                {
                    sql += *it;
                    sql += "=?,";
                }
            
            sql = sql.substr(0, sql.length() - 1); //remove last ","
            
            if (where.length()) {
                sql += " where ";
                sql += where;
            }
            
            return sql;
        }
#endif
        
        int DB2::ExecuteUpdate(RecyclableStatement &statementHandle) {
            long changes = 0;
            statementHandle.executeUpdate(&changes);
            return (int)changes;

        }
        void DB2::Bind(RecyclableStatement &statementHandle, float value, int index) {
            statementHandle.bind(index, value);
        }
        void DB2::Bind(RecyclableStatement &statementHandle, int value, int index) {
            statementHandle.bind(index, value);
        }
#if !ANDROID || !__LP64__
        void DB2::Bind(RecyclableStatement &statementHandle, long value, int index) {
            statementHandle.bind(index, value);
        }
#endif
        void DB2::Bind(RecyclableStatement &statementHandle, int64_t value, int index) {
            statementHandle.bind(index, value);
        }
        void DB2::Bind(RecyclableStatement &statementHandle, const std::string &value, int index) {
            statementHandle.bind(index, value);
        }
        void DB2::Bind(RecyclableStatement &statementHandle, const void *value, int size, int index) {
            statementHandle.bind(index, value, size);
        }
        
        float DB2::getFloatValue(const RecyclableStatement &statementHandle, int index) {
            return statementHandle.getFloatValue(index);
        }
        
        int DB2::getIntValue(const RecyclableStatement &statementHandle, int index) {
            return statementHandle.getIntValue(index);
        }
        
        int64_t DB2::getBigIntValue(const RecyclableStatement &statementHandle, int index) {
            return statementHandle.getInt64Value(index);
        }
        
        std::string DB2::getStringValue(const RecyclableStatement &statementHandle, int index) {
            return statementHandle.getStringValue(index);
        }
        
        const void* DB2::getBlobValue(const RecyclableStatement &statementHandle, int index, int &size) {
            return statementHandle.getBlobValue(index, size);
        }
        
        bool DB2::IsTableExist(std::string tableName)
        {
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("name");
            
            std::string sql = GetSelectSql("sqlite_master", columns, "type='table' AND name=?");
#else
            std::string sql = GetSelectSql("sqlite_master", {"name"}, "type='table' AND name=?");
#endif
            int error = 0;
            RecyclableStatement statement(m_db, sql, error);
            if (error != 0) {
                return false;
            }
            statement.bind(1, tableName);
            return statement.executeSelect();
        }

        
        void DB2::Upgrade() {
            if(!IsTableExist(VERSION_TABLE_NAME)) {
                CreateDB2Version1();
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back(VERSION_COLUMN_VERSION);
            
            std::string sql = GetSelectSql(VERSION_TABLE_NAME, columns, "", "_version desc ", 1);
#else
            std::string sql = GetSelectSql(VERSION_TABLE_NAME, {VERSION_COLUMN_VERSION}, "", "_version desc ", 1);
#endif
            int error = 0;
            RecyclableStatement statementHandle(m_db, sql, error);
            if (error != 0) {
                return;
            }
            
            if (statementHandle.executeSelect()) {
                int version = getIntValue(statementHandle, 0);
                if (version == 1) {
                    UpgradeDB2Version2();
                    version = 2;
                }
                
                if(version == 2) {
                    UpgradeDB2Version3();
                    version = 3;
                }
                
                if (version == 3) {
                    UpgradeDB3Version4();
                    version = 4;
                }
                
                if (version == 4) {
                    UpgradeDB4Version5();
                    version = 5;
                }
                
                if(version == 5) {
                    UpgradeDB5Version6();
                    version = 6;
                }
                
                if(version == 6) {
                    UpgradeDB6Version7();
                    version = 7;
                }
            }
        }
        
        bool DB2::executeSql(const std::string &sql) {
            sqlite3_stmt *stmt = NULL;
            bool ret = false;
            int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, NULL);
            if (rc == SQLITE_OK) {
                int rc = sqlite3_step(stmt);
                if (rc == SQLITE_DONE) {
                    ret = true;
                }
            }
            if((rc = sqlite3_finalize(stmt)) != SQLITE_OK) {
                
            }
            return ret;
        }
        bool DB2::UpgradeDB2Version2() {
            static const std::string createUserSettingTableSql = "CREATE TABLE IF NOT EXISTS t_user_setting(_id INTEGER PRIMARY KEY AUTOINCREMENT, _scope INTEGER NOT NULL, _key TEXT NOT NULL, _value TEXT NOT NULL, _update_dt INTEGER DEFAULT 0)";
            if (!executeSql(createUserSettingTableSql)) {
                return false;
            }
            
            static const std::string createUserSettingIndex = "CREATE UNIQUE INDEX IF NOT EXISTS user_setting_index ON t_user_setting(_scope, _key)";
            if (!executeSql(createUserSettingIndex)) {
                return false;
            }
            
            return SetDBVersion(2);
        }
        
        bool DB2::UpgradeDB2Version3() {
            //Sqlite does not support add multi column.
            static const std::string alertConversationTableSql1 = "ALTER TABLE t_conversation ADD COLUMN _read_dt INTEGER DEFAULT 0";
            if (!executeSql(alertConversationTableSql1)) {
                return false;
            }
            
            static const std::string alertConversationTableSql2 = "ALTER TABLE t_conversation ADD COLUMN _update_dt INTEGER DEFAULT 0";
            if (!executeSql(alertConversationTableSql2)) {
                return false;
            }
            
            return SetDBVersion(3);
        }
        
        bool DB2::UpgradeDB3Version4() {
            static const std::string alertMessageTableSql1 = "ALTER TABLE t_message ADD COLUMN _to TEXT DEFAULT ''";
            if (!executeSql(alertMessageTableSql1)) {
                return false;
            }
            
            return SetDBVersion(4);
        }
        
        bool DB2::UpgradeDB4Version5() {
            static const std::string createChannelTableSql = "CREATE TABLE IF NOT EXISTS t_channel(_id INTEGER PRIMARY KEY AUTOINCREMENT,_uid TEXT NOT NULL,_name TEXT,_portrait TEXT,_owner TEXT,_status INTEGER,_desc TEXT,_extra TEXT,_secret TEXT, _callback TEXT, _update_dt INTEGER DEFAULT 0)";
            if (!executeSql(createChannelTableSql)) {
                return false;
            }
            
            static const std::string createChannelUidIndexSql = "CREATE UNIQUE INDEX IF NOT EXISTS channel_uid_index ON t_channel(_uid)";
            if (!executeSql(createChannelUidIndexSql)) {
                return false;
            }
            
            return SetDBVersion(5);
        }
        
        bool DB2::UpgradeDB5Version6() {
            static const std::string createChannelTableSql = "ALTER TABLE t_friend ADD COLUMN _alias TEXT";
            if (!executeSql(createChannelTableSql)) {
                return false;
            }
            
            return SetDBVersion(6);
        }
        
            bool DB2::UpgradeDB6Version7() {
                static const std::string createMessageIndex1 = "CREATE INDEX IF NOT EXISTS message_index1 ON t_message(_conv_type, _conv_line=, _conv_target=, _status)";
                if (!executeSql(createMessageIndex1)) {
                    return false;
                }
                
                static const std::string createMessageIndex2 = "CREATE INDEX IF NOT EXISTS message_index2 ON t_message(_status)";
                if (!executeSql(createMessageIndex2)) {
                    return false;
                }
                
                static const std::string createMessageIndex3 = "CREATE INDEX IF NOT EXISTS message_index3 ON t_message(_uid)";
                if (!executeSql(createMessageIndex3)) {
                    return false;
                }
                
                static const std::string createMessageIndex4 = "CREATE INDEX IF NOT EXISTS message_index4 ON t_message(_timestamp)";
                if (!executeSql(createMessageIndex4)) {
                    return false;
                }
                
                return SetDBVersion(7);
            }
            
        
        
        bool DB2::CreateDB2Version1() {
            
            //create message table
            static const std::string createMessageTableSql = "CREATE TABLE IF NOT EXISTS t_message(_id INTEGER PRIMARY KEY AUTOINCREMENT,_conv_type INTEGER NOT NULL,_conv_target TEXT NOT NULL,_conv_line INTEGER NOT NULL,_from TEXT NOT NULL,_cont_type INTEGER NOT NULL,_cont_searchable TEXT DEFAULT NULL,_cont_push TEXT DEFAULT NULL,_cont TEXT DEFAULT NULL,_cont_data BLOB DEFAULT NULL,_cont_local TEXT DEFAULT NULL,_cont_media_type TEXT DEFAULT NULL,_cont_remote_media_url TEXT DEFAULT NULL,_cont_local_media_path TEXT DEFAULT NULL,_direction INTEGER DEFAULT 0,_status INTEGER DEFAULT 0,_uid INTEGER DEFAULT 0,_timestamp INTEGER DEFAULT 0)";
            if (!executeSql(createMessageTableSql)) {
                return false;
            }
            
            static const std::string createUserTableSql = "CREATE TABLE IF NOT EXISTS t_user(_id INTEGER PRIMARY KEY AUTOINCREMENT,_uid TEXT NOT NULL,_name TEXT,_display_name TEXT NOT NULL,_portrait TEXT,_gender INTEGER,_mobile TEXT,_email TEXT,_address TEXT,_company TEXT,_social TEXT,_type INTEGER DEFAULT 0,_extra TEXT,_update_dt INTEGER DEFAULT 0)";
            if (!executeSql(createUserTableSql)) {
                return false;
            }
            
            static const std::string createUserUidIndex = "CREATE UNIQUE INDEX IF NOT EXISTS user_uid_index ON t_user(_uid)";
            if (!executeSql(createUserUidIndex)) {
                return false;
            }
            
            static const std::string createUserNameIndex = "CREATE UNIQUE INDEX IF NOT EXISTS user_name_index ON t_user(_name)";
            if (!executeSql(createUserNameIndex)) {
                return false;
            }
            
            static const std::string createGroupTableSql = "CREATE TABLE IF NOT EXISTS t_group(_id INTEGER PRIMARY KEY AUTOINCREMENT,_uid TEXT NOT NULL,_name TEXT,_portrait TEXT,_owner TEXT,_type INTEGER,_member_count INTEGER,_extra TEXT,_update_dt INTEGER DEFAULT 0)";
            if (!executeSql(createGroupTableSql)) {
                return false;
            }
            
            static const std::string createGroupUidIndexSql = "CREATE UNIQUE INDEX IF NOT EXISTS group_uid_index ON t_group(_uid)";
            if (!executeSql(createGroupUidIndexSql)) {
                return false;
            }
            
            static const std::string createGroupMemberTableSql = "CREATE TABLE IF NOT EXISTS t_group_member(_id INTEGER PRIMARY KEY AUTOINCREMENT,_gid TEXT NOT NULL,_mid TEXT NOT NULL,_alias TEXT,_type INTEGER,_update_dt INTEGER DEFAULT 0)";
            if (!executeSql(createGroupMemberTableSql)) {
                return false;
            }
            
            static const std::string createGroupMemberIndex = "CREATE UNIQUE INDEX IF NOT EXISTS group_member_index ON t_group_member(_gid,_mid)";
            if (!executeSql(createGroupMemberIndex)) {
                return false;
            }
            
            static const std::string createFriendTableSql = "CREATE TABLE IF NOT EXISTS t_friend(_id INTEGER PRIMARY KEY AUTOINCREMENT,_friend_uid TEXT NOT NULL,_state INTEGER,_update_dt INTEGER DEFAULT 0)";
            if (!executeSql(createFriendTableSql)) {
                return false;
            }
            
            static const std::string createFriendUidIndex = "CREATE UNIQUE INDEX IF NOT EXISTS friend_index ON t_friend(_friend_uid)";
            if (!executeSql(createFriendUidIndex)) {
                return false;
            }
            
            static const std::string createFriendRequestTableSql = "CREATE TABLE IF NOT EXISTS t_friend_request(_id INTEGER PRIMARY KEY AUTOINCREMENT,_direction INTEGER,_target_uid TEXT NOT NULL,_reason TEXT,_status INTEGER,_read_status INTEGER,_update_dt INTEGER DEFAULT 0)";
            if (!executeSql(createFriendRequestTableSql)) {
                return false;
            }
            
            static const std::string createFriendRequestIndex = "CREATE UNIQUE INDEX IF NOT EXISTS friend_request_index ON t_friend_request(_target_uid)";
            if (!executeSql(createFriendRequestIndex)) {
                return false;
            }
            
            static const std::string createTimelineTableSql = "CREATE TABLE IF NOT EXISTS t_timeline(_head INTEGER DEFAULT 0)";
            if (!executeSql(createTimelineTableSql)) {
                return false;
            }
            
            static const std::string createUserServerTableSql = "CREATE TABLE IF NOT EXISTS t_user_server(_id INTEGER PRIMARY KEY AUTOINCREMENT,_uid TEXT NOT NULL,_host TEXT  NOT NULL, _long_port INTEGER, _short_port INTEGER, _update_dt INTEGER)";
            if (!executeSql(createUserServerTableSql)) {
                return false;
            }
            
            static const std::string createUserServerIndex = "CREATE UNIQUE INDEX IF NOT EXISTS user_server_index ON t_user_server(_uid)";
            if (!executeSql(createUserServerIndex)) {
                return false;
            }
            //set timeline to 0;
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_head");
            std::string sql = GetInsertSql(TIMELINE_TABLE_NAME, columns);
#else
            std::string sql = GetInsertSql(TIMELINE_TABLE_NAME, {"_head"});
#endif
            int error = 0;
            RecyclableStatement statementHandleTimeline(m_db, sql, error);
            if (error != 0) {
                return false;
            }
            
            Bind(statementHandleTimeline, 0, 1);
            
            if(!ExecuteInsert(statementHandleTimeline)) {
                return false;
            }
            
            static const std::string createConversationTableSql = "CREATE TABLE IF NOT EXISTS t_conversation(_conv_type INTEGER NOT NULL,_conv_target TEXT NOT NULL,_conv_line INTEGER NOT NULL,_draft TEXT DEFAULT NULL,_istop INTEGER DEFAULT 0,_issilent INTEGER DEFAULT 0,_timestamp INTEGER DEFAULT 0)";
            if (!executeSql(createConversationTableSql)) {
                return false;
            }
            
            static const std::string createConversationIndex = "CREATE UNIQUE INDEX IF NOT EXISTS conv_index ON t_conversation(_conv_type,_conv_target,_conv_line)";
            if (!executeSql(createConversationIndex)) {
                return false;
            }
            
            static const std::string createTableVersionSql = "CREATE TABLE IF NOT EXISTS t_version(_version INTEGER PRIMARY KEY ON CONFLICT REPLACE)";
            if (!executeSql(createTableVersionSql)) {
                return false;
            }
            
            static const std::string createRobotTableSql = "CREATE TABLE IF NOT EXISTS t_my_robot(_id INTEGER PRIMARY KEY AUTOINCREMENT,_uid TEXT NOT NULL,_owner TEXT NOT NULL,_secret TEXT,_callback TEXT,_extra TEXT,_state INTEGER,_update_dt INTEGER DEFAULT 0)";
            if (!executeSql(createRobotTableSql)) {
                return false;
            }
            
            static const std::string createRobotUidIndex = "CREATE UNIQUE INDEX IF NOT EXISTS robot_uid_index ON t_my_robot(_uid)";
            if (!executeSql(createRobotUidIndex)) {
                return false;
            }
            
            static const std::string createThingTableSql = "CREATE TABLE IF NOT EXISTS t_my_thing(_id INTEGER PRIMARY KEY AUTOINCREMENT,_uid TEXT NOT NULL,_owner TEXT NOT NULL,_token TEXT,_extra TEXT,_state INTEGER,_update_dt INTEGER DEFAULT 0)";
            if (!executeSql(createThingTableSql)) {
                return false;
            }
            
            static const std::string createThingUidIndex = "CREATE UNIQUE INDEX IF NOT EXISTS thing_uid_index ON t_my_thing(_uid)";
            if (!executeSql(createThingUidIndex)) {
                return false;
            }
            
            //and set version to 1
            return SetDBVersion(1);
        }
        
        bool DB2::SetDBVersion(int version) {
#ifdef __ANDROID__
            std::list<std::string> columns2;
            columns2.push_back(VERSION_COLUMN_VERSION);
            std::string sql = GetInsertSql(VERSION_TABLE_NAME, columns2);
#else
            std::string sql = GetInsertSql(VERSION_TABLE_NAME, {VERSION_COLUMN_VERSION});
#endif
            int error = 0;
            RecyclableStatement statementHandle(m_db, sql, error);
            if (error != 0) {
                return false;
            }
            
            Bind(statementHandle, version, 1);
            if(!ExecuteInsert(statementHandle)) {
                return false;
            }
            return true;
        }
        
        bool DB2::UpdateUserServerAddress(const std::string &userId, const UserServerAddress &userServer) {
            if (!isOpened()) {
                return false;
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns2;
            columns2.push_back("_uid");
            columns2.push_back("_host");
            columns2.push_back("_long_port");
            columns2.push_back("_short_port");
            columns2.push_back("_update_dt");
            std::string sql = GetInsertSql("t_user_server", columns2, true);
#else
            std::string sql = GetInsertSql("t_user_server", {"_uid", "_host", "_long_port", "_short_port", "_update_dt"}, true);
#endif
            int error = 0;
            RecyclableStatement statementHandle(m_db, sql, error);
            if (error != 0) {
                return false;
            }
            
            Bind(statementHandle, userId, 1);
            Bind(statementHandle, userServer.host, 2);
            Bind(statementHandle, userServer.longLinkPort, 3);
            Bind(statementHandle, userServer.shortLinkPort, 4);
            Bind(statementHandle, (int64_t)time(NULL), 5);
            
            if(!ExecuteInsert(statementHandle)) {
                return false;
            }

            return true;
        }
        
        UserServerAddress DB2::GetUserServerAddress(const std::string &userId) {
            if (!isOpened()) {
                return UserServerAddress();
            }
            
#ifdef __ANDROID__
            std::list<std::string> columns;
            columns.push_back("_host");
            columns.push_back("_long_port");
            columns.push_back("_short_port");
            columns.push_back("_update_dt");
            std::string sql = GetSelectSql("t_user_server", columns, "_uid=?");
#else
            std::string sql = GetSelectSql("t_user_server", {"_host", "_long_port", "_short_port", "_update_dt"}, "_uid=?");
#endif
            int error = 0;
            RecyclableStatement statementHandle(m_db, sql, error);
            if (error != 0) {
                return UserServerAddress();
            }
            
            Bind(statementHandle, userId, 1);
            
            UserServerAddress result;
            if(statementHandle.executeSelect()) {
                result.host = getStringValue(statementHandle, 0);
                result.longLinkPort = getIntValue(statementHandle, 1);
                result.shortLinkPort = getIntValue(statementHandle, 2);
                result.updateDt = getBigIntValue(statementHandle, 3);
            }
            
            return result;
        }
        
        bool DB2::BEGIN() {
            if (!isOpened()) {
                return false;
            }
            char *pErrMsg = NULL;
            int ret = sqlite3_exec(m_db, "begin;", 0, 0, &pErrMsg);
            if (SQLITE_OK != ret)
            {
                sqlite3_free(pErrMsg);
                return false;
            }
            return true;
        }
        
        void DB2::COMMIT() {
            if (!isOpened()) {
            }
            char *szErrMsg = NULL;
            int rc = sqlite3_exec(m_db, "commit;", 0, 0,&szErrMsg);
            if (SQLITE_OK != rc)
            {
                sqlite3_free(szErrMsg);
                return;
            }
        }
    }
}
