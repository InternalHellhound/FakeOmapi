#pragma once

#include <aidl/android/se/omapi/BnSecureElementSession.h>
#include <aidl/android/se/omapi/ISecureElementChannel.h>
#include <aidl/android/se/omapi/ISecureElementListener.h>
#include <aidl/android/se/omapi/ISecureElementReader.h>

using aidl::android::se::omapi::BnSecureElementSession;
using aidl::android::se::omapi::ISecureElementChannel;
using aidl::android::se::omapi::ISecureElementListener;
using aidl::android::se::omapi::ISecureElementReader;


namespace aidl::android::se {
class Channel;
class SecureElementChannel;
class SecureElementReader;
}

namespace aidl::android::se::omapi {
using aidl::android::se::SecureElementChannel;
using aidl::android::se::SecureElementReader;
using aidl::android::se::Channel;

class SecureElementSession : public BnSecureElementSession {
    public:
        SecureElementSession(SecureElementReader* reader);
        // virtual ~SecureElementSession() = default;
        ::ndk::ScopedAStatus getReader(std::shared_ptr<ISecureElementReader>* outReader);
        ::ndk::ScopedAStatus getAtr(std::vector<uint8_t>* outAtr);
        ::ndk::ScopedAStatus close();
        ::ndk::ScopedAStatus closeChannels();
        ::ndk::ScopedAStatus removeChannel(Channel* channel);
        ::ndk::ScopedAStatus removeChannels();
        ::ndk::ScopedAStatus isClosed(bool* isClosed);
        ::ndk::ScopedAStatus openBasicChannel(const std::vector<uint8_t>& aid, int8_t p2,
                                                const std::shared_ptr<ISecureElementListener>& listener, std::shared_ptr<ISecureElementChannel>* outChannel);
        ::ndk::ScopedAStatus openLogicalChannel(const std::vector<uint8_t>& aid, int8_t p2,
                                                const std::shared_ptr<ISecureElementListener>& listener, std::shared_ptr<ISecureElementChannel>* outChannel);

    private:
        std::shared_ptr<SecureElementReader> mReader;
        std::vector<Channel*> mChannels;
        std::mutex mLock;
        bool mIsClosed;
        std::vector<uint8_t> mAtr;
        std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
            std::vector<uint8_t> bytes;
            for (size_t i = 0; i < hex.length(); i += 2) {
                std::string byteString = hex.substr(i, 2);
                uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
                bytes.push_back(byte);
            }
            return bytes;
        }
        const std::vector<uint8_t> mUuid = hexStringToBytes(UUID_HEX);
};
}  // namespace aidl::android::se::omapi