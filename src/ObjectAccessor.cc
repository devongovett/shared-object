#include "ObjectAccessor.hh"
#include <thread>

thread_local Napi::FunctionReference ObjectAccessor::constructor;

void ObjectAccessor::init(Napi::Env env) {
  Napi::Function func = DefineClass(env, "ObjectAccessor", {
    InstanceMethod("get", &ObjectAccessor::get),
    InstanceMethod("set", &ObjectAccessor::set),
    InstanceMethod("delete", &ObjectAccessor::del),
    InstanceMethod("has", &ObjectAccessor::has),
    InstanceMethod("keys", &ObjectAccessor::keys),
    InstanceMethod("getHandle", &ObjectAccessor::getHandle)
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
}

Napi::Object ObjectAccessor::create(Value::ValuePtr value) {
  Napi::Object res = constructor.New({});
  ObjectAccessor *wrapper = Unwrap(res);
  wrapper->value = value;
  return res;
}

Napi::Value ObjectAccessor::get(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "Expected a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  return value->get(env, info[0].As<Napi::String>().Utf8Value());
}

Napi::Value ObjectAccessor::set(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "Expected a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  if (info.Length() < 2) {
    Napi::TypeError::New(env, "Expected a value").ThrowAsJavaScriptException();
    return env.Null();
  }

  value->set(info[0].As<Napi::String>().Utf8Value(), info[1]);
  return env.Undefined();
}

Napi::Value ObjectAccessor::del(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "Expected a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  value->del(info[0].As<Napi::String>().Utf8Value());
  return env.Undefined();
}

Napi::Value ObjectAccessor::has(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "Expected a string").ThrowAsJavaScriptException();
    return env.Null();
  }

  return value->has(env, info[0].As<Napi::String>().Utf8Value());
}

Napi::Value ObjectAccessor::keys(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return value->keys(env);
}

Napi::Value ObjectAccessor::getHandle(const Napi::CallbackInfo &info) {
  intptr_t handle = Value::getHandle(value);
  return Napi::Number::New(info.Env(), handle);
}
