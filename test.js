const sharedObject = require('.');

let t = sharedObject.create({test: 2, foo: {bar: 'test'}});
console.log(t.foo.bar)

console.log(t)

let h = sharedObject.getHandle(t.foo);
console.log(h)

const {Worker} = require('worker_threads');

const w = new Worker(`
  const { parentPort } = require('worker_threads');
  const sharedObject = require('.');
  parentPort.on('message', (msg) => {
    let o = sharedObject.get(msg);
    console.log(msg, o.bar);
    o.bar = 'baz';
    parentPort.postMessage('done');
  });
`, { eval: true });
w.unref();
w.once('message', (msg) => {
 console.log(msg, t.foo.bar);
});
w.postMessage(h);
