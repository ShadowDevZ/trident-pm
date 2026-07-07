/**
 * @file libtrident.h
 * @brief Main header to include
 *
 *
 */
#pragma once
#include <cstdint>
#include <cstddef>
#include <string>

#include "trderr.h"
#include "binarySerializer.h"
#include "ioflags.h"
#include "tstreaminfo.h"
#include <filesystem>

#include "hrddefs.h"
#include "sdescdefs.h"

#if CHAR_BIT != 8
#error "Unsupported platform"
#endif
namespace Trd {

    class TrFileHeader;
    class TrSectionDescriptor;
    class DtblDirectory;
    class Treg;

    class TrPkg {
      public:
        ~TrPkg() {
            dbgprintf("~Destructor called\n");
            try {
                closePkg();
            } catch (...) { dbgprintf("Exception in closepkg\n"); }
        }
        /**
           * @brief Method to manipulate the header class
           *
           * @return TrFileHeader
           */
        TrFileHeader header();
        DtblDirectory dtbl();
        /**
           * @brief Method to manipulate the Section Descriptor (TOC)
           *
           * @return TrSectiaccessModelonDescriptor
           */
        TrSectionDescriptor sd();
        Treg treg();

        const Trd::Impl::TStreamInfo& getTstream() const {
            return fstrInfo;
        }
        Trd::Impl::TStreamInfo& getTstream() {
            return fstrInfo;
        }

        explicit TrPkg(const std::filesystem::path& path, const TRDAccessModel& access) {
            openPackage(path, access);
        }
        explicit TrPkg(const std::filesystem::path& path, TrdOpenIO open, TrdAccessIO access,
                       TrdXattrIO xattr = TrdXattrIO::None) {

            openPackage(path, {open, access, xattr, _TrdInternalIO::None});
        }
        /**
           * @brief Opens the TRPX package
           *
           * @param path absolute or relative path to the package
           * @param accessModel additional flags to define access
           */
        void openPackage(const std::filesystem::path& path, const TRDAccessModel& accessModel);
        /**
           * @brief Closes the package. No need to call this because of RAII
           *
           */
        void closePkg();

      private:
        Trd::Impl::TStreamInfo fstrInfo{};
        Trd::TRD_HEADER trdHdr{};
        Trd::Impl::TRD_SECTION_DESCRIPTOR trdSD{};
        // TrFileHeader headerSection;
        friend class TrFileHeader;
        friend class TrSectionDescriptor;
        friend class DtblDirectory;
        // std::shared_ptr<TRDFstreamObject> fInfo;
    };
};
