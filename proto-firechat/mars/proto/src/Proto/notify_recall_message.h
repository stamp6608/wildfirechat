//
//  notify_recall_message.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef notify_recall_message_hpp
#define notify_recall_message_hpp

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
        
                
        class NotifyRecallMessage : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            NotifyRecallMessage() {}
            virtual ~NotifyRecallMessage() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            int64_t messageUid;
            std::string operatorId;

        };

    }
}
#endif /* notify_recall_message_hpp */

