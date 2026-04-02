#pragma once

#include "tstreaminfo.h"
#include "binarySerializer.h"
#include "sdescdefs.h"
#include "trderr.h"
#include "tstreaminfo.h"

namespace Trd {
    
    class TrPkg;


/**
 * @brief TRPX Table of content section containing offsets to another sections 
 * All functions which only check the internal variables should firstly call read()
 * to read the information stored in the stream and then the internal variable can be accessed
 */
class TrSectionDescriptor {

public:

    /**
     * @brief Checks whether the class SD object contains the right values
     * 
     * @return true Object is valid and can be used
     * @return false Object is invalid  or contains incorrect values
     */
    bool isValid();
    /**
     * @brief Writes blank (default) SD section which has to be populated by
     * updateSD() function. When SD is marked as READY it cannot be blanked out again
     * unless the status is changed
     * 
     * @return std::expected<void, Err::TrdError> 
     */
    std::expected<void, Err::TrdError> createWriteBlank();
    
    /**
     * @brief Writes the class SD object to the stream
     * 
     * @return std::expected<void, Err::TrdError> 
     */
    std::expected<void, Err::TrdError> write();
    /**
     * @brief Sets the status of the SD object to the specified boolean value
     * @attention This function does NOT trigger a write() operation it only modifies
     * the internal value. To write the changes use the write() function
     * 
     * @param ready If true marks the SD as ready meaning it contains valid data
     * that points to the correct stream offsets
     */
    void changeReadyStatus(bool ready);
    /**
     * @brief Checks the internal object whether the status is marked as ready
     * meaning that all offsets and fields are valid
     * @attention This function does NOT check the values which are stored inside the stream
     * it only checks the internal variable. To check whether the variable in the TStream is ready
     * it must firstly be read by read() function
     * 
     * @return true SD is ready and valid
     * @return false SD is invalid or missing some values
     */
    bool isReady() const;
    /**
     * @brief Reads the SD section from the stream.
     * 
     * @return std::expected<void, Err::TrdError> 
     */
    std::expected<void, Err::TrdError> read();
    /**
     * @brief Function to check if VALID SD exists
     * This function should be called outside of the class for example in dtbl
     * to determine if it can be written.
     * 
     * @return std::expected<void, Err::TrdError> 
     */
    static std::expected<void, Err::TrdError> isSdPresent();
    /**
     * @brief Returns the starting stream offset where SD is located
     * 
     * @return std::expected<u64, Err::TrdError> offset if present
     */
    static std::expected<u64, Err::TrdError> getStartOffset();
    /**
     * @brief Returns the last offset where the SD is located
     * 
     * @return std::expected<u64, Err::TrdError> 
     */
    static std::expected<u64, Err::TrdError> getEndOffset();
    /**
     * @brief Returns the internal SECTION_DESCRIPTOR
     * 
     * @return const Impl::TRD_SECTION_DESCRIPTOR& 
     */
    const Impl::TRD_SECTION_DESCRIPTOR& getSD() const;
    /**
     * @brief Updates and writes the public fields to the steram
     * 
     * @param update reference to the TRD_SD_UPDATEFIELD which should be filled with values to be updated
     * @param setReadyStatus If the SD should be written as ready upon writing. This is same as
     * calling changeReadyStatus(true) and then updating the SD without this option
     * @param checkReady If the ready field should be checked. This option exists when we are writing
     * the sd sequentionally and for example the dtbl exists but the treg is not present
     * this would render the SD invalid unless this option is set to false
     * @return std::expected<void, Trd::Err::TrdError> 
     */
    std::expected<void, Trd::Err::TrdError> updateSD(const Impl::TRD_SD_UPDATEFIELD& update, 
                                                    std::optional<bool> setReadyStatus = std::nullopt,
                                                    bool checkReady = true);
    /**
     * @brief Reads the SD from the Tstream and returns the SD object 
     * 
     * @param tStream reference to the TStream where SD is supposed to be read from
     * @return std::expected<Impl::TRD_SECTION_DESCRIPTOR, Err::TrdError> full SD struct if present
     */
    static std::expected<Impl::TRD_SECTION_DESCRIPTOR, Err::TrdError> readBack(Impl::TStreamInfo& tStream);

    explicit TrSectionDescriptor(TrPkg& pkg) : trpkg(pkg) {};

private:
    friend class TrPkg;
    TrPkg& trpkg;
    std::expected<void, Trd::Err::TrdError> IwriteSDNoValidate(const Impl::TRD_SECTION_DESCRIPTOR& sd);
    static std::expected<void, Trd::Err::TrdError> iCheckCRC(const Impl::TRD_SECTION_DESCRIPTOR& sd);
    static std::expected<void, Trd::Err::TrdError> iValidateSD(const Impl::TRD_SECTION_DESCRIPTOR& sd,
                                                              bool checkReady=true);
    static std::expected<void, Trd::Err::TrdError> iFieldCheckSD(const Impl::TRD_SECTION_DESCRIPTOR& sd);


};

};