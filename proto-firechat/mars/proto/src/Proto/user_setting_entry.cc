//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "user_setting_entry.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* UserSettingEntry::getTypeName() {
            return "UserSettingEntry";
        }
        
        bool UserSettingEntry::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            scope = getInt(msg, keyScope, 0);
            key = getString(msg, keyKey, 0);
            value = getString(msg, keyValue, 0);
            updateDt = getInt64(msg, keyUpdateDt, 0);
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void UserSettingEntry::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt(msg, keyScope, scope);
            setString(msg, keyKey, key);
            setString(msg, keyValue, value);
            setInt64(msg, keyUpdateDt, updateDt);
        }
    }
}
