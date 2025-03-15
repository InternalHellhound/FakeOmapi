#include "Terminal.h"
#include "Session.h"

#include <android/binder_manager.h>

namespace aidl::android::se {
using aidl::android::se::SecureElementSession;

void onClientDeath(void* cookie) {
    LOG(INFO) << "SecureElementSession has died";
    SecureElementSession* session = static_cast<SecureElementSession*>(cookie);
    // if (session && !session->isClosed()) {
    //     session->close();
    // }
}

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

Terminal::Terminal(const std::string& name) {
    mName = name;
    mTag = "SecureElement-Terminal-" + getName();
    mDeathRecipient = AIBinder_DeathRecipient_new(onClientDeath);
    mAidlCallback = ndk::SharedRefBase::make<AidlCallback>(this);
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
    } else {
        closeChannels();
        //initializeAccessControl();
        mDefaultApplicationSelectedOnBasicChannel = true;
    }
    //sendStateChangedBroadcast(state);
}

std::vector<uint8_t> Terminal::transmit(const std::vector<uint8_t>& cmd) {
    std::lock_guard<std::mutex> lock(mLock);
    
    if (!mIsConnected) {
        return {};
    }

    std::vector<uint8_t> response;
    if (mAidlHal != nullptr) {
        mAidlHal->transmit(cmd, &response);
    }

    if (response.size() == 0) {
        LOG(ERROR) << "Error in transmit()";
        std::cout << "Error in transmit()" << std::endl;
        return {};
    }

    size_t len = response.size();
    uint8_t sw1 = len >= 2 ? response[len-2] : 0;
    uint8_t sw2 = len >= 1 ? response[len-1] : 0;

    if (sw1 == 0x6C) {
        std::vector<uint8_t> newCmd(cmd);
        newCmd.back() = sw2;
        return transmit(newCmd);
    } else if (sw1 == 0x61) {
        do {
            std::vector<uint8_t> getResponseCmd = {cmd[0], 0xC0, 0x00, 0x00, sw2};
            auto tmp = transmit(getResponseCmd);
            
            response.insert(response.end()-2, tmp.begin(), tmp.end()-2);
            sw1 = tmp[tmp.size()-2];
            sw2 = tmp[tmp.size()-1];
        } while (sw1 == 0x61);
    }

    return response;
}

void Terminal::initialize(bool retryOnFail) {
    std::lock_guard<std::mutex> lock(mLock);
    if (mAidlHal == nullptr) {
        const std::string bName = std::string(ISecureElement::descriptor) + getName();
        AIBinder* binder = AServiceManager_waitForService(bName.c_str());
        mAidlHal = ISecureElement::fromBinder(ndk::SpAIBinder(binder));
        if (mAidlHal != nullptr) {
            mAidlHal->init(mAidlCallback);
            AIBinder_linkToDeath(mAidlHal->asBinder().get(),
                                mDeathRecipient, this);
            mIsConnected = true;
        }
    }
}
}