/*
 * Created by Steve Tchatchouang
 *
 * Copyright (c) 2022 All rights reserved
 */

#ifndef TKS_BUFFERS_STORAGE_H
#define TKS_BUFFERS_STORAGE_H

#include "ProtoBuffer.h"
#include <memory>

#include <vector>
#include <cstdint>

class BuffersStorage {
public:
    ProtoBuffer* get_free_buffer(uint32_t size);
    void reuse_free_buffer(ProtoBuffer *buffer);
    static BuffersStorage &get();
private:
    explicit BuffersStorage(bool thread_safe);
    std::vector<std::unique_ptr<ProtoBuffer>> m_free_buffers8;
    std::vector<std::unique_ptr<ProtoBuffer>> m_free_buffers128;
    std::vector<std::unique_ptr<ProtoBuffer>> m_free_buffers1024;
    std::vector<std::unique_ptr<ProtoBuffer>> m_free_buffers4096;
    std::vector<std::unique_ptr<ProtoBuffer>> m_free_buffers16384;
    std::vector<std::unique_ptr<ProtoBuffer>> m_free_buffers32768;
    std::vector<std::unique_ptr<ProtoBuffer>> m_free_buffersBig;

    bool m_is_thread_safe = true;
    pthread_mutex_t m_mutex{};
};
#endif //TKS_BUFFERS_STORAGE_H
