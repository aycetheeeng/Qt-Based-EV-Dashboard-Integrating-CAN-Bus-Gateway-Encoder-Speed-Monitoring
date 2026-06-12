/*
 * can_tx.c
 *
 *  Created on: 28 May 2026
 *      Author: aycex
 */

#include "can_tx.h"

void MotorVeVitesKontrol(void) {
    /* --- ADIM 1: VİTESİ (PRND) OKU (ADC3) --- */
    HAL_ADC_Start(&hadc3);
    if (HAL_ADC_PollForConversion(&hadc3, 10) == HAL_OK) {
        PRND_val = HAL_ADC_GetValue(&hadc3);
    }
    HAL_ADC_Stop(&hadc3);

    /* --- ADIM 2: ARALIKLARA GÖRE HEDEF VİTESİ SEÇ --- */
    bool target_p = (PRND_val < 4096 && PRND_val > 4050);
    bool target_r = (PRND_val < 3250 && PRND_val > 3180);
    bool target_n = (PRND_val < 2520 && PRND_val > 2430);
    bool target_d = (PRND_val < 1650 && PRND_val > 1570);

    if (target_p)      hedef_vites = VITES_P;
    else if (target_r) hedef_vites = VITES_R;
    else if (target_n) hedef_vites = VITES_N;
    else if (target_d) hedef_vites = VITES_D;

    /* --- ADIM 3: POTU (GAZ) OKU (ADC1) --- */
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        adc_val = HAL_ADC_GetValue(&hadc1);
        gaz_degeri = (int16_t)((adc_val * 100) / 4095);
    }
    HAL_ADC_Stop(&hadc1);

    /* --- ADIM 4: AKILLI VİTES GEÇİŞ MATRİSİ (SHIFT INHIBIT) - GÜNCELLENEN KISIM --- */
    if (mevcut_vites != hedef_vites) {

        // KURAL 1: N (Boş) vitesine her an güvenle geçilebilir
        if (hedef_vites == VITES_N) {
            mevcut_vites = VITES_N;
        }

        // KURAL 2: P vitesine geçebilmek için araç durmalı ve gaz bırakılmalı
        else if (hedef_vites == VITES_P) {
            if (motor_puls_sayaci == 0 && gaz_degeri < 5) {
                mevcut_vites = VITES_P;
            }
        }

        // KURAL 3: R (Geri) vitesine geçebilmek için araç durmalı ve gaz bırakılmalı
        else if (hedef_vites == VITES_R) {
            if (motor_puls_sayaci == 0 && gaz_degeri < 5) {
                mevcut_vites = VITES_R;
            }
        }

        // KURAL 4: D (Sürüş) vitesine geçiş kontrolü (Çift Yönlü Güvenlik Kilidi)
        else if (hedef_vites == VITES_D) {
            // Eğer araç zaten tamamen duruyorsa doğrudan D'ye geçebilir
            if (motor_puls_sayaci == 0 && gaz_degeri < 5) {
                mevcut_vites = VITES_D;
            }
            // Seyir halindeyken geçiş şartı: Sadece araç ileri süzülüyorsa (N'deyse ve yönü D ise) izin ver
            // Araç geri gidiyorsa veya R'den N'ye alınıp hala arkaya süzülüyorsa D'ye geçiş bloke edilir!
            else if (mevcut_vites == VITES_N && son_surus_yonu == VITES_D) {
                mevcut_vites = VITES_D;
            }
        }
    }

    /* --- ADIM 5: SÜRÜŞ MODLARINA GÖRE AKTİF GAZ (RAMPA) FİLTRESİ --- */
    rampa_sayaci++;

    if (mevcut_vites == VITES_N) {
        // N Modunda Yavaşlama Dinamiği (Süzülme)
        if (rampa_sayaci >= N_YAVASLAMA_PERIYODU) {
            rampa_sayaci = 0;
            if (aktif_gaz > 0) {
                aktif_gaz--;
            }
        }
    }
    else if (mevcut_vites == VITES_D) {
        // SADECE D MODUNDA: Tork Şoku Engelleme (Hızlanma Rampası)
        if (rampa_sayaci >= D_HIZLANMA_PERIYODU) {
            rampa_sayaci = 0;

            if (aktif_gaz < gaz_degeri) {
                aktif_gaz++; // İleri giderken tatlı tatlı hızlan
            }
            else if (aktif_gaz > gaz_degeri) {
                aktif_gaz = gaz_degeri; // Gazdan çekince anında tepki ver
            }
        }
    }
    else if (mevcut_vites == VITES_R) {
        // R Modunda rampa tamamen devre dışı! Manevra için pottan ne okunuyorsa ANINDA motora iletilir.
        aktif_gaz = gaz_degeri;
    }
    else {
        // P modunda hız doğrudan 0
        aktif_gaz = 0;
    }

    /* --- ADIM 6: ONAYLANAN VİTESE GÖRE MOTORU YÖNET --- */
    switch (mevcut_vites) {

        case VITES_P:
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
            break;

        case VITES_N:
            if (aktif_gaz < 5) {
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
            } else {
                if (son_surus_yonu == VITES_D) {
                    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
                } else {
                    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
                }
                uint8_t min_calisma = 50;
                uint16_t hesap = min_calisma + ((uint16_t)aktif_gaz * (100 - min_calisma) / 100);
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint8_t)hesap);
            }
            break;

        case VITES_D:
            son_surus_yonu = VITES_D;
            if (aktif_gaz < 5) {
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
            } else {
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);

                uint8_t min_calisma = 50;
                uint16_t hesap = min_calisma + ((uint16_t)aktif_gaz * (100 - min_calisma) / 100);
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint8_t)hesap);
            }
            break;

        case VITES_R:
            son_surus_yonu = VITES_R;
            if (aktif_gaz < 5) {
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
            } else {
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);

                uint8_t min_calisma = 50;
                uint16_t hesap = min_calisma + ((uint16_t)aktif_gaz * (100 - min_calisma) / 100);
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint8_t)hesap);
            }
            break;
    }
}


