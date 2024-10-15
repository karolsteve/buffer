/*
 * Created by Steve Tchatchouang
 *
 * Copyright (c) 2022 All rights reserved
 */

#include "ProtoBuffer.h"

#include "Bytes.h"
#include "BuffersStorage.h"
#ifdef ANDROID
#endif
#include <cstdlib>
#include <memory>
#include <memory.h>

ProtoBuffer::ProtoBuffer(uint32_t size) {
#ifdef ANDROID
    if (jclass_ByteBuffer != nullptr) {
        JNIEnv *env = nullptr;
        if (javaVm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
            DEBUG_E("can't get jnienv");
            exit(1);
        }
        m_java_byte_buffer = env->CallStaticObjectMethod(jclass_ByteBuffer,
                                                         jclass_ByteBuffer_allocateDirect, size);
        if (m_java_byte_buffer == nullptr) {
            DEBUG_E("can't create javaByteBuffer");
            exit(1);
        }
        jobject globalRef = env->NewGlobalRef(m_java_byte_buffer);
        env->DeleteLocalRef(m_java_byte_buffer);
        m_java_byte_buffer = globalRef;
        m_buffer = (uint8_t *) env->GetDirectBufferAddress(m_java_byte_buffer);
        m_buffer_owner = false;
    } else {
#endif
        m_buffer = new uint8_t[size];
        m_buffer_owner = true;
#ifdef ANDROID
    }
#endif
    if (m_buffer == nullptr) {
        exit(1);
    }
    m_limit = m_capacity = size;
}

ProtoBuffer::ProtoBuffer(bool calculate) : m_calculate_size_only(calculate) {

}

ProtoBuffer::ProtoBuffer(uint8_t *buff, uint32_t len) {
    m_buffer = buff;
    m_sliced = true;
    m_limit = m_capacity = len;
}

ProtoBuffer::~ProtoBuffer() {
#ifdef ANDROID
    if (m_java_byte_buffer != nullptr) {
        JNIEnv *env = 0;
        if (javaVm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
            DEBUG_E("can't get jnienv");
            exit(1);
        }
        env->DeleteGlobalRef(m_java_byte_buffer);
        m_java_byte_buffer = nullptr;
    }
#endif
    if (m_buffer_owner && !m_sliced && m_buffer != nullptr) {
        delete[] m_buffer;
        m_buffer = nullptr;
    }
}

void ProtoBuffer::write_bytes_internal(uint8_t *b, uint32_t offset, uint32_t len) {
    memcpy(m_buffer + m_position, b + offset, sizeof(uint8_t) * len);
    m_position += len;
}

uint32_t ProtoBuffer::position() const {
    return m_position;
}

void ProtoBuffer::position(uint32_t position) {
    if (position > m_limit) {
        return;
    }
    m_position = position;
}

uint32_t ProtoBuffer::limit() const {
    return m_limit;
}

void ProtoBuffer::limit(uint32_t limit) {
    if (limit > m_capacity) {
        return;
    }
    if (m_position > limit) {
        m_position = limit;
    }
    m_limit = limit;
}

uint32_t ProtoBuffer::capacity() const {
    return m_capacity;
}

uint32_t ProtoBuffer::remaining() const {
    return m_limit - m_position;
}

bool ProtoBuffer::has_remaining() const {
    return m_position < m_limit;
}

void ProtoBuffer::rewind() {
    m_position = 0;
}

void ProtoBuffer::compact() {
    if (m_position == m_limit) {
        return;
    }
    memmove(m_buffer, m_buffer + m_position, sizeof(uint8_t) * (m_limit - m_position));
    m_position = (m_limit - m_position);
    m_limit = m_capacity;
}

void ProtoBuffer::flip() {
    m_limit = m_position;
    m_position = 0;
}

void ProtoBuffer::clear() {
    m_position = 0;
    m_limit = m_capacity;
}

void ProtoBuffer::skip(uint32_t len) {
    if (!m_calculate_size_only) {
        if (m_position + len > m_limit) {
            return;
        }
        m_position += len;
    } else {
        m_capacity += len;
    }
}

void ProtoBuffer::clear_capacity() {
    if (!m_calculate_size_only) {
        return;
    }
    m_capacity = 0;
}

uint8_t *ProtoBuffer::bytes() {
    return m_buffer;
}

