//
//  general_result.hpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef general_result_hpp
#define general_result_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class GeneralResult : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            GeneralResult() {}
            virtual ~GeneralResult() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            int errorCode;
        };
    }
}
#endif /* add_friends_request_hpp */

