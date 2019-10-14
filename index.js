const binding = require('bindings')('shared-object.node');
const inspect = require('util').inspect;

const map = new WeakMap();

function createProxy(t) {
  function get(key) {
    let r = t.get(key);
    if (typeof r === 'object') {
      return createProxy(r);
    }
  
    return r;
  }

  // Node doesn't properly format proxies, so we implement a custom formatter
  const base = {
    [inspect.custom](depth, options) {
      if (depth < 0) {
        return options.stylize('[SharedObject]', 'special');
      }
  
      const newOptions = Object.assign({}, options, {
        depth: options.depth === null ? null : options.depth - 1
      });
  
      const o = {};
      for (let key of t.keys()) {
        o[key] = get(key);
      }

      let res = inspect(o, newOptions);
      return `SharedObject ${res}`;
    }
  };
  
  let proxy = new Proxy(base, {
    get(target, key) {
      return get(key);
    },
    set(target, key, value) {
      t.set(key, value);
    },
    has(target, key) {
      return t.has(key);
    },
    deleteProperty(target, key) {
      t.delete(key);
    },
    ownKeys(target) {
      return t.keys();
    },
    getOwnPropertyDescriptor(target, key) {
      return {
        configurable: true,
        enumerable: true,
        value: t.get(key)
      };
    }
  });

  map.set(proxy, t);
  return proxy;
}

exports.create = function (o) {
  return createProxy(binding.create(o));
};

exports.get = function (handle) {
  return createProxy(binding.get(handle));
};

exports.getHandle = function (o) {
  return map.get(o).getHandle();
};
