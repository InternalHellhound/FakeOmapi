#include "Service.h"

// #define TERMINAL 0x73682a8500000113000000020000000000000000000000000000003f

// std::array<uint8_t, 28> TERMINAL = {
//     0x73, 0x68, 0x2A, 0x85, 0x00, 0x00, 0x01, 0x13,
//     0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x3F
// };

namespace aidl::android::se::omapi {
    SecureElementService::SecureElementService() {
        mTerminals.insert({"eSE1", "TERMINAL"});
    }

    ndk::ScopedAStatus SecureElementService::getReaders(std::vector<std::string>* readers) {
            int callingUid = AIBinder_getCallingUid();
            LOG(INFO) << "getReaders() for uid: " << callingUid << std::endl;
            std::string packageName = getPackageNameFromCallingUid(callingUid);

            if (packageName != "") {
                LOG(INFO) << "getReaders() find package name for uid[" << callingUid << "]: " << packageName.c_str() << std::endl;

                std::lock_guard<std::mutex> lock(mTerminalsMutex);
                for (const auto& pair : mTerminals) {
                    readers->push_back(pair.first);
                }
            } else {
                LOG(INFO) << "Can't find package name using PackageManager, find eseReader instead" << std::endl;
                const std::string& prefix = ESE_TERMINAL;
                std::lock_guard<std::mutex> lock(mTerminalsMutex);

                for (const auto& pair : mTerminals) {
                    if (pair.first.find(prefix) == 0) {
                        readers->push_back(pair.first);
                        LOG(INFO) << "Find eseReader: " << pair.first << std::endl;
                    }
                }
        }
        return ndk::ScopedAStatus::ok();
    };

    ndk::ScopedAStatus SecureElementService::getReader(const std::string& readerName,
                                                        std::shared_ptr<ISecureElementReader>* readerObj) {
        LOG(INFO) << __func__;
        std::cout << "getReader for " << readerName.c_str() << std::endl;
        LOG(INFO) << "readerObj pointer address: 0x" 
        << std::hex << reinterpret_cast<uintptr_t>((*readerObj).get());

        for (const auto& pair : mTerminals) {
            if (pair.first.compare(readerName) == 0) {
                std::cout << "Find reader for: " << readerName.c_str() << std::endl;
                // readerObj = pair.second;
                LOG(INFO) << "readerObj pointer address: 0x" 
                << std::hex << reinterpret_cast<uintptr_t>((*readerObj).get());
                return ndk::ScopedAStatus::ok();
            }
        }
        return ndk::ScopedAStatus::ok();
    };
    ndk::ScopedAStatus SecureElementService::isNfcEventAllowed(const std::string& readerName,
                                            const std::vector<uint8_t>& aid,
                                            const std::vector<std::string>& packageNames,
                                            int32_t userId,
                                            std::vector<bool>* isAllowed) {
        LOG(INFO) << __func__;
        std::cout << "isNfcEventAllowed" << std::endl;
        // TODO: implement isNfcEventAllowed, return false for now
        for (auto const packageName : packageNames) {
            isAllowed->push_back(false);
        }
        return ndk::ScopedAStatus::ok();
    };
}