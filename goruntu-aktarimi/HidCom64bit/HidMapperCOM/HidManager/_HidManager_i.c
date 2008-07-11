

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Jun 26 10:49:55 2008
 */
/* Compiler settings for _HidManager.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IButtonParams,0xC8B15683,0xFE3E,0x4a49,0x86,0x4B,0xED,0x9D,0xAA,0xA2,0x89,0x66);


MIDL_DEFINE_GUID(IID, IID_IHidManager,0xC1607EF3,0xCA5E,0x44bb,0xA0,0xBC,0x18,0x22,0x46,0x70,0xDB,0x31);


MIDL_DEFINE_GUID(IID, LIBID_HidManager,0x17A600B7,0x4DD1,0x4d8d,0xBA,0xF8,0xE4,0xFF,0xC5,0x92,0xB9,0x99);


MIDL_DEFINE_GUID(IID, DIID_IHidEvent,0x18976042,0x0890,0x4dd6,0xAC,0x79,0xC0,0x42,0xAB,0x95,0x89,0x4C);


MIDL_DEFINE_GUID(CLSID, CLSID_CButtonParams,0xD0CE718F,0x574B,0x41a3,0xB1,0x57,0x44,0x96,0xFB,0xC4,0x07,0x38);


MIDL_DEFINE_GUID(CLSID, CLSID_CHidManager,0xE0425E75,0x6644,0x45dd,0x87,0xFB,0x50,0x32,0x40,0xB2,0x13,0x6A);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



