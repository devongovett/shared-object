#include <napi.h>
#include <node_api.h>
#include "Value.hh"
#include "ObjectAccessor.hh"

Napi::Value create(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "Expected an object").ThrowAsJavaScriptException();
    return env.Null();
  }

  Value::ValuePtr value = Value::create(info[0]);
  return value->toJSValue(env, value);
}

Napi::Value get(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "Expected a number").ThrowAsJavaScriptException();
    return env.Null();
  }

  intptr_t handle = info[0].As<Napi::Number>().Int64Value();
  Value::ValuePtr value = Value::getValueByHandle(handle);
  if (value == nullptr) {
    return env.Null();
  }

  return value->toJSValue(env, value);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "create"),
    Napi::Function::New(env, create)
  );

  exports.Set(
    Napi::String::New(env, "get"),
    Napi::Function::New(env, get)
  );

  ObjectAccessor::init(env);
  return exports;
}

NODE_API_MODULE(sharedObject, Init)
