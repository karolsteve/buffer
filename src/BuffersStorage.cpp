/*
 * Created by Steve Tchatchouang
 *
 * Copyright (c) 2022 All rights reserved
 */

#include "BuffersStorage.h"

BuffersStorage::BuffersStorage(bool thread_safe) : m_is_thread_safe(thread_safe)
{
    if (thread_safe) {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    for (uint32_t a = 0; a < 4; a++)
    {
        m_free_buffers8.emplace_back(new ProtoBuffer((uint32_t)8));
    }
    for (uint32_t a = 0; a < 5; a++)
    {
        m_free_buffers128.emplace_back(new ProtoBuffer((uint32_t)128));
    }
}

ProtoBuffer *BuffersStorage::get_free_buffer(uint32_t size)
{
    uint32_t byteCount;
    std::vector<std::unique_ptr<ProtoBuffer>> *arrayToGetFrom = nullptr;
    ProtoBuffer *buffer = nullptr;
    if (size <= 8)
    {
        arrayToGetFrom = &m_free_buffers8;
        byteCount = 8;
    }
    else if (size <= 128)
    {
        arrayToGetFrom = &m_free_buffers128;
        byteCount = 128;
    }
    else if (size <= 1024 + 200)
    {
        arrayToGetFrom = &m_free_buffers1024;
        byteCount = 1024 + 200;
    }
    else if (size <= 4096 + 200)
    {
        arrayToGetFrom = &m_free_buffers4096;
        byteCount = 4096 + 200;
    }
    else if (size <= 16384 + 200)
    {
        arrayToGetFrom = &m_free_buffers16384;
        byteCount = 16384 + 200;
    }
    else if (size <= 40000)
    {
        arrayToGetFrom = &m_free_buffers32768;
        byteCount = 40000;
    }
    else if (size <= 160000)
    {
        arrayToGetFrom = &m_free_buffersBig;
        byteCount = 160000;
    }
    else
    {
        buffer = new ProtoBuffer(size);
    }

    if (arrayToGetFrom != nullptr)
    {
        if (m_is_thread_safe) {
            pthread_mutex_lock(&m_mutex);
        }

        if (!arrayToGetFrom->empty())
        {
            buffer = (*arrayToGetFrom)[0].release();
            arrayToGetFrom->erase(arrayToGetFrom->begin());
        }

         if (m_is_thread_safe) {
            pthread_mutex_unlock(&m_mutex);
        }

        if (buffer == nullptr)
        {
            buffer = new ProtoBuffer(byteCount);
            DEBUG_D("launch new %u buffer", byteCount);
        }
    }
    if (buffer != nullptr)
    {
        buffer->limit(size);
        buffer->rewind();
    }
    return buffer;
}

void BuffersStorage::reuse_free_buffer(ProtoBuffer *buffer)
{
    if (buffer == nullptr)
    {
        return;
    }
    std::vector<std::unique_ptr<ProtoBuffer>> *arrayToReuse = nullptr;
    uint32_t capacity = buffer->capacity();
    uint32_t maxCount = 10;
    if (capacity == 8)
    {
        arrayToReuse = &m_free_buffers8;
        maxCount = 80;
    }
    else if (capacity == 128)
    {
        arrayToReuse = &m_free_buffers128;
        maxCount = 80;
    }
    else if (capacity == 1024 + 200)
    {
        arrayToReuse = &m_free_buffers1024;
    }
    else if (capacity == 4096 + 200)
    {
        arrayToReuse = &m_free_buffers4096;
    }
    else if (capacity == 16384 + 200)
    {
        arrayToReuse = &m_free_buffers16384;
    }
    else if (capacity == 40000)
    {
        arrayToReuse = &m_free_buffers32768;
    }
    else if (capacity == 160000)
    {
        arrayToReuse = &m_free_buffersBig;
    }
    if (arrayToReuse != nullptr)
    {
        if (m_is_thread_safe) {
            pthread_mutex_lock(&m_mutex);
        }

        if (arrayToReuse->size() < maxCount)
        {
            arrayToReuse->emplace_back(buffer);
        }
        else
        {
            DEBUG_D("too more %d buffers", capacity);
            delete buffer;
        }

        if (m_is_thread_safe) {
            pthread_mutex_unlock(&m_mutex);
        }
    }
    else
    {
        delete buffer;
    }
}

BuffersStorage &BuffersStorage::get()
{
    static BuffersStorage instance{true};
    return instance;
}
