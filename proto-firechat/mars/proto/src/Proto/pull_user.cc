//
//  pull_user.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "pull_user.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* UserRequest::getTypeName() {
            return "UserRequest";
        }
        
        bool UserRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            uid = getString(msg, keyUid, 0);
            updateDt = getInt64(msg, keyUpdateDt, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void UserRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyUid, uid);
            setInt64(msg, keyUpdateDt, updateDt);
        }
        
        
        const char* PullUserRequest::getTypeName() {
            return "PullUserRequest";
        }
        
        bool PullUserRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            for (int i = 0; i < getSize(msg, keyRequest); i++) {
                struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyRequest, i);
                UserRequest request;
                request.unserializeFromPBMsg(tmpMsg, false);
                requests.push_back(request);
            }
            
            if(destroy)
                finishRead(msg);
            return true;
        }
        
        void PullUserRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            for (std::list<UserRequest>::iterator it = requests.begin(); it != requests.end(); it++) {
                struct pbc_wmessage *tmpMsg = setSubMessaage(msg, keyRequest);
                (*it).serializeToPBMsg(tmpMsg);
            }
        }
        
        
        const char* UserResult::getTypeName() {
            return "UserResult";
        }
        
        bool UserResult::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyUser, 0);
            user.unserializeFromPBMsg(tmpMsg, false);
            code = (PullUserResultCode)getInt(msg, keyCode, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void UserResult::serializeToPBMsg(struct pbc_wmessage* msg) {
            struct pbc_wmessage *tmpMsg = setSubMessaage(msg, keyUser);
            user.serializeToPBMsg(tmpMsg);
            setInt(msg, keyCode, code);
        }
        
        
        const char* PullUserResult::getTypeName() {
            return "PullUserResult";
        }
        
        bool PullUserResult::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            for (int i = 0; i < getSize(msg, keyResult); i++) {
                struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyResult, i);
                UserResult result;
                result.unserializeFromPBMsg(tmpMsg, false);
                results.push_back(result);
            }
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void PullUserResult::serializeToPBMsg(struct pbc_wmessage* msg) {
            for (std::list<UserResult>::iterator it = results.begin(); it != results.end(); it++) {
                struct pbc_wmessage *tmpMsg = setSubMessaage(msg, keyResult);
                (*it).serializeToPBMsg(tmpMsg);
            }
        }
    }
}
