
/* Tests e-vs-i or i-vs-m aspects for pcmp{e,i}str{i,m}.  Does not
   check the core arithmetic in any detail.  */

#include <string.h>
#include <stdio.h>
#include <assert.h>

typedef  unsigned char  V128[16];
typedef  unsigned int   UInt;
typedef  signed int     Int;
typedef  unsigned char  UChar;
typedef  unsigned long long int ULong;
typedef  UChar          Bool;
#define False ((Bool)0)
#define True  ((Bool)1)

void show_V128 ( V128* vec )
{
   Int i;
   for (i = 15; i >= 0; i--)
      printf("%02x", (UInt)( (*vec)[i] ));
}

void expand ( V128* dst, char* summary )
{
   Int i;
   assert( strlen(summary) == 16 );
   for (i = 0; i < 16; i++) {
      UChar xx = 0;
      UChar x = summary[15-i];
      if      (x >= '0' && x <= '9') { xx = x - '0'; }
      else if (x >= 'A' && x <= 'F') { xx = x - 'A' + 10; }
      else if (x >= 'a' && x <= 'f') { xx = x - 'a' + 10; }
      else assert(0);

      assert(xx < 16);
      xx = (xx << 4) | xx;
      assert(xx < 256);
      (*dst)[i] = xx;
   }
}

