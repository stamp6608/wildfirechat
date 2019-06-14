//
//  notify_and_pull_message.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef notify_and_pull_message_hpp
#define notify_and_pull_message_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "message.h"
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        typedef enum {
            Pull_Normal = 0,
            Pull_ChatRoom = 1,
            Pull_Group = 2
        } PullType;
                
        class NotifyMessage : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            NotifyMessage() {}
            virtual ~NotifyMessage() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            PullType type;
            int64_t head;
            std::string target;

        };

        class PullMessageRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            PullMessageRequest() {}
            virtual ~PullMessageRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            PullType type;
            int64_t id;
        };
        

        class PullMessageResult : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            PullMessageResult() {}
            virtual ~PullMessageResult() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::list<Message> messages;
            int64_t current;
            int64_t head;
        };
    }
}
#endif /* add_friends_request_hpp */

