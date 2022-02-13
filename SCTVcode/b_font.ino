// -------------------- 12x20 proportional font ------------------

// Table is in ASCII character order. 
// An index table Font points to entries in this list
// to allow variable segment count.

// The lower left corner is the origin (0,0).
// The center of the typical character is at (6,10).
// The upper right corner of the typical character is (12,20).
// Descending lower case characters exist.

// Each segment uses seven signed int parameters:

// cir      Draw a circle, ellipse or arc
// XCenter  X Offset from LL corner to center of circle
// YCenter  Y Offset from LL corner to center of circle
// XSize    Width of circle in diameter units
// YSize    Height of circle in diameter units
// FirstO   Start angle 0..7 is 0 deg .. 315 deg 0 = E, 90 = N, 180 = W, 270 = S
// LastO    End angle 1..14 is 45 deg .. 630 deg
// The character ends with a 0x80 which tells DoChar to stop, plus the char width.

// lin      Draw a straight line
// XStart   X start of line
// YStart   Y start of line
// XEnd     X end of line
// YEnd     Y end of line
// FirstO   ignored
// LastO    ignored
// The character ends with a 0x80 which tells DoChar to stop, plus the char width.

const int OurBigKern = 2;  // this font's kern size for 40-up scales
const int OurLilKern = 3;  // this font's kern size for 30-down scales
const int OurBigGap = 4;   // this font's inter-row gap for 40-up scales
const int OurLilGap = 10;  // this font's inter-row gap for 30-down scales
const int OurChrHt = 20;   // this font's character height

// circle:              cir,XC,YC,XS,YS,FO,LO{,width|0x80}
// line:                lin,XS,YS,XE,YE,FO,LO{,width|0x80}
const int Space[]   = {0x86};      // space does nothing but move over
const int Exclam[]  = { lin, 1, 6, 1,20, 6,13, 
                        cir, 1, 1, 2, 2, 6,13, 0x82};
const int DQuot[]   = { lin, 0,12, 0,20, 6,13, 
                        lin, 6,12, 6,20, 6,13, 0x86};
const int Sharp[]   = { lin, 2, 4, 4,20, 6,13, 
                        lin, 7, 4, 9,20, 6,13, 
                        lin, 0, 9,10, 9, 6,13, 
                        lin, 1,15,11,15, 6,13, 0x8c};
const int Dollar[]  = { cir, 5, 6,10, 8, 4, 9,
                        cir, 5,14,10, 8, 0, 5, 
                        lin, 5, 2, 5,22, 6,13, 0x8a};
const int Percent[] = { cir, 3,17, 6, 6, 6,13, 
                        cir, 9, 3, 6, 6, 6,13, 
                        lin, 3, 0,15,20, 6,13, 0x8c};
const int Amper[]   = { cir, 4,15, 8,10, 6,12,
                        cir, 4, 5, 8,10, 2, 5, 
                        cir, 4, 8,16,16, 6, 7, 
                        lin, 1,12,11, 0, 6,13, 0x8c};
const int Apost[]   = { cir, 3,19, 2, 2, 6,13, 
                        cir, 0,19, 8,12, 6, 7, 0x84};
const int LParen[]  = { cir, 4,10, 8,20, 2, 5, 0x84};
const int RParen[]  = { cir, 0,10, 8,20, 6, 9, 0x84};
const int Aster[]   = { lin, 0,10,12,10, 6,13, 
                        lin, 2, 4,10,16, 6,13, 
                        lin, 2,16,10, 4, 6,13, 0x8c};
const int Plus[]    = { lin, 0,10,12,10, 6,13, 
                        lin, 6, 4, 6,16, 6,13, 0x8c};
const int Comma[]   = { cir, 3, 1, 2, 2, 6,13, 
                        cir, 0, 1, 8,12, 6, 7, 0x84};
const int Minus[]   = { lin, 0,10,12,10, 6,13, 0x8c};
const int Period[]  = { cir, 1, 1, 2, 2, 6,13, 0x82};
const int Slash[]   = { lin, 0, 0,12,20, 6,13, 0x8c};
const int Zero[]    = { cir, 6,10,12,20, 6,13, 0x8c};
const int One[]     = { lin, 7, 0, 7,20, 6,13, 
                        lin, 3,16, 7,20, 6,13, 0x8c};
const int Two[]     = { cir, 6,14,12,12, 6,11,
                        cir, 6, 0,12,16, 2, 3, 
                        lin, 0, 0,12, 0, 6,13, 0x8c};
const int Three[]   = { cir, 6, 6,12,12, 5, 9,
                        lin, 1,20,11,20, 6,13, 
                        lin, 6,12,11,20, 6,13, 0x8c};
