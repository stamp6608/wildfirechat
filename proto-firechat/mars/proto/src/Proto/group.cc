//
//  group.cpp
//  proto
//
//  Created by WF Chat on 2017/11/29.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "group.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{

        const char* GroupInfo::getTypeName() {
            return "GroupInfo";
        }
        
        bool GroupInfo::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            targetId = getString(msg, keyTargetId, 0);
            name = getString(msg, keyName, 0);
            portrait = getString(msg, keyPortrait, 0);
            owner = getString(msg, keyOwner, 0);
            type = (GroupType)getInt(msg, keyType, 0);
            memberCount = getInt(msg, keyMemberCount, 0);
            extra = getString(msg, keyExtra, 0);
            updateDt = getInt64(msg, keyUpdateDt, 0);
            memberUpdateDt = getInt64(msg, keyMemberUpdateDt, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void GroupInfo::serializeToPBMsg(struct pbc_wmessage* msg)  {
            setString(msg, keyTargetId, targetId);
            setString(msg, keyName, name);
            setString(msg, keyPortrait, portrait);
            setString(msg, keyOwner, owner);
            setInt(msg, keyType, type);
            setInt(msg, keyMemberCount, memberCount);
            setString(msg, keyExtra, extra);
            setInt64(msg, keyUpdateDt, updateDt);
            setInt64(msg, keyMemberUpdateDt, memberUpdateDt);
        }
        
        const char* GroupMember::getTypeName() {
            return "GroupMember";
        }
        
        bool GroupMember::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            memberId = getString(msg, keyMemberId, 0);
            alias = getString(msg, keyAlias, 0);
            type = (GroupMemberType)getInt(msg, keyType, 0);
            updateDt = getInt64(msg, keyUpdateDt, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void GroupMember::serializeToPBMsg(struct pbc_wmessage* msg)  {
            setString(msg, keyMemberId, memberId);
            setString(msg, keyAlias, alias);
            setInt(msg, keyType, type);
            setInt64(msg, keyUpdateDt, updateDt);
        }
        
        const char* Group::getTypeName() {
            return "Group";
        }
        
        bool Group::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            struct pbc_rmessage *infoMsg = getSubMessage(msg, keyGroupInfo, 0);
            if(!groupInfo.unserializeFromPBMsg(infoMsg, false)) {
                if(destroy)
                finishRead(msg);
                return false;
            }
            int size = getSize(msg, keyMembers);
            for (int i = 0; i < size; i++) {
                struct pbc_rmessage *memberMsg = getSubMessage(msg, keyMembers, i);
                GroupMember member;
                if(member.unserializeFromPBMsg(memberMsg, false)) {
                    if(destroy)
                    finishRead(msg);
                    return false;
                }
                members.push_back(member);
            }
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void Group::serializeToPBMsg(struct pbc_wmessage* msg)  {
            struct pbc_wmessage *infomsg = setSubMessaage(msg, keyGroupInfo);
            groupInfo.serializeToPBMsg(infomsg);
            for (std::vector<GroupMember>::iterator it = members.begin(); it != members.end(); it++) {
                struct pbc_wmessage *membermsg = setSubMessaage(msg, keyMembers);
                (*it).serializeToPBMsg(membermsg);
            }
        }

    }
}
