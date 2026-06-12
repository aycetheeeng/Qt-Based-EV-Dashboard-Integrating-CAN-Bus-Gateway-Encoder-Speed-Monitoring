/*
 * can_tx.h
 *
 *  Created on: 29 May 2026
 *      Author: aycex
 */


#ifndef INC_CAN_TX_H_
#define INC_CAN_TX_H_
#endif /* INC_CAN_TX_H_ */


#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include <stdbool.h>

#define MAX_PULSE 407.0f
#define N_YAVASLAMA_PERIYODU 400 // N vitesindeyken yavaşlama hızı (Büyük değer = daha geç yavaşlar)
#define D_HIZLANMA_PERIYODU  400 // N'den D'ye geçince ani fırlamayı önleyen hızlanma rampası
                                // (Büyük değer = daha yumuşak hızlanır, tork şokunu engeller)

void Hiz_Hesapla(void);
void Guc_Hesapla(void);
void Enerji_Hesapla(void);

typedef enum {
    VITES_P = 0,
    VITES_R = 1,
    VITES_N = 2,
    VITES_D = 3
} VitesKonumu_t;

VitesKonumu_t mevcut_vites = VITES_P;
VitesKonumu_t hedef_vites = VITES_P;
VitesKonumu_t son_surus_yonu = VITES_D;


// --- YENİ BİRLEŞTİRİLMİŞ RAMPA DEĞİŞKENLERİ ---
int16_t aktif_gaz = 0;          // Motora gerçekten uygulanan anlık PWM hızı (0-100)
uint16_t rampa_sayaci = 0;      // Hızlanma ve yavaşlama zamanlaması için ortak sayaç

int PRND_val = 0;

// --- YENİ: N VİTESİ YAVAŞLAMA DEĞİŞKENLERİ ---
int16_t n_simule_gaz = 0;       // N vitesindeki sanal azalan hızımız
uint16_t n_yavaslama_sayaci = 0; // Yavaşlama hızını ayarlamak için sayaç


  // 1. CAN Hattını Başlat (Ekrana veri gitmesi için şart)

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim3;
//extern TIM_HandleTypeDef htim4;


//void MesajHazirla(CAN_TxHeaderTypeDef *pHeader) {
//    pHeader->IDE = CAN_ID_STD;
//    pHeader->RTR = CAN_RTR_DATA;
//    pHeader->DLC = 8U;
//    pHeader->TransmitGlobalTime = DISABLE;
//}

  // Sinyal durumlarını tutan değişkenler (statik veya global)
uint8_t sol_aktif = 0;
uint8_t sag_aktif = 0;
uint32_t son_degisim_zaman = 0;
bool lamba_durumu = false;

uint8_t btn2_lock = 0;
uint8_t btn3_lock = 0;
uint8_t btn4_lock = 0;

uint8_t TxData101[8];
uint8_t TxData102[8];
uint8_t TxData103[8];

CAN_TxHeaderTypeDef TxHeader;

uint8_t   TxData[8]                   = {0};      // Test verisi
uint32_t  TxMailbox                   = 0U;

uint8_t   mode                        = 1;        // Başlangıç modumuz 1 olsun
uint8_t   son_durum                   = 0;

//uint32_t  motor_puls_sayaci           = 0;

/* --- ZAMANLAMA --- */
uint32_t  su_anki_zaman               = 0;        // HAL_GetTick() değerini tutmak için
uint32_t son_zaman = 0;


volatile  uint32_t adc_val            = 0;        // Debug için global yaptık
volatile  uint32_t motor_puls_sayaci  = 0;

//volatile  uint32_t last_capture       = 0;

volatile  uint32_t pulse_period       = 0;
volatile  int16_t  gaz_degeri         = 0;
volatile  float motor_rpm = 0;

const     float kalibrasyon_katsayisi = 0.25f;   // Sabit çarpanımız (78.04'ü 220'ye tamamlayan sihirli sayı)

int       ppr                         = 10;       // Motorunun PPR değerini buraya yaz
int       reduksiyon                  = 10;       // Dişli oranını buraya yaz
int       tam_sayi_kalibre_hiz        = 0;        // Ekrana basılacak temiz tam sayı
int       PRND_val;

float     tekerlek_cevre              = 1.88;     // 60cm çap için metre cinsinden
float     gercek_fiziksel_hiz         = 0;        // Motorun gerçekteki hızı (78 km/h olan)
float     kalibre_edilen_hiz          = 0;        // Ekranda görünecek olan (220 km/h olan)
float     okunan_voltaj;                          // PA1'deki 0-3.3V arası değer için
float     gercek_sensor_voltaji;                  // 2 ile çarpılmış hali için
float     amper;                                  // Sonuç olan Amper değeri için
float     anlik_amper;                            // Bu Live Watch'ta zıplamasını izleyeceğin ham olan (YENİ!)

/* --- YENİ EKLENECEK GÜÇ VE ENERJİ DEĞİŞKENLERİ --- */

float     pil_voltaji                 = 12.0f;    // Aracın ana batarya voltajı (Simülasyon için)
float     anlik_watt                  = 0.0f;     // P = V * I (Anlık Güç)
float     anlik_kw                    = 0.0f;     // Dashboard'daki küçük kW yazısı için
float     sanal_kw                    = 0.0f;     // Dashboard'daki büyük 15 kW gibi rakamlar için (Gerekirse)
float     toplam_harcanan_wh          = 0.0f;     // Zamanla biriken enerji (Watt-saat)
float     toplam_kwh                  = 0.0f;     // Yeşil rakam (18.5 kWh) için toplam enerji
float     verimlilik                  = 0.0f;     // kWh / 100km göstergesi için
float     toplam_batarya_kapasitesi   = 75.0f;    // kWh (Togg/Tesla referansımız)
float     kalan_batarya_kwh           = 0.0f;     // O an pilde kalan enerji
float     batarya_yuzdesi             = 0.0f;     // % (0-100 arası)
float     kalan_menzil_km             = 0.0f;     // Kaç km daha gideriz

// Dashboard'da Görülecek 3 Ana Veri:

float     anlik_tuketim_100km         = 0.0f;     // 3. ANLIK TÜKETİM (100 km'de kaç kWh yakarım?)
float     batarya_yuzdesi_baslangic   = 0.0f;

uint32_t  adc_degeri_akim             = 0;
uint32_t  now                         = 0;

// SINYAL DURUMLARI (0: Kapalı, 1: Yanıp Sönüyor)
uint8_t   solSinyalAktif              = 0;
uint8_t   sagSinyalAktif              = 0;
uint8_t   dortluAktif                 = 0;

// Butonların "basılı mıydı?" durumunu takip etmek için
uint8_t btn2_lastState = 1;
uint8_t btn3_lastState = 1;
uint8_t btn4_lastState = 1;

uint8_t sinyal_durumu = 0;
uint32_t can_zaman_2 = 0;

//srand(HAL_GetTick());
