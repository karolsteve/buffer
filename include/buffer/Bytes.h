/*
 * Created by Steve Tchatchouang
 *
 * Copyright (c) 2022 All rights reserved
 */

#ifndef TKS_PROTO_BUFF_BYTES_H
#define TKS_PROTO_BUFF_BYTES_H

#include <cstdint>

class Bytes
{
private:
    uint8_t *m_bytes;
    uint32_t m_len;

public:
    Bytes();
    Bytes(Bytes &) = delete;
    Bytes &operator=(Bytes const &) = delete;

    explicit Bytes(uint32_t length);

    explicit Bytes(Bytes *bytes);

    Bytes(uint8_t *buffer, uint32_t len);

    Bytes(const void *buffer, uint32_t len);

    ~Bytes();

    void alloc(uint32_t len);

    [[nodiscard]] uint32_t len() const;

    [[nodiscard]] uint8_t *bytes() const;

    bool equals(Bytes *bytes);
};

#endif // TKS_PROTO_BUFF_BYTES_H
