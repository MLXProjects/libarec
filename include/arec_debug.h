#ifndef __arec_debug_h__
#define __arec_debug_h__

/* debug */
#define AREC_DEBUG_LEVEL 6

#if AREC_DEBUG_LEVEL >= 6
#define PPLOGE(...) \
		printf("E/PKGPARSER(): "); \
		printf(__VA_ARGS__); \
		printf("\n");
#else
#define PPLOGE(...)
#endif
#if AREC_DEBUG_LEVEL >= 5
#define PPLOGI(...) \
		printf("I/PKGPARSER(): "); \
		printf(__VA_ARGS__); \
		printf("\n");
#else
#define PPLOGI(...)
#endif
#if AREC_DEBUG_LEVEL >= 4
#define RLOGV(...) \
		printf("V/AREC(): "); \
		printf(__VA_ARGS__); \
		printf("\n");
#else
#define RLOGV(...)
#endif
#if AREC_DEBUG_LEVEL >= 3
#define RLOGW(...) \
		printf("W/AREC(): "); \
		printf(__VA_ARGS__); \
		printf("\n");
#else
#define RLOGW(...)
#endif
#if AREC_DEBUG_LEVEL >= 2
#define RLOGE(...) \
		printf("E/AREC(): "); \
		printf(__VA_ARGS__); \
		printf("\n");
#else
#define RLOGE(...)
#endif
#if AREC_DEBUG_LEVEL >= 1
#define RLOGI(...) \
		printf("I/AREC(): "); \
		printf(__VA_ARGS__); \
		printf("\n");
#else
#define RLOGI(...)
#endif

#define RLOGC(...) \
		fprintf(stdout, "C/AREC(): "); \
		fprintf(stdout, __VA_ARGS__); \
		fprintf(stdout, "\n");

#endif /* __arec_debug_h__ */