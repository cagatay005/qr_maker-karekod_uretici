#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "karekod.h"

#ifndef KAREKOD_TEST
	#define test_edilebilir static
#else
	#define test_edilebilir
#endif

test_edilebilir void tamponaBitEkle(unsigned int deger, int bitSayisi, uint8_t tampon[], int *bitUzunlugu);

test_edilebilir void hataDuzeltmeEkleVeBirlestir(uint8_t veri[], int surum, enum karekod_HataDuzeltme hataDuzeltme, uint8_t sonuc[]);
test_edilebilir int veriKodSozcukSayisiAl(int surum, enum karekod_HataDuzeltme hataDuzeltme);
test_edilebilir int hamVeriModulSayisiAl(int surum);

test_edilebilir void reedSolomonBolenHesapla(int derece, uint8_t sonuc[]);
test_edilebilir void reedSolomonKalanHesapla(const uint8_t veri[], int veriUzunlugu,
	const uint8_t uretec[], int derece, uint8_t sonuc[]);
test_edilebilir uint8_t reedSolomonCarp(uint8_t x, uint8_t y);

test_edilebilir void islevModulleriniBaslat(int surum, uint8_t karekod[]);
static void acikIslevModulleriniCiz(uint8_t karekod[], int surum);
static void bicimBitleriniCiz(enum karekod_HataDuzeltme hataDuzeltme, enum karekod_Maske maske, uint8_t karekod[]);
test_edilebilir int hizalamaDeseniKonumlariniAl(int surum, uint8_t sonuc[7]);
static void dikdortgenDoldur(int sol, int ust, int genislik, int yukseklik, uint8_t karekod[]);

static void kodSozcukleriniCiz(const uint8_t veri[], int veriUzunlugu, uint8_t karekod[]);
static void maskeUygula(const uint8_t islevModulleri[], uint8_t karekod[], enum karekod_Maske maske);
static long cezaPuaniAl(const uint8_t karekod[]);
static int bulucuCezaDesenleriSay(const int gecmis[7], int karekodBoyutu);
static int bulucuCezaSonlandirVeSay(bool mevcutRenk, int mevcutUzunluk, int gecmis[7], int karekodBoyutu);
static void bulucuCezaGecmisEkle(int mevcutUzunluk, int gecmis[7], int karekodBoyutu);

test_edilebilir bool sinirliModulAl(const uint8_t karekod[], int x, int y);
test_edilebilir void sinirliModulAyarla(uint8_t karekod[], int x, int y, bool koyuMu);
test_edilebilir void sinirsizModulAyarla(uint8_t karekod[], int x, int y, bool koyuMu);
static bool bitAl(int x, int i);

test_edilebilir int bolumBitUzunluguHesapla(enum karekod_Mod mod, size_t karakterSayisi);
test_edilebilir int toplamBitleriAl(const struct karekod_Bolum bolumler[], size_t uzunluk, int surum);
static int karakterSayisiBitleriNumarasi(enum karekod_Mod mod, int surum);

static const char *ALFASAYISAL_KARAKTER_SETI = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

#define UZUNLUK_TASMA -1

