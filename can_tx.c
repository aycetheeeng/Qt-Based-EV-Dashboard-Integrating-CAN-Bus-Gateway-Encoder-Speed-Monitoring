/*
 * can_tx.c
 *
 *  Created on: 28 May 2026
 *      Author: aycex
 */

#include "can_tx.h"

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

void prndhesap(void){

      // PRND için
      HAL_ADC_Start(&hadc3);
      if (HAL_ADC_PollForConversion(&hadc3, 10) == HAL_OK)
      {
          PRND_val = HAL_ADC_GetValue(&hadc3);
      }
      HAL_ADC_Stop(&hadc3);
}


void ADCHizOlc(void){
      /* --- ADIM 1: POTU OKU (ADC1) --- */
      HAL_ADC_Start(&hadc1);
      if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
      {
          adc_val = HAL_ADC_GetValue(&hadc1);
          gaz_degeri = (int16_t)((adc_val * 100) / 4095);
      }
      HAL_ADC_Stop(&hadc1);

  	  /////// potu çevirince hızı ayarlıyoruz ama ölü bölgeyi ayırarak
  		 if (gaz_degeri < 5)
  		 {
  		  	  // MOTORU DURDUR: Gaz pedalı sıfıra yakınsa motoru tamamen kapat
  		  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
  		  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
  		  	  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
  		 }
  		 else
  		 {
  		  	  // MOTORU SÜR: Gaz verildiyse motoru çalıştır
  		  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
  		  	  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);

  		  	  uint8_t min_calisma = 50; 	  	                         // Motorun "vınlamadan" dönmeye başladığı en düşük sınır
  		  	  // 0-100 aralığını 70-100 arasına yayıyoruz (Mapping)
  		  	  // Bu sayede potu çevirdiğinde motor vınlamadan tık diye döner
  		  	  uint8_t pwm_cikisi = min_calisma + ((gaz_degeri * (100 - min_calisma)) / 100);
  		  	  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_cikisi);
  		 }
}

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
        TxData103[3] = sol_aktif;

        // SAĞ SİNYAL
        TxData103[4] = sag_aktif;

        // DÖRTLÜ DURUMU
        TxData103[5] =
            (sol_aktif && sag_aktif);

        // BOŞ
        TxData103[6] = 0;
        TxData103[7] = 0;

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



//void UARTgonderim(void){
//
//	uint32_t su_anki_zaman = HAL_GetTick();
//
//
//	// uint32_t su_anki_zaman;
//	  	/* =====================================================
//	  	   CAN GÖNDERİM KATMANI (500ms'de bir)
//	  	   ===================================================== */
//	  	if (su_anki_zaman - can_zaman_2 >= 500)
//	  	{
//	  	    can_zaman_2 = su_anki_zaman;
//
//	  	    /* =========================
//	  	       0x101 → SÜRÜŞ
//	  	       ========================= */
//	  	    TxHeader.StdId = 0x101;
//	  	    TxHeader.DLC = 8;
//
//	  	    TxData[0] = (uint8_t)tam_sayi_kalibre_hiz;
//
//	  	    int16_t guc_send = (int16_t)(anlik_kw * 10);
//	  	    TxData[1] = (uint8_t)(guc_send & 0xFF);
//	  	    TxData[2] = (uint8_t)((guc_send >> 8) & 0xFF);
//
//	  	    uint16_t tuketim_send = (uint16_t)(anlik_tuketim_100km * 10);
//	  	    TxData[3] = (uint8_t)(tuketim_send & 0xFF);
//	  	    TxData[4] = (uint8_t)((tuketim_send >> 8) & 0xFF);
//
//	  	    TxData[5] = 0;
//	  	    TxData[6] = 0;
//	  	    TxData[7] = 0;
//
//	  	    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);
//	  	    HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
//
//
//	  	    /* =========================
//	  	       0x102 → BATARYA
//	  	       ========================= */
//	  	    TxHeader.StdId = 0x102;
//
//	  	    TxData[0] = (uint8_t)batarya_yuzdesi;
//
//	  	    uint16_t menzil_send = (uint16_t)kalan_menzil_km;
//	  	    TxData[1] = (uint8_t)(menzil_send & 0xFF);
//	  	    TxData[2] = (uint8_t)((menzil_send >> 8) & 0xFF);
//
//	  	    uint16_t trip_send = (uint16_t)(toplam_kwh * 100);
//	  	    TxData[3] = (uint8_t)(trip_send & 0xFF);
//	  	    TxData[4] = (uint8_t)((trip_send >> 8) & 0xFF);
//
//	  	    TxData[5] = 0;
//	  	    TxData[6] = 0;
//	  	    TxData[7] = 0;
//
//	  	    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);
//	  	    HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
//	  	}
//
//
//	  	/* =====================================================
//	  	   5) CAN GÖNDERİM KATMANI (0x103)
//	  	   ===================================================== */
//
//
//	  	if (HAL_GetTick() - can_zaman_2 >= 500)
//	  	{
//	  	    can_zaman_2 = HAL_GetTick();
//
//	  	    TxHeader.StdId = 0x103;
//	  	    TxHeader.IDE   = CAN_ID_STD;
//	  	    TxHeader.DLC   = 1;
//
//	  	    TxData[0] = sinyal_durumu;
//
//	  	    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);
//
//	  	    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) == HAL_OK)
//	  	    {
//	  	        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
//	  	    }
//	  	}
//  }



