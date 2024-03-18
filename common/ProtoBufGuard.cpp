
#include <ProtoBufGuard.h>

#include <google/protobuf/stubs/common.h>

ProtoBufGuard::ProtoBufGuard()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

ProtoBufGuard::~ProtoBufGuard()
{
    google::protobuf::ShutdownProtobufLibrary();
}