void batarya(void)
{
    srand(HAL_GetTick());

    batarya_yuzdesi_baslangic = (float)(rand() % 41 + 55);
    batarya_yuzdesi = batarya_yuzdesi_baslangic;
    kalan_batarya_kwh =
        (batarya_yuzdesi * toplam_batarya_kapasitesi) / 100.0f;
    toplam_kwh = 0;
}

void sinyalbutonlari(void) {

    uint8_t btn4 = (HAL_GPIO_ReadPin(GPIOD, BTN4_Pin) == GPIO_PIN_RESET);
    uint8_t btn2 = (HAL_GPIO_ReadPin(GPIOD, BTN2_Pin) == GPIO_PIN_RESET);
    uint8_t btn3 = (HAL_GPIO_ReadPin(GPIOD, BTN3_Pin) == GPIO_PIN_RESET);

    uint8_t blink = ((HAL_GetTick() / 350) % 2);

    if (btn4)
    {
        sol_aktif = 1;
        sag_aktif = 1;
    }
    else if (btn2)
    {
        sol_aktif = 1;
        sag_aktif = 0;
    }
    else if (btn3)
    {
        sol_aktif = 0;
        sag_aktif = 1;
    }
    else
    {
        sol_aktif = 0;
        sag_aktif = 0;
    }

    if (sol_aktif && sag_aktif)      sinyal_durumu = 3;
    else if (sol_aktif)              sinyal_durumu = 1;
    else if (sag_aktif)              sinyal_durumu = 2;
    else                             sinyal_durumu = 0;


    if (sinyal_durumu == 3)
    {
        HAL_GPIO_WritePin(GPIOB, SOL_SINYAL_Pin, blink);
        HAL_GPIO_WritePin(GPIOB, SAG_SINYAL_Pin, blink);
        HAL_GPIO_WritePin(GPIOB, DORTLU_Pin, blink);
    }
    else if (sinyal_durumu == 1)
    {
        HAL_GPIO_WritePin(GPIOB, SOL_SINYAL_Pin, blink);
        HAL_GPIO_WritePin(GPIOB, SAG_SINYAL_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, DORTLU_Pin, GPIO_PIN_RESET);
    }
    else if (sinyal_durumu == 2)
    {
        HAL_GPIO_WritePin(GPIOB, SAG_SINYAL_Pin, blink);
        HAL_GPIO_WritePin(GPIOB, SOL_SINYAL_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, DORTLU_Pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, SOL_SINYAL_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, SAG_SINYAL_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, DORTLU_Pin, GPIO_PIN_RESET);
    }

}

