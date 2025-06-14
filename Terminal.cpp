#include "Terminal.h"
#include "Reader.h"
#include "Session.h"
#include "Service.h"
#include "Channel.h"

#include "ByteArrayConverter.h"

#include <android/binder_manager.h>
#include <aidl/android/hardware/secure_element/LogicalChannelResponse.h>

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
    //std::lock_guard<std::mutex> lock(mLock);
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
        const std::string bName = std::string(ISecureElement::descriptor) + "/" + getName();
        LOG(INFO) << __func__ << ": Getting Secure Element service: " << bName;
        AIBinder* binder = AServiceManager_waitForService(bName.c_str());
        mAidlHal = ISecureElement::fromBinder(ndk::SpAIBinder(binder));
        if (mAidlHal != nullptr) {
            LOG(INFO) << __func__ << ": Successfully get SE service: " << bName;
            mAidlHal->init(mAidlCallback);
            AIBinder_linkToDeath(mAidlHal->asBinder().get(),
                                mDeathRecipient, this);
            mIsConnected = true;
        } else {
            LOG(ERROR) << __func__ << ": Failed to get SE service: " << bName;
        }
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
    // LOG(ERROR) << __func__ << " 还没写";
    if (aid.empty()) {
        LOG(INFO) << __func__ << ": AID is empty";
    } else if (aid.size() < 5 || aid.size() > 16) {
        LOG(ERROR) << ": AID out of range";
        return nullptr;
    } else if (!mIsConnected) {
        LOG(ERROR) << __func__ << ": SE is not connected";
        return nullptr;
    }

    if (packageName != "") {
        LOG(WARNING) << __func__ << ": Enable access control on logical channel for: " << packageName;
    } else if (!uuid.empty()) {
        LOG(WARNING) << __func__ << ": Enable access control on logical channel for uid: " << pid << ", uuid: " << hex2string(uuid);
    }

    if (packageName != "" || !uuid.empty()) {
        LOG(ERROR) << __func__ << ": setUpChannelAccess 没写";
        // channelAccess = setUpChannelAccess(aid, packageName, uuid, pid, false);
    }

    std::lock_guard<std::mutex> lock(mLock);
    // int* status = int[1];
    // status[0] = 0;
    if (mAidlHal != nullptr) {
        // response[0] = new LogicalChannelResponse();
        ::aidl::android::hardware::secure_element::LogicalChannelResponse* responseArray =
            new ::aidl::android::hardware::secure_element::LogicalChannelResponse[1];
        ::aidl::android::hardware::secure_element::LogicalChannelResponse aidlRs;
        ndk::ScopedAStatus oStatus = mAidlHal->openLogicalChannel(aid.empty() ? std::vector<uint8_t>() : aid, p2, &aidlRs);
        if (!oStatus.isOk()) {
            LOG(ERROR) << __func__ << ": openLogicalChannel failed: " << oStatus.getDescription();
            delete[] responseArray;
            return nullptr;
        } else {
            responseArray[0] = aidlRs;
            int channelNumber = responseArray[0].channelNumber;
            std::vector<uint8_t> selectResponse = responseArray[0].selectResponse;
            Channel* logicalChannel = new ::aidl::android::se::Channel(session, this, channelNumber, selectResponse, aid, listener, pid);
            mChannels.insert(std::make_pair(channelNumber, logicalChannel));
            delete[] responseArray;
            return logicalChannel;
        }
    } else {
        LOG(ERROR) << __func__ << ": Can't find mAidlHal, and don't support HIDL hal, returning...";
        return nullptr;
    }

    return nullptr;
}


bool Terminal::reset() {
    LOG(INFO) << __func__;
    return true;
}

