#include "Terminal.h"
#include "Reader.h"
#include "Session.h"
#include "Service.h"

#include "ByteArrayConverter.h"

#include <android/binder_manager.h>

namespace aidl::android::se {
using aidl::android::se::omapi::SecureElementSession;

void Terminal::onClientDeath(void* cookie) {
    LOG(INFO) << __func__ << ": Die";
    SecureElementSession* session = static_cast<SecureElementSession*>(cookie);
    // if (session && !session->isClosed()) {
    //     session->close();
    // }
    mIsConnected = false;
    // if (mAccessControlEnforcer != nullptr) {
    //     mAccessControlEnforcer.reset();
    // }
    this->handler(EVENT_GET_HAL, 0, GET_SERVICE_DELAY_MILLIS);
}

void Terminal::onClientDeathWrapper(void* cookie) {
    LOG(INFO) << "Binder has died";
    Terminal* self = static_cast<Terminal*>(cookie);
    self->onClientDeath(cookie);
}

Terminal::AidlCallback::AidlCallback(Terminal* terminal) {
        mTerminal = terminal;
}
::ndk::ScopedAStatus Terminal::AidlCallback::onStateChange(bool state, const std::string& debugReason) {
    mTerminal->stateChange(state, debugReason);
    return ::ndk::ScopedAStatus::ok();
}

Terminal::Terminal(const std::string name) {
    mName = name;
    mTag = "SecureElement-Terminal-" + getName();
    mDeathRecipient = AIBinder_DeathRecipient_new(onClientDeathWrapper);
    mAidlCallback = ndk::SharedRefBase::make<AidlCallback>(this);
}

std::string Terminal::getName() {
    return mName;
}

void Terminal::stateChange(bool state, const std::string& reason) {
    LOG(INFO) << __func__ << ": state: " << state << ", reason: " << reason;
    std::lock_guard<std::mutex> lock(mLock);
    mIsConnected = state;
    if (!state) {
        LOG(INFO) << "state: not connected";
        // if (mAccessControlEnforcer != nullptr) {
        //     mAccessControlEnforcer->reset();
        // }
    } else {
        LOG(INFO) << "state: connected";
        /* Unimplemented yet */
        this->closeChannels();
        //initializeAccessControl();
        mDefaultApplicationSelectedOnBasicChannel = true;
    }
    this->handler(EVENT_NOTIFY_STATE_CHANGE, state, 0);
}

std::vector<uint8_t> Terminal::transmit(const std::vector<uint8_t>& cmd) {
    LOG(INFO) << __func__;
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
    LOG(INFO) << __func__;
    std::lock_guard<std::mutex> lock(mLock);
    if (mAidlHal == nullptr) {
        const std::string bName = std::string(ISecureElement::descriptor) + getName();
        LOG(INFO) << __func__ << ": Getting Secure Element service: " << bName;
        AIBinder* binder = AServiceManager_waitForService(bName.c_str());
        mAidlHal = ISecureElement::fromBinder(ndk::SpAIBinder(binder));
        if (mAidlHal != nullptr) {
            LOG(INFO) << __func__ << ": Successfully get SE service: " << bName;
            mAidlHal->init(mAidlCallback);
            AIBinder_linkToDeath(mAidlHal->asBinder().get(),
                                mDeathRecipient, this);
            mIsConnected = true;
        }
        LOG(ERROR) << __func__ << ": Failed to get SE service: " << bName;
    }
}

std::shared_ptr<ISecureElementReader> Terminal::newSecureElementReader(std::shared_ptr<omapi::SecureElementService> service) {
    LOG(INFO) << __func__;
    return ndk::SharedRefBase::make<SecureElementReader>(service, this);
}

Channel* Terminal::openBasicChannel(ISecureElementSession* session, const std::vector<uint8_t>& aid, uint8_t p2, const std::shared_ptr<ISecureElementListener>& listener, const std::string& packageName, const std::vector<uint8_t>& uuid, int pid) {
    LOG(INFO) << __func__;
    LOG(ERROR) << __func__ << " 还没写";
    return nullptr;
}

Channel* Terminal::openLogicalChannel(ISecureElementSession* session, const std::vector<uint8_t>& aid, uint8_t p2, const std::shared_ptr<ISecureElementListener>& listener, const std::string& packageName, const std::vector<uint8_t>& uuid, int pid) {
    LOG(INFO) << __func__;
    LOG(ERROR) << __func__ << " 还没写";
    return nullptr;
}


bool Terminal::reset() {
    LOG(INFO) << __func__;
    return true;
}

bool Terminal::isSecureElementPresent() {
    LOG(INFO) << __func__;
    bool p;
    if (mAidlHal != nullptr) {
        mAidlHal->isCardPresent(&p);
        return p;
    }
    LOG(ERROR) << __func__ << ": Can't find mAidlHal!, please init it first.";
    return false;
}

std::vector<uint8_t> Terminal::getAtr() {
    LOG(INFO) << __func__;
    if (!mIsConnected) {
        LOG(ERROR) << "Not connected";
        return {};
    }

    std::vector<uint8_t> atr;
    if (mAidlHal != nullptr) {
        LOG(INFO) << "Fetching atr from AIDL hal";
        mAidlHal->getAtr(&atr);
        if (atr.empty()) {
            LOG(ERROR) << "Atr is empty!";
            return {};
        }
    } else {
        LOG(ERROR) << "No AIDL hal found!";
        return {};
    }
    if (DEBUG) {
        LOG(INFO) << "ATR: " << hex2string(atr);
    }
    return atr;
}

void Terminal::closeChannels() {
    LOG(INFO) << __func__;
}

void Terminal::handler(int event, int msg, int delay) {
    LOG(INFO) << __func__ << ": event: " << event << ", msg: " << msg << ", delay: " << delay;
    if (event == EVENT_GET_HAL) {
        LOG(INFO) << "EVENT_GET_HAL";
        if (mName.starts_with(SecureElementService::ESE_TERMINAL)) {
            initialize(true);
        } else {
            initialize(false);
        }
    }
    if (event == EVENT_NOTIFY_STATE_CHANGE) {
        LOG(INFO) << "EVENT_NOTIFY_STATE_CHANGE";
        //sendStateChangedBroadcast()
    }
}

}