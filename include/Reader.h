#pragma once

#include "Terminal.h"
#include "Service.h"

#include <aidl/android/se/omapi/BnSecureElementReader.h>
#include <aidl/android/se/omapi/ISecureElementSession.h>

using aidl::android::se::omapi::BnSecureElementReader;
using aidl::android::se::omapi::ISecureElementSession;

namespace aidl::android::se {
namespace omapi {
class SecureElementSession;
}

using aidl::android::se::omapi::SecureElementService;
using aidl::android::se::omapi::SecureElementSession;
using aidl::android::se::Terminal;

class SecureElementReader : public BnSecureElementReader {
    public:
        SecureElementReader(std::shared_ptr<SecureElementService> service, Terminal* terminal);
        std::vector<uint8_t> getAtr();
        ::ndk::ScopedAStatus isSecureElementPresent(bool* isTrue);
        ::ndk::ScopedAStatus closeSessions();
        void removeSession(SecureElementSession* session);
        ::ndk::ScopedAStatus openSession(std::shared_ptr<ISecureElementSession>* session);
        Terminal& getTerminal();
        ::ndk::ScopedAStatus reset(bool* isReset);

    private:
        std::mutex mLock;
        std::shared_ptr<SecureElementService> mService;
        Terminal* mTerminal;
        std::vector<std::shared_ptr<SecureElementSession>> mSessions;
};
}