#pragma once

#include <aidl/android/se/omapi/BnSecureElementChannel.h>
#include <aidl/android/se/omapi/ISecureElementListener.h>
#include <aidl/android/se/omapi/ISecureElementSession.h>

#include <utils/RefBase.h>

#include "Terminal.h"

using aidl::android::se::omapi::BnSecureElementChannel;
using aidl::android::se::omapi::ISecureElementListener;
using aidl::android::se::omapi::ISecureElementSession;

namespace aidl::android::se {
// namespace omapi {
// class SecureElementSession;
// }
// using aidl::android::se::omapi::SecureElementSession;

class Channel : public ::android::RefBase {
    public:
        Channel(ISecureElementSession* session, 
            Terminal* terminal,
            int channelNumber,
            const std::vector<uint8_t>& selectResponse,
            const std::vector<uint8_t>& aid,
            const std::shared_ptr<ISecureElementListener>& listener);

        virtual ~Channel() = default;

        void binderDied();
        void close() const;
        std::vector<uint8_t> transmit(const std::vector<uint8_t>& command);
        // std::shared_ptr<ChannelAccess> getChannelAccess();
        // void setChannelAccess(std::shared_ptr<ChannelAccess> channelAccess);
        bool hasSelectedAid();
        int getChannelNumber() const;
        std::vector<uint8_t> getSelectResponse();
        bool isBasicChannel();
        bool isClosed();
    private:
        ISecureElementSession* mSession;
        Terminal* mTerminal;
        int mChannelNumber;
        std::vector<uint8_t> mSelectResponse;
        std::vector<uint8_t> mAid;
        const std::shared_ptr<ISecureElementListener>& mListener;

        bool selectNext();
        std::vector<uint8_t> setChannelToClassByte(uint8_t data, int channelNumber);
        void setCallingPid(int pid);
        void checkCommand(std::vector<uint8_t>& command);
        bool mIsClosed;
        friend class SecureElementChannel;
    };
    class SecureElementChannel : public BnSecureElementChannel {
        public:
            SecureElementChannel(const std::shared_ptr<Channel>& channel);
            ndk::ScopedAStatus close();
            ndk::ScopedAStatus isClosed(bool* _aidl_return);
            ndk::ScopedAStatus isBasicChannel(bool* _aidl_return);
            ndk::ScopedAStatus getSelectResponse(std::vector<uint8_t>* _aidl_return);
            ndk::ScopedAStatus transmit(const std::vector<uint8_t>& command, std::vector<uint8_t>* outResponse);
            ndk::ScopedAStatus selectNext(bool* _aidl_return);
        private:
            std::shared_ptr<Channel> mChannel;
    };
}  // namespace aidl::android::se
