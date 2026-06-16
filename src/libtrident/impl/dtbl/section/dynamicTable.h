/**
 * @file dnyamicTable.h
 * @brief Common definitions for single table
 * 
 * 
 */
#pragma once
//this file is only used by treg and should not be handled directly
/*
treg is kept in memory and written as footer
basically what we need is through treg obtain
direct offset where specific table is, its size
and parse accordingly

dtbl will have minimal implementation as all tables
and how they are parsed are directly up to treg 
implementation
this class is never called by end user and instead
is only invoked by treg

WE ALSO NEED DYNSEC_PAGED flag so large sections are paged inside file instead of singular blob
these sections share the same uuid but have the paging attribute set

VERY IMPORTANT single dtbl entry needs to have optimized
IO operations for large file sizes as we could write 
some random file.bin (1GB in size) into the section.
Copying it all into ram is plainly a stupid idea.
More ideal would be chunks of 4096 or so
again we need to extend TStream functionality and implement optimized
writer/reader to get the file in chunks with something like while chunk.nextData()...
this would be separate TStream functions which would return singular file or data CHUNK
which then will be written to the file so we would read singular chunk from file.bin
increase the length attribute by the CHUNK size, write the data to the section whilst
there are another chunks to process. 

this will be something like 
DataChunk TStream::Read/WriteDataAsChunk(myFile);
nextChunkData = struct with some info on which chunk are we reading and the data and other info
while (nextChunkData = ReadDataAsChunk(myFile, nextChunkdata.chunkOffset))
then
Dtbl::WriteEntryChunksData(entryOffsetVal, nextChunkData); this wont modify the chunk index
        this will read the data from the struct write it to the file at specific offset
        and increase dtblEntry.size += nextChunkData.length

if we arent putting data from file into DataChunk then we simply copy everything into singular chunk
as the data in this case is less than few kilobytes in size and will be serialized into the
corrsepoinding struct/class


again dtbl is dumb by design as all the work is done by treg calls which will be exposed to the
end user like for example TREG_META Trd::queryTableMetadata(STBL_PAYLOAD,preferExtended=false)
then we call Trd::ReadDtbl(STBL_PAYLOAD, &myoutputbuff, tregMeta) where treg meta contains size,...
*/
#include "datatypes.h"
#include <optional>
#include "trderr.h"
#include "sdesc.h"
#include "chunkData.h"
//all section manipulations have to be done via treg interface
//tbl is just a dumb array

//all operations that do modifications like updating values will be added later
namespace Trd {

    class TrPkg;

    class DtblDirectory {
      public:
        /**
         * @brief Returns the location of the first byte where DTBL starts
         * Do not add + 1 to the result. The returned value is already a seek offset
         * to the first byte where you can perform IO operations on
         * 
         */
        std::expected<u64, Trd::Err::TrdError> getOffset();

        /* checks whether the treg is already written as if it is we either have
        to block the operation OR copy the small treg into memory then we can truncate the data
        append the section and add back the treg which is risky so better approach would be probably
        to create carbon copy of the file including header and treg and then recreate the dtbl and treg
        instead of doing it inplace as a single error could render the whole package unusable.
        This will be a bool to check if user wants to do it in place*/
        bool isSafeToAccess() const;

        std::expected<void, Trd::Err::TrdError> invalidateSDEntry();
        std::expected<Impl::SD_TBLENTRY, Trd::Err::TrdError> readSDEntry();
        /**
         * @brief Writes the SD table entry for the DTBL
         * 
         * @param offset fseek ofset to the starting byte of the DTBL directory
         * @param size total size of the whole DTBL directory section. Size NEEDS to match
         * @param available if true section contains valid data that can be trusted and checked
         * otherwise it can contain any data if false
         * @return std::expected<void, Trd::Err::TrdError> 
         */
        std::expected<void, Trd::Err::TrdError> writeSDEntry(u64 offset, u64 size, bool available);

        std::expected<void, Trd::Err::TrdError> writeRawEntry(std::span<const u8> data,
                                                              u64 writeOffset);
        std::expected<std::vector<u8>, Trd::Err::TrdError> readRawEntry(u64 readOffset,
                                                                        u64 size) const;
        //todo also validate SD and if safe to write
        std::expected<void, Trd::Err::TrdError> writeEntryInChunks(std::span<const u8> data,
                                                                   u64 writeOffset);
        std::expected<Trd::Impl::IOReadChunk, Trd::Err::TrdError> readEntryChunk();

        //probably add to treg instead
        //void findFreeHole();

        explicit DtblDirectory(TrPkg& pkg) : trpkg(pkg), internalSD{pkg} {};

      private:
        constexpr u64 badOffset() const {
            return Consts::Header::LT_HDR_SZB_01A + Consts::SD::TRD_SECTIONSD_SIZE;
        }
        friend class TrPkg;
        TrPkg& trpkg;
        TrSectionDescriptor internalSD;
    };
};