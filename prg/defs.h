#ifndef DEFS
#define DEFS 1

#define MAXLEX 100
#define TEnd 0x200
#define TError 0x100

#define Tid 0x1

#define TConstDec 0x20
#define TConstHex 0x21

#define TSqBrackOpen 0x30
#define TSqBrackClose 0x31
#define TCrBrackOpen 0x32
#define TCrBrackClose 0x33
#define TEgBrackOpen 0x34
#define TEgBrackClose 0x35

#define TPlus 0x40
#define TMinus 0x41
#define TDiv 0x42
#define TMul 0x43
#define TRest 0x44
#define TGrEq 0x45
#define TLessEq 0x46
#define TEq 0x47
#define TNotEq 0x48
#define TAnd 0x49
#define TOr 0x4A
#define TGr 0x4B
#define TLess 0x4C
#define TNot 0x4D
#define TAssign 0x4E

#define TComma 0x50
#define TSem 0x51

#define TVoid 0x60
#define TInt 0x61
#define TInt64 0x62
#define TMain 0x63
#define TFor 0x64

#define TMaxLenOverflow 0x300

#endif