// HIZ HESAPLAMA

void Hiz_Hesapla() {
    // 1. Önceki hesaplamaların aynen kalsın
    float rps = (float)motor_puls_sayaci / (ppr * reduksiyon);
    gercek_fiziksel_hiz = rps * tekerlek_cevre * 3.6f;
    kalibre_edilen_hiz = gercek_fiziksel_hiz * kalibrasyon_katsayisi;

    // 2. Limitör
    if (kalibre_edilen_hiz > 220.0f) {
        kalibre_edilen_hiz = 220.0f;
    }

    // 3. TAM SAYIYA DÖNÜŞTÜRME (Yuvarlayarak)
    // 0.5 ekleyip int'e zorlarsak en yakın tam sayıya yuvarlar
    tam_sayi_kalibre_hiz = (int)(kalibre_edilen_hiz + 0.5f);

    // Sayacı sıfırla
    motor_puls_sayaci = 0;
}

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








//void Hiz_Hesapla(void) {
//    uint32_t puls_temp;
//
//    __disable_irq();
//    puls_temp = motor_puls_sayaci;
//    motor_puls_sayaci = 0;
//    __enable_irq();
//
//    // HESAPLAMA
//    float tekerlek_tur_sayisi = (float)puls_temp / (ppr * reduksiyon);
//    gercek_fiziksel_hiz = (tekerlek_tur_sayisi * tekerlek_cevre) * 3.6f;
//    kalibre_edilen_hiz = gercek_fiziksel_hiz * 2.819f;
//
//    // YENİ: Düşük hızlarda yuvarlama yerine küçük bir eşik kontrolü
//    if (puls_temp > 0 && kalibre_edilen_hiz < 1.0f) {
//        tam_sayi_kalibre_hiz = 1; // En azından hareket ettiğini göster
//    } else {
//        tam_sayi_kalibre_hiz = (int)(kalibre_edilen_hiz + 0.5f);
//    }
//}

//GPIO_PinState pin_durumu = GPIO_PIN_RESET;

//void Hiz_Hesapla(void) {
//
//	pin_durumu = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_12);
//    // 1. Donanımdaki sayaç değerini senin değişkenine kopyala
//    motor_puls_sayaci = __HAL_TIM_GET_COUNTER(&htim4);
//
//    // 2. Bir sonraki saniye/periyot için donanım sayacını hemen sıfırla
//    __HAL_TIM_SET_COUNTER(&htim4, 0);
//
//    // 3. Senin Değişkenlerinle Matematiksel Hesaplama
//    // Tek kanal olduğu için ekstra bir bölme/çarpma yok, formülün orijinal halini koruyoruz
//    float tekerlek_tur_sayisi = (float)motor_puls_sayaci / (ppr * reduksiyon);
//
//    gercek_fiziksel_hiz = (tekerlek_tur_sayisi * tekerlek_cevre) * 3.6f;
//    kalibre_edilen_hiz = gercek_fiziksel_hiz * 2.819f;
//
//    // Live Watch'ta gördüğün tam sayı dönüşümü (Eşik kontrollü)
//    if (motor_puls_sayaci > 0 && kalibre_edilen_hiz < 0.5f) {
//        tam_sayi_kalibre_hiz = 1;
//    } else {
//        tam_sayi_kalibre_hiz = (int)(kalibre_edilen_hiz + 0.5f);
//    }
//}


