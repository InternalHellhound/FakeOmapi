#include "Terminal.h"

namespace aidl::android::se {
Terminal::AidlCallback {
    AidlCallback(Terminal* terminal)
    ::ndk::ScopedAStatus onStateChange(bool in_connected, const std::string& in_debugReason) {
        stateChange(in_connected, in_debugReason);
        return ::ndk::ScopedAStatus::ok();
    }
    ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) {
        *_aidl_return = SecureElementService::VERSION;
        return ::ndk::ScopedAStatus::ok();
    }
    ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) {
        *_aidl_return = SecureElementService::HASH;
        return ::ndk::ScopedAStatus::ok();
    }
}

Terminal::SecureElementDeathRecipient {
    SecureElementDeathRecipient(Terminal* terminal)
    : mTerminal(terminal) {}
    void binderDied(){onDied();}
    void onDied() {
        std::lock_guard<std::mutex> lock(mTerminal->mLock);
        mTerminal->mIsConnected = false;
        mTerminal->mGetHalRetryCount = 0;
    }
}

Terminal::Terminal(const std::string& name) {
    mName = name;
    mTag = "SecureElement-Terminal-";
}
}