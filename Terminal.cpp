#include "Terminal.h"

namespace aidl::android::se {
Terminal::AidlCallback::AidlCallback(Terminal* terminal) {
        mTerminal = terminal;
}
::ndk::ScopedAStatus Terminal::AidlCallback::onStateChange(bool in_connected, const std::string& in_debugReason) {
    mTerminal->stateChange(in_connected, in_debugReason);
    return ::ndk::ScopedAStatus::ok();
}

// Terminal::SecureElementDeathRecipient {
//     SecureElementDeathRecipient(Terminal* terminal)
//     : mTerminal(terminal) {}
//     void binderDied(){onDied();}
//     void onDied() {
//         std::lock_guard<std::mutex> lock(mTerminal->mLock);
//         mTerminal->mIsConnected = false;
//         mTerminal->mGetHalRetryCount = 0;
//     }
// }

Terminal::Terminal(const std::string& name) : mAidlCallback(this) {
    mName = name;
    mTag = "SecureElement-Terminal-" + getName();
}

std::string Terminal::getName() {
    return mName;
}

void Terminal::stateChange(bool state, const std::string& reason) {
    std::lock_guard<std::mutex> lock(mLock);
    mIsConnected = state;
    if (!state) {
        // if (mAccessControlEnforcer != nullptr) {
        //     mAccessControlEnforcer->reset();
        // }
    }
    Terminal::closeChannels();
}

}