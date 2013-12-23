#ifndef DEFS
#define DEFS 1

#define MAXLEX 100
#define TEnd 0x200
#define TError 0x100

#define Tid 0x1

#define TConstDec 0x20
#define TConstHex 0x21
#define TConst 0x22

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

/* Vn -------------------- */


#define TPrg 0x70

#define TFirstVn TPrg
#define TVarListOrMain 0x71
#define TVarList 0x72
#define TListTail 0x73
#define TListElem 0x74
#define TPosAssignOrArray 0x75
#define TArrLen 0x76
#define TPossArrInit 0x77
#define TConstList 0x78
#define TConstTail 0x79
#define TPosAss 0x7A
#define TBlock 0x7B
#define TOps 0x7C
#define TOp 0x7D
#define TPosInd 0x7E
#define TFunCall 0x7F

#define TExpr 0x80
#define TA1 0x81
#define TA1H 0x82
#define TA2 0x83
#define TA2H 0x84
#define TA5 0x85
#define TA5H 0x86
#define TA6 0x87
#define TForCycle 0x88
#define TLastVn TForCycle

#endif
