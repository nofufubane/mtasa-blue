/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/HookSystem.h
 *  PURPOSE:     Function hook installation system
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define     MAX_JUMPCODE_SIZE           20

template <typename T>
void* FunctionPointerToVoidP(T func)
{
    union
    {
        T     a;
        void* b;
    } c = {func};
    return c.b;
}

template <typename T>
bool HookInstall(DWORD dwInstallAddress, T dwHookHandler, int iJmpCodeSize = 5)
{
    BYTE JumpBytes[MAX_JUMPCODE_SIZE];
    MemSetFast(JumpBytes, 0x90, MAX_JUMPCODE_SIZE);
    if (CreateJump(dwInstallAddress, (DWORD)FunctionPointerToVoidP(dwHookHandler), JumpBytes))
    {
        MemCpy((PVOID)dwInstallAddress, JumpBytes, iJmpCodeSize);
        return true;
    }
    else
    {
        return false;
    }
}

BYTE* CreateJump(DWORD dwFrom, DWORD dwTo, BYTE* ByteArray);

// Auto detect requirement of US/EU hook installation
#define EZHookInstall(type) \
        __if_not_exists( RETURN_##type##_US ) \
        { \
            HookInstall( HOOKPOS_##type, (DWORD)HOOK_##type, HOOKSIZE_##type ) \
        } \
        __if_exists( RETURN_##type##_US ) \
        { \
            if ( pGame->GetGameVersion () == VERSION_US_10 ) \
            { \
                EZHookInstall_HERE( type, US ) \
            } \
            else \
            { \
                EZHookInstall_HERE( type, EU ) \
            } \
        }

// US/EU hook installation
// Includes additional return pointer copies if required
#define EZHookInstall_HERE(type,CO) \
        HookInstall( HOOKPOS_##type##_##CO##, (DWORD)HOOK_##type, HOOKSIZE_##type##_##CO## ); \
        RETURN_##type##_BOTH = RETURN_##type##_##CO##; \
        __if_exists( RETURN_##type##B_##CO## ) \
        { \
            RETURN_##type##B_BOTH = RETURN_##type##B_##CO##; \
        } \
        __if_exists( RETURN_##type##C_##CO## ) \
        { \
            RETURN_##type##C_BOTH = RETURN_##type##C_##CO##; \
        }

// Structure for holding hook info
struct SHookInfo
{
    template <class T>
    SHookInfo(DWORD dwAddress, T dwHook, uint uiSize) : dwAddress(dwAddress), dwHook((DWORD)dwHook), uiSize(uiSize)
    {
    }
    DWORD dwAddress;
    DWORD dwHook;
    uint  uiSize;
};

#define MAKE_HOOK_INFO(type)  SHookInfo ( HOOKPOS_##type, HOOK_##type, HOOKSIZE_##type )

// Structure for holding poke info
struct SPokeInfo
{
    DWORD dwAddress;
    BYTE  ucValue;
};
