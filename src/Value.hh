#ifndef VALUE_H
#define VALUE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <napi.h>
#include <node_api.h>

class Value {
public:
  typedef std::shared_ptr<Value> ValuePtr;
  typedef std::vector<ValuePtr> Array;
  typedef std::unordered_map<std::string, ValuePtr> Object;
  typedef std::unordered_map<ValuePtr, ValuePtr> Map;
  typedef std::unordered_set<ValuePtr> Set;

  static ValuePtr create(Napi::Value value);

  virtual Napi::Value toJSValue(Napi::Env env, Value::ValuePtr ptr) { return env.Undefined(); }
  virtual Napi::Value get(Napi::Env env, std::string key) { return env.Undefined(); }
  virtual void set(std::string key, Napi::Value val) {}
  virtual void del(std::string key) {}
  virtual Napi::Value has(Napi::Env env, std::string key) { return env.Undefined(); }
  virtual Napi::Value keys(Napi::Env env) { return env.Undefined(); }
  virtual ~Value();

  static intptr_t getHandle(ValuePtr ptr);
  static ValuePtr getValueByHandle(intptr_t handle);
};

#endif
