//
//  modify_my_info_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "modify_my_info_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{

        const char* InfoEntry::getTypeName() {
            return "InfoEntry";
        }
        
        bool InfoEntry::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            type = getInt(msg, keyType, 0);
            value = getString(msg, keyValue, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void InfoEntry::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt(msg, keyType, type);
            setString(msg, keyValue, value);
        }

        
        const char* ModifyMyInfoRequest::getTypeName() {
            return "ModifyMyInfoRequest";
        }
        
        bool ModifyMyInfoRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            for (int i = 0; i < getSize(msg, keyEntry); i++) {
                struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyEntry, i);
                InfoEntry entry;
                entry.unserializeFromPBMsg(tmpMsg, false);
                entrys.push_back(entry);
            }
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void ModifyMyInfoRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            for (std::list<InfoEntry>::iterator it = entrys.begin(); it != entrys.end(); it++) {
                struct pbc_wmessage *tmpMsg = setSubMessaage(msg, keyEntry);
                (*it).serializeToPBMsg(tmpMsg);
            }
        }
    }
}