void one_test ( char* summL, UInt edxIN, char* summR, UInt eaxIN )
{
   V128 argL, argR;
   expand( &argL, summL );
   expand( &argR, summR );
   printf("\n");
   printf("edx %08x  argL ", edxIN);
   show_V128(&argL);
   printf("  eax %08x  argR ", eaxIN);
   show_V128(&argR);
   printf("\n");

   ULong block[ 2/*in:argL*/          // 0  0
                + 2/*in:argR*/        // 2  16
                + 1/*in:edx*/         // 4  32
                + 1/*in:eax*/         // 5  40
                + 2/*inout:xmm0*/     // 6  48
                + 1/*inout:ecx*/      // 8  64
                + 1/*out:rflags*/ ];  // 9  72
   assert(sizeof(block) == 80);

   UChar* blockC = (UChar*)&block[0];

   /* ---------------- ISTRI_4A ---------------- */
   memset(blockC, 0x55, 80);
   memcpy(blockC + 0,  &argL,  16);
   memcpy(blockC + 16, &argR,  16);
   memcpy(blockC + 24, &edxIN, 4);
   memcpy(blockC + 32, &eaxIN, 4);
   memcpy(blockC + 40, &edxIN, 4);
   __asm__ __volatile__(
      "movupd    0(%0), %%xmm2"           "\n\t"
      "movupd    16(%0), %%xmm6"          "\n\t"
      "movl      32(%0), %%edx"           "\n\t"
      "movl      40(%0), %%eax"           "\n\t"
      "movupd    48(%0), %%xmm0"          "\n\t"
      "movw      64(%0), %%cx"            "\n\t"
      "pcmpistri $0x4A, %%xmm2, %%xmm6"   "\n\t"
      "movupd    %%xmm0, 48(%0)"          "\n\t"
      "movw      %%cx, 64(%0)"            "\n\t"
      "pushfl"                            "\n\t"
      "popl      %%edi"                   "\n\t"
      "movl      %%edi, 72(%0)"           "\n\t"
      : /*out*/ 
      : /*in*/"r"(blockC) 
      : /*trash*/"memory","cc","xmm2","xmm6","xmm0","edx","eax","ecx","edi"
   );
   printf("  istri $0x4A:  ");
   printf("    xmm0 ");
   show_V128( (V128*)(blockC+48) );
   printf("  ecx %08x  flags %08x\n", (UInt)block[8], (UInt)block[9] & 0x8D5);

   /* ---------------- ISTRI_0A ---------------- */
   memset(blockC, 0x55, 80);
   memcpy(blockC + 0,  &argL,  16);
   memcpy(blockC + 16, &argR,  16);
   memcpy(blockC + 24, &edxIN, 4);
   memcpy(blockC + 32, &eaxIN, 4);
   memcpy(blockC + 40, &edxIN, 4);
   __asm__ __volatile__(
      "movupd    0(%0), %%xmm2"           "\n\t"
      "movupd    16(%0), %%xmm6"          "\n\t"
      "movl      32(%0), %%edx"           "\n\t"
      "movl      40(%0), %%eax"           "\n\t"
      "movupd    48(%0), %%xmm0"          "\n\t"
      "movw      64(%0), %%cx"            "\n\t"
      "pcmpistri $0x0A, %%xmm2, %%xmm6"   "\n\t"
      "movupd    %%xmm0, 48(%0)"          "\n\t"
      "movw      %%cx, 64(%0)"            "\n\t"
      "pushfl"                            "\n\t"
      "popl      %%edi"                   "\n\t"
      "movl      %%edi, 72(%0)"           "\n\t"
      : /*out*/ 
      : /*in*/"r"(blockC) 
      : /*trash*/"memory","cc","xmm2","xmm6","xmm0","edx","eax","ecx","edi"
   );
   printf("  istri $0x0A:  ");
   printf("    xmm0 ");
   show_V128( (V128*)(blockC+48) );
   printf("  ecx %08x  flags %08x\n", (UInt)block[8], (UInt)block[9] & 0x8D5);

   /* ---------------- ISTRM_4A ---------------- */
   memset(blockC, 0x55, 80);
   memcpy(blockC + 0,  &argL,  16);
   memcpy(blockC + 16, &argR,  16);
   memcpy(blockC + 24, &edxIN, 4);
   memcpy(blockC + 32, &eaxIN, 4);
   memcpy(blockC + 40, &edxIN, 4);
   __asm__ __volatile__(
      "movupd    0(%0), %%xmm2"           "\n\t"
      "movupd    16(%0), %%xmm6"          "\n\t"
      "movl      32(%0), %%edx"           "\n\t"
      "movl      40(%0), %%eax"           "\n\t"
      "movupd    48(%0), %%xmm0"          "\n\t"
      "movw      64(%0), %%cx"            "\n\t"
      "pcmpistrm $0x4A, %%xmm2, %%xmm6"   "\n\t"
      "movupd    %%xmm0, 48(%0)"          "\n\t"
      "movw      %%cx, 64(%0)"            "\n\t"
      "pushfl"                            "\n\t"
      "popl      %%edi"                   "\n\t"
      "movl      %%edi, 72(%0)"           "\n\t"
      : /*out*/ 
      : /*in*/"r"(blockC) 
      : /*trash*/"memory","cc","xmm2","xmm6","xmm0","edx","eax","ecx","edi"
   );
   printf("  istrm $0x4A:  ");
   printf("    xmm0 ");
   show_V128( (V128*)(blockC+48) );
   printf("  ecx %08x  flags %08x\n", (UInt)block[8], (UInt)block[9] & 0x8D5);

   /* ---------------- ISTRM_0A ---------------- */
   memset(blockC, 0x55, 80);
   memcpy(blockC + 0,  &argL,  16);
   memcpy(blockC + 16, &argR,  16);
   memcpy(blockC + 24, &edxIN, 4);
   memcpy(blockC + 32, &eaxIN, 4);
   memcpy(blockC + 40, &edxIN, 4);
   __asm__ __volatile__(
      "movupd    0(%0), %%xmm2"           "\n\t"
      "movupd    16(%0), %%xmm6"          "\n\t"
      "movl      32(%0), %%edx"           "\n\t"
      "movl      40(%0), %%eax"           "\n\t"
      "movupd    48(%0), %%xmm0"          "\n\t"
      "movw      64(%0), %%cx"            "\n\t"
      "pcmpistrm $0x0A, %%xmm2, %%xmm6"   "\n\t"
      "movupd    %%xmm0, 48(%0)"          "\n\t"
      "movw      %%cx, 64(%0)"            "\n\t"
      "pushfl"                            "\n\t"
      "popl      %%edi"                   "\n\t"
      "movl      %%edi, 72(%0)"           "\n\t"
      : /*out*/ 
      : /*in*/"r"(blockC) 
      : /*trash*/"memory","cc","xmm2","xmm6","xmm0","edx","eax","ecx","edi"
   );
   printf("  istrm $0x0A:  ");
   printf("    xmm0 ");
   show_V128( (V128*)(blockC+48) );
   printf("  ecx %08x  flags %08x\n", (UInt)block[8], (UInt)block[9] & 0x8D5);

   /* ---------------- ESTRI_4A ---------------- */
   memset(blockC, 0x55, 80);
   memcpy(blockC + 0,  &argL,  16);
   memcpy(blockC + 16, &argR,  16);
   memcpy(blockC + 24, &edxIN, 4);
   memcpy(blockC + 32, &eaxIN, 4);
   memcpy(blockC + 40, &edxIN, 4);
   __asm__ __volatile__(
      "movupd    0(%0), %%xmm2"           "\n\t"
      "movupd    16(%0), %%xmm6"          "\n\t"
      "movl      32(%0), %%edx"           "\n\t"
      "movl      40(%0), %%eax"           "\n\t"
      "movupd    48(%0), %%xmm0"          "\n\t"
      "movw      64(%0), %%cx"            "\n\t"
      "pcmpestri $0x4A, %%xmm2, %%xmm6"   "\n\t"
      "movupd    %%xmm0, 48(%0)"          "\n\t"
      "movw      %%cx, 64(%0)"            "\n\t"
      "pushfl"                            "\n\t"
      "popl      %%edi"                   "\n\t"
      "movl      %%edi, 72(%0)"           "\n\t"
      : /*out*/ 
      : /*in*/"r"(blockC) 
      : /*trash*/"memory","cc","xmm2","xmm6","xmm0","edx","eax","ecx","edi"
   );
   printf("  estri $0x4A:  ");
   printf("    xmm0 ");
   show_V128( (V128*)(blockC+48) );
   printf("  ecx %08x  flags %08x\n", (UInt)block[8], (UInt)block[9] & 0x8D5);

   /* ---------------- ESTRI_0A ---------------- */
   memset(blockC, 0x55, 80);
   memcpy(blockC + 0,  &argL,  16);
   memcpy(blockC + 16, &argR,  16);
   memcpy(blockC + 24, &edxIN, 4);
   memcpy(blockC + 32, &eaxIN, 4);
   memcpy(blockC + 40, &edxIN, 4);
   __asm__ __volatile__(
      "movupd    0(%0), %%xmm2"           "\n\t"
      "movupd    16(%0), %%xmm6"          "\n\t"
      "movl      32(%0), %%edx"           "\n\t"
      "movl      40(%0), %%eax"           "\n\t"
      "movupd    48(%0), %%xmm0"          "\n\t"
      "movw      64(%0), %%cx"            "\n\t"
      "pcmpestri $0x0A, %%xmm2, %%xmm6"   "\n\t"
      "movupd    %%xmm0, 48(%0)"          "\n\t"
      "movw      %%cx, 64(%0)"            "\n\t"
      "pushfl"                            "\n\t"
      "popl      %%edi"                   "\n\t"
      "movl      %%edi, 72(%0)"           "\n\t"
      : /*out*/ 
      : /*in*/"r"(blockC) 
      : /*trash*/"memory","cc","xmm2","xmm6","xmm0","edx","eax","ecx","edi"
   );
   printf("  estri $0x0A:  ");
   printf("    xmm0 ");
   show_V128( (V128*)(blockC+48) );
   printf("  ecx %08x  flags %08x\n", (UInt)block[8], (UInt)block[9] & 0x8D5);

   /* ---------------- ESTRM_4A ---------------- */
   memset(blockC, 0x55, 80);
   memcpy(blockC + 0,  &argL,  16);
   memcpy(blockC + 16, &argR,  16);
   memcpy(blockC + 24, &edxIN, 4);
   memcpy(blockC + 32, &eaxIN, 4);
   memcpy(blockC + 40, &edxIN, 4);
   __asm__ __volatile__(
      "movupd    0(%0), %%xmm2"           "\n\t"
      "movupd    16(%0), %%xmm6"          "\n\t"
      "movl      32(%0), %%edx"           "\n\t"
      "movl      40(%0), %%eax"           "\n\t"
      "movupd    48(%0), %%xmm0"          "\n\t"
      "movw      64(%0), %%cx"            "\n\t"
      "pcmpestrm $0x4A, %%xmm2, %%xmm6"   "\n\t"
      "movupd    %%xmm0, 48(%0)"          "\n\t"
      "movw      %%cx, 64(%0)"            "\n\t"
      "pushfl"                            "\n\t"
      "popl      %%edi"                   "\n\t"
      "movl      %%edi, 72(%0)"           "\n\t"
      : /*out*/ 
      : /*in*/"r"(blockC) 
      : /*trash*/"memory","cc","xmm2","xmm6","xmm0","edx","eax","ecx","edi"
   );
   printf("  estrm $0x4A:  ");
   printf("    xmm0 ");
   show_V128( (V128*)(blockC+48) );
   printf("  ecx %08x  flags %08x\n", (UInt)block[8], (UInt)block[9] & 0x8D5);

   /* ---------------- ESTRM_0A ---------------- */
   memset(blockC, 0x55, 80);
   memcpy(blockC + 0,  &argL,  16);
   memcpy(blockC + 16, &argR,  16);
   memcpy(blockC + 24, &edxIN, 4);
   memcpy(blockC + 32, &eaxIN, 4);
   memcpy(blockC + 40, &edxIN, 4);
   __asm__ __volatile__(
      "movupd    0(%0), %%xmm2"           "\n\t"
      "movupd    16(%0), %%xmm6"          "\n\t"
      "movl      32(%0), %%edx"           "\n\t"
      "movl      40(%0), %%eax"           "\n\t"
      "movupd    48(%0), %%xmm0"          "\n\t"
      "movw      64(%0), %%cx"            "\n\t"
      "pcmpestrm $0x0A, %%xmm2, %%xmm6"   "\n\t"
      "movupd    %%xmm0, 48(%0)"          "\n\t"
      "movw      %%cx, 64(%0)"            "\n\t"
      "pushfl"                            "\n\t"
      "popl      %%edi"                   "\n\t"
      "movl      %%edi, 72(%0)"           "\n\t"
      : /*out*/ 
      : /*in*/"r"(blockC) 
      : /*trash*/"memory","cc","xmm2","xmm6","xmm0","edx","eax","ecx","edi"
   );
   printf("  estrm $0x0A:  ");
   printf("    xmm0 ");
   show_V128( (V128*)(blockC+48) );
   printf("  ecx %08x  flags %08x\n", (UInt)block[8], (UInt)block[9] & 0x8D5);




}

