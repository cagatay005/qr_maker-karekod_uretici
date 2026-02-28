# 🇹🇷 Karekod (QR Code) Üretici - C ile Sıfırdan Özel Kütüphane

Bu proje, hiçbir dış kütüphane (üçüncü parti bağımlılık) kullanılmadan, baştan sona **C programlama dili** ile yazılmış bir Karekod (QR Code) üreticisidir. Karekod standardının (ISO/IEC 18004) gerektirdiği tüm karmaşık matematiksel işlemler, hata düzeltme algoritmaları ve piksel tabanlı görüntü oluşturma süreçleri sıfırdan geliştirilmiştir.

## Özellikler

* **Bağımlılık Yok:** Yalnızca standart C kütüphaneleri (`stdio.h`, `stdlib.h`, `string.h` vb.) kullanılmıştır.
* **Türkçe API:** Kütüphane fonksiyonları ve değişkenleri tamamen Türkçe terminoloji ile yazılmıştır.
* **Otomatik Veri Analizi:** Girilen metnin tipine göre (Sayısal, Alfasayısal veya Bayt) en uygun sıkıştırma modunu otomatik seçer.
* **Reed-Solomon Hata Düzeltme:** Karekodun fiziksel olarak hasar görmesi veya kirlenmesi durumunda bile okunabilmesini sağlayan hata düzeltme kodlarını hesaplar.
* **Dinamik Maskeleme:** Karekod okuyucuların kafasını karıştırabilecek desenleri (örneğin devasa siyah bloklar) engellemek için 8 farklı maske tipini dener, ceza puanlarını hesaplar ve en ideal maskeyi uygular.
* **Doğrudan `.bmp` Çıktısı:** Oluşturulan 1-0 matrisini, manuel BMP başlık (header) hesaplamaları ile `.bmp` formatında bir resim dosyasına dönüştürür.

## Nasıl Çalışır? (Adım Adım Karekod Üretimi)

Bu program bir metni alıp resme dönüştürürken şu adımları izler:

1. **Veri Kodlama (Data Encoding):**
   Kullanıcının girdiği metin karakterleri analiz edilir. Sadece rakamsa **Sayısal Mod**, harf ve rakam karışıkta **Alfasayısal Mod**, özel karakterler içeriyorsa **Bayt Modu** kullanılarak metin, bilgisayarın anlayacağı uzun bir 1 ve 0 (bit) dizisine dönüştürülür.
2. **Hata Düzeltme Bloklarının Eklenmesi:**
   Üretilen bit dizisinin sonuna, Galois Cismi (Galois Field) matematiği kullanılarak `Reed-Solomon` hata düzeltme kodları eklenir. Bu sayede karekodun bir kısmı yırtılsa bile telefon kamerası eksik veriyi tamamlayabilir.
3. **İşlev Modüllerinin Çizimi (Function Patterns):**
   Karekodun köşelerindeki 3 büyük kare (Bulucu Desenler), aralardaki noktalı çizgiler (Zamanlama Desenleri) ve küçük hizalama kareleri matrise yerleştirilir.
4. **Verinin Matrise Dizilmesi:**
   Oluşturulan veri ve hata düzeltme bitleri, karekodun sağ alt köşesinden başlanarak zikzaklar halinde boş modüllere (piksellere) yerleştirilir.
5. **Maskeleme (Masking):**
   Uygulanan veri, barkod okuyucuların hizalamasını bozabilecek optik yanılsamalar yaratabilir. Sistem 8 farklı XOR maskesini matris üzerinde dener, `cezaPuaniAl` fonksiyonu ile en düşük cezayı alan (en dengeli siyah-beyaz dağılımına sahip) maskeyi kalıcı olarak uygular.
6. **BMP Dosyasının Oluşturulması:**
   Oluşturulan soyut matris, `main.c` içerisindeki `bmpOlarakKaydet` fonksiyonu ile fiziksel bir resim dosyasına dönüştürülür. Her bir karekod modülü 10x10 piksellik bloklar halinde çizilir ve etrafına 4 birimlik "Sessiz Bölge" (Çerçeve) eklenir.

## Dosya Yapısı

