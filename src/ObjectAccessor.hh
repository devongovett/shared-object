#ifndef OBJECT_ACCESSOR_H
#define OBJECT_ACCESSOR_H

#include <napi.h>
#include <node_api.h>
#include "Value.hh"

class ObjectAccessor : public Napi::ObjectWrap<ObjectAccessor> {
  public:
    static void init(Napi::Env env);
    ObjectAccessor(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ObjectAccessor>(info) {}
    static Napi::Object create(Value::ValuePtr value);

  private:
    thread_local static Napi::FunctionReference constructor;
    Value::ValuePtr value;
    Napi::Value get(const Napi::CallbackInfo &info);
    Napi::Value set(const Napi::CallbackInfo &info);
    Napi::Value del(const Napi::CallbackInfo &info);
    Napi::Value has(const Napi::CallbackInfo &info);
    Napi::Value keys(const Napi::CallbackInfo &info);
    Napi::Value getHandle(const Napi::CallbackInfo &info);
};

#endif