void Terminal::closeChannel(Channel* channel) {
    LOG(INFO) << __func__;
    if (channel == nullptr) {
        LOG(WARNING) << __func__ << ": Attempt to close a null channel.";
        return;
    }
    int channelNumber = channel->getChannelNumber();
    LOG(INFO) << __func__ << ": Closing channel " << channelNumber;
    std::lock_guard<std::mutex> lock(mLock);
    if (mIsConnected) {
        if (mAidlHal != nullptr) {
            LOG(INFO) << __func__ << ": Closing channel " << channelNumber << " using AIDL HAL.";
            ::ndk::ScopedAStatus status = mAidlHal->closeChannel(static_cast<int8_t>(channelNumber));
            if (!status.isOk()) {
                LOG(ERROR) << "Error closing channel " << channelNumber
                           << " using AIDL HAL: " << status.getDescription()
                           << ", serviceSpecificError: " << status.getServiceSpecificError();
            }
        } else {
            LOG(WARNING) << __func__ << ": mAidlHal is null. Cannot close channel " << channelNumber << " via HAL.";
        }
    } else {
        LOG(WARNING) << __func__ << ": Not connected. Channel " << channelNumber << " cannot be closed via HAL.";
    }
    auto it = mChannels.find(channelNumber);
    if (it != mChannels.end()) {
        if (it->second.get() == channel) {
            LOG(INFO) << __func__ << ": Removing channel " << channelNumber << " from map.";
            mChannels.erase(it);
        } else {
            LOG(WARNING) << __func__ << ": Channel " << channelNumber << " found in map, but pointer mismatch. Removing by ID anyway.";
            mChannels.erase(it);
        }
    } else {
        LOG(WARNING) << __func__ << ": Channel " << channelNumber << " not found in map for removal.";
    }
    if (mChannels.count(channelNumber)) {
        LOG(ERROR) << mTag << ": Removing channel " << channelNumber << " failed, still in map.";
    }
}

void Terminal::closeChannels() {
    LOG(INFO) << __func__;
    std::vector<std::shared_ptr<Channel>> channelsToClose;
    {
        //std::lock_guard<std::mutex> lock(mLock);
        if (mChannels.empty()) {
            LOG(INFO) << __func__ << ": No channels to close.";
            return;
        }
        LOG(INFO) << __func__ << ": Preparing to close " << mChannels.size() << " channels.";
        for (const auto& pair : mChannels) {
            channelsToClose.push_back(pair.second);
        }
    }

    for (const auto& channelPtr : channelsToClose) {
        if (channelPtr) {
            LOG(INFO) << __func__ << ": Requesting close for channel " << channelPtr->getChannelNumber();
            channelPtr->close();
        }
    }
}

void Terminal::close() {
    LOG(INFO) << __func__;
    //std::lock_guard<std::mutex> lock(mLock);
    if (mAidlHal != nullptr) {
        LOG(INFO) << __func__ << ": Unlinking death recipient.";
        AIBinder* binder = mAidlHal->asBinder().get();
        if (binder) {
            AIBinder_unlinkToDeath(binder, mDeathRecipient, this);
        }
    }
    mIsConnected = false;
}

bool Terminal::isSecureElementPresent() {
    LOG(INFO) << __func__;
    bool p;
    if (mAidlHal != nullptr) {
        mAidlHal->isCardPresent(&p);
        LOG(INFO) << __func__ << ": " << p;
        return p;
    }
    LOG(ERROR) << __func__ << ": Can't find mAidlHal!, please init it first.";
    return false;
}

std::vector<uint8_t> Terminal::getAtr() {
    LOG(INFO) << __func__;
    std::vector<uint8_t> atr;

    if (!mIsConnected) {
        LOG(ERROR) << "Not connected";
        return atr;
    }

    if (mAidlHal != nullptr) {
        LOG(INFO) << "Fetching atr from AIDL hal";
        mAidlHal->getAtr(&atr);
        if (atr.empty()) {
            LOG(ERROR) << "Atr is empty!";
            return atr;
        }
    } else {
        LOG(ERROR) << "No AIDL hal found!";
        return atr;
    }
    if (DEBUG) {
        LOG(INFO) << "ATR: " << hex2string(atr);
    }
    return atr;
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
