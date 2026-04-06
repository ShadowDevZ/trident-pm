#pragma once
#include <vector>
#include "datatypes.h"
#include <functional>
#include <algorithm>
#include <concepts>
//fwd to avoid cyclic dependency

namespace Trd::Err {
    class TrdError;

    //Singleton class for adding callbacks to TrdErrors
    class TrdErrorCallback {
      public:
        using ErrCb = std::function<void(const Trd::Err::TrdError&)>;

        static TrdErrorCallback& instance() {
            static TrdErrorCallback inst;
            return inst;
        }

        template <std::invocable<const Trd::Err::TrdError&> F>
        u32 registerCallback(F&& cb) {
            const u32 id = ++nextId;
            callbacks.push_back({id, std::function<void(const Trd::Err::TrdError&)>(std::forward<F>(cb))});
            return id;
        }

        void callHandlers(const Trd::Err::TrdError& err) {
            for (auto& entry : callbacks) {
                entry.cb(err);
            }
        }

        void unregisterCallback(u32 id) {
            std::erase_if(callbacks, [id](const auto& entry) { return entry.cbId == id; });
        }

        TrdErrorCallback(const TrdErrorCallback&) = delete;
        TrdErrorCallback& operator=(const TrdErrorCallback&) = delete;

      private:
        TrdErrorCallback() = default;
        ~TrdErrorCallback() = default;

        struct CbEntry {
            u32 cbId = 0;
            ErrCb cb{};
        };

        u32 nextId = 0;
        std::vector<CbEntry> callbacks;
    };
};