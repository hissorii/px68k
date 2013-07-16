#ifdef __cplusplus
extern "C" {
#endif

#if 1

#define	juliet_load()
#define	juliet_unload()

#define	juliet_prepare()

#define	juliet_YM2151Reset()
#define	juliet_YM2151IsEnable()	FALSE

#define	juliet_YM2151W(r,d)

#else

BOOL juliet_load(void);
void juliet_unload(void);

BOOL juliet_prepare(void);

void juliet_YM2151Reset(void);
int juliet_YM2151IsEnable(void);
int juliet_YM2151IsBusy(void);
void juliet_YM2151W(BYTE reg, BYTE data);

void juliet_YMF288Reset(void);
int juliet_YMF288Enable(void);
int juliet_YMF288IsBusy(void);
void juliet_YMF288A(BYTE addr, BYTE data);
void juliet_YMF288B(BYTE addr, BYTE data);
void juliet_YMF288W(BYTE addr, BYTE data);
BYTE juliet_YMF288R(BYTE addr);

#endif

#ifdef __cplusplus
}
#endif
