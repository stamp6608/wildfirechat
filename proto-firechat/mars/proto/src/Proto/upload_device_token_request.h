//
//  upload_device_token_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef upload_device_token_request_hpp
#define upload_device_token_request_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        typedef enum {
            Platform_UNSET = 0,
            Platform_iOS = 1,
            Platform_Android = 2,
            Platform_Windows = 3,
            Platform_OSX = 4,
            Platform_WEB = 5
        } Platform;

        
        class UploadDeviceTokenRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            UploadDeviceTokenRequest() {}
            virtual ~UploadDeviceTokenRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            
            Platform platform;
            std::string appName;
            std::string deviceToken;
            int pushType;
        };
    }
}
#endif /* add_friends_request_hpp */

