/*
 * Created by Steve Tchatchouang
 *
 * Copyright (c) 2022 All rights reserved
 */

#include "ByteStream.h"
#include "ProtoBuffer.h"

void ByteStream::append(ProtoBuffer *buffer) {
    if (buffer == nullptr) {
        return;
    }
    m_buffers_queue.push_back(buffer);
}

bool ByteStream::has_data() {
    size_t size = m_buffers_queue.size();
    for (uint32_t a = 0; a < size; a++) {
        if (m_buffers_queue[a]->has_remaining()) {
            return true;
        }
    }
    return false;
}

void ByteStream::get(ProtoBuffer *dst) {
    if (dst == nullptr) {
        return;
    }

    size_t size = m_buffers_queue.size();
    ProtoBuffer *buffer;
    for (uint32_t a = 0; a < size; a++) {
        buffer = m_buffers_queue[a];
        if (buffer->remaining() > dst->remaining()) {
            dst->write_bytes(buffer->bytes(), buffer->position(), dst->remaining());
            break;
        }
        dst->write_bytes(buffer->bytes(), buffer->position(), buffer->remaining());
        if (!dst->has_remaining()) {
            break;
        }
    }
}

void ByteStream::discard(uint32_t count) {
    uint32_t remaining;
    ProtoBuffer *buffer;
    while (count > 0) {
        if (m_buffers_queue.empty()) {
            break;
        }
        buffer = m_buffers_queue[0];
        remaining = buffer->remaining();
        if (count < remaining) {
            buffer->position(buffer->position() + count);
            break;
        }
        buffer->reuse();
        m_buffers_queue.erase(m_buffers_queue.begin());
        count -= remaining;
    }
}

void ByteStream::clean() {
    if (m_buffers_queue.empty()) {
        return;
    }
    size_t size = m_buffers_queue.size();
    for (uint32_t a = 0; a < size; a++) {
        ProtoBuffer *buffer = m_buffers_queue[a];
        buffer->reuse();
    }
    m_buffers_queue.clear();
}
