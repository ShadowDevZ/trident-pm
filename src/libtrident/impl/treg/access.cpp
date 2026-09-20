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

void TregAccess::appendPermission(PermissionFlags perms) {
    u8 updated = getHIByte(access);
    updated |= static_cast<u8>(perms);
    updateHIByte(access, updated);
}
void TregAccess::removePermission(PermissionFlags perms) {
    u8 original = getHIByte(access);
    original &= ~(static_cast<u8>(perms));
    updateHIByte(access, original);
}
void TregAccess::clearPermissions() {
    u8 original = getHIByte(access);
    original = static_cast<u8>(PermissionFlags::ReadOnly);
    updateHIByte(access, original);
}

PermissionFlags TregAccess::getAccessPerms(access_word v) {
    return static_cast<PermissionFlags>(getHIByte(v));
}
ValueDataTemplate TregAccess::getAccessVdt(access_word v) {
    u8 low = getLOByte(v);
    //returns lower nibble from the first byte
    return static_cast<ValueDataTemplate>(low & 0x0F);
}
void TregAccess::setAccessVdt(ValueDataTemplate vdt) {

    u8 hiNibble = (getLOByte(access) >> 4) & 0x0F;
    u8 loNibble = static_cast<u8>(vdt);

    u8 lowByte = (hiNibble << 4) | (loNibble & 0x0F);
    updateLOByte(access, lowByte);
}
u8 TregAccess::getAccessReserved(access_word v) {
    u8 low = getLOByte(v);
    //returns higher nibble from the first byte
    return ((low >> 4) & 0xF0);
}
bool TregAccess::permExists(PermissionFlags perms) const {
    u8 check = getHIByte(access);
    if (check & static_cast<u8>(perms))
        return true;

    return false;
}