/*
 * Created by Steve Tchatchouang
 *
 * Copyright (c) 2022 All rights reserved
 */

#include "Bytes.h"
#include <memory>
#include <memory.h>

Bytes::Bytes():m_bytes(nullptr), m_len(0) {

}

Bytes::Bytes(uint32_t length): m_bytes(new uint8_t[length]), m_len(length) {
}

Bytes::Bytes(uint8_t *buffer, uint32_t len) : m_bytes(new uint8_t[len]), m_len(len){
    memcpy(m_bytes, buffer, len);
}

Bytes::Bytes(const void *buffer, uint32_t len) : m_bytes(new uint8_t[len]), m_len(len){
    memcpy(m_bytes, buffer, len);
}

Bytes::~Bytes() {
    if(m_bytes != nullptr){
        delete[] m_bytes;
        m_bytes = nullptr;
    }
}

Bytes::Bytes(Bytes *bytes) : m_bytes(new uint8_t[bytes->m_len]), m_len(bytes->m_len){
    memcpy(m_bytes, bytes->m_bytes, m_len);
}

bool Bytes::equals(Bytes *bytes) {
    return m_len == bytes->m_len && !memcmp(m_bytes, bytes->m_bytes, m_len);
}

uint32_t Bytes::len() const {
    return m_len;
}

uint8_t *Bytes::bytes() const{
    return m_bytes;
}

void Bytes::alloc(uint32_t len) {
    if(m_bytes != nullptr){
        delete[] m_bytes;
        m_bytes = nullptr;
    }

    m_bytes = new uint8_t[len];
    m_len = len;
}