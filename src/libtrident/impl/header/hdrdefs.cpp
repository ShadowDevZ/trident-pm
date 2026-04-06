#include "hrddefs.h"

using namespace Trd;

std::optional<std::vector<u8>> TRD_HEADER::serialize() const {
    if (_reserved0 != 0) { 
            //todo check here the fields that should be const like header so we dont have to check manually
            //in code always, as the header field only matters when doing CRC, serialization and deserialization
        return std::nullopt;
    }
    Trd::Impl::BinarySerializer bs;
        
    
    bs.addContainer(std::span<const u8>(magic));
    bs.addTrivial(exSignature,fmtVersion,compression, buildFlags,
                    architecture, dynHdrChksum, dynFileLen, _reserved0);
        
        
    return bs.getFormattedData();
}

bool TRD_HEADER::deserialize(const std::vector<u8>& dataIn) {
    Impl::BinarySerializer bs(dataIn);
        
    bs.readContainer(std::span<u8>(magic));
      
    bs.readTrivial(exSignature, fmtVersion,compression,
                                buildFlags, architecture, dynHdrChksum,
                                dynFileLen, _reserved0);
       
    dbgprintf("xsize:%ld:\n", bs.getReadOffset());
    if (bs.getReadOffset() != this->size() || _reserved0 != 0){
        return false;
    }
    return true;
}

std::optional<u32> TRD_HEADER::checksumCRC32() const {
     if (_reserved0 != 0) 
        return std::nullopt;
        
    Impl::Crc32Gen crc;
    crc.addData(magic, exSignature,fmtVersion,
                    compression,buildFlags,architecture, _reserved0);
    return crc.getCrc32();
        //todo implement recalcCRC()
}