| Dosya | Açıklama |
| :--- | :--- |
| `main.c` | Kullanıcı ile etkileşime giren, girdiyi alan ve sonuç matrisini BMP formatında bilgisayara kaydeden ana program. |
| `karekod.c` | Karekod standardının tüm çekirdek algoritmalarını (kodlama, hata düzeltme, maskeleme, puanlama) içeren asıl kütüphane dosyası. |
| `karekod.h` | Kütüphanenin dışarıya açılan fonksiyonlarını, modlarını ve veri yapılarını (Struct, Enum) tanımlayan başlık dosyası. |

## Kurulum ve Kullanım

Projeyi derlemek ve çalıştırmak için sisteminizde bir C derleyicisi (GCC vb.) kurulu olmalıdır.

**Derleme:**
```bash
gcc main.c karekod.c -o karekod_uretici
```
**Çalıştırma**
```bash
./karekod_uretici
```
# ENG QR Code Generator - Custom C Library from Scratch
This project is a complete QR Code (Karekod) generator written entirely in the **C programming language** from scratch, without the use of any external libraries or third-party dependencies. All complex mathematical operations, error correction algorithms, and pixel-based image generation processes required by the QR code standard (ISO/IEC 18004) were developed manually.

## Features
• **Zero Dependencies:** Utilizes only standard C libraries (`stdio.h`, `stdlib.h`, `string.h`, etc.).
• **Custom API:** Library functions and variables are written with Turkish terminology, showcasing a localized implementation approach.
• **Automatic Data Analysis:** Automatically selects the most efficient compression mode (Numeric, Alphanumeric, or Byte) based on the input text.
• **Reed-Solomon Error Correction:** Calculates error correction codes that allow the QR code to be readable even if it is physically damaged, torn, or dirty.
• **Dynamic Masking:** To prevent patterns that might confuse QR readers (like massive black blocks), it tests 8 different mask patterns, calculates penalty scores, and applies the most ideal mask.
• **Direct `.bmp` Output:** Converts the generated 1-0 matrix into a physical image file in `.bmp` format using manual BMP header calculations.

## How It Works (Step-by-Step Generation)
When converting text to an image, this program follows these educational steps:
1. **Data Encoding:**
The user's input string is analyzed. It converts the text into a long sequence of 1s and 0s (bits) using **Numeric Mode** (for digits only), **Alphanumeric Mode** (for mixed alphanumeric), or **Byte Mode** (for special characters).
2. **Appending Error Correction Blocks:**
Using Galois Field mathematics, `Reed-Solomon` error correction codes are appended to the end of the generated bit sequence. This ensures the smartphone camera can recover missing data if part of the QR code is destroyed.
3. **Drawing Function Patterns:**
The three large squares at the corners (Finder Patterns), the dotted lines connecting them (Timing Patterns), and the smaller alignment squares are placed onto the matrix.
4. **Data Placement:**
The generated data and error correction bits are placed into the empty modules (pixels) in a zigzag pattern, starting from the bottom right corner of the matrix.
5. **Masking:**
The raw data placement might create optical illusions that disrupt alignment for barcode readers. The system tests 8 different XOR masks on the matrix, calculates a score using the penalty evaluation function, and permanently applies the mask with the lowest penalty (the most balanced black-and-white distribution).
6. **BMP File Generation:**
The abstract logical matrix is converted into a physical image file via the `bmpOlarakKaydet` function in `main.c`. Each QR module is drawn as a 10x10 pixel block, and a 4-unit "Quiet Zone" (Border) is added around it.

## File Structure
| File | Description |
| :--- | :--- |
| `main.c` | The main program that interacts with the user, takes input, and saves the resulting matrix to the computer in BMP format. |
| `karekod.c` | The core library file containing all QR Code standard algorithms (encoding, error correction, masking, penalty calculation). |
| `karekod.h` | The header file defining the library's public functions, modes, and data structures (Structs, Enums). |

## Installation and Usage
To compile and run the project, you need a C compiler (like GCC) installed on your system.
**Compilation**
```bash
gcc main.c karekod.c -o qr_maker
```
**Execution**
```bash
./qr_maker
```
When the program runs, it will prompt you for a text or URL, and then it will output a high-resolution file named karekod.bmp (or karekod1.bmp, etc., if the file already exists) in your current directory.
