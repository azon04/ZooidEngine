#ifndef __LOOKUP3__
#define __LOOKUP3__

#include <stdio.h>      /* defines printf for tests */
#include <time.h>       /* defines time_t for timings in the test */
#include <stdint.h>     /* defines uint32_t etc */
#ifndef _MSC_VER
#include <sys/param.h>  /* attempt to define endianness */
#endif
#ifdef linux
# include <endian.h>    /* attempt to define endianness */
#endif

/*
* My best guess at if you are big-endian or little-endian.  This may
* need adjustment.
*/
#if (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && \
     __BYTE_ORDER == __LITTLE_ENDIAN) || \
    (defined(i386) || defined(__i386__) || defined(__i486__) || \
     defined(__i586__) || defined(__i686__) || defined(vax) || defined(MIPSEL))
# define HASH_LITTLE_ENDIAN 1
# define HASH_BIG_ENDIAN 0
#elif (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && \
       __BYTE_ORDER == __BIG_ENDIAN) || \
      (defined(sparc) || defined(POWERPC) || defined(mc68000) || defined(sel))
# define HASH_LITTLE_ENDIAN 0
# define HASH_BIG_ENDIAN 1
#elif (defined(_MSC_VER))
# include <stdint.h>
# define HASH_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)
# define HASH_LITTLE_ENDIAN !(*(uint16_t *)"\0\xff" < 0x100)
#else
# define HASH_LITTLE_ENDIAN 0
# define HASH_BIG_ENDIAN 0
#endif

uint32_t hashword(
	const uint32_t *k,                   /* the key, an array of uint32_t values */
	size_t          length,               /* the length of the key, in uint32_ts */
	uint32_t        initval);        /* the previous hash, or an arbitrary value */

uint32_t hashlittle(const void *key, size_t length, uint32_t initval);

uint32_t hashbig(const void *key, size_t length, uint32_t initval);

#endif
