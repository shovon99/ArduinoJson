// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2018
// MIT License

#pragma once

#include "../JsonVariant.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename Destination>
class MsgPackVisitor {
 public:
  MsgPackVisitor(Destination* output) : _output(output) {}

  void acceptFloat(JsonFloat /*value*/) {}

  void acceptArray(const JsonArray& /*array*/) {}

  void acceptObject(const JsonObject& /*object*/) {}

  void acceptString(const char* /*value*/) {
    write(0xC0);
  }

  void acceptRawJson(const char* /*value*/) {}

  void acceptNegativeInteger(JsonUInt value) {
    JsonUInt negated = JsonUInt(~value + 1);
    if (value <= 0x20) {
      write(uint8_t(negated));
    } else if (value <= 0x80) {
      write(0xD0);
      write(uint8_t(negated));
    } else if (value <= 0x8000) {
      write(0xD1);
      write(uint8_t(negated >> 8));
      write(uint8_t(negated));
    } else if (value <= 0x80000000) {
      write(0xD2);
      write(uint8_t(negated >> 24));
      write(uint8_t(negated >> 16));
      write(uint8_t(negated >> 8));
      write(uint8_t(negated));
    }
  }

  void acceptPositiveInteger(JsonUInt value) {
    if (value <= 0x7F) {
      write(uint8_t(value));
    } else if (value <= 0xFF) {
      write(0xCC);
      write(uint8_t(value));
    } else if (value <= 0xFFFF) {
      write(0xCD);
      write(uint8_t(value >> 8));
      write(uint8_t(value));
    } else if (value <= 0xFFFFFFFF) {
      write(0xCE);
      write(uint8_t(value >> 24));
      write(uint8_t(value >> 16));
      write(uint8_t(value >> 8));
      write(uint8_t(value));
    }
#if ARDUINOJSON_USE_LONG_LONG || ARDUINOJSON_USE_INT64
    else {
      write(0xCF);
      write(uint8_t(value >> 56));
      write(uint8_t(value >> 48));
      write(uint8_t(value >> 40));
      write(uint8_t(value >> 32));
      write(uint8_t(value >> 24));
      write(uint8_t(value >> 16));
      write(uint8_t(value >> 8));
      write(uint8_t(value));
    }
#endif
  }

  void acceptBoolean(bool value) {
    _output->push_back(static_cast<char>(value ? 0xC3 : 0xC2));
  }

  void acceptUndefined() {
    write(0xC0);
  }

 private:
  void write(uint8_t c) {
    _output->push_back(char(c));
  }

  Destination* _output;
};
}  // namespace Internals

template <typename Destination>
inline size_t serializeMsgPack(const JsonVariant& variant,
                               Destination& output) {
  variant.visit(Internals::MsgPackVisitor<Destination>(&output));
  return output.size();
}

}  // namespace ArduinoJson
