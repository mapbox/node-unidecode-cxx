#include <nan.h>
#include <string>
#include "unidecode.cxx"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;


NAN_METHOD(decode) {
  String::Utf8Value utf8_value(info[0].As<String>());
  string input = string(*utf8_value, utf8_value.length());
  string output;
  unidecode(&input, &output);

  info.GetReturnValue().Set(New(output).ToLocalChecked());
}

NAN_MODULE_INIT(InitAll) {
  Set(target, New<String>("decode").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(decode)).ToLocalChecked());
}

NODE_MODULE(unidecode, InitAll)
