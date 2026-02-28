#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "karekod.h" 

#define PIKSEL_BOYUTU 10 
#define CERCEVE 4        

void dosyaIsmiUret(char *tampon) {
    int sayac = 0;
    FILE *dosya;
    
    while (1) {
        if (sayac == 0) {
            sprintf(tampon, "karekod.bmp");
        } else {
            sprintf(tampon, "karekod%d.bmp", sayac);
        }
        
        dosya = fopen(tampon, "r");
        if (dosya) {
            fclose(dosya);
            sayac++;
        } else {
            break;
        }
    }
}

void bmpOlarakKaydet(const uint8_t karekodDizisi[], const char *dosyaAdi) {
    int boyut = karekod_boyutAl(karekodDizisi);
    int goruntuBoyutu = boyut + (2 * CERCEVE);
    int gercekBoyut = goruntuBoyutu * PIKSEL_BOYUTU;

    int satirDolgusu = (gercekBoyut * 3 + 3) & (~3);
    int dosyaBoyutu = 54 + gercekBoyut * satirDolgusu;
    
    uint8_t bmpDosyaBasligi[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    uint8_t bmpBilgiBasligi[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};

    bmpDosyaBasligi[2] = (uint8_t)(dosyaBoyutu);
    bmpDosyaBasligi[3] = (uint8_t)(dosyaBoyutu >> 8);
    bmpDosyaBasligi[4] = (uint8_t)(dosyaBoyutu >> 16);
    bmpDosyaBasligi[5] = (uint8_t)(dosyaBoyutu >> 24);

    bmpBilgiBasligi[4] = (uint8_t)(gercekBoyut);
    bmpBilgiBasligi[5] = (uint8_t)(gercekBoyut >> 8);
    bmpBilgiBasligi[6] = (uint8_t)(gercekBoyut >> 16);
    bmpBilgiBasligi[7] = (uint8_t)(gercekBoyut >> 24);
    
    int32_t yukseklik = -gercekBoyut;
    bmpBilgiBasligi[8]  = (uint8_t)((uint32_t)yukseklik);
    bmpBilgiBasligi[9]  = (uint8_t)((uint32_t)yukseklik >> 8);
    bmpBilgiBasligi[10] = (uint8_t)((uint32_t)yukseklik >> 16);
    bmpBilgiBasligi[11] = (uint8_t)((uint32_t)yukseklik >> 24);

    FILE *f = fopen(dosyaAdi, "wb");
    if (!f) {
        printf("Hata: Dosya olusturulamadi!\n");
        return;
    }
    
    fwrite(bmpDosyaBasligi, 1, 14, f);
    fwrite(bmpBilgiBasligi, 1, 40, f);

    for (int y = 0; y < gercekBoyut; y++) {
        for (int x = 0; x < gercekBoyut; x++) {
            int qr_x = (x / PIKSEL_BOYUTU) - CERCEVE;
            int qr_y = (y / PIKSEL_BOYUTU) - CERCEVE;
            
            uint8_t renk[3]; 
            
            if (qr_x >= 0 && qr_x < boyut && qr_y >= 0 && qr_y < boyut) {
                bool koyuMu = karekod_modulAl(karekodDizisi, qr_x, qr_y);
                if (koyuMu) {
                    renk[0] = 0; renk[1] = 0; renk[2] = 0;      
                } else {
                    renk[0] = 255; renk[1] = 255; renk[2] = 255; 
                }
            } else {
                renk[0] = 255; renk[1] = 255; renk[2] = 255;    
            }
            fwrite(renk, 1, 3, f);
        }
        uint8_t dolgu[3] = {0,0,0};
        fwrite(dolgu, 1, (4 - (gercekBoyut * 3) % 4) % 4, f);
    }
    fclose(f);
}

char *dinamikMetinAl() {
    size_t boyut = 256;
    size_t uzunluk = 0;
    char *tampon = (char *)malloc(boyut);
    
    if (!tampon) return NULL;

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        tampon[uzunluk++] = (char)c;
        if (uzunluk == boyut) {
            boyut *= 2;
            char *yeniTampon = (char *)realloc(tampon, boyut);
            if (!yeniTampon) {
                free(tampon);
                return NULL;
            }
            tampon = yeniTampon;
        }
    }
    tampon[uzunluk] = '\0';
    
    return tampon;
}

int main() {
    printf("========================================\n");
    printf(" Karekod Uretici / QR Code Generator\n");
    printf("========================================\n");
    printf("Lutfen karekoda cevirmek istediginiz metni veya linki girin\nPlease enter the text or link you want to convert to a QR code: \n> ");

    char *girisMetni = dinamikMetinAl();

    if (!girisMetni || strlen(girisMetni) == 0) {
        printf("\nHata: Gecerli bir metin girmediniz.\nError: You did not enter valid text.\n");
        if (girisMetni) free(girisMetni);
        return 1;
    }

    uint8_t *karekodDizisi = (uint8_t *)malloc(karekod_TAMPON_UZUNLUGU_MAKS);
    uint8_t *geciciTampon = (uint8_t *)malloc(karekod_TAMPON_UZUNLUGU_MAKS);

    if (!karekodDizisi || !geciciTampon) {
        printf("\nHata: Bellek tahsis edilemedi!\nError: Memory allocation failed!\n");
        free(girisMetni);
        if (karekodDizisi) free(karekodDizisi);
        if (geciciTampon) free(geciciTampon);
        return 1;
    }

    bool basarili = karekod_metinKodla(girisMetni, geciciTampon, karekodDizisi, 
                                       karekod_HataDuzeltme_DUSUK, 
                                       karekod_SURUM_MIN, 
                                       karekod_SURUM_MAKS, 
                                       karekod_Maske_OTOMATIK, true);

    if (basarili) {
        char dosyaAdi[50];
        dosyaIsmiUret(dosyaAdi); 
        bmpOlarakKaydet(karekodDizisi, dosyaAdi);
        
        printf("\nBasarili! Telefonunuzun hemen okuyabilecegi karekod uretildi.\nSuccess! A QR code that your phone can immediately read has been generated.\n");
        printf("Dosya-File: %s\n", dosyaAdi);
    } else {
        printf("\nHata: Karekod olusturulamadi. Metin cok uzun veya desteklenmiyor olabilir.\nError: QR code could not be generated. The text may be too long or unsupported.\n");
    }

    free(girisMetni);
    free(karekodDizisi);
    free(geciciTampon);

    return 0;
}
