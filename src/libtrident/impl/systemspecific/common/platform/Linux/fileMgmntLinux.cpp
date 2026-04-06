#if defined(__linux__) || defined(__unix__)

#include "include/linuxSpecific.h"
#include <unistd.h>
#include <fcntl.h>
using namespace Trd;
using namespace Trd;

std::expected<std::filesystem::path, Trd::Err::TrdError> LinuxSpecific::createTemporaryFile() {
    char templatePath[] = "/tmp/.tmp_tridentpkgXXXXXX";
    const int fd = mkstemp(templatePath);
    if (fd == -1) {
        return std::unexpected(Trd::Err::TrdError(Trd::Err::Code::FileWriteFailure));
    }
    return std::filesystem::path(templatePath);
}

std::expected<Impl::AuxiliaryStat, Trd::Err::TrdError> LinuxSpecific::getAuxiliaryStat(const std::filesystem::path& path) {
    struct statx stx{};
    const int ret = statx(AT_FDCWD, path.c_str(), AT_SYMLINK_NOFOLLOW, STATX_BASIC_STATS | STATX_BTIME, &stx);
    if (ret != 0) {
        //again in future we need to pass the error code
        return std::unexpected(Err::TrdError{Err::Code::OSFunctionCallFailed});
    }
    Impl::AuxiliaryStat extStat{};

    extStat.optOwnerGID = stx.stx_gid;
    extStat.optOwnerUID = stx.stx_uid;

    extStat.times.lastAccess = std::chrono::system_clock::time_point(std::chrono::seconds(stx.stx_atime.tv_sec) + std::chrono::nanoseconds(stx.stx_atime.tv_nsec));

    extStat.times.lastModify = std::chrono::system_clock::time_point(std::chrono::seconds(stx.stx_mtime.tv_sec) + std::chrono::nanoseconds(stx.stx_mtime.tv_nsec));
    extStat.times.lastMetadataChange = std::chrono::system_clock::time_point(std::chrono::seconds(stx.stx_ctime.tv_sec) + std::chrono::nanoseconds(stx.stx_ctime.tv_nsec));
    if (stx.stx_mask & STATX_BTIME) {
        extStat.times.fileCreated = std::chrono::system_clock::time_point(std::chrono::seconds(stx.stx_btime.tv_sec) + std::chrono::nanoseconds(stx.stx_btime.tv_nsec));
    }

    return extStat;
}
#endif