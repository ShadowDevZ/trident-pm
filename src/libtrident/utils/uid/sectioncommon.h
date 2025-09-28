#include <optional>

#pragma once
namespace LibTrident::Sections {
    template <typename T>
    class SectionCommon {
        public:
            virtual bool Write() = 0;
            virtual bool Read() = 0;
            virtual std::optional<T> ReadBack() = 0;

            virtual T& GetObject() = 0;
            virtual const T& GetObject() const = 0;
            virtual ~SectionCommon() = default;
            virtual bool IsValid() = 0;
            virtual bool StatusOk() = 0;

    };

};