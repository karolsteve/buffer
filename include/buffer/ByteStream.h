/*
 * Created by Steve Tchatchouang
 *
 * Copyright (c) 2022 All rights reserved
 */

#ifndef TKS_PROTO_BUFFER_BYTESTREAM_H
#define TKS_PROTO_BUFFER_BYTESTREAM_H

#include <vector>
#include <cstdint>

class ProtoBuffer;

class ByteStream {
public:
    ByteStream() = default;

    ~ByteStream() = default;

    void append(ProtoBuffer *buffer);

    bool has_data();

    void get(ProtoBuffer *dst);

    void discard(uint32_t count);

    void clean();

private:
    std::vector<ProtoBuffer *> m_buffers_queue;
};

#endif //TKS_PROTO_BUFFER_BYTESTREAM_H