test_edilebilir const int8_t BLOK_BASINA_HATA_DUZELTME_KOD_SOZCUKLERI[4][41] = {
	{-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
	{-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},
	{-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
	{-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
};

#define karekod_REED_SOLOMON_DERECE_MAKS 30

test_edilebilir const int8_t HATA_DUZELTME_BLOK_SAYISI[4][41] = {
	{-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},
	{-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},
	{-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},
	{-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},
};

static const int CEZA_N1 =  3;
static const int CEZA_N2 =  3;
static const int CEZA_N3 = 40;
static const int CEZA_N4 = 10;

bool karekod_metinKodla(const char *metin, uint8_t geciciTampon[], uint8_t karekod[],
		enum karekod_HataDuzeltme hataDuzeltme, int minSurum, int maksSurum, enum karekod_Maske maske, bool hataDuzeltmeyiArtir) {
	
	size_t metinUzunlugu = strlen(metin);
	if (metinUzunlugu == 0)
		return karekod_bolumleriKodlaGelismis(NULL, 0, hataDuzeltme, minSurum, maksSurum, maske, hataDuzeltmeyiArtir, geciciTampon, karekod);
	size_t tamponUzunlugu = (size_t)karekod_SURUM_ICIN_TAMPON_UZUNLUGU(maksSurum);
	
	struct karekod_Bolum bolum;
	if (karekod_sayisalMi(metin)) {
		if (karekod_bolumTamponBoyutuHesapla(karekod_Mod_SAYISAL, metinUzunlugu) > tamponUzunlugu)
			goto basarisiz;
		bolum = karekod_sayisalOlustur(metin, geciciTampon);
	} else if (karekod_alfasayisalMi(metin)) {
		if (karekod_bolumTamponBoyutuHesapla(karekod_Mod_ALFASAYISAL, metinUzunlugu) > tamponUzunlugu)
			goto basarisiz;
		bolum = karekod_alfasayisalOlustur(metin, geciciTampon);
	} else {
		if (metinUzunlugu > tamponUzunlugu)
			goto basarisiz;
		for (size_t i = 0; i < metinUzunlugu; i++)
			geciciTampon[i] = (uint8_t)metin[i];
		bolum.mod = karekod_Mod_BAYT;
		bolum.bitUzunlugu = bolumBitUzunluguHesapla(bolum.mod, metinUzunlugu);
		if (bolum.bitUzunlugu == UZUNLUK_TASMA)
			goto basarisiz;
		bolum.karakterSayisi = (int)metinUzunlugu;
		bolum.veri = geciciTampon;
	}
	return karekod_bolumleriKodlaGelismis(&bolum, 1, hataDuzeltme, minSurum, maksSurum, maske, hataDuzeltmeyiArtir, geciciTampon, karekod);
	
basarisiz:
	karekod[0] = 0;
	return false;
}

bool karekod_ikiliKodla(uint8_t veriVeGecici[], size_t veriUzunlugu, uint8_t karekod[],
		enum karekod_HataDuzeltme hataDuzeltme, int minSurum, int maksSurum, enum karekod_Maske maske, bool hataDuzeltmeyiArtir) {
	
	struct karekod_Bolum bolum;
	bolum.mod = karekod_Mod_BAYT;
	bolum.bitUzunlugu = bolumBitUzunluguHesapla(bolum.mod, veriUzunlugu);
	if (bolum.bitUzunlugu == UZUNLUK_TASMA) {
		karekod[0] = 0;
		return false;
	}
	bolum.karakterSayisi = (int)veriUzunlugu;
	bolum.veri = veriVeGecici;
	return karekod_bolumleriKodlaGelismis(&bolum, 1, hataDuzeltme, minSurum, maksSurum, maske, hataDuzeltmeyiArtir, veriVeGecici, karekod);
}

test_edilebilir void tamponaBitEkle(unsigned int deger, int bitSayisi, uint8_t tampon[], int *bitUzunlugu) {
	assert(0 <= bitSayisi && bitSayisi <= 16 && (unsigned long)deger >> bitSayisi == 0);
	for (int i = bitSayisi - 1; i >= 0; i--, (*bitUzunlugu)++)
		tampon[*bitUzunlugu >> 3] |= ((deger >> i) & 1) << (7 - (*bitUzunlugu & 7));
}

bool karekod_bolumleriKodla(const struct karekod_Bolum bolumler[], size_t uzunluk,
		enum karekod_HataDuzeltme hataDuzeltme, uint8_t geciciTampon[], uint8_t karekod[]) {
	return karekod_bolumleriKodlaGelismis(bolumler, uzunluk, hataDuzeltme,
		karekod_SURUM_MIN, karekod_SURUM_MAKS, karekod_Maske_OTOMATIK, true, geciciTampon, karekod);
}

bool karekod_bolumleriKodlaGelismis(const struct karekod_Bolum bolumler[], size_t uzunluk, enum karekod_HataDuzeltme hataDuzeltme,
		int minSurum, int maksSurum, enum karekod_Maske maske, bool hataDuzeltmeyiArtir, uint8_t geciciTampon[], uint8_t karekod[]) {
	assert(bolumler != NULL || uzunluk == 0);
	assert(karekod_SURUM_MIN <= minSurum && minSurum <= maksSurum && maksSurum <= karekod_SURUM_MAKS);
	assert(0 <= (int)hataDuzeltme && (int)hataDuzeltme <= 3 && -1 <= (int)maske && (int)maske <= 7);
	
	int surum, kullanilanVeriBitleri;
	for (surum = minSurum; ; surum++) {
		int veriKapasiteBitleri = veriKodSozcukSayisiAl(surum, hataDuzeltme) * 8;
		kullanilanVeriBitleri = toplamBitleriAl(bolumler, uzunluk, surum);
		if (kullanilanVeriBitleri != UZUNLUK_TASMA && kullanilanVeriBitleri <= veriKapasiteBitleri)
			break;
		if (surum >= maksSurum) {
			karekod[0] = 0;
			return false;
		}
	}
	assert(kullanilanVeriBitleri != UZUNLUK_TASMA);
	
	for (int i = (int)karekod_HataDuzeltme_ORTA; i <= (int)karekod_HataDuzeltme_YUKSEK; i++) {
		if (hataDuzeltmeyiArtir && kullanilanVeriBitleri <= veriKodSozcukSayisiAl(surum, (enum karekod_HataDuzeltme)i) * 8)
			hataDuzeltme = (enum karekod_HataDuzeltme)i;
	}
	
	memset(karekod, 0, (size_t)karekod_SURUM_ICIN_TAMPON_UZUNLUGU(surum) * sizeof(karekod[0]));
	int bitUzunlugu = 0;
	for (size_t i = 0; i < uzunluk; i++) {
		const struct karekod_Bolum *bolum = &bolumler[i];
		tamponaBitEkle((unsigned int)bolum->mod, 4, karekod, &bitUzunlugu);
		tamponaBitEkle((unsigned int)bolum->karakterSayisi, karakterSayisiBitleriNumarasi(bolum->mod, surum), karekod, &bitUzunlugu);
		for (int j = 0; j < bolum->bitUzunlugu; j++) {
			int bit = (bolum->veri[j >> 3] >> (7 - (j & 7))) & 1;
			tamponaBitEkle((unsigned int)bit, 1, karekod, &bitUzunlugu);
		}
	}
	assert(bitUzunlugu == kullanilanVeriBitleri);
	
	int veriKapasiteBitleri = veriKodSozcukSayisiAl(surum, hataDuzeltme) * 8;
	assert(bitUzunlugu <= veriKapasiteBitleri);
	int sonlandiriciBitler = veriKapasiteBitleri - bitUzunlugu;
	if (sonlandiriciBitler > 4)
		sonlandiriciBitler = 4;
	tamponaBitEkle(0, sonlandiriciBitler, karekod, &bitUzunlugu);
	tamponaBitEkle(0, (8 - bitUzunlugu % 8) % 8, karekod, &bitUzunlugu);
	assert(bitUzunlugu % 8 == 0);
	
	for (uint8_t dolguBayti = 0xEC; bitUzunlugu < veriKapasiteBitleri; dolguBayti ^= 0xEC ^ 0x11)
		tamponaBitEkle(dolguBayti, 8, karekod, &bitUzunlugu);
	
	hataDuzeltmeEkleVeBirlestir(karekod, surum, hataDuzeltme, geciciTampon);
	islevModulleriniBaslat(surum, karekod);
	kodSozcukleriniCiz(geciciTampon, hamVeriModulSayisiAl(surum) / 8, karekod);
	acikIslevModulleriniCiz(karekod, surum);
	islevModulleriniBaslat(surum, geciciTampon);
	
	if (maske == karekod_Maske_OTOMATIK) {
		long minCeza = LONG_MAX;
		for (int i = 0; i < 8; i++) {
			enum karekod_Maske testMaskesi = (enum karekod_Maske)i;
			maskeUygula(geciciTampon, karekod, testMaskesi);
			bicimBitleriniCiz(hataDuzeltme, testMaskesi, karekod);
			long ceza = cezaPuaniAl(karekod);
			if (ceza < minCeza) {
				maske = testMaskesi;
				minCeza = ceza;
			}
			maskeUygula(geciciTampon, karekod, testMaskesi);
		}
	}
	assert(0 <= (int)maske && (int)maske <= 7);
	maskeUygula(geciciTampon, karekod, maske);
	bicimBitleriniCiz(hataDuzeltme, maske, karekod);
	return true;
}

test_edilebilir void hataDuzeltmeEkleVeBirlestir(uint8_t veri[], int surum, enum karekod_HataDuzeltme hataDuzeltme, uint8_t sonuc[]) {
	assert(0 <= (int)hataDuzeltme && (int)hataDuzeltme < 4 && karekod_SURUM_MIN <= surum && surum <= karekod_SURUM_MAKS);
	int blokSayisi = HATA_DUZELTME_BLOK_SAYISI[(int)hataDuzeltme][surum];
	int blokHataDuzeltmeUzunlugu = BLOK_BASINA_HATA_DUZELTME_KOD_SOZCUKLERI[(int)hataDuzeltme][surum];
	int hamKodSozcukleri = hamVeriModulSayisiAl(surum) / 8;
	int veriUzunlugu = veriKodSozcukSayisiAl(surum, hataDuzeltme);
	int kisaBlokSayisi = blokSayisi - hamKodSozcukleri % blokSayisi;
	int kisaBlokVeriUzunlugu = hamKodSozcukleri / blokSayisi - blokHataDuzeltmeUzunlugu;
	
	uint8_t rsBolen[karekod_REED_SOLOMON_DERECE_MAKS];
	reedSolomonBolenHesapla(blokHataDuzeltmeUzunlugu, rsBolen);
	const uint8_t *dat = veri;
	for (int i = 0; i < blokSayisi; i++) {
		int datUzunlugu = kisaBlokVeriUzunlugu + (i < kisaBlokSayisi ? 0 : 1);
		uint8_t *hataDzt = &veri[veriUzunlugu];
		reedSolomonKalanHesapla(dat, datUzunlugu, rsBolen, blokHataDuzeltmeUzunlugu, hataDzt);
		for (int j = 0, k = i; j < datUzunlugu; j++, k += blokSayisi) {
			if (j == kisaBlokVeriUzunlugu)
				k -= kisaBlokSayisi;
			sonuc[k] = dat[j];
		}
		for (int j = 0, k = veriUzunlugu + i; j < blokHataDuzeltmeUzunlugu; j++, k += blokSayisi)
			sonuc[k] = hataDzt[j];
		dat += datUzunlugu;
	}
}

test_edilebilir int veriKodSozcukSayisiAl(int surum, enum karekod_HataDuzeltme hataDuzeltme) {
	int v = surum, e = (int)hataDuzeltme;
	assert(0 <= e && e < 4);
	return hamVeriModulSayisiAl(v) / 8
		- BLOK_BASINA_HATA_DUZELTME_KOD_SOZCUKLERI[e][v]
		* HATA_DUZELTME_BLOK_SAYISI[e][v];
}

test_edilebilir int hamVeriModulSayisiAl(int surum) {
	assert(karekod_SURUM_MIN <= surum && surum <= karekod_SURUM_MAKS);
	int sonuc = (16 * surum + 128) * surum + 64;
	if (surum >= 2) {
		int hizalamaSayisi = surum / 7 + 2;
		sonuc -= (25 * hizalamaSayisi - 10) * hizalamaSayisi - 55;
		if (surum >= 7)
			sonuc -= 36;
	}
	assert(208 <= sonuc && sonuc <= 29648);
	return sonuc;
}

test_edilebilir void reedSolomonBolenHesapla(int derece, uint8_t sonuc[]) {
	assert(1 <= derece && derece <= karekod_REED_SOLOMON_DERECE_MAKS);
	memset(sonuc, 0, (size_t)derece * sizeof(sonuc[0]));
	sonuc[derece - 1] = 1;
	
	uint8_t kok = 1;
	for (int i = 0; i < derece; i++) {
		for (int j = 0; j < derece; j++) {
			sonuc[j] = reedSolomonCarp(sonuc[j], kok);
			if (j + 1 < derece)
				sonuc[j] ^= sonuc[j + 1];
		}
		kok = reedSolomonCarp(kok, 0x02);
	}
}

test_edilebilir void reedSolomonKalanHesapla(const uint8_t veri[], int veriUzunlugu,
		const uint8_t uretec[], int derece, uint8_t sonuc[]) {
	assert(1 <= derece && derece <= karekod_REED_SOLOMON_DERECE_MAKS);
	memset(sonuc, 0, (size_t)derece * sizeof(sonuc[0]));
	for (int i = 0; i < veriUzunlugu; i++) {
		uint8_t carpan = veri[i] ^ sonuc[0];
		memmove(&sonuc[0], &sonuc[1], (size_t)(derece - 1) * sizeof(sonuc[0]));
		sonuc[derece - 1] = 0;
		for (int j = 0; j < derece; j++)
			sonuc[j] ^= reedSolomonCarp(uretec[j], carpan);
	}
}

#undef karekod_REED_SOLOMON_DERECE_MAKS

test_edilebilir uint8_t reedSolomonCarp(uint8_t x, uint8_t y) {
	uint8_t z = 0;
	for (int i = 7; i >= 0; i--) {
		z = (uint8_t)((z << 1) ^ ((z >> 7) * 0x11D));
		z ^= ((y >> i) & 1) * x;
	}
	return z;
}

test_edilebilir void islevModulleriniBaslat(int surum, uint8_t karekod[]) {
	int boyut = surum * 4 + 17;
	memset(karekod, 0, (size_t)((boyut * boyut + 7) / 8 + 1) * sizeof(karekod[0]));
	karekod[0] = (uint8_t)boyut;
	
	dikdortgenDoldur(6, 0, 1, boyut, karekod);
	dikdortgenDoldur(0, 6, boyut, 1, karekod);
	
	dikdortgenDoldur(0, 0, 9, 9, karekod);
	dikdortgenDoldur(boyut - 8, 0, 8, 9, karekod);
	dikdortgenDoldur(0, boyut - 8, 9, 8, karekod);
	
	uint8_t hizalamaKonumlari[7];
	int hizalamaSayisi = hizalamaDeseniKonumlariniAl(surum, hizalamaKonumlari);
	for (int i = 0; i < hizalamaSayisi; i++) {
		for (int j = 0; j < hizalamaSayisi; j++) {
			if (!((i == 0 && j == 0) || (i == 0 && j == hizalamaSayisi - 1) || (i == hizalamaSayisi - 1 && j == 0)))
				dikdortgenDoldur(hizalamaKonumlari[i] - 2, hizalamaKonumlari[j] - 2, 5, 5, karekod);
		}
	}
	
	if (surum >= 7) {
		dikdortgenDoldur(boyut - 11, 0, 3, 6, karekod);
		dikdortgenDoldur(0, boyut - 11, 6, 3, karekod);
	}
}

static void acikIslevModulleriniCiz(uint8_t karekod[], int surum) {
	int boyut = karekod_boyutAl(karekod);
	for (int i = 7; i < boyut - 7; i += 2) {
		sinirliModulAyarla(karekod, 6, i, false);
		sinirliModulAyarla(karekod, i, 6, false);
	}
	
	for (int dy = -4; dy <= 4; dy++) {
		for (int dx = -4; dx <= 4; dx++) {
			int mesafe = abs(dx);
			if (abs(dy) > mesafe)
				mesafe = abs(dy);
			if (mesafe == 2 || mesafe == 4) {
				sinirsizModulAyarla(karekod, 3 + dx, 3 + dy, false);
				sinirsizModulAyarla(karekod, boyut - 4 + dx, 3 + dy, false);
				sinirsizModulAyarla(karekod, 3 + dx, boyut - 4 + dy, false);
			}
		}
	}
	
	uint8_t hizalamaKonumlari[7];
	int hizalamaSayisi = hizalamaDeseniKonumlariniAl(surum, hizalamaKonumlari);
	for (int i = 0; i < hizalamaSayisi; i++) {
		for (int j = 0; j < hizalamaSayisi; j++) {
			if ((i == 0 && j == 0) || (i == 0 && j == hizalamaSayisi - 1) || (i == hizalamaSayisi - 1 && j == 0))
				continue;
			for (int dy = -1; dy <= 1; dy++) {
				for (int dx = -1; dx <= 1; dx++)
					sinirliModulAyarla(karekod, hizalamaKonumlari[i] + dx, hizalamaKonumlari[j] + dy, dx == 0 && dy == 0);
			}
		}
	}
	
	if (surum >= 7) {
		int kalan = surum;
		for (int i = 0; i < 12; i++)
			kalan = (kalan << 1) ^ ((kalan >> 11) * 0x1F25);
		long bitler = (long)surum << 12 | kalan;
		assert(bitler >> 18 == 0);
		
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 3; j++) {
				int k = boyut - 11 + j;
				sinirliModulAyarla(karekod, k, i, (bitler & 1) != 0);
				sinirliModulAyarla(karekod, i, k, (bitler & 1) != 0);
				bitler >>= 1;
			}
		}
	}
}

static void bicimBitleriniCiz(enum karekod_HataDuzeltme hataDuzeltme, enum karekod_Maske maske, uint8_t karekod[]) {
	assert(0 <= (int)maske && (int)maske <= 7);
	static const int tablo[] = {1, 0, 3, 2};
	int veri = tablo[(int)hataDuzeltme] << 3 | (int)maske;
	int kalan = veri;
	for (int i = 0; i < 10; i++)
		kalan = (kalan << 1) ^ ((kalan >> 9) * 0x537);
	int bitler = (veri << 10 | kalan) ^ 0x5412;
	assert(bitler >> 15 == 0);
	
	for (int i = 0; i <= 5; i++)
		sinirliModulAyarla(karekod, 8, i, bitAl(bitler, i));
	sinirliModulAyarla(karekod, 8, 7, bitAl(bitler, 6));
	sinirliModulAyarla(karekod, 8, 8, bitAl(bitler, 7));
	sinirliModulAyarla(karekod, 7, 8, bitAl(bitler, 8));
	for (int i = 9; i < 15; i++)
		sinirliModulAyarla(karekod, 14 - i, 8, bitAl(bitler, i));
	
	int boyut = karekod_boyutAl(karekod);
	for (int i = 0; i < 8; i++)
		sinirliModulAyarla(karekod, boyut - 1 - i, 8, bitAl(bitler, i));
	for (int i = 8; i < 15; i++)
		sinirliModulAyarla(karekod, 8, boyut - 15 + i, bitAl(bitler, i));
	sinirliModulAyarla(karekod, 8, boyut - 8, true);
}

test_edilebilir int hizalamaDeseniKonumlariniAl(int surum, uint8_t sonuc[7]) {
	if (surum == 1)
		return 0;
	int hizalamaSayisi = surum / 7 + 2;
	int adim = (surum * 8 + hizalamaSayisi * 3 + 5) / (hizalamaSayisi * 4 - 4) * 2;
	for (int i = hizalamaSayisi - 1, konum = surum * 4 + 10; i >= 1; i--, konum -= adim)
		sonuc[i] = (uint8_t)konum;
	sonuc[0] = 6;
	return hizalamaSayisi;
}

static void dikdortgenDoldur(int sol, int ust, int genislik, int yukseklik, uint8_t karekod[]) {
	for (int dy = 0; dy < yukseklik; dy++) {
		for (int dx = 0; dx < genislik; dx++)
			sinirliModulAyarla(karekod, sol + dx, ust + dy, true);
	}
}

static void kodSozcukleriniCiz(const uint8_t veri[], int veriUzunlugu, uint8_t karekod[]) {
	int boyut = karekod_boyutAl(karekod);
	int i = 0;
	for (int sag = boyut - 1; sag >= 1; sag -= 2) {
		if (sag == 6)
			sag = 5;
		for (int dikey = 0; dikey < boyut; dikey++) {
			for (int j = 0; j < 2; j++) {
				int x = sag - j;
				bool yukariDogru = ((sag + 1) & 2) == 0;
				int y = yukariDogru ? boyut - 1 - dikey : dikey;
				if (!sinirliModulAl(karekod, x, y) && i < veriUzunlugu * 8) {
					bool koyu = bitAl(veri[i >> 3], 7 - (i & 7));
					sinirliModulAyarla(karekod, x, y, koyu);
					i++;
				}
			}
		}
	}
	assert(i == veriUzunlugu * 8);
}

static void maskeUygula(const uint8_t islevModulleri[], uint8_t karekod[], enum karekod_Maske maske) {
	assert(0 <= (int)maske && (int)maske <= 7);
	int boyut = karekod_boyutAl(karekod);
	for (int y = 0; y < boyut; y++) {
		for (int x = 0; x < boyut; x++) {
			if (sinirliModulAl(islevModulleri, x, y))
				continue;
			bool tersCevir;
			switch ((int)maske) {
				case 0:  tersCevir = (x + y) % 2 == 0;                    break;
				case 1:  tersCevir = y % 2 == 0;                          break;
				case 2:  tersCevir = x % 3 == 0;                          break;
				case 3:  tersCevir = (x + y) % 3 == 0;                    break;
				case 4:  tersCevir = (x / 3 + y / 2) % 2 == 0;            break;
				case 5:  tersCevir = x * y % 2 + x * y % 3 == 0;          break;
				case 6:  tersCevir = (x * y % 2 + x * y % 3) % 2 == 0;    break;
				case 7:  tersCevir = ((x + y) % 2 + x * y % 3) % 2 == 0;  break;
				default:  assert(false);  return;
			}
			bool deger = sinirliModulAl(karekod, x, y);
			sinirliModulAyarla(karekod, x, y, deger ^ tersCevir);
		}
	}
}

static long cezaPuaniAl(const uint8_t karekod[]) {
	int boyut = karekod_boyutAl(karekod);
	long sonuc = 0;
	
	for (int y = 0; y < boyut; y++) {
		bool calismaRengi = false;
		int calismaX = 0;
		int calismaGecmisi[7] = {0};
		for (int x = 0; x < boyut; x++) {
			if (sinirliModulAl(karekod, x, y) == calismaRengi) {
				calismaX++;
				if (calismaX == 5)
					sonuc += CEZA_N1;
				else if (calismaX > 5)
					sonuc++;
			} else {
				bulucuCezaGecmisEkle(calismaX, calismaGecmisi, boyut);
				if (!calismaRengi)
					sonuc += bulucuCezaDesenleriSay(calismaGecmisi, boyut) * CEZA_N3;
				calismaRengi = sinirliModulAl(karekod, x, y);
				calismaX = 1;
			}
		}
		sonuc += bulucuCezaSonlandirVeSay(calismaRengi, calismaX, calismaGecmisi, boyut) * CEZA_N3;
	}
	for (int x = 0; x < boyut; x++) {
		bool calismaRengi = false;
		int calismaY = 0;
		int calismaGecmisi[7] = {0};
		for (int y = 0; y < boyut; y++) {
			if (sinirliModulAl(karekod, x, y) == calismaRengi) {
				calismaY++;
				if (calismaY == 5)
					sonuc += CEZA_N1;
				else if (calismaY > 5)
					sonuc++;
			} else {
				bulucuCezaGecmisEkle(calismaY, calismaGecmisi, boyut);
				if (!calismaRengi)
					sonuc += bulucuCezaDesenleriSay(calismaGecmisi, boyut) * CEZA_N3;
				calismaRengi = sinirliModulAl(karekod, x, y);
				calismaY = 1;
			}
		}
		sonuc += bulucuCezaSonlandirVeSay(calismaRengi, calismaY, calismaGecmisi, boyut) * CEZA_N3;
	}
	
	for (int y = 0; y < boyut - 1; y++) {
		for (int x = 0; x < boyut - 1; x++) {
			bool renk = sinirliModulAl(karekod, x, y);
			if (renk == sinirliModulAl(karekod, x + 1, y) &&
			    renk == sinirliModulAl(karekod, x, y + 1) &&
			    renk == sinirliModulAl(karekod, x + 1, y + 1))
				sonuc += CEZA_N2;
		}
	}
	
	int koyu = 0;
	for (int y = 0; y < boyut; y++) {
		for (int x = 0; x < boyut; x++) {
			if (sinirliModulAl(karekod, x, y))
				koyu++;
		}
	}
	int toplam = boyut * boyut;
	int k = (int)((labs(koyu * 20L - toplam * 10L) + toplam - 1) / toplam) - 1;
	assert(0 <= k && k <= 9);
	sonuc += k * CEZA_N4;
	assert(0 <= sonuc && sonuc <= 2568888L);
	return sonuc;
}

static int bulucuCezaDesenleriSay(const int gecmis[7], int boyut) {
	int n = gecmis[1];
	assert(n <= boyut * 3);
	(void)boyut;
	bool merkez = n > 0 && gecmis[2] == n && gecmis[3] == n * 3 && gecmis[4] == n && gecmis[5] == n;
	return (merkez && gecmis[0] >= n * 4 && gecmis[6] >= n ? 1 : 0)
	     + (merkez && gecmis[6] >= n * 4 && gecmis[0] >= n ? 1 : 0);
}

static int bulucuCezaSonlandirVeSay(bool mevcutRenk, int mevcutUzunluk, int gecmis[7], int boyut) {
	if (mevcutRenk) {
		bulucuCezaGecmisEkle(mevcutUzunluk, gecmis, boyut);
		mevcutUzunluk = 0;
	}
	mevcutUzunluk += boyut;
	bulucuCezaGecmisEkle(mevcutUzunluk, gecmis, boyut);
	return bulucuCezaDesenleriSay(gecmis, boyut);
}

static void bulucuCezaGecmisEkle(int mevcutUzunluk, int gecmis[7], int boyut) {
	if (gecmis[0] == 0)
		mevcutUzunluk += boyut;
	memmove(&gecmis[1], &gecmis[0], 6 * sizeof(gecmis[0]));
	gecmis[0] = mevcutUzunluk;
}

int karekod_boyutAl(const uint8_t karekod[]) {
	assert(karekod != NULL);
	int sonuc = karekod[0];
	assert((karekod_SURUM_MIN * 4 + 17) <= sonuc
		&& sonuc <= (karekod_SURUM_MAKS * 4 + 17));
	return sonuc;
}

bool karekod_modulAl(const uint8_t karekod[], int x, int y) {
	assert(karekod != NULL);
	int boyut = karekod[0];
	return (0 <= x && x < boyut && 0 <= y && y < boyut) && sinirliModulAl(karekod, x, y);
}

test_edilebilir bool sinirliModulAl(const uint8_t karekod[], int x, int y) {
	int boyut = karekod[0];
	assert(21 <= boyut && boyut <= 177 && 0 <= x && x < boyut && 0 <= y && y < boyut);
	int indeks = y * boyut + x;
	return bitAl(karekod[(indeks >> 3) + 1], indeks & 7);
}

test_edilebilir void sinirliModulAyarla(uint8_t karekod[], int x, int y, bool koyuMu) {
	int boyut = karekod[0];
	assert(21 <= boyut && boyut <= 177 && 0 <= x && x < boyut && 0 <= y && y < boyut);
	int indeks = y * boyut + x;
	int bitIndeks = indeks & 7;
	int baytIndeks = (indeks >> 3) + 1;
	if (koyuMu)
		karekod[baytIndeks] |= 1 << bitIndeks;
	else
		karekod[baytIndeks] &= (1 << bitIndeks) ^ 0xFF;
}

test_edilebilir void sinirsizModulAyarla(uint8_t karekod[], int x, int y, bool koyuMu) {
	int boyut = karekod[0];
	if (0 <= x && x < boyut && 0 <= y && y < boyut)
		sinirliModulAyarla(karekod, x, y, koyuMu);
}

static bool bitAl(int x, int i) {
	return ((x >> i) & 1) != 0;
}

bool karekod_sayisalMi(const char *metin) {
	assert(metin != NULL);
	for (; *metin != '\0'; metin++) {
		if (*metin < '0' || *metin > '9')
			return false;
	}
	return true;
}

bool karekod_alfasayisalMi(const char *metin) {
	assert(metin != NULL);
	for (; *metin != '\0'; metin++) {
		if (strchr(ALFASAYISAL_KARAKTER_SETI, *metin) == NULL)
			return false;
	}
	return true;
}

size_t karekod_bolumTamponBoyutuHesapla(enum karekod_Mod mod, size_t karakterSayisi) {
	int temp = bolumBitUzunluguHesapla(mod, karakterSayisi);
	if (temp == UZUNLUK_TASMA)
		return SIZE_MAX;
	assert(0 <= temp && temp <= INT16_MAX);
	return ((size_t)temp + 7) / 8;
}

test_edilebilir int bolumBitUzunluguHesapla(enum karekod_Mod mod, size_t karakterSayisi) {
	if (karakterSayisi > (unsigned int)INT16_MAX)
		return UZUNLUK_TASMA;
	long sonuc = (long)karakterSayisi;
	if (mod == karekod_Mod_SAYISAL)
		sonuc = (sonuc * 10 + 2) / 3;
	else if (mod == karekod_Mod_ALFASAYISAL)
		sonuc = (sonuc * 11 + 1) / 2;
	else if (mod == karekod_Mod_BAYT)
		sonuc *= 8;
	else if (mod == karekod_Mod_KANJI)
		sonuc *= 13;
	else if (mod == karekod_Mod_ECI && karakterSayisi == 0)
		sonuc = 3 * 8;
	else {
		assert(false);
		return UZUNLUK_TASMA;
	}
	assert(sonuc >= 0);
	if (sonuc > INT16_MAX)
		return UZUNLUK_TASMA;
	return (int)sonuc;
}

struct karekod_Bolum karekod_baytOlustur(const uint8_t veri[], size_t uzunluk, uint8_t tampon[]) {
	assert(veri != NULL || uzunluk == 0);
	struct karekod_Bolum sonuc;
	sonuc.mod = karekod_Mod_BAYT;
	sonuc.bitUzunlugu = bolumBitUzunluguHesapla(sonuc.mod, uzunluk);
	assert(sonuc.bitUzunlugu != UZUNLUK_TASMA);
	sonuc.karakterSayisi = (int)uzunluk;
	if (uzunluk > 0)
		memcpy(tampon, veri, uzunluk * sizeof(tampon[0]));
	sonuc.veri = tampon;
	return sonuc;
}

struct karekod_Bolum karekod_sayisalOlustur(const char *rakamlar, uint8_t tampon[]) {
	assert(rakamlar != NULL);
	struct karekod_Bolum sonuc;
	size_t uzunluk = strlen(rakamlar);
	sonuc.mod = karekod_Mod_SAYISAL;
	int bitUzunlugu = bolumBitUzunluguHesapla(sonuc.mod, uzunluk);
	assert(bitUzunlugu != UZUNLUK_TASMA);
	sonuc.karakterSayisi = (int)uzunluk;
	if (bitUzunlugu > 0)
		memset(tampon, 0, ((size_t)bitUzunlugu + 7) / 8 * sizeof(tampon[0]));
	sonuc.bitUzunlugu = 0;
	
	unsigned int birikenVeri = 0;
	int birikenSayi = 0;
	for (; *rakamlar != '\0'; rakamlar++) {
		char c = *rakamlar;
		assert('0' <= c && c <= '9');
		birikenVeri = birikenVeri * 10 + (unsigned int)(c - '0');
		birikenSayi++;
		if (birikenSayi == 3) {
			tamponaBitEkle(birikenVeri, 10, tampon, &sonuc.bitUzunlugu);
			birikenVeri = 0;
			birikenSayi = 0;
		}
	}
	if (birikenSayi > 0)
		tamponaBitEkle(birikenVeri, birikenSayi * 3 + 1, tampon, &sonuc.bitUzunlugu);
	assert(sonuc.bitUzunlugu == bitUzunlugu);
	sonuc.veri = tampon;
	return sonuc;
}

struct karekod_Bolum karekod_alfasayisalOlustur(const char *metin, uint8_t tampon[]) {
	assert(metin != NULL);
	struct karekod_Bolum sonuc;
	size_t uzunluk = strlen(metin);
	sonuc.mod = karekod_Mod_ALFASAYISAL;
	int bitUzunlugu = bolumBitUzunluguHesapla(sonuc.mod, uzunluk);
	assert(bitUzunlugu != UZUNLUK_TASMA);
	sonuc.karakterSayisi = (int)uzunluk;
	if (bitUzunlugu > 0)
		memset(tampon, 0, ((size_t)bitUzunlugu + 7) / 8 * sizeof(tampon[0]));
	sonuc.bitUzunlugu = 0;
	
	unsigned int birikenVeri = 0;
	int birikenSayi = 0;
	for (; *metin != '\0'; metin++) {
		const char *gecici = strchr(ALFASAYISAL_KARAKTER_SETI, *metin);
		assert(gecici != NULL);
		birikenVeri = birikenVeri * 45 + (unsigned int)(gecici - ALFASAYISAL_KARAKTER_SETI);
		birikenSayi++;
		if (birikenSayi == 2) {
			tamponaBitEkle(birikenVeri, 11, tampon, &sonuc.bitUzunlugu);
			birikenVeri = 0;
			birikenSayi = 0;
		}
	}
	if (birikenSayi > 0)
		tamponaBitEkle(birikenVeri, 6, tampon, &sonuc.bitUzunlugu);
	assert(sonuc.bitUzunlugu == bitUzunlugu);
	sonuc.veri = tampon;
	return sonuc;
}

struct karekod_Bolum karekod_eciOlustur(long atamaDegeri, uint8_t tampon[]) {
	struct karekod_Bolum sonuc;
	sonuc.mod = karekod_Mod_ECI;
	sonuc.karakterSayisi = 0;
	sonuc.bitUzunlugu = 0;
	if (atamaDegeri < 0)
		assert(false);
	else if (atamaDegeri < (1 << 7)) {
		memset(tampon, 0, 1 * sizeof(tampon[0]));
		tamponaBitEkle((unsigned int)atamaDegeri, 8, tampon, &sonuc.bitUzunlugu);
	} else if (atamaDegeri < (1 << 14)) {
		memset(tampon, 0, 2 * sizeof(tampon[0]));
		tamponaBitEkle(2, 2, tampon, &sonuc.bitUzunlugu);
		tamponaBitEkle((unsigned int)atamaDegeri, 14, tampon, &sonuc.bitUzunlugu);
	} else if (atamaDegeri < 1000000L) {
		memset(tampon, 0, 3 * sizeof(tampon[0]));
		tamponaBitEkle(6, 3, tampon, &sonuc.bitUzunlugu);
		tamponaBitEkle((unsigned int)(atamaDegeri >> 10), 11, tampon, &sonuc.bitUzunlugu);
		tamponaBitEkle((unsigned int)(atamaDegeri & 0x3FF), 10, tampon, &sonuc.bitUzunlugu);
	} else
		assert(false);
	sonuc.veri = tampon;
	return sonuc;
}

test_edilebilir int toplamBitleriAl(const struct karekod_Bolum bolumler[], size_t uzunluk, int surum) {
	assert(bolumler != NULL || uzunluk == 0);
	long sonuc = 0;
	for (size_t i = 0; i < uzunluk; i++) {
		int karakterSayisi  = bolumler[i].karakterSayisi;
		int bitUzunlugu = bolumler[i].bitUzunlugu;
		assert(0 <= karakterSayisi  && karakterSayisi  <= INT16_MAX);
		assert(0 <= bitUzunlugu && bitUzunlugu <= INT16_MAX);
		int ccbitler = karakterSayisiBitleriNumarasi(bolumler[i].mod, surum);
		assert(0 <= ccbitler && ccbitler <= 16);
		if (karakterSayisi >= (1L << ccbitler))
			return UZUNLUK_TASMA;
		sonuc += 4L + ccbitler + bitUzunlugu;
		if (sonuc > INT16_MAX)
			return UZUNLUK_TASMA;
	}
	assert(0 <= sonuc && sonuc <= INT16_MAX);
	return (int)sonuc;
}

static int karakterSayisiBitleriNumarasi(enum karekod_Mod mod, int surum) {
	assert(karekod_SURUM_MIN <= surum && surum <= karekod_SURUM_MAKS);
	int i = (surum + 7) / 17;
	switch (mod) {
		case karekod_Mod_SAYISAL     : { static const int gecici[] = {10, 12, 14}; return gecici[i]; }
		case karekod_Mod_ALFASAYISAL : { static const int gecici[] = { 9, 11, 13}; return gecici[i]; }
		case karekod_Mod_BAYT        : { static const int gecici[] = { 8, 16, 16}; return gecici[i]; }
		case karekod_Mod_KANJI       : { static const int gecici[] = { 8, 10, 12}; return gecici[i]; }
		case karekod_Mod_ECI         : return 0;
		default:  assert(false);  return -1;
	}
}

#undef UZUNLUK_TASMA