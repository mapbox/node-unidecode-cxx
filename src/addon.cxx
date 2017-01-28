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
#include <cstdlib>
#include <sstream>

constexpr std::size_t default_scratch_space = 4092;

class Decoder : public Nan::ObjectWrap {
    public:
        static void Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target) {
            v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
            tpl->SetClassName(Nan::New("Decoder").ToLocalChecked());
            tpl->InstanceTemplate()->SetInternalFieldCount(1);
            SetPrototypeMethod(tpl, "decode", decode);
            constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
            Nan::Set(
                target,
                Nan::New("Decoder").ToLocalChecked(),
                Nan::GetFunction(tpl).ToLocalChecked()
            );
        }
    private:
        Decoder()
         : scratch1(default_scratch_space),
           scratch2(default_scratch_space) {}
        std::vector<char> scratch1;
        std::vector<char> scratch2;

    static NAN_METHOD(New) {
        if (info.IsConstructCall()) {
            Decoder *dawg = new Decoder();
            dawg->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
        } else {
            Nan::ThrowTypeError("Decoder needs to be called as a constructor");
        }
    }

    static NAN_METHOD(decode) {
        try {
            Decoder* obj = Nan::ObjectWrap::Unwrap<Decoder>(info.This());
            v8::Local<v8::Value> val = info[0];
            if (!val.IsEmpty()) {
                v8::Local<v8::String> string = val->ToString();
                if (!string.IsEmpty()) {
                    size_t str_len = string->Length();
                    auto & scratch1 = obj->scratch1;
                    auto & scratch2 = obj->scratch2;
                    size_t utf8_max_len = 3 * str_len + 1;
                    size_t ascii_max_len = 4 * str_len + 1;
                    if (utf8_max_len > scratch1.size()) {
                      scratch1.resize(utf8_max_len);
                    }
                    if (ascii_max_len > scratch2.size()) {
                      scratch2.resize(ascii_max_len);
                    }
                    // Also passing v8::String::HINT_MANY_WRITES_EXPECTED flattens string
                    // but I've not enabled this yet as it does not clearly increase performance
                    const int flags =
                        v8::String::NO_NULL_TERMINATION | v8::String::REPLACE_INVALID_UTF8 | v8::String::HINT_MANY_WRITES_EXPECTED;
                    size_t length = string->WriteUtf8(&scratch1[0], static_cast<int>(utf8_max_len), 0, flags);
                    scratch1[length] = '\0';
                    size_t final_size = unidecode(&scratch1[0], length, &scratch2[0]);
                    if (final_size > ascii_max_len) {
                      // Should never happen, but if it did we need to know
                      std::stringstream s;
                      s << "preallocated space was too small " << ascii_max_len << " / " << final_size;
                      return Nan::ThrowTypeError(s.str().c_str());
                    }
                    info.GetReturnValue().Set(Nan::New(scratch2.data(),final_size).ToLocalChecked());
                    return;
                }
            }
            info.GetReturnValue().Set(Nan::New("").ToLocalChecked());
            return;
        } catch (std::exception const& ex) {
            return Nan::ThrowTypeError(ex.what());
        }
    }

    static inline Nan::Persistent<v8::Function> & constructor() {
        static Nan::Persistent<v8::Function> my_constructor;
        return my_constructor;
    }

};

NAN_METHOD(decode) {
  try {
      Nan::Utf8String utf8_value(info[0]);
      int len = utf8_value.length();
      if (len <= 0) {
          // This is done to match behavior of JS module
          info.GetReturnValue().Set(Nan::New("").ToLocalChecked());
      } else {
          std::string output;
          std::size_t ulen = static_cast<std::size_t>(len);
          size_t ascii_max_len = ulen * 4 + 1;
          output.resize(ascii_max_len);
          size_t final_size = unidecode(*utf8_value, ulen, &output[0]);
          if (final_size > ascii_max_len) {
             // Should never happen, but if it did we need to know
            std::stringstream s;
            s << "preallocated space was too small " << ascii_max_len << " / " << final_size;
            return Nan::ThrowTypeError(s.str().c_str());
          }
          info.GetReturnValue().Set(Nan::New(output.data(),final_size).ToLocalChecked());
      }
  } catch (std::exception const& ex) {
      return Nan::ThrowTypeError(ex.what());
  }
}

NAN_METHOD(decodable) {
  try {
      Nan::Utf8String utf8_value(info[0]);
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


static NAN_MODULE_INIT(InitAll) {
  Decoder::Init(target);
  Nan::Set(target, Nan::New<v8::String>("decode").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(decode)).ToLocalChecked());
  Nan::Set(target, Nan::New<v8::String>("decodable").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(decodable)).ToLocalChecked());
}

NODE_MODULE(unidecode, InitAll)
