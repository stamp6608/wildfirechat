//
//  DB2.h
//  proto
//
//  Created by WF Chat on 2017/11/18.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef DB2_hpp
#define DB2_hpp

#include <stdio.h>
#include "sqlite3.h"
#include <list>
#ifndef ANDROID
#include <initializer_list>
#endif
#include <string>

namespace mars {
    namespace stn {

        class UserServerAddress;
        class DB2;
        class RecyclableStatement {
        public:
            RecyclableStatement(sqlite3 *db, const std::string &sql, int &error);
            RecyclableStatement(DB2 *db2, const std::string &sql, int &error);
            bool executeSelect();
            bool executeInsert(long *rowId);
            bool executeDelete(long *changes);
            bool executeUpdate(long *changes);
            int bind(int icol, int value);
#if !ANDROID || !__LP64__
            int bind(int icol, long value);
#endif
            int bind(int icol, int64_t value);
            int bind(int icol, float value);
            int bind(int icol, const char* value);
            int bind(int icol, const std::string& value);
            int bind(int icol, const void* value, int size);
            float getFloatValue(int icol)  const;
            int getIntValue(int icol)  const;
            int64_t getInt64Value(int icol)  const;
            std::string getStringValue(int icol)  const;
            const void * getBlobValue(int icol, int &size)  const;
            virtual ~RecyclableStatement() ;
        private:
            sqlite3 *m_db;
            sqlite3_stmt* m_stmt;
            std::string m_sql;
        };
        
        extern const std::string VERSION_TABLE_NAME;
        extern const std::string VERSION_COLUMN_VERSION;
        
        extern const std::string MESSAGE_TABLE_NAME;
        extern const std::string USER_TABLE_NAME;
        extern const std::string GROUP_TABLE_NAME;
        extern const std::string CHANNEL_TABLE_NAME;
        extern const std::string GROUP_MEMBER_TABLE_NAME;
        extern const std::string TIMELINE_TABLE_NAME;
        extern const std::string USER_SETTING_TABLE_NAME;
        
        extern const std::string CONVERSATION_TABLE_NAME;
        
        extern const std::string FRIEND_TABLE_NAME;
        extern const std::string FRIEND_REQUEST_TABLE_NAME;
        
        
        
        class DB2 {
            
        private:
            DB2();
            virtual ~DB2();
            std::string secret;
            
        public:
            static DB2* Instance();
            void Open(const std::string &sec);
            void Upgrade();
            bool isOpened();
#ifdef __ANDROID__
            std::string GetSelectSqlEx(const std::string &tableNameLeft, const std::list<std::string> &columnsLeft, const std::string &tableNameRight, const std::list<std::string> &columnsRight, const std::string &where,const std::string &orderBy = "", int limit = 0, int offset = 0, const std::string &groupBy = "");
            
            std::string GetSelectSqlEx2(const std::string &tableNameLeft, const std::list<std::string> &columnsLeft, const std::string &tableNameMiddle, const std::list<std::string> &columnsMiddle, const std::string &tableNameRight, const std::list<std::string> &columnsRight, const std::string &where,const std::string &orderBy = "", int limit = 0, int offset = 0, const std::string &groupBy = "");
            
            std::string GetSelectSql(const std::string &tableName, const std::list<std::string> &columns, const std::string &where = "", const std::string &orderBy = "", int limit = 0, int offset = 0, const std::string &groupBy = "");
            std::string GetInsertSql(const std::string &table, const std::list<std::string> &columns, bool replace = false);
            std::string GetUpdateSql(const std::string &table, const std::list<std::string> &columns, const std::string &where = "");
#else
          std::string GetSelectSqlEx(const std::string &tableNameLeft, const std::initializer_list<std::string> &columnsLeft, const std::string &tableNameRight, const std::initializer_list<std::string> &columnsRight, const std::string &where, const std::string &orderBy = "", int limit = 0, int offset = 0, const std::string &groupBy = "");
            
            std::string GetSelectSqlEx2(const std::string &tableNameLeft, const std::initializer_list<std::string> &columnsLeft, const std::string &tableNameMiddle, const std::initializer_list<std::string> &columnsMiddle, const std::string &tableNameRight, const std::initializer_list<std::string> &columnsRight, const std::string &where, const std::string &orderBy = "", int limit = 0, int offset = 0, const std::string &groupBy = "");
            
            std::string GetSelectSql(const std::string &tableName, const std::initializer_list<std::string> &columns, const std::string &where = "", const std::string &orderBy = "", int limit = 0, int offset = 0, const std::string &groupBy = "");
            std::string GetInsertSql(const std::string &table, const std::initializer_list<std::string> &columns, bool replace = false);
            std::string GetUpdateSql(const std::string &table, const std::initializer_list<std::string> &columns, const std::string &where = "");
            
            std::string GetUpdateSql(const std::string &table, const std::list<std::string> &columns, const std::string &where = "");
#endif
            bool ExecuteInsert(RecyclableStatement &statementHandle, long *rowId = NULL);
            
            std::string GetDeleteSql(const std::string &table, const std::string &where = "");
            int ExecuteDelete(RecyclableStatement &statementHandle);
            
            int ExecuteUpdate(RecyclableStatement &statementHandle);
            
            
            
            void Bind(RecyclableStatement &statementHandle, float value, int index);
            void Bind(RecyclableStatement &statementHandle, int value, int index);
#if !ANDROID || !__LP64__
            void Bind(RecyclableStatement &statementHandle, long value, int index);
#endif
            void Bind(RecyclableStatement &statementHandle, int64_t value, int index);
            void Bind(RecyclableStatement &statementHandle, const std::string &value, int index);
            void Bind(RecyclableStatement &statementHandle, const void *value, int size, int index);
            
            
            float getFloatValue(const RecyclableStatement &statementHandle, int index);
            int getIntValue(const RecyclableStatement &statementHandle, int index);
            int64_t getBigIntValue(const RecyclableStatement &statementHandle, int index);
            std::string getStringValue(const RecyclableStatement &statementHandle, int index);
            const void* getBlobValue(const RecyclableStatement &statementHandle, int index, int &size);
            
            void closeDB();
            bool BEGIN();
            
            void COMMIT();
            bool UpdateUserServerAddress(const std::string &userId, const UserServerAddress &userServer);
            UserServerAddress GetUserServerAddress(const std::string &userId);
            friend RecyclableStatement;
        private:
            static DB2* instance_;
            
            bool SetDBVersion(int version);
            bool CreateDB2Version1();
            bool UpgradeDB2Version2();
            bool UpgradeDB2Version3();
            bool UpgradeDB3Version4();
            bool UpgradeDB4Version5();
            bool UpgradeDB5Version6();
            bool UpgradeDB6Version7();
            
            bool IsTableExist(std::string tableName);
            bool executeSql(const std::string &sql);
            bool opened;
            sqlite3 *m_db;
        };
        
    }
}
#endif /* DB22_hpp */