void farlar(void){
  // FARLAR (BTN5)
  if (HAL_GPIO_ReadPin(BTN5_GPIO_Port, BTN5_Pin) == GPIO_PIN_RESET)
  {
      HAL_GPIO_WritePin(FARLAR_GPIO_Port, FARLAR_Pin, GPIO_PIN_SET);
  }
  else
  {
      HAL_GPIO_WritePin(FARLAR_GPIO_Port, FARLAR_Pin, GPIO_PIN_RESET);
  }
}

void drivemod(void)
{
	// DRIVE MOD
	if (HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) == GPIO_PIN_RESET)
	{
		HAL_Delay(50); // Buton arkını (debounce) engellemek için bekle

	  	if (HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) == GPIO_PIN_RESET)
	  	{
	  		mode++; // Modu 1 artır (1'di 2 oldu, 2'ydi 3 oldu...)

	  		if (mode > 3)
	  			{
	  				mode = 1; // 4. basışta (veya 3'ü geçince) tekrar başa, yani 1'e dön
	  			}

	  	// Kullanıcı elini butondan çekene kadar burada bekle (Sonsuz artışı önler)
	  		while (HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) == GPIO_PIN_RESET);
	  	}
	}
	  	      // =================================================================
	  	      // 2. ADIM: SWITCH-CASE İLE MODLARIN KONTROLÜ
	  	      // =================================================================
	  	      switch (mode)
	  	      {
	  	          case 1:
	  	              // Mod 1 iken ne yapılmasını istiyorsan buraya yaz (Örn: Ekrana/Live Expressions'a 1 yazmak veya bir LED yakmak)
	  	              // __NOP(); // Şimdilik boş kalmasın diye işlemciye boş geç komutu
	  	              break;

	  	          case 2:
	  	              // Mod 2 iken yapılacaklar
	  	              break;

	  	          case 3:
	  	              // Mod 3 iken yapılacaklar
	  	              break;

	  	          default:
	  	              mode = 1;
	  	              break;
	  	      }
}

//void prndhesap(void){
//
//      // PRND için
//      HAL_ADC_Start(&hadc3);
//      if (HAL_ADC_PollForConversion(&hadc3, 10) == HAL_OK)
//      {
//          PRND_val = HAL_ADC_GetValue(&hadc3);
//      }
//      HAL_ADC_Stop(&hadc3);
//
//}


//void ADCHizOlc(void){
//      /* --- ADIM 1: POTU OKU (ADC1) --- */
//      HAL_ADC_Start(&hadc1);
//      if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
//      {
//          adc_val = HAL_ADC_GetValue(&hadc1);
//          gaz_degeri = (int16_t)((adc_val * 100) / 4095);
//      }
//      HAL_ADC_Stop(&hadc1);
//
//  	  /////// potu çevirince hızı ayarlıyoruz ama ölü bölgeyi ayırarak
//  		 if (gaz_degeri < 5)
//  		 {
//  		  	  // MOTORU DURDUR: Gaz pedalı sıfıra yakınsa motoru tamamen kapat
//  		  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
//  		  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
//  		  	  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
//  		 }
//
//  		else
//  		{
//  		    // MOTORU SÜR
//  		    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
//  		    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
//
//  		    uint8_t min_calisma = 50;
//
//  		    // HATA BURADA: Hesaplamayı uint16_t içinde yapıyoruz
//  		    uint16_t hesap = min_calisma + ((uint16_t)gaz_degeri * (100 - min_calisma) / 100);
//
//  		    // PWM çıkışına güvenle aktarıyoruz
//  		    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint8_t)hesap);
//  		}
//  		 else
//  		 {
//  		  	  // MOTORU SÜR: Gaz verildiyse motoru çalıştır
//  		  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
//  		  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
//
//  		  	  uint8_t min_calisma = 50; 	  	                         // Motorun "vınlamadan" dönmeye başladığı en düşük sınır
//  		  	  // 0-100 aralığını 70-100 arasına yayıyoruz (Mapping)
//  		  	  // Bu sayede potu çevirdiğinde motor vınlamadan tık diye döner
//  		  	  uint8_t pwm_cikisi = min_calisma + ((gaz_degeri * (100 - min_calisma)) / 100);
//  		  	  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_cikisi);
//  		 }
//}

