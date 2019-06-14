//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "modify_user_setting_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* ModifyUserSettingReq::getTypeName() {
            return "ModifyUserSettingReq";
        }
        
        bool ModifyUserSettingReq::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            scope = getInt(msg, keyScope, 0);
            key = getString(msg, keyKey, 0);
            value = getString(msg, keyValue, 0);
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void ModifyUserSettingReq::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt(msg, keyScope, scope);
            setString(msg, keyKey, key);
            setString(msg, keyValue, value);
        }
    }
}