const int Four[]    = { lin, 8, 0, 8,20, 6,13, 
                        lin, 0, 6,12, 6, 6,13, 
                        lin, 0, 6, 8,20, 6,13, 0x8c};
const int Five[]    = { cir, 6, 6,12,12, 5,10,
                        lin, 2,10, 4,20, 6,13, 
                        lin, 4,20,12,20, 6,13, 0x8c};
const int Six[]     = { cir, 6, 6,12,12, 6,13, 
                        lin, 1,10, 8,20, 6,13, 0x8c};
const int Seven[]   = { lin, 0, 0,12,20, 6,13, 
                        lin, 0,20,12,20, 6,13, 0x8c};
const int Eight[]   = { cir, 6, 6,12,12, 6,13, 
                        cir, 6,16, 8, 8, 6,13, 0x8c};
const int Nine[]    = { cir, 6,14,12,12, 6,13, 
                        lin, 4, 0,11,10, 6,13, 0x8c};
const int Colon[]   = { cir, 2, 6, 4, 4, 6,13, 
                        cir, 2,14, 4, 4, 6,13, 0x84};
const int SemiCol[] = { cir, 3,14, 2, 2, 6,13, 
                        cir, 3, 6, 2, 2, 6,13, 
                        cir, 0, 6, 8,12, 6, 7, 0x84};
const int LThan[]   = { lin, 0,10,12,18, 6,13, 
                        lin, 0,10,12, 2, 6,13, 0x8c};
const int Equal[]   = { lin, 0,13,12,13, 6,13, 
                        lin, 0, 7,12, 7, 6,13, 0x8c};
const int GThan[]   = { lin, 0,18,12,10, 6,13, 
                        lin, 0, 2,12,10, 6,13, 0x8c};
const int Quest[]   = { cir, 5,14,10,10, 6,11,
                        cir, 5, 7, 4, 4, 2, 7,
                        cir, 5, 1, 2, 2, 6,13, 0x8a};
const int AtSign[]  = { cir, 3,10, 6,10, 6,13, 
                        cir, 3,10,14,20, 0, 6,
                        cir, 8,10, 4, 4, 4, 7, 0x8c};
const int BigA[]    = { lin, 0, 0, 6,20, 6,13, 
                        lin, 6,20,12, 0, 6,13, 
                        lin, 3, 8, 9, 8, 6,13, 0x8c};
const int BigB[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin, 0, 0, 8, 0, 6,13, 
                        lin, 0,10, 8,10, 6,13, 
                        lin, 0,20, 8,20, 6,13, 
                        cir, 8, 5,10,10, 6, 9, 
                        cir, 8,15,10,10, 6, 9, 0x8c};
const int BigC[]    = { cir, 7,10,14,20, 1, 6, 0x8c};
const int BigD[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin, 0, 0, 4, 0, 6,13, 
                        lin, 0,20, 4,20, 6,13, 
                        cir, 4,10,16,20, 6, 9, 0x8c};
const int BigE[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin, 0, 0,12, 0, 6,13, 
                        lin, 0,10, 8,10, 6,13, 
                        lin, 0,20,12,20, 6,13, 0x8c};
const int BigF[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin, 0,10, 8,10, 6,13, 
                        lin, 0,20,12,20, 6,13, 0x8c};
const int BigG[]    = { cir, 7,10,14,20, 1, 6,
                        lin,11, 2,11, 8, 6,13, 
                        lin, 7, 8,11, 8, 6,13, 0x8c};
const int BigH[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin,12, 0,12,20, 6,13, 
                        lin, 0,10,12,10, 6,13, 0x8c};
const int BigI[]    = { lin, 2, 0, 2,20, 6,13, 
                        lin, 1, 0, 5, 0, 6,13, 
                        lin, 1,20, 5,20, 6,13, 0x84};
const int BigJ[]    = { lin,12, 6,12,20, 6,13, 
                        cir, 6, 6,12,12, 4, 7, 0x8c};
const int BigK[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin, 0,10,12, 0, 6,13, 
                        lin, 0,10,12,20, 6,13, 0x8c};
const int BigL[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin, 0, 0,12, 0, 6,13, 0x8c};
const int BigM[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin,12, 0,12,20, 6,13, 
                        lin, 0,20, 6,10, 6,13, 
                        lin, 6,10,12,20, 6,13, 0x8c};
const int BigN[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin,12, 0,12,20, 6,13, 
                        lin, 0,20,12, 0, 6,13, 0x8c};
const int BigO[]    = { cir, 6,10,12,20, 6,13, 0x8c};
const int BigP[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin, 0,10, 8,10, 6,13, 
                        lin, 0,20, 8,20, 6,13, 
                        cir, 8,15,10,10, 6, 9, 0x8c};
