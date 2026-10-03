#include "access.h"
using namespace Trd::Impl;
using namespace Trd;
void updateHIByte(access_word& v, u8 hiByte) {
    v = ((v & 0x00FF) | static_cast<u16>(hiByte) << 8);
}
void updateLOByte(access_word& v, u8 loByte) {
    v = ((v & 0xFF00) | static_cast<u16>(loByte));
}
u8 getLOByte(access_word v) {
    return v & 0xFF;
}
u8 getHIByte(access_word v) {
    return (v >> 8) & 0xFF;
}

void TregAccess::appendFlagPFL(u8 f) {
    u8 updated = getHIByte(access);
    updated |= f;
    updateHIByte(access, updated);
}
void TregAccess::clearFlagPFL(u8 perms) {
    u8 original = getHIByte(access);
    original &= ~(perms);
    updateHIByte(access, original);
}
void TregAccess::clearPermissions() {
    u8 original = getHIByte(access);
    original = 0;
    updateHIByte(access, original);
}

u8 TregAccess::getPFL(access_word v) {
    return getHIByte(v);
}
ValueDataTemplate TregAccess::getAccessVdt(access_word v) {
    u8 low = getLOByte(v);
    //returns lower nibble from the first byte
    return static_cast<ValueDataTemplate>(low & 0x0F);
}
void TregAccess::setAccessVDT(ValueDataTemplate vdt) {

    u8 hiNibble = (getLOByte(access) >> 4) & 0x0F;
    u8 loNibble = static_cast<u8>(vdt);

    u8 lowByte = (hiNibble << 4) | (loNibble & 0x0F);
    updateLOByte(access, lowByte);
}
u8 TregAccess::getReservedNibble(access_word v) {
    u8 low = getLOByte(v);
    //returns higher nibble from the first byte
    return ((low >> 4) & 0xF0);
}
bool TregAccess::existsFlagPFL(u8 perms) const {
    u8 check = getHIByte(access);
    if (check & perms)
        return true;

    return false;
}