//}


void akimoku(void){
  /* --- YENİ ADIM: AKIM SENSÖRÜNÜ OKU (ADC2 - PA1) --- */
  HAL_ADC_Start(&hadc2);
  if (HAL_ADC_PollForConversion(&hadc2, 10) == HAL_OK)
  {
	  adc_degeri_akim = HAL_ADC_GetValue(&hadc2);
	  okunan_voltaj = (adc_degeri_akim * 3.3f) / 4095.0f;
	  gercek_sensor_voltaji = okunan_voltaj * 1.9898f;

       // Anlık amper hesabı
       anlik_amper = (gercek_sensor_voltaji - 2.68f) / 0.185f;

       // FİLTRE: 0.98 çok yavaştı, 0.90 yaparak tepkiyi hızlandıralım
       amper = (amper * 0.90f) + (anlik_amper * 0.10f);

       // ÖLÜ BÖLGE: 0.08 yerine 0.02 yapalım ki 0.33A'lik motor akımını görebilelim
       if (amper < 0.02f && amper > -0.02f) {
           amper = 0.00f;
       }

  }
  HAL_ADC_Stop(&hadc2);
}

void Hesaplamalar(void){

    /* --- ZAMANLAYICI KONTROLLERİ --- */
	  uint32_t su_anki_zaman = HAL_GetTick();

	  	      // 1. Hız Hesabı (Her 1000ms'de bir)
	  	      if (su_anki_zaman - son_zaman >= 1000)
	  	      {
	  	          Hiz_Hesapla();
	  	          Guc_Hesapla();
	  	          Enerji_Hesapla();

	  	          son_zaman = su_anki_zaman;
	  	      }
}

