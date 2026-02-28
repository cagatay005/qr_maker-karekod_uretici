#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum karekod_HataDuzeltme {
	karekod_HataDuzeltme_DUSUK = 0,
	karekod_HataDuzeltme_ORTA,
	karekod_HataDuzeltme_CEYREK,
	karekod_HataDuzeltme_YUKSEK,
};

enum karekod_Maske {
	karekod_Maske_OTOMATIK = -1,
	karekod_Maske_0 = 0,
	karekod_Maske_1,
	karekod_Maske_2,
	karekod_Maske_3,
	karekod_Maske_4,
	karekod_Maske_5,
	karekod_Maske_6,
	karekod_Maske_7,
};

enum karekod_Mod {
	karekod_Mod_SAYISAL      = 0x1,
	karekod_Mod_ALFASAYISAL  = 0x2,
	karekod_Mod_BAYT         = 0x4,
	karekod_Mod_KANJI        = 0x8,
	karekod_Mod_ECI          = 0x7,
};

struct karekod_Bolum {
	enum karekod_Mod mod;
	int karakterSayisi;
	uint8_t *veri;
	int bitUzunlugu;
};

#define karekod_SURUM_MIN   1
#define karekod_SURUM_MAKS  40

#define karekod_SURUM_ICIN_TAMPON_UZUNLUGU(n)  ((((n) * 4 + 17) * ((n) * 4 + 17) + 7) / 8 + 1)

#define karekod_TAMPON_UZUNLUGU_MAKS  karekod_SURUM_ICIN_TAMPON_UZUNLUGU(karekod_SURUM_MAKS)

bool karekod_metinKodla(const char *metin, uint8_t geciciTampon[], uint8_t karekod[],
	enum karekod_HataDuzeltme hataDuzeltme, int minSurum, int maksSurum, enum karekod_Maske maske, bool hataDuzeltmeyiArtir);

bool karekod_ikiliKodla(uint8_t veriVeGecici[], size_t veriUzunlugu, uint8_t karekod[],
	enum karekod_HataDuzeltme hataDuzeltme, int minSurum, int maksSurum, enum karekod_Maske maske, bool hataDuzeltmeyiArtir);

bool karekod_bolumleriKodla(const struct karekod_Bolum bolumler[], size_t uzunluk,
	enum karekod_HataDuzeltme hataDuzeltme, uint8_t geciciTampon[], uint8_t karekod[]);

bool karekod_bolumleriKodlaGelismis(const struct karekod_Bolum bolumler[], size_t uzunluk, enum karekod_HataDuzeltme hataDuzeltme,
	int minSurum, int maksSurum, enum karekod_Maske maske, bool hataDuzeltmeyiArtir, uint8_t geciciTampon[], uint8_t karekod[]);

bool karekod_sayisalMi(const char *metin);

bool karekod_alfasayisalMi(const char *metin);

size_t karekod_bolumTamponBoyutuHesapla(enum karekod_Mod mod, size_t karakterSayisi);

struct karekod_Bolum karekod_baytOlustur(const uint8_t veri[], size_t uzunluk, uint8_t tampon[]);

struct karekod_Bolum karekod_sayisalOlustur(const char *rakamlar, uint8_t tampon[]);

struct karekod_Bolum karekod_alfasayisalOlustur(const char *metin, uint8_t tampon[]);

struct karekod_Bolum karekod_eciOlustur(long atamaDegeri, uint8_t tampon[]);

int karekod_boyutAl(const uint8_t karekod[]);

bool karekod_modulAl(const uint8_t karekod[], int x, int y);

#ifdef __cplusplus
}
#endif