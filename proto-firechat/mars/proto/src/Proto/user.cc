//
//  user.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "user.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{

        const char* User::getTypeName() {
            return "User";
        }
        
        bool User::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            uid = getString(msg, keyUid, 0);
            name = getString(msg, keyName, 0);
            displayName = getString(msg, keyDisplayName, 0);
            portrait = getString(msg, keyPortrait, 0);
            mobile = getString(msg, keyMobile, 0);
            email = getString(msg, keyEmail, 0);
            address = getString(msg, keyAddress, 0);
            company = getString(msg, keyCompany, 0);
            extra = getString(msg, keyExtra, 0);
            updateDt = getInt64(msg, keyUpdateDt, 0);
            gender = getInt(msg, keyGender, 0);
            social = getString(msg, keySocial, 0);
            type = getInt(msg, keyType, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void User::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyUid, uid);
            setString(msg, keyName, name);
            setString(msg, keyDisplayName, displayName);
            setString(msg, keyPortrait, portrait);
            setString(msg, keyMobile, mobile);
            setString(msg, keyEmail, email);
            setString(msg, keyAddress, address);
            setString(msg, keyCompany, company);
            setString(msg, keyExtra, extra);
            setInt64(msg, keyUpdateDt, updateDt);
            setInt(msg, keyGender, gender);
            setString(msg, keySocial, social);
            setInt(msg, keyType, type);
        }
    }
}