void UARTgonderim(void)
{
    uint32_t su_anki_zaman = HAL_GetTick();
    //uint8_t blink = ((HAL_GetTick() / 350) % 2);

    /* =====================================================
       CAN GÖNDERİM KATMANI (500ms'de bir)
       ===================================================== */
    if (su_anki_zaman - can_zaman_2 >= 500)
    {
        can_zaman_2 = su_anki_zaman;

        /* =====================================================
           0x101 → SÜRÜŞ VERİLERİ
           ===================================================== */

        TxHeader.StdId = 0x101;
        TxHeader.IDE   = CAN_ID_STD;
        TxHeader.RTR   = CAN_RTR_DATA;
        TxHeader.DLC   = 8;

        // HIZ
        TxData101[0] = (uint8_t)tam_sayi_kalibre_hiz;

        // GÜÇ (kW x10)
        int16_t guc_send = (int16_t)(anlik_kw * 10);

        TxData101[1] = (uint8_t)(guc_send & 0xFF);
        TxData101[2] = (uint8_t)((guc_send >> 8) & 0xFF);

        // TÜKETİM
        uint16_t tuketim_send =
            (uint16_t)(anlik_tuketim_100km * 10);

        TxData101[3] = (uint8_t)(tuketim_send & 0xFF);
        TxData101[4] = (uint8_t)((tuketim_send >> 8) & 0xFF);

        // AMPER
        int16_t amper_send = (int16_t)(amper * 10);

        TxData101[5] = (uint8_t)(amper_send & 0xFF);
        TxData101[6] = (uint8_t)((amper_send >> 8) & 0xFF);

        // PRND
        TxData101[7] = PRND_val;

        while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);

        HAL_CAN_AddTxMessage(
            &hcan1,
            &TxHeader,
            TxData101,
            &TxMailbox
        );

        /* =====================================================
           0x102 → BATARYA VERİLERİ
           ===================================================== */

        TxHeader.StdId = 0x102;
        TxHeader.DLC   = 8;

        // BATARYA %
        TxData102[0] = (uint8_t)batarya_yuzdesi;

        // MENZİL
        uint16_t menzil_send =
            (uint16_t)kalan_menzil_km;

        TxData102[1] = (uint8_t)(menzil_send & 0xFF);
        TxData102[2] = (uint8_t)((menzil_send >> 8) & 0xFF);

        // TOPLAM TÜKETİM
        uint16_t trip_send =
            (uint16_t)(toplam_kwh * 100);

        TxData102[3] = (uint8_t)(trip_send & 0xFF);
        TxData102[4] = (uint8_t)((trip_send >> 8) & 0xFF);

        // BOŞ BYTE
        TxData102[5] = 0;
        TxData102[6] = 0;
        TxData102[7] = 0;

        while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);

        HAL_CAN_AddTxMessage(
            &hcan1,
            &TxHeader,
            TxData102,
            &TxMailbox
        );

        /* =====================================================
           0x103 → BUTON / FAR / SİNYAL
           ===================================================== */

        TxHeader.StdId = 0x103;
        TxHeader.DLC   = 8;

        // SİNYAL DURUMU
        TxData103[0] = sinyal_durumu;

        // DRIVE MODE
        TxData103[1] = mode;

        // FAR DURUMU
        TxData103[2] =
            HAL_GPIO_ReadPin(
                FARLAR_GPIO_Port,
                FARLAR_Pin
            );

        // SOL SİNYAL
        //TxData103[3] = (sol_aktif ? blink : 0);
        TxData103[3] = sol_aktif;

        // SAĞ SİNYAL
        //TxData103[4] = (sag_aktif ? blink : 0);
        TxData103[4] = sag_aktif;

        // DÖRTLÜ DURUMU
        TxData103[5] =
            (sol_aktif && sag_aktif);

        // BOŞ
        //TxData103[6] = 0;
        //TxData103[7] = 0;

        // BOŞ olan 6 ve 7. baytları 12-bit ADC verisi için birleştirdik
        TxData103[6] = (PRND_val >> 8) & 0xFF; // Üst bitler (High Byte)
        TxData103[7] = PRND_val & 0xFF;        // Alt bitler (Low Byte)

        while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);

        if(HAL_CAN_AddTxMessage(
            &hcan1,
            &TxHeader,
            TxData103,
            &TxMailbox
        ) == HAL_OK)
        {
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
        }
}
}

// HIZ HESAPLAMA

void Hiz_Hesapla(void)
{
    kalibre_edilen_hiz =
        ((float)motor_puls_sayaci * 220.0f) / MAX_PULSE;

    if (kalibre_edilen_hiz > 220.0f)
    {
        kalibre_edilen_hiz = 220.0f;
    }

    tam_sayi_kalibre_hiz = (int)(kalibre_edilen_hiz + 0.5f);

    motor_puls_sayaci = 0;
}


//void Hiz_Hesapla() {
//    // 1. Önceki hesaplamaların aynen kalsın
//    float rps = (float)motor_puls_sayaci / (ppr * reduksiyon);
//    gercek_fiziksel_hiz = rps * tekerlek_cevre * 3.6f;
//    kalibre_edilen_hiz = gercek_fiziksel_hiz * kalibrasyon_katsayisi;
//
//    // 2. Limitör
//    if (kalibre_edilen_hiz > 220.0f) {
//        kalibre_edilen_hiz = 220.0f;
//    }
//
//    // 3. TAM SAYIYA DÖNÜŞTÜRME (Yuvarlayarak)
//    // 0.5 ekleyip int'e zorlarsak en yakın tam sayıya yuvarlar
//    tam_sayi_kalibre_hiz = (int)(kalibre_edilen_hiz + 0.5f);
//
//    // Sayacı sıfırla
//    motor_puls_sayaci = 0;
//}