const int BigQ[]    = { cir, 6,10,12,20, 6,13, 
                        lin, 8, 6,12, 0, 6,13, 0x8c};
const int BigR[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin, 0,10, 8,10, 6,13, 
                        lin, 0,20, 8,20, 6,13, 
                        cir, 8,15,10,10, 6, 9, 
                        lin, 6,10,12, 0, 6,13, 0x8c};
const int BigS[]    = { cir, 6, 5,12,10, 4, 9,
                        cir, 6,15,12,10, 0, 5, 0x8c};
const int BigT[]    = { lin, 6, 0, 6,20, 6,13, 
                        lin, 0,20,12,20, 6,13, 0x8c};
const int BigU[]    = { lin, 0, 6, 0,20, 6,13, 
                        lin,12, 6,12,20, 6,13, 
                        cir, 6, 6,12,12, 4, 7, 0x8c};
const int BigV[]    = { lin, 0,20, 6, 0, 6,13, 
                        lin, 6, 0,12,20, 6,13, 0x8c};
const int BigW[]    = { lin, 0, 0, 0,20, 6,13, 
                        lin,12, 0,12,20, 6,13, 
                        lin, 0, 0, 6,10, 6,13, 
                        lin, 6,10,12, 0, 6,13, 0x8c};
const int BigX[]    = { lin, 0,20,12, 0, 6,13, 
                        lin, 0, 0,12,20, 6,13, 0x8c};
const int BigY[]    = { lin, 6, 0, 6,10, 6,13, 
                        lin, 0,20, 6,10, 6,13, 
                        lin, 6,10,12,20, 6,13, 0x8c};
const int BigZ[]    = { lin, 0, 0,12,20, 6,13, 
                        lin, 0, 0,12, 0, 6,13, 
                        lin, 0,20,12,20, 6,13, 0x8c};
const int LftSqBr[] = { lin, 0, 0, 0,20, 6,13, 
                        lin, 0, 0, 4, 0, 6,13, 
                        lin, 0,20, 4,20, 6,13, 0x84};
const int BackSl[]  = { lin, 0,20,12, 0, 6,13, 0x8c};
const int RtSqBr[]  = { lin, 4, 0, 4,20, 6,13, 
                        lin, 0, 0, 4, 0, 6,13, 
                        lin, 0,20, 4,20, 6,13, 0x84};
const int Carat[]   = { lin, 0,10, 6,16, 6,13, 
                        lin, 6,16,12,10, 6,13, 0x8c};
const int UnderSc[] = { lin, 0, 0,12, 0, 6,13, 0x8c};
const int BackQu[]  = { lin, 0,20, 4,12, 6,13, 0x84};
const int SmallA[]  = { cir, 5, 6,10,12, 6,13, 
                        lin,10, 0,10,12, 6,13, 0x8a};
const int SmallB[]  = { cir, 5, 6,10,12, 6,13, 
                        lin, 0, 0, 0,20, 6,13, 0x8a};
const int SmallC[]  = { cir, 5, 6,10,12, 1, 6, 0x88};
const int SmallD[]  = { cir, 5, 6,10,12, 6,13, 
                        lin,10, 0,10,20, 6,13, 0x8a};
const int SmallE[]  = { cir, 5, 6,10,12, 0, 6,
                        lin, 0, 6,10, 6, 6,13, 0x8a};
const int SmallF[]  = { cir, 7,16, 6, 8, 0, 3, 
                        lin, 0,10, 8,10, 6,13, 
                        lin, 4, 0, 4,16, 6,13, 0x8a};
const int SmallG[]  = { cir, 5, 6,10,12, 6,13, 
                        lin,10, 0,10,12, 6,13, 
                        cir, 5, 0,10,12, 5, 7, 0x8a};
const int SmallH[]  = { cir, 4, 8, 8, 8, 0, 3, 
                        lin, 0, 0, 0,20, 6,13, 
                        lin, 8, 0, 8, 8, 6,13, 0x88};
const int SmallI[]  = { cir, 1,16, 2, 2, 6,13, 
                        lin, 1, 0, 1,12, 6,13, 0x82};
const int SmallJ[]  = { cir, 6,16, 2, 2, 6,13, 
                        lin, 6, 0, 6,12, 6,13, 
                        cir, 3, 0, 6, 8, 5, 7, 0x88};
const int SmallK[]  = { lin, 0, 0, 0,20, 6,13, 
                        lin, 0, 4, 8,12, 6,13, 
                        lin, 1, 6, 7, 0, 6,13, 0x88};
const int SmallL[]  = { lin, 1, 0, 1,20, 6,13, 0x82};
const int SmallM[]  = { lin, 0, 0, 0,12, 6,13, 
                        cir, 4, 8, 8, 8, 0, 3, 
                        lin, 8, 0, 8, 8, 6,13, 
                        cir,12, 8, 8, 8, 0, 3, 
                        lin,16, 0,16, 8, 6,13, 0x90};
