//
// Created by Steve Tchatchouang on 03/10/2022.
//

#ifndef TKS_PROTO_BUFFER_H
#define TKS_PROTO_BUFFER_H

#include <cstdint>
#include <string>
#include "fastlog/FastLog.h"

#ifdef ANDROID

#include <jni.h>

#endif
class Bytes;

class ProtoBuffer
{
private:
    void write_bytes_internal(uint8_t *b, uint32_t offset, uint32_t len);

    uint8_t *m_buffer{nullptr};
    bool m_calculate_size_only{false};
    bool m_sliced{false};
    uint32_t m_position{0};
    uint32_t m_limit{0};
    uint32_t m_capacity{0};
    bool m_buffer_owner{true};
#ifdef ANDROID
    jobject m_java_byte_buffer{nullptr};
#endif
public:
    explicit ProtoBuffer(uint32_t size);

    explicit ProtoBuffer(bool calculate);

    ProtoBuffer(ProtoBuffer &) = delete;
    ProtoBuffer &operator=(ProtoBuffer const &) = delete;

    ProtoBuffer(uint8_t *buff, uint32_t len);

    ~ProtoBuffer();

    [[nodiscard]] uint32_t position() const;

    void position(uint32_t position);

    [[nodiscard]] uint32_t limit() const;

    void limit(uint32_t limit);

    [[nodiscard]] uint32_t capacity() const;

    [[nodiscard]] uint32_t remaining() const;

    [[nodiscard]] bool has_remaining() const;

    void rewind();

    void compact();

    void flip();

    void clear();

    void skip(uint32_t len);

    void clear_capacity();

    uint8_t *bytes();

    void write_int(int32_t x, bool *error = nullptr);
    void write_int_BE(int32_t x, bool *error = nullptr);

    void write_int(uint32_t x, bool *error = nullptr);

    void write_long(int64_t x, bool *error = nullptr);

    void write_bytes(uint8_t *bytes, uint32_t len, bool *error = nullptr);

    void write_bytes(uint8_t *bytes, uint32_t offset, uint32_t len, bool *error = nullptr);

    void write_bytes(Bytes *bytes, bool *error = nullptr);

    void write_bytes(ProtoBuffer *buff, bool *error = nullptr);

    void write_byte(uint8_t i, bool *error = nullptr);

    void write_string(const std::string& s, bool *error = nullptr);

    void write_byte_array(uint8_t *b, uint32_t offset, uint32_t len, bool *error = nullptr);

    void write_byte_array(uint8_t *b, uint32_t len, bool *error = nullptr);

    void write_byte_array(Bytes *b, bool *error = nullptr);

    void write_byte_array(ProtoBuffer *buff, bool *error = nullptr);

    void write_double(double d, bool *error = nullptr);

    int32_t read_int(bool *error = nullptr);
    
    uint32_t read_u_int(bool *error = nullptr);

    int32_t read_int_BE(bool *error = nullptr);

    int64_t read_long(bool *error = nullptr);

    uint8_t read_byte(bool *error = nullptr);

    void read_bytes(uint8_t *b, uint32_t len, bool *error = nullptr);

    Bytes *read_bytes(uint32_t len, bool *error = nullptr);

    std::string read_string(bool *error = nullptr);

    Bytes *read_byte_array(bool *error = nullptr);

    ProtoBuffer *read_proto_buff(bool copy, bool *error = nullptr);

    double read_double(bool *error = nullptr);

    void reuse();

#ifdef ANDROID

    jobject get_java_byte_buffer();

#endif
};

#endif // TKS_PROTO_BUFFER_H