//void Hiz_Hesapla() {
//    su_anki_zaman = HAL_GetTick(); // STM32'nin anlık milisaniyesi
//
//    // 1. İki hesaplama arasında geçen süreyi buluyoruz
//    uint32_t gecen_sure_ms = su_anki_zaman - son_zaman;
//
//    // KRİTİK DÜZELTME: Zamanı hemen burada güncelliyoruz ki hafıza karışmasın!
//    son_zaman = su_anki_zaman;
//
//    // Koruma: Eğer fonksiyon 0 ms veya 1 ms gibi aşırı kısa sürede tekrar çağrıldıysa
//    // hesaplama yapma, bir sonraki ana döngüyü bekle.
//    if (gecen_sure_ms < 5) {
//        return;
//    }
//
//    // Milisaniyeyi saniyeye çeviriyoruz
//    float gecen_sure_saniye = (float)gecen_sure_ms / 1000.0f;
//
//    // 2. rps hesaplama (Puls / Süre)
//    float rps = ((float)motor_puls_sayaci / (ppr * reduksiyon)) / gecen_sure_saniye;
//
//    // Sayacı sıfırla (Süreyi kaydettiğimiz için artık güvenle sıfırlayabiliriz)
//    motor_puls_sayaci = 0;
//
//    // 3. Hız hesaplamaları
//    gercek_fiziksel_hiz = rps * tekerlek_cevre * 3.6f;
//    kalibre_edilen_hiz = gercek_fiziksel_hiz * kalibrasyon_katsayisi;
//
//    // 4. Limitör
//    if (kalibre_edilen_hiz > 220.0f) {
//        kalibre_edilen_hiz = 220.0f;
//    }
//
//    // 5. TAM SAYIYA DÖNÜŞTÜRME
//    tam_sayi_kalibre_hiz = (int)(kalibre_edilen_hiz + 0.5f);
//}



//GUC HESAPLAMA

void Guc_Hesapla() {
    // 1. ANLIK GÜÇ HESABI (Power - kW)
    if (amper > 0.02f) { // Gaza basılıyorsa

        // Hızın karesiyle artan aerodinamik direnç modeli (220 km/h -> 160 kW)
        float hiz_orani = kalibre_edilen_hiz / 220.0f;
        anlik_kw = hiz_orani * hiz_orani * 160.0f;

        // Araç yavaş bile gitse ekranlar, klima ve sistem için min. güç harcanır
        if (anlik_kw < 3.0f && kalibre_edilen_hiz > 5.0f) {
            anlik_kw = 4.5f;
        }
    } else {
        anlik_kw = 0.0f; // Ayak gazdan çekildi (Süzülme)
    }
}

// ENERJİ HESAPLAMA

void Enerji_Hesapla() {
    // 2. TOPLAM ENERJİ HESABI (Trip Energy - kWh)
    // Her 1 saniyede çalıştığı için gücü (kW) saniyeye çevirip ekliyoruz
    float saniyelik_harcanan = anlik_kw / 3600.0f;
    toplam_kwh += saniyelik_harcanan; // Bu sayaç gibidir, yavaş yavaş artar.

    // Batarya düşüşü
    kalan_batarya_kwh = (batarya_yuzdesi_baslangic * toplam_batarya_kapasitesi / 100.0f) - toplam_kwh;
    if (kalan_batarya_kwh < 0) kalan_batarya_kwh = 0.0f;
    batarya_yuzdesi = (kalan_batarya_kwh / toplam_batarya_kapasitesi) * 100.0f;

    // 3. ANLIK TÜKETİM HESABI (Consumption - kWh/100km)
    // Gerçek Araç Formülü: (Güç / Hız) * 100
    if (kalibre_edilen_hiz > 5.0f) {
        anlik_tuketim_100km = (anlik_kw / (float)kalibre_edilen_hiz) * 100.0f;

        // Menzili de artık doğrudan 100km tüketimine bağladık!
        if (anlik_tuketim_100km > 0) {
            kalan_menzil_km = (kalan_batarya_kwh / anlik_tuketim_100km) * 100.0f;
        }
    } else {
        // Araç dururken veya çok yavaşken "100 km" hesabı sonsuza gider, bu yüzden 0 gösterilir.
        anlik_tuketim_100km = 0.0f;
        // Dururken ortalama verimlilik (15 kWh/100km) üzerinden menzil tahmini yapılır
        kalan_menzil_km = (kalan_batarya_kwh / 15.0f) * 100.0f;
    }
}

