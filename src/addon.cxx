/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2015 NAN contributors
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 ********************************************************************/

#include <nan.h>
#include <string>
#include <sstream>
#include "unidecode.cxx"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;


// Simple synchronous access to the `Estimate()` function
NAN_METHOD(decode) {
  // expect a number as the first argument
  String::Utf8Value utf8_value(info[0].As<String>());
  string input = string(*utf8_value, utf8_value.length());
  stringbuf output;
  unidecode(input, &output);

  info.GetReturnValue().Set(New(output.str()).ToLocalChecked());
}

// Expose synchronous and asynchronous access to our
// Estimate() function
NAN_MODULE_INIT(InitAll) {
  Set(target, New<String>("decode").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(decode)).ToLocalChecked());
}

NODE_MODULE(unidecode, InitAll)
