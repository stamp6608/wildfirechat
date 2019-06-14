//
//  friend_request.h
//  proto
//
//  Created by WF Chat on 2017/11/29.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef friend_request_hpp
#define friend_request_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        typedef enum {
            RequestStatus_Sent = 0,
            RequestStatus_Accepted = 1,
            RequestStatus_Rejected = 3
        } FriendRequestStatus;
        


        
        class FriendRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            FriendRequest() {}
            virtual ~FriendRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string fromUid;
            std::string toUid;
            std::string reason;
            FriendRequestStatus status;
            int64_t updateDt = 5;
            int fromReadStatus = 6;
            int toReadStatus = 7;
        };
    }
}
#endif /* friend_request_hpp */
