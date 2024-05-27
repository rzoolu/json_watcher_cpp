#pragma once

// RAII based protobuf init/deinit object. Releases protobuf resources
// upon destruction.
// Shall be used once in application linking to protobuf.

struct ProtoBufGuard
{
    ProtoBufGuard();
    ~ProtoBufGuard();
};