int main ( void )
{
   one_test("aaaaaaaaaaaaaaaa", 0, "aaaaaaaa0aaaaaaa", 0 );
   one_test("0000000000000000", 0, "aaaaaaaa0aaaaaaa", 0 );

   one_test("aaaaaaaaaaaaaaaa", 0, "aaaaaaaaaaaaaaaa", 0 );
   one_test("aaaaaaaaaaaaaaaa", 5, "aaaaaaaaaaaaaaaa", 0 );
   one_test("aaaaaaaaaaaaaaaa", 0, "aaaaaaaaaaaaaaaa", 6 );

   one_test("aaaaaaaaaaaaaaaa", 5, "aaaaaaaaaaaaaaaa", 6 );
   one_test("aaaaaaaaaaaaaaaa", 5, "aaaaaaaaaaaaaaaa", 15 );
   one_test("aaaaaaaaaaaaaaaa", 5, "aaaaaaaaaaaaaaaa", 16 );
   one_test("aaaaaaaaaaaaaaaa", 5, "aaaaaaaaaaaaaaaa", 17 );

   one_test("aaaaaaaaaaaaaaaa", 5, "aaaaaaaaaaaaaaaa", -6 );
   one_test("aaaaaaaaaaaaaaaa", 5, "aaaaaaaaaaaaaaaa", -15 );
   one_test("aaaaaaaaaaaaaaaa", 5, "aaaaaaaaaaaaaaaa", -16 );
   one_test("aaaaaaaaaaaaaaaa", 5, "aaaaaaaaaaaaaaaa", -17 );

   one_test("aaaaaaaaaaaaaaaa", 5,  "aaaaaaaaaaaaaaaa", 6 );
   one_test("aaaaaaaaaaaaaaaa", 15, "aaaaaaaaaaaaaaaa", 6 );
   one_test("aaaaaaaaaaaaaaaa", 16, "aaaaaaaaaaaaaaaa", 6 );
   one_test("aaaaaaaaaaaaaaaa", 17, "aaaaaaaaaaaaaaaa", 6 );

   one_test("aaaaaaaaaaaaaaaa", -5,  "aaaaaaaaaaaaaaaa", 6 );
   one_test("aaaaaaaaaaaaaaaa", -15, "aaaaaaaaaaaaaaaa", 6 );
   one_test("aaaaaaaaaaaaaaaa", -16, "aaaaaaaaaaaaaaaa", 6 );
   one_test("aaaaaaaaaaaaaaaa", -17, "aaaaaaaaaaaaaaaa", 6 );

   return 0;
}
