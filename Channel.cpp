#include "Channel.h"

namespace aidl::android::se {
int Channel::getChannelNumber() const {
    LOG(INFO) << __func__;
    return 0;
}

void Channel::close() const {
    LOG(INFO) << __func__;
}


SecureElementChannel::SecureElementChannel(const std::shared_ptr<Channel>& channel) {
    LOG(INFO) << __func__;
}

ndk::ScopedAStatus SecureElementChannel::close() {
    LOG(INFO) << __func__;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus SecureElementChannel::isClosed(bool* _aidl_return) {
    LOG(INFO) << __func__;
    return ndk::ScopedAStatus::ok();
}
ndk::ScopedAStatus SecureElementChannel::isBasicChannel(bool* _aidl_return) {
    LOG(INFO) << __func__;
    return ndk::ScopedAStatus::ok();
}
ndk::ScopedAStatus SecureElementChannel::getSelectResponse(std::vector<uint8_t>* _aidl_return) {
    LOG(INFO) << __func__;
    return ndk::ScopedAStatus::ok();
}
ndk::ScopedAStatus SecureElementChannel::transmit(const std::vector<uint8_t>& command, std::vector<uint8_t>* outResponse) {
    LOG(INFO) << __func__;
    return ndk::ScopedAStatus::ok();
}
ndk::ScopedAStatus SecureElementChannel::selectNext(bool* _aidl_return) {
    LOG(INFO) << __func__;
    return ndk::ScopedAStatus::ok();
}
}