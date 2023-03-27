#pragma once
#include "RustApi.h"
#include "FFITypes.h"
FString Utf82FString(const NativeString& utfstr);
FString Utf8Ref2FString(const RefString& utfstr);
FName Utf82FName(const NativeString& utfstr);
FText Utf82FText(const NativeString& utfstr);
FText Utf8Ref2FText(const RefString& utfstr);
const char* FString2Utf8(FString fstr);
const char* FName2Utf8(FName fname);
const char* FText2Utf8(FText text);
void register_all(Plugin* plugin);
#define RSTR_TO_TCHAR(str, len) (TCHAR*)FUTF8ToTCHAR((const ANSICHAR*)str,(int32)len).Get()