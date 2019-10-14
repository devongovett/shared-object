#include "Value.hh"
#include "ObjectAccessor.hh"
#include <mutex>

static std::unordered_map<intptr_t, std::weak_ptr<Value>> handles;

class NumberValue : public Value {
public:
  double value;
  NumberValue(Napi::Number value) : value(value.DoubleValue()) {}
  Napi::Value toJSValue(Napi::Env env, Value::ValuePtr ptr) override {
    return Napi::Number::New(env, value);
  }
};

class BooleanValue : public Value {
public:
  bool value;
  BooleanValue(Napi::Boolean value) : value(value.Value()) {}
  Napi::Value toJSValue(Napi::Env env, Value::ValuePtr ptr) override {
    return Napi::Boolean::New(env, value);
  }
};

class NullValue : public Value {
public:
  Napi::Value toJSValue(Napi::Env env, Value::ValuePtr ptr) override {
    return env.Null();
  }
};

class UndefinedValue : public Value {
public:
  Napi::Value toJSValue(Napi::Env env, Value::ValuePtr ptr) override {
    return env.Undefined();
  }
};

class StringValue : public Value {
public:
  std::string value;
  StringValue(Napi::String value) : value(value.Utf8Value()) {}
  Napi::Value toJSValue(Napi::Env env, Value::ValuePtr ptr) override {
    return Napi::String::New(env, value);
  }
};

class ObjectValue : public Value {
public:
  Value::Object value;
  std::mutex mutex;
  
  ObjectValue(Napi::Object v) {
    Napi::Array properties = v.GetPropertyNames();
    uint32_t len = properties.Length();

    for (uint32_t i = 0; i < len; i++) {
      Napi::Value key = properties.Get(i);
      std::string prop = key.As<Napi::String>().Utf8Value();
      Value::ValuePtr val = Value::create(v.Get(key));
      value[prop] = val;
    }
  }

  Napi::Value get(Napi::Env env, std::string key) override {
    auto it = value.find(key);
    if (it != value.end()) {
      return it->second->toJSValue(env, it->second);
    } else {
      return env.Undefined();
    }
  }

  void set(std::string key, Napi::Value val) override {
    std::unique_lock<std::mutex> lock(mutex);
    Value::ValuePtr ptr = Value::create(val);
    value[key] = ptr;
  }

  void del(std::string key) override {
    std::unique_lock<std::mutex> lock(mutex);
    value.erase(key);
  }

  Napi::Value has(Napi::Env env, std::string key) override {
    return Napi::Boolean::New(env, value.find(key) != value.end());
  }

  Napi::Value keys(Napi::Env env) override {
    Napi::Array keys = Napi::Array::New(env);
    uint32_t i = 0;

    for (auto it = value.begin(); it != value.end(); it++) {
      keys.Set(i++, Napi::String::New(env, it->first));
    }

    return keys;
  }

  Napi::Value toJSValue(Napi::Env env, Value::ValuePtr ptr) override {
    return ObjectAccessor::create(ptr);
  }
};

Value::ValuePtr Value::create(Napi::Value value) {
  if (value.IsNumber()) {
    return std::make_shared<NumberValue>(value.As<Napi::Number>());
  } else if (value.IsBoolean()) {
    return std::make_shared<BooleanValue>(value.As<Napi::Boolean>());
  } else if (value.IsNull()) {
    return std::make_shared<NullValue>();
  } else if (value.IsUndefined()) {
    return std::make_shared<UndefinedValue>();
  } else if (value.IsString()) {
    return std::make_shared<StringValue>(value.As<Napi::String>());
  } else if (value.IsObject()) {
    return std::make_shared<ObjectValue>(value.As<Napi::Object>());
  }

  // TODO: array, map, set
}

intptr_t Value::getHandle(Value::ValuePtr ptr) {
  intptr_t p = reinterpret_cast<intptr_t>(ptr.get());
  handles.emplace(p, ptr);
  return p;
}

Value::ValuePtr Value::getValueByHandle(intptr_t handle) {
  auto it = handles.find(handle);
  if (it != handles.end()) {
    return it->second.lock();
  } else {
    return nullptr;
  }
}

Value::~Value() {
  intptr_t p = reinterpret_cast<intptr_t>(this);
  handles.erase(p);
}