const int SmallN[]  = { lin, 0, 0, 0,12, 6,13, 
                        cir, 4, 8, 8, 8, 0, 3, 
                        lin, 8, 0, 8, 8, 6,13, 0x88};
const int SmallO[]  = { cir, 5, 6,10,12, 6,13, 0x8a};
const int SmallP[]  = { cir, 5, 6,10,12, 6,13, 
                        lin, 0,-4, 0,12, 6,13, 0x8a};
const int SmallQ[]  = { cir, 5, 6,10,12, 6,13, 
                        lin,10,-4,10,12, 6,13, 0x8a};
const int SmallR[]  = { lin, 0, 0, 0,12, 6,13, 
                        cir, 5, 6,10,12, 1, 3, 0x88};
const int SmallS[]  = { cir, 4, 9, 8, 6, 0, 5, 
                        cir, 4, 3, 8, 6, 4, 9, 0x88};
const int SmallT[]  = { // cir, 8, 4, 8, 8, 4, 5, 
                        lin, 0,12, 8,12, 6,13, 
                        lin, 4, 0, 4,16, 6,13, 0x88};
const int SmallU[]  = { lin, 8, 0, 8,12, 6,13, 
                        cir, 4, 4, 8, 8, 4, 7, 
                        lin, 0, 4, 0,12, 6,13, 0x88};
const int SmallV[]  = { lin, 0,12, 4, 0, 6,13, 
                        lin, 4, 0, 8,12, 6,13, 0x88};
const int SmallW[]  = { lin, 0,12, 4, 0, 6,13, 
                        lin, 4, 0, 8,12, 6,13, 
                        lin, 8,12,12, 0, 6,13, 
                        lin,12, 0,16,12, 6,13, 0x90};
const int SmallX[]  = { lin, 0,12, 8, 0, 6,13, 
                        lin, 0, 0, 8,12, 6,13, 0x88};
const int SmallY[]  = { lin, 0,12, 4, 0, 6,13, 
                        lin, 4, 0, 8,12, 6,13, 
                        cir, 1, 0, 6, 8, 6, 7, 0x88};
const int SmallZ[]  = { lin, 0, 0, 8, 0, 6,13, 
                        lin, 0,12, 8,12, 6,13, 
                        lin, 0, 0, 8,12, 6,13, 0x88};
const int LfBrace[] = { cir, 8, 6, 8,12, 4, 5, 
                        cir, 0, 6, 8, 8, 0, 1, 
                        cir, 0,14, 8, 8, 6, 7, 
                        cir, 8,14, 8,12, 2, 3, 0x88};
const int VertBar[] = { lin, 1, 1, 1,21, 6,13, 0x82};
const int RtBrace[] = { cir, 0, 6, 8,12, 6, 7, 
                        cir, 8, 6, 8, 8, 2, 3, 
                        cir, 8,14, 8, 8, 4, 5, 
                        cir, 0,14, 8,12, 0, 1, 0x88};
const int Tilde[]   = { cir, 3,12, 6, 4, 0, 3, 
                        cir, 9,12, 6, 4, 4, 7, 0x8c};
const int Rubout[]  = { lin, 0,10, 6,20, 6,13, 
                        lin, 0, 0,12,20, 6,13, 
                        lin, 6, 0,12,10, 6,13, 0x8c};

// All the characters, arranged in ASCII order because ASCII rules
const int * const Font[] = 
{Space, Exclam,DQuot, Sharp,  Dollar, Percent,Amper, Apost,
 LParen,RParen,Aster, Plus,   Comma,  Minus,  Period,Slash,
 Zero,  One,   Two,   Three,  Four,   Five,   Six,   Seven,
 Eight, Nine,  Colon, SemiCol,LThan,  Equal,  GThan, Quest,
 AtSign,BigA,  BigB,  BigC,   BigD,   BigE,   BigF,  BigG,
 BigH,  BigI,  BigJ,  BigK,   BigL,   BigM,   BigN,  BigO,
 BigP,  BigQ,  BigR,  BigS,   BigT,   BigU,   BigV,  BigW,
 BigX,  BigY,  BigZ,  LftSqBr,BackSl, RtSqBr, Carat, UnderSc,
 BackQu,SmallA,SmallB,SmallC, SmallD, SmallE, SmallF,SmallG,
 SmallH,SmallI,SmallJ,SmallK, SmallL, SmallM, SmallN,SmallO,
 SmallP,SmallQ,SmallR,SmallS, SmallT, SmallU, SmallV,SmallW,
 SmallX,SmallY,SmallZ,LfBrace,VertBar,RtBrace,Tilde, Rubout};
