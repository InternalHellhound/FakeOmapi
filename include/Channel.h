#pragma once

#include <aidl/android/se/omapi/BnSecureElementChannel.h>
#include <aidl/android/se/omapi/ISecureElementListener.h>

#include <binder/IBinder.h>

#include "Terminal.h"
#include "Session.h"

using aidl::android::se::omapi::BnSecureElementChannel;
using aidl::android::se::omapi::ISecureElementListener;

namespace aidl::android::se {
    class Channel : ::android::IBinder::DeathRecipient {
        public:
            Channel();
        };
}  // namespace aidl::android::se

// namespace aidl::android::se {
// class Channel : ::android::IBinder::DeathRecipient {
//     public:
//         Channel(SecureElementSession session, 
//             Terminal* terminal,
//             int channelNumber,
//             const std::vector<uint8_t>& selectResponse,
//             const std::vector<uint8_t>& aid,
//             const ISecureElementListener& listener);

//         ~Channel();

//         void binderDied();
//         void close();
//         std::vector<uint8_t> transmit(const std::vector<uint8_t>& command);
//         std::shared_ptr<ChannelAccess> getChannelAccess();
//         void setChannelAccess(std::shared_ptr<ChannelAccess> channelAccess);
//         bool hasSelectedAid();
//         int getChannelNumber();
//         std::vector<uint8_t> getSelectResponse();
//         bool isBasicChannel();
//         bool isClosed();
//         class SecureElementChannel : public BnSecureElementChannel {
//             public:
//                 SecureElementChannel(std::shared_ptr<Channel> channel);
//                 ndk::ScopedAStatus close();
//                 ndk::ScopedAStatus isClosed(bool* _aidl_return);
//                 ndk::ScopedAStatus isBasicChannel(bool* _aidl_return);
//                 ndk::ScopedAStatus getSelectResponse(std::vector<uint8_t>* _aidl_return);
//                 ndk::ScopedAStatus transmit(const std::vector<uint8_t>& command, std::vector<uint8_t>* outResponse);
//                 ndk::ScopedAStatus selectNext(bool* _aidl_return);
//             private:
//                 std::shared_ptr<Channel> mChannel;
//         };
//     private:
//         std::shared_ptr<Terminal>* mTerminal;
//         int mChannelNumber;
//         std::vector<uint8_t> mSelectResponse;

//         bool selectNext();
//         std::vector<uint8_t> setChannelToClassByte(uint8_t data, int channelNumber);
//         void setCallingPid(int pid);
//         void checkCommand(std::vector<uint8_t>& command);
//     };
// }  // namespace aidl::android::se