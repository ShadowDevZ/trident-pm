#include <optional>
#include <expected>
#include "trderr.h"
#pragma once
namespace LibTrident::Sections {
    template <typename T>
    class SectionCommon {
        public:
            virtual std::expected<void, Err::TrdError> Write() = 0;
            virtual std::expected<void, Err::TrdError> Read() = 0;
            virtual std::expected<T, Err::TrdError> ReadBack() = 0;

            virtual T& GetObject() = 0;
            virtual const T& GetObject() const = 0;
            virtual ~SectionCommon() = default;
            virtual bool IsValid() = 0;

    };

};