void ProtoBuffer::write_int(int32_t x, bool *error) {
    if (!m_calculate_size_only) {
        if (m_position + 4 > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("write int32 error");
            return;
        }
        m_buffer[m_position++] = (uint8_t) x;
        m_buffer[m_position++] = (uint8_t) (x >> 8);
        m_buffer[m_position++] = (uint8_t) (x >> 16);
        m_buffer[m_position++] = (uint8_t) (x >> 24);
    } else {
        m_capacity += 4;
    }
}

void ProtoBuffer::write_int_BE(int32_t x, bool *error) {
    if (!m_calculate_size_only) {
        if (m_position + 4 > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("write int32 error");
            return;
        }
        m_buffer[m_position++] = (uint8_t) (x >> 24);
        m_buffer[m_position++] = (uint8_t) (x >> 16);
        m_buffer[m_position++] = (uint8_t) (x >> 8);
        m_buffer[m_position++] = (uint8_t) x;
    } else {
        m_capacity += 4;
    }
}

void ProtoBuffer::write_int(uint32_t x, bool *error) {
    write_int((int32_t) x, error);
}

void ProtoBuffer::write_long(int64_t x, bool *error) {
    if (!m_calculate_size_only) {
        if (m_position + 8 > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("write int64 error");
            return;
        }
        m_buffer[m_position++] = (uint8_t) x;
        m_buffer[m_position++] = (uint8_t) (x >> 8);
        m_buffer[m_position++] = (uint8_t) (x >> 16);
        m_buffer[m_position++] = (uint8_t) (x >> 24);
        m_buffer[m_position++] = (uint8_t) (x >> 32);
        m_buffer[m_position++] = (uint8_t) (x >> 40);
        m_buffer[m_position++] = (uint8_t) (x >> 48);
        m_buffer[m_position++] = (uint8_t) (x >> 56);
    } else {
        m_capacity += 8;
    }
}

void ProtoBuffer::write_bytes(uint8_t *bytes, uint32_t len, bool *error) {
    if (!m_calculate_size_only) {
        if (m_position + len > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("write bytes error");
            return;
        }
        write_bytes_internal(bytes, 0, len);
    } else {
        m_capacity += len;
    }
}

void ProtoBuffer::write_bytes(uint8_t *bytes, uint32_t offset, uint32_t len, bool *error) {
    if (!m_calculate_size_only) {
        if (m_position + len > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("write bytes error");
            return;
        }
        write_bytes_internal(bytes, offset, len);
    } else {
        m_capacity += len;
    }
}

void ProtoBuffer::write_bytes(Bytes *bytes, bool *error) {
    if (!m_calculate_size_only) {
        if (m_position + bytes->len() > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("write bytes error");
            return;
        }
        write_bytes_internal(bytes->bytes(), 0, bytes->len());
    } else {
        m_capacity += bytes->len();
    }
}

void ProtoBuffer::write_bytes(ProtoBuffer *buff, bool *error) {
    uint32_t length = buff->m_limit - buff->m_position;
    if (length == 0) {
        return;
    }
    if (!m_calculate_size_only) {
        if (m_position + length > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("write bytes error");
            return;
        }
        write_bytes_internal(buff->m_buffer + buff->m_position, 0, length);
        buff->position(buff->limit());
    } else {
        m_capacity += length;
    }
}

void ProtoBuffer::write_byte(uint8_t i, bool *error) {
    if (!m_calculate_size_only) {
        if (m_position + 1 > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("write byte error");
            return;
        }
        m_buffer[m_position++] = i;
    } else {
        m_capacity += 1;
    }
}

void ProtoBuffer::write_string(const std::string& s, bool *error) {
    write_byte_array((uint8_t *) s.c_str(), (uint32_t) s.length(), error);
}

void ProtoBuffer::write_byte_array(uint8_t *b, uint32_t offset, uint32_t len, bool *error) {
    if (len <= 253) {
        if (!m_calculate_size_only) {
            if (m_position + 1 > m_limit) {
                if (error != nullptr) {
                    *error = true;
                }
                DEBUG_E("write byte array error");
                return;
            }
            m_buffer[m_position++] = (uint8_t) len;
        } else {
            m_capacity += 1;
        }
    } else {
        if (!m_calculate_size_only) {
            if (m_position + 4 > m_limit) {
                if (error != nullptr) {
                    *error = true;
                }
                DEBUG_E("write byte array error");
                return;
            }
            m_buffer[m_position++] = (uint8_t) 254;
            m_buffer[m_position++] = (uint8_t) len;
            m_buffer[m_position++] = (uint8_t) (len >> 8);
            m_buffer[m_position++] = (uint8_t) (len >> 16);
        } else {
            m_capacity += 4;
        }
    }
    if (!m_calculate_size_only) {
        if (m_position + len > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("write byte array error");
            return;
        }
        write_bytes_internal(b, offset, len);
    } else {
        m_capacity += len;
    }
    uint32_t addition = (len + (len <= 253 ? 1 : 4)) % 4;
    if (addition != 0) {
        addition = 4 - addition;
    }
    if (!m_calculate_size_only && m_position + addition > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("write byte array error");
        return;
    }
    for (uint32_t a = 0; a < addition; a++) {
        if (!m_calculate_size_only) {
            m_buffer[m_position++] = (uint8_t) 0;
        } else {
            m_capacity += 1;
        }
    }
}

void ProtoBuffer::write_byte_array(uint8_t *b, uint32_t len, bool *error) {
    write_byte_array(b, 0, len, error);
}

void ProtoBuffer::write_byte_array(Bytes *b, bool *error) {
    write_byte_array(b->bytes(), 0, b->len(), error);
}

void ProtoBuffer::write_byte_array(ProtoBuffer *buff, bool *error) {
    buff->rewind();
    write_byte_array(buff->m_buffer, 0, buff->limit(), error);
}

void ProtoBuffer::write_double(double d, bool *error) {
    int64_t value;
    memcpy(&value, &d, sizeof(int64_t));
    write_long(value, error);
}

int32_t ProtoBuffer::read_int(bool *error) {
    if (m_position + 4 > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read int32 error");
        return 0;
    }
    int32_t result = ((m_buffer[m_position] & 0xff)) |
                     ((m_buffer[m_position + 1] & 0xff) << 8) |
                     ((m_buffer[m_position + 2] & 0xff) << 16) |
                     ((m_buffer[m_position + 3] & 0xff) << 24);
    m_position += 4;
    return result;
}

uint32_t ProtoBuffer::read_u_int(bool *error) {
    return (uint32_t) read_int(error);
}

int32_t ProtoBuffer::read_int_BE(bool *error) {
    if (m_position + 4 > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read big int32 error");
        return 0;
    }
    int32_t result = ((m_buffer[m_position] & 0xff) << 24) |
                     ((m_buffer[m_position + 1] & 0xff) << 16) |
                     ((m_buffer[m_position + 2] & 0xff) << 8) |
                     ((m_buffer[m_position + 3] & 0xff));
    m_position += 4;
    return result;
}

int64_t ProtoBuffer::read_long(bool *error) {
    if (m_position + 4 > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read int64 error");
        return 0;
    }
    int64_t result = ((int64_t) (m_buffer[m_position] & 0xff)) |
                     ((int64_t) (m_buffer[m_position + 1] & 0xff) << 8) |
                     ((int64_t) (m_buffer[m_position + 2] & 0xff) << 16) |
                     ((int64_t) (m_buffer[m_position + 3] & 0xff) << 24) |
                     ((int64_t) (m_buffer[m_position + 4] & 0xff) << 32) |
                     ((int64_t) (m_buffer[m_position + 5] & 0xff) << 40) |
                     ((int64_t) (m_buffer[m_position + 6] & 0xff) << 48) |
                     ((int64_t) (m_buffer[m_position + 7] & 0xff) << 56);
    m_position += 8;
    return result;
}

uint8_t ProtoBuffer::read_byte(bool *error) {
    if (m_position + 1 > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read byte error");
        return 0;
    }
    return m_buffer[m_position++];
}

void ProtoBuffer::read_bytes(uint8_t *b, uint32_t len, bool *error) {
    if (m_position + len > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read bytes error");
        return;
    }
    memcpy(b, m_buffer + m_position, len);
    m_position += len;
}

Bytes *ProtoBuffer::read_bytes(uint32_t len, bool *error) {
    if (m_position + len > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read bytes error");
        return nullptr;
    }
    auto *byteArray = new Bytes(len);
    memcpy(byteArray->bytes(), m_buffer + m_position, sizeof(uint8_t) * len);
    m_position += len;
    return byteArray;
}

std::string ProtoBuffer::read_string(bool *error) {
    uint32_t sl = 1;
    if (m_position + 1 > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read string error. position is %d. limit is %d", m_position, m_limit);
        return "";
    }
    uint32_t l = m_buffer[m_position++];
    if (l >= 254) {
        if (m_position + 3 > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("read string error2");
            return "";
        }
        l = m_buffer[m_position] | (m_buffer[m_position + 1] << 8) | (m_buffer[m_position + 2] << 16);
        m_position += 3;
        sl = 4;
    }
    uint32_t addition = (l + sl) % 4;
    if (addition != 0) {
        addition = 4 - addition;
    }
    if (m_position + l + addition > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read string error position %d limit %d", m_position, m_limit);
        return "";
    }
    std::string result = std::string((const char *) (m_buffer + m_position), l);
    m_position += l + addition;
    return result;
}

Bytes *ProtoBuffer::read_byte_array(bool *error) {
    uint32_t sl = 1;
    if (m_position + 1 > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read byte array error");
        return nullptr;
    }
    uint32_t l = m_buffer[m_position++];
    if (l >= 254) {
        if (m_position + 3 > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("read byte array error");
            return nullptr;
        }
        l = m_buffer[m_position] | (m_buffer[m_position + 1] << 8) |
            (m_buffer[m_position + 2] << 16);
        m_position += 3;
        sl = 4;
    }
    uint32_t addition = (l + sl) % 4;
    if (addition != 0) {
        addition = 4 - addition;
    }
    if (m_position + l + addition > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read byte array error");
        return nullptr;
    }
    auto *result = new Bytes(l);
    memcpy(result->bytes(), m_buffer + m_position, sizeof(uint8_t) * l);
    m_position += l + addition;
    return result;
}

ProtoBuffer *ProtoBuffer::read_proto_buff(bool copy, bool *error) {
    uint32_t sl = 1;
    if (m_position + 1 > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read byte buffer error");
        return nullptr;
    }
    uint32_t l = m_buffer[m_position++];
    if (l >= 254) {
        if (m_position + 3 > m_limit) {
            if (error != nullptr) {
                *error = true;
            }
            DEBUG_E("read byte buffer error");
            return nullptr;
        }
        l = m_buffer[m_position] | (m_buffer[m_position + 1] << 8) |
            (m_buffer[m_position + 2] << 16);
        m_position += 3;
        sl = 4;
    }
    uint32_t addition = (l + sl) % 4;
    if (addition != 0) {
        addition = 4 - addition;
    }
    if (m_position + l + addition > m_limit) {
        if (error != nullptr) {
            *error = true;
        }
        DEBUG_E("read byte buffer error");
        return nullptr;
    }
    ProtoBuffer *result;
    if (copy) {
        result = BuffersStorage::get().get_free_buffer(l);
        memcpy(result->m_buffer, m_buffer + m_position, sizeof(uint8_t) * l);
    } else {
        result = new ProtoBuffer(m_buffer + m_position, l);
    }
    m_position += l + addition;
    return result;
}

double ProtoBuffer::read_double(bool *error) {
    double value;
    int64_t value2 = read_long(error);
    memcpy(&value, &value2, sizeof(double));
    return value;
}

void ProtoBuffer::reuse() {
    if (m_sliced) {
        return;
    }
    BuffersStorage::get().reuse_free_buffer(this);
}

#ifdef ANDROID

jobject ProtoBuffer::get_java_byte_buffer() {
    if (m_java_byte_buffer == nullptr && javaVm != nullptr) {
        JNIEnv *env = 0;
        if (javaVm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
            DEBUG_E("can't get jnienv");
            exit(1);
        }
        m_java_byte_buffer = env->NewDirectByteBuffer(m_buffer, m_capacity);
        if (m_java_byte_buffer == nullptr) {
            DEBUG_E("can't allocate NativeByteBuffer buffer");
            exit(1);
        }
        jobject globalRef = env->NewGlobalRef(m_java_byte_buffer);
        env->DeleteLocalRef(m_java_byte_buffer);
        m_java_byte_buffer = globalRef;
    }
    return m_java_byte_buffer;
}

#endif