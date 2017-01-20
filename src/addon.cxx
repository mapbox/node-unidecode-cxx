#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#include <nan.h>
#pragma clang diagnostic pop

#include <string>
#include "unidecode.cxx"

NAN_METHOD(decode) {
  try {
      v8::Local<v8::String> val = info[0].As<v8::String>();
      v8::String::Utf8Value utf8_value(val);
      int len = utf8_value.length();
      if (len <= 0) {
          // This is done to match behavior of JS module
          info.GetReturnValue().Set(Nan::New("").ToLocalChecked());
      } else {
          std::string output;
          std::size_t ulen = static_cast<std::size_t>(len);
          output.reserve(ulen);
          unidecode(*utf8_value, ulen, output);
          info.GetReturnValue().Set(Nan::New(output.data(),output.size()).ToLocalChecked());
      }
  } catch (std::exception const& ex) {
      return Nan::ThrowTypeError(ex.what());
  }
}

NAN_METHOD(decodable) {
  try {
      v8::Local<v8::String> val = info[0].As<v8::String>();
      v8::String::Utf8Value utf8_value(val);
      int len = utf8_value.length();
      if (len <= 0) {
          // This is done to match behavior of JS module
          info.GetReturnValue().Set(Nan::New<v8::Boolean>(false));
      } else {
          std::size_t ulen = static_cast<std::size_t>(len);
          info.GetReturnValue().Set(Nan::New<v8::Boolean>(can_decode(*utf8_value, ulen)));
      }
  } catch (std::exception const& ex) {
      return Nan::ThrowTypeError(ex.what());
  }
}


NAN_MODULE_INIT(InitAll) {
  Nan::Set(target, Nan::New<v8::String>("decode").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(decode)).ToLocalChecked());
  Nan::Set(target, Nan::New<v8::String>("decodable").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(decodable)).ToLocalChecked());
}

NODE_MODULE(unidecode, InitAll)
