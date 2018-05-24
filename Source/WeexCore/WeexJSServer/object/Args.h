//
// Created by jianbai.gbj on 15/05/2018.
//

#ifndef ARGS_H
#define ARGS_H


#include <wtf/text/WTFString.h>

#include "./wson/wson.h"

/**
 * auto conver for json and wson
 */
namespace WeexCore { 
    static const int ARGS_TYPE_JSON = 0;
    static const int ARGS_TYPE_WSON = 1;
    class Args{
        public:
            Args();
            ~Args();

            int getType(){
                return type;
            };

            void setString(WTF::String string){
                this->json = string;
                this->type = ARGS_TYPE_JSON;
            }
            /**object will auto free when args destructor */
            void setWson(wson_buffer* buffer){
                this->wson = buffer;
                this->type = ARGS_TYPE_WSON;
            }

            inline const char* getValue(){
                if(type == ARGS_TYPE_WSON){
                    if(wson){
                       return (char*)(wson->data);
                    }
                    return nullptr;
                }else{
                    return json.utf8().data();
                }
            }

            inline int getLength(){
                if(type == ARGS_TYPE_WSON){
                    if(wson){
                       return (wson->position);
                    }
                    return 0;
                }else{
                    return strlen(json.utf8().data());
                }
            }

        private:
            int type; 
        public:
            WTF::String json;
            wson_buffer* wson = nullptr;
     };
};

#endif
