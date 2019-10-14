{
  "targets": [
    {
      "target_name": "shared-object",
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "sources": [ "src/binding.cc", "src/Value.cc", "src/ObjectAccessor.cc" ],
      "include_dirs" : ["<!@(node -p \"require('node-addon-api').include\")"],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"]
    }
  ]
}