//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//    if(htim->Instance == TIM4)
//    {
//        uint32_t now = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
//
//        if(last_capture != 0)
//        {
//            if(now >= last_capture)
//                pulse_period = now - last_capture;
//            else
//                pulse_period = (0xFFFF - last_capture) + now;
//
//            if(pulse_period > 0)
//            {
//                float freq = 1000000.0f / pulse_period;
//
//                // pulses_per_rev = senin sistem:
//                float pulses_per_rev = (float)(ppr * reduksiyon);
//
//                motor_rpm = (freq / pulses_per_rev) * 60.0f;
//            }
//        }
//        last_capture = now;
//    }
//}



// main fonskiyonundan sonra altında
// PE9 dan kesme ile encoder sinyal pinini okuma (ver1)                                       ***

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//    if(GPIO_Pin == GPIO_PIN_9) { // PE9'dan sinyal gelince
//        motor_puls_sayaci++;
//
//                // BAYRAĞI TEMİZLEMEZSEN İŞLEMCİ BİR DAHA KESMEYE GİRMEZ!
//                __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
//    }
//}


// PE9 dan kesme ile encoder sinyal pinini okuma (ver2)                                        ***

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//    if(GPIO_Pin == GPIO_PIN_9)
//    {
//        motor_puls_sayaci++;
//    }
//}

// PE9 dan kesme ile encoder sinyal pinini okuma (ver3)                                         ***

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//    static uint32_t son_tetiklenme_zamani = 0;
//    uint32_t simdiki_zaman = HAL_GetTick();
//
//    if(GPIO_Pin == GPIO_PIN_9)
//    {
//        // Sadece son tetiklenmeden bu yana 5ms geçtiyse say
//        // Bu, gürültüden kaynaklanan hızlı 0-1 değişimlerini görmezden gelmenizi sağlar.
//        if ((simdiki_zaman - son_tetiklenme_zamani) > 2)
//        {
//            motor_puls_sayaci++;
//            son_tetiklenme_zamani = simdiki_zaman;
//        }
//    }
//}

//
///* ================= DÖRTLÜ YEŞİL SİNYALLER ================= */
//
//if(HAL_GPIO_ReadPin(GPIOD, BTN4_Pin) == GPIO_PIN_RESET)
//{
//    if((HAL_GetTick() / 350) % 2 == 0)
//    {
//        HAL_GPIO_WritePin(GPIOB, SOL_SINYAL_Pin, GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOB, SAG_SINYAL_Pin, GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOB, DORTLU_Pin, GPIO_PIN_SET);
//    }
//    else
//    {
//        HAL_GPIO_WritePin(GPIOB, SOL_SINYAL_Pin, GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOB, SAG_SINYAL_Pin, GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(GPIOB, DORTLU_Pin, GPIO_PIN_RESET);
//    }
//}
//
///* ================= SOL ================= */
//
//else if(HAL_GPIO_ReadPin(GPIOD, BTN2_Pin) == GPIO_PIN_RESET)
//{
//    HAL_GPIO_WritePin(GPIOB, SAG_SINYAL_Pin, GPIO_PIN_RESET);
//
//    if((HAL_GetTick() / 350) % 2 == 0)
//    {
//        HAL_GPIO_WritePin(GPIOB, SOL_SINYAL_Pin, GPIO_PIN_SET);
//    }
//    else
//    {
//        HAL_GPIO_WritePin(GPIOB, SOL_SINYAL_Pin, GPIO_PIN_RESET);
//    }
//}
//
///* ================= SAĞ ================= */
//
//else if(HAL_GPIO_ReadPin(GPIOD, BTN3_Pin) == GPIO_PIN_RESET)
//{
//    HAL_GPIO_WritePin(GPIOB, SOL_SINYAL_Pin, GPIO_PIN_RESET);
//
//    if((HAL_GetTick() / 350) % 2 == 0)
//    {
//        HAL_GPIO_WritePin(GPIOB, SAG_SINYAL_Pin, GPIO_PIN_SET);
//    }
//    else
//    {
//        HAL_GPIO_WritePin(GPIOB, SAG_SINYAL_Pin, GPIO_PIN_RESET);
//    }
//}
//
///* ================= HİÇBİRİ ================= */
//
//else
//{
//    HAL_GPIO_WritePin(GPIOB, SOL_SINYAL_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOB, SAG_SINYAL_Pin, GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOB, DORTLU_Pin, GPIO_PIN_RESET);
//}
//
//
//if (sol_aktif && sag_aktif) sinyal_durumu = 3;
//else if (sol_aktif)         sinyal_durumu = 1;
//else if (sag_aktif)         sinyal_durumu = 2;
//else                        sinyal_durumu = 0;
//
