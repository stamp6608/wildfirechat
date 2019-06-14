//
//  group.hpp
//  proto
//
//  Created by WF Chat on 2017/11/29.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef group_hpp
#define group_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        typedef enum {
            //member can add quit change group name and portrait, owner can do all the operations
            GroupType_Normal = 0,
            //every member can add quit change group name and portrait, no one can kickoff others
            GroupType_Free = 1,
            //member can only quit, owner can do all the operations
            GroupType_Restricted = 2,
        } GroupType;
        
        
        class GroupInfo : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            GroupInfo() {}
            virtual ~GroupInfo() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string targetId;
            std::string name;
            std::string portrait;
            std::string owner;
            GroupType type;
            int memberCount;
            std::string extra;
            int64_t updateDt;
            int64_t memberUpdateDt;
        };
        
        typedef enum {
            GroupMemberType_Normal = 0,
            GroupMemberType_Manager = 1,
            GroupMemberType_Owner = 2,
            GroupMemberType_Silent = 3,
            GroupMemberType_Removed = 4,
        } GroupMemberType;
        
        class GroupMember : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            GroupMember() {}
            virtual ~GroupMember() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string memberId;
            std::string alias;
            GroupMemberType type;
            int64_t updateDt;
        };
        
        class Group : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            Group() {}
            virtual ~Group() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            GroupInfo groupInfo;
            std::vector<GroupMember> members;
        };
    }
}
#endif /* group_hpp */
