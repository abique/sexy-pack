#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

enum sp_serialize_type
{
  SP_SERIALIZE_EOB   = 'B',
  SP_SERIALIZE_ERROR = 'E',

  SP_SERIALIZE_DICT  = 'd',
  SP_SERIALIZE_ARRAY = 'a',
  SP_SERIALIZE_END   = 'e',
  SP_SERIALIZE_STR   = 's',
  SP_SERIALIZE_BOOL  = 'b',
  SP_SERIALIZE_INT8  = 'c',
  SP_SERIALIZE_INT16 = 'j',
  SP_SERIALIZE_INT32 = 'i',
  SP_SERIALIZE_INT64 = 'l',

  /* IEEE 754 format */
  SP_SERIALIZE_FLOAT = 'f',
  SP_SERIALIZE_DOUBLE = 'g',
};

struct sp_serializer
{
  /* output buffer */
  uint8_t *out;
  uint8_t *out_end;
};

/* Begins a dictionary. Returns false on EOB. */
bool
sp_serializer_dict(struct sp_serializer *s);

/* Begins an array. Returns false on EOB. */
bool
sp_serializer_array(struct sp_serializer *s);

/* Ends an array or dictionary. Returns false on EOB. */
bool
sp_serializer_end(struct sp_serializer *s);

/* Pushes a string. Returns false on EOB.
 * The string might not be null terminated. */
bool
sp_serializer_str(struct sp_serializer *s,
                  const char           *str,
                  int32_t               len);

/* Pushes an 8 bits integer. Returns false on EOB. */
bool
sp_serializer_bool(struct sp_serializer *s, bool value);

/* Pushes an 8 bits integer. Returns false on EOB. */
bool
sp_serializer_int8(struct sp_serializer *s, int8_t value);

/* Pushes an 16 bits integer. Returns false on EOB. */
bool
sp_serializer_int16(struct sp_serializer *s, int16_t value);

/* Pushes an 32 bits integer. Returns false on EOB. */
bool
sp_serializer_int32(struct sp_serializer *s, int32_t value);

/* Pushes an 64 bits integer. Returns false on EOB. */
bool
sp_serializer_int64(struct sp_serializer *s, int64_t value);

/* Pushes a float. Returns false on EOB. */
bool
sp_serializer_float(struct sp_serializer *s, float value);

/* Pushes a double. Returns false on EOB. */
bool
sp_serializer_double(struct sp_serializer *s, double value);

struct sp_deserializer
{
  /* input buffer */
  const uint8_t *in;
  const uint8_t *in_end;

  /* decoded value */
  enum sp_serialize_type  type;
  union {
    bool                  b;
    int8_t                i8;
    int16_t               i16;
    int32_t               i32;
    int64_t               i64;
    float                 f;
    double                d;
    const char           *s;
  };
  int32_t                 slen;
};

/* Decodes one token into *d.
 * Returns false if the type is: SP_SERIALIZE_{ERROR,EOB}.
 */
bool
sp_deserialize(struct sp_deserializer *d);

#ifdef __cplusplus
}
#endif
