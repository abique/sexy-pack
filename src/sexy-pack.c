#include <string.h>

#include <sexy-pack.h>

bool
sp_serializer_dict(struct sp_serializer *s)
{
  if (!s->out || s->out + 1 > s->out_end)
    return false;
  *s->out = SP_SERIALIZE_DICT;
  ++s->out;
  return true;
}

bool
sp_serializer_array(struct sp_serializer *s)
{
  if (!s->out || s->out + 1 > s->out_end)
    return false;
  *s->out = SP_SERIALIZE_ARRAY;
  ++s->out;
  return true;
}

bool
sp_serializer_end(struct sp_serializer *s)
{
  if (!s->out || s->out + 1 > s->out_end)
    return false;
  *s->out = SP_SERIALIZE_END;
  ++s->out;
  return true;
}

bool
sp_serializer_bool(struct sp_serializer *s, bool value)
{
  if (!s->out || s->out + 2 > s->out_end)
    return false;
  s->out[0] = SP_SERIALIZE_BOOL;
  s->out[1] = value ? true : false; /* ensure bit 0 */
  s->out += 2;
  return true;
}

bool
sp_serializer_int8(struct sp_serializer *s, int8_t value)
{
  if (!s->out || s->out + 2 > s->out_end)
    return false;
  s->out[0] = SP_SERIALIZE_INT8;
  s->out[1] = value;
  s->out += 2;
  return true;
}

static inline void
sp_serializer_int16_priv(struct sp_serializer *s, int16_t value)
{
  s->out[1] = value >> 8;
  s->out[2] = value;
}

bool
sp_serializer_int16(struct sp_serializer *s, int16_t value)
{
  if (!s->out || s->out + 3 >= s->out_end)
    return false;

  *s->out = SP_SERIALIZE_INT16;
  sp_serializer_int16_priv(s, value);
  s->out += 3;
  return true;
}

static inline void
sp_serializer_int32_priv(struct sp_serializer *s, int32_t value)
{
  s->out[1] = value >> 24;
  s->out[2] = value >> 16;
  s->out[3] = value >> 8;
  s->out[4] = value;
}

bool
sp_serializer_int32(struct sp_serializer *s, int32_t value)
{
  if (!s->out || s->out + 5 >= s->out_end)
    return false;

  *s->out = SP_SERIALIZE_INT32;
  sp_serializer_int32_priv(s, value);
  s->out += 5;
  return true;
}

static inline void
sp_serializer_int64_priv(struct sp_serializer *s, int64_t value)
{
  s->out[1] = value >> 56;
  s->out[2] = value >> 48;
  s->out[3] = value >> 40;
  s->out[4] = value >> 32;
  s->out[5] = value >> 24;
  s->out[6] = value >> 16;
  s->out[7] = value >> 8;
  s->out[8] = value;
}

bool
sp_serializer_int64(struct sp_serializer *s, int64_t value)
{
  if (!s->out || s->out + 9 >= s->out_end)
    return false;

  *s->out = SP_SERIALIZE_INT64;
  sp_serializer_int64_priv(s, value);
  s->out += 9;
  return true;
}

bool
sp_serializer_float(struct sp_serializer *s, float value)
{
  if (!s->out || s->out + 5 >= s->out_end)
    return false;

  union {
    float   f;
    int32_t i;
  } u;

  u.f = value;
  *s->out = SP_SERIALIZE_FLOAT;
  sp_serializer_int32_priv(s, u.i);
  s->out += 5;
  return true;
}

bool
sp_serializer_double(struct sp_serializer *s, double value)
{
  if (!s->out || s->out + 9 >= s->out_end)
    return false;

  union {
    double  f;
    int64_t i;
  } u;

  u.f = value;
  *s->out = SP_SERIALIZE_DOUBLE;
  sp_serializer_int64_priv(s, u.i);
  s->out += 9;
  return true;
}

bool
sp_serializer_str(struct sp_serializer *s,
                  const char           *str,
                  int32_t               len)
{
  if (!s->out || s->out + 5 + len >= s->out_end)
    return false;

  *s->out = SP_SERIALIZE_STR;
  sp_serializer_int32_priv(s, len);
  memcpy(s->out + 5, str, len);
  s->out += 5 + len;
  return true;
}

bool
sp_deserialize(struct sp_deserializer *d)
{
  if (!d->in || !d->in_end || d->in >= d->in_end) {
    d->type = SP_SERIALIZE_EOB;
    return false;
  }

  d->type = (enum sp_serialize_type)*d->in;
  switch (d->type) {
  case SP_SERIALIZE_DICT:
    ++d->in;
    return true;

  case SP_SERIALIZE_ARRAY:
    ++d->in;
    return true;

  case SP_SERIALIZE_END:
    ++d->in;
    return true;

  case SP_SERIALIZE_BOOL:
  case SP_SERIALIZE_INT8:
    if (d->in + 5 > d->in_end) {
      d->type = SP_SERIALIZE_EOB;
      return false;
    }
    d->b   = d->in[1];
    d->in += 2;
    return true;

  case SP_SERIALIZE_INT16:
    if (d->in + 3 > d->in_end) {
      d->type = SP_SERIALIZE_EOB;
      return false;
    }
    d->i16 = (d->in[3] << 8) | d->in[4];
    d->in += 3;
    return true;

  case SP_SERIALIZE_INT32:
  case SP_SERIALIZE_FLOAT:
    if (d->in + 5 > d->in_end) {
      d->type = SP_SERIALIZE_EOB;
      return false;
    }
    d->i32 = (d->in[1] << 24) | (d->in[2] << 16) | (d->in[3] << 8) | d->in[4];
    d->in += 5;
    return true;

  case SP_SERIALIZE_INT64:
  case SP_SERIALIZE_DOUBLE:
    if (d->in + 9 > d->in_end) {
      d->type = SP_SERIALIZE_EOB;
      return false;
    }
    d->i64 = (d->in[1] << 56) | (d->in[2] << 48) | (d->in[3] << 40) | (d->in[4] << 32) |
      (d->in[1] << 24) | (d->in[2] << 16) | (d->in[3] << 8) | d->in[4];
    d->in += 9;
    return true;

  case SP_SERIALIZE_STR:
    if (d->in + 5 > d->in_end) {
      d->type = SP_SERIALIZE_EOB;
      return false;
    }
    d->slen = (d->in[1] << 24) | (d->in[2] << 16) | (d->in[3] << 8) | d->in[4];
    if (d->in + 5 + d->slen > d->in_end) {
      d->type = SP_SERIALIZE_EOB;
      return false;
    }
    d->s  = (const char *)d->in + 5;
    d->in = d->in + 5 + d->slen;
    return true;

  default:
    d->type = SP_SERIALIZE_ERROR;
    return false;
  }
}
