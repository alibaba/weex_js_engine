#include "Args.h"

namespace WeexCore{

   Args::Args(){
       this->type = ARGS_TYPE_JSON;
   }
   
   Args::~Args(){
       if(this->type == ARGS_TYPE_WSON){
           if(this->wson && this->wson != nullptr && this->wson != NULL){
               wson_buffer_free(this->wson);
               this->wson = nullptr;
           }
       }
   }
   
}