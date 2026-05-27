/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c (TARGET: STM32F407 - ECU NODE)
  * @project        : EV_Dashboard_Project
  * @module         : Electronic Control Unit (ECU) Simulation
  * @description    : This is the MAIN firmware for the F407 board.
  * It acts as the 'Sender' in the CAN network.
  * @author         : Ayçe Mısırlılar
  * @version        : V1.1.0_F407_TX
  * @date           : 05-April-2026
  ******************************************************************************
  * @architecture   : Classic CAN (CAN 2.0B) @ 500kbps
  * @hardware       : STM32F407VGT6 Discovery / Custom Board
  * @peripheral     : CAN1 (Pins: PB8/PB9)
  * * [MISRA C:2012 Compliant] - [AUTOSAR ASW Layer Design]
  ******************************************************************************
  * @history
  * 05-04-2026 | V1.1.0 | A.Misirlilar | Created F407 Specific TX Logic.
  ******************************************************************************
  * @attention
  * DO NOT flash this code to the G431 Gateway board.
  * This firmware is hardware-specific for the F407 platform.
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;
CAN_HandleTypeDef hcan1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
CAN_TxHeaderTypeDef TxHeader;

uint8_t   TxData[8]                   = {0};      // Test verisi
uint32_t  TxMailbox                   = 0U;

uint8_t   mode                        = 1;        // Başlangıç modumuz 1 olsun
uint8_t   son_durum                   = 0;
uint32_t  motor_puls_sayaci           = 0;

/* --- ZAMANLAMA --- */
uint32_t  son_zaman                   = 0;        // 1000ms sayacı için
uint32_t  su_anki_zaman               = 0;        // HAL_GetTick() değerini tutmak için

volatile  uint32_t adc_val            = 0;        // Debug için global yaptık
volatile  uint32_t motor_puls_sayaci  = 0;
volatile  uint32_t last_capture       = 0;
volatile  uint32_t pulse_period       = 0;
volatile  int16_t  gaz_degeri         = 0;
volatile  float motor_rpm = 0;


const     float kalibrasyon_katsayisi = 2.819f;   // Sabit çarpanımız (78.04'ü 220'ye tamamlayan sihirli sayı)

int       ppr                         = 11;       // Motorunun PPR değerini buraya yaz
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN1_Init(void);
static void MX_TIM3_Init(void);
static void MX_ADC2_Init(void);
static void MX_ADC3_Init(void);
static void MX_TIM4_Init(void);

/* USER CODE BEGIN PFP */

void Hiz_Hesapla(void);
void Guc_Hesapla(void);
void Enerji_Hesapla(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_CAN1_Init();
  MX_TIM3_Init();
  MX_ADC2_Init();
  MX_ADC3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

  // 1. CAN Hattını Başlat (Ekrana veri gitmesi için şart)

  HAL_CAN_Start(&hcan1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  //HAL_TIM_Base_Start(&htim4);

  /* Mesaj Ayarları: ID: 0x103 (Dashboard bu ID'yi bekliyor) */
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.DLC = 8U; /* 8 byte veri yolluyoruz */
  TxHeader.TransmitGlobalTime = DISABLE;

  srand(HAL_GetTick());

  batarya_yuzdesi_baslangic = (float)(rand() % 41 + 55); // %55-%95 arası başla
  batarya_yuzdesi = batarya_yuzdesi_baslangic;
  kalan_batarya_kwh = (batarya_yuzdesi * toplam_batarya_kapasitesi) / 100.0f;
  toplam_kwh = 0; // Her resetlendiğinde harcananı sıfırla
  uint32_t son_zaman = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

// DRIVE MOD BUTONU

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


	      // SOL SİNYAL (BTN2)

	      if (HAL_GPIO_ReadPin(BTN2_GPIO_Port, BTN2_Pin) == GPIO_PIN_RESET)
	      {
	          HAL_GPIO_WritePin(SOL_SINYAL_GPIO_Port, SOL_SINYAL_Pin, GPIO_PIN_SET);
	      }
	      else // Butona basılmıyorsa (yani SET ise) LED'i söndür!
	      {
	          HAL_GPIO_WritePin(SOL_SINYAL_GPIO_Port, SOL_SINYAL_Pin, GPIO_PIN_RESET);
	      }


	      // SAĞ SİNYAL (BTN3)

	      if (HAL_GPIO_ReadPin(BTN3_GPIO_Port, BTN3_Pin) == GPIO_PIN_RESET)
	      {
	          HAL_GPIO_WritePin(SAG_SINYAL_GPIO_Port, SAG_SINYAL_Pin, GPIO_PIN_SET);
	      }
	      else
	      {
	          HAL_GPIO_WritePin(SAG_SINYAL_GPIO_Port, SAG_SINYAL_Pin, GPIO_PIN_RESET);
	      }

	      // DÖRTLÜLER (BTN4) --------------------------------------------------------
	      if (HAL_GPIO_ReadPin(BTN4_GPIO_Port, BTN4_Pin) == GPIO_PIN_RESET)
	      {
	          HAL_GPIO_WritePin(DORTLU_GPIO_Port, DORTLU_Pin, GPIO_PIN_SET);
	      }
	      else
	      {
	          HAL_GPIO_WritePin(DORTLU_GPIO_Port, DORTLU_Pin, GPIO_PIN_RESET);
	      }
//        DÖRTLÜLER (BTN4) - KAPANMA GECİKMESİ ÇÖZÜMÜ
	      if (HAL_GPIO_ReadPin(BTN4_GPIO_Port, BTN4_Pin) == GPIO_PIN_RESET)
	      {
	      	  HAL_GPIO_WritePin(DORTLU_GPIO_Port, DORTLU_Pin, GPIO_PIN_SET); // Butona basıldığı an LED hiç beklemeden ŞAK diye yanar
	      }
	      else
	      {
//	      Buton kapatılmaya çalışıldığında (GND'den ayrılırken)
//	      Mekanik kararsızlığı aşmak için işlemciyi 80 ms bekletiyoruz
	      HAL_Delay(80);
//	      // 80 ms sonra hala bırakılmış durumdaysa LED'i kesin olarak söndür
	      if (HAL_GPIO_ReadPin(BTN4_GPIO_Port, BTN4_Pin) == GPIO_PIN_SET)
	      {
	      	  HAL_GPIO_WritePin(DORTLU_GPIO_Port, DORTLU_Pin, GPIO_PIN_RESET);
	      }
	      }

	      // FARLAR (BTN5)
	      if (HAL_GPIO_ReadPin(BTN5_GPIO_Port, BTN5_Pin) == GPIO_PIN_RESET)
	      {
	          HAL_GPIO_WritePin(FARLAR_GPIO_Port, FARLAR_Pin, GPIO_PIN_SET);
	      }
	      else
	      {
	          HAL_GPIO_WritePin(FARLAR_GPIO_Port, FARLAR_Pin, GPIO_PIN_RESET);
	      }

      /* --- ZAMANLAYICI KONTROLLERİ --- */
	  uint32_t su_anki_zaman = HAL_GetTick();

	  	      // 1. Hız Hesabı (Her 1000ms'de bir)
	  	      if (su_anki_zaman - son_zaman >= 1000)
	  	      {
	  	          Hiz_Hesapla();
	  	          Guc_Hesapla();    // YENİ
	  	          Enerji_Hesapla(); // YENİ

	  	          son_zaman = su_anki_zaman;
	  	      }
	  	      // PRND için
	  	      HAL_ADC_Start(&hadc3);
	  	      if (HAL_ADC_PollForConversion(&hadc3, 10) == HAL_OK)
	  	      {
	  	          PRND_val = HAL_ADC_GetValue(&hadc3);
	  	      }
	  	      HAL_ADC_Stop(&hadc1);

	  	      /* --- ADIM 1: POTU OKU (ADC1) --- */
	  	      HAL_ADC_Start(&hadc1);
	  	      if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
	  	      {
	  	          adc_val = HAL_ADC_GetValue(&hadc1);
	  	          gaz_degeri = (int16_t)((adc_val * 100) / 4095);
	  	      }
	  	      HAL_ADC_Stop(&hadc1);

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

	  	      /* --- ADIM 2: MOTOR SÜRÜCÜ KONTROLÜ --- */
//	  	      if (gaz_degeri < 5)
//	  	      {
//	  	          HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
//	  	          HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
//	  	          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
//	  	      }
//	  	      else
//	  	      {
//	  	          HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
//	  	          HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
//	  	          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, gaz_degeri);
//	  	      }
// Önce potansiyometreden değeri oku (senin kodunda bu zaten vardır)
// gaz_degeri = ADC_oku();

// ///	  // potu çevirince hızı ayarlıyoruz ama ölü bölgeyi ayırarak

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

	  	    /* --- ADIM 3: CAN BUS HABERLEŞMESİ (Her 500ms'de bir) --- */
	  	    static uint32_t can_zaman = 0;
	  	    if (su_anki_zaman - can_zaman >= 500)
	  	    {
	  	        // --- MESAJ 1: HIZ, GÜÇ VE TÜKETİM (0x101) ---
	  	        TxHeader.StdId = 0x101;
	  	        TxHeader.DLC = 8;
	  	        TxData[0] = (uint8_t)tam_sayi_kalibre_hiz;

	  	        int16_t guc_send = (int16_t)(anlik_kw * 10);
	  	        TxData[1] = (uint8_t)(guc_send & 0xFF);
	  	        TxData[2] = (uint8_t)((guc_send >> 8) & 0xFF);

	  	        uint16_t tuketim_send = (uint16_t)(anlik_tuketim_100km * 10);
	  	        TxData[3] = (uint8_t)(tuketim_send & 0xFF);
	  	        TxData[4] = (uint8_t)((tuketim_send >> 8) & 0xFF);

	  	        // Mesajı gönder ve boş mailbox bekle
	  	        while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);
	  	        HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);

	  	        // Küçücük bir nefes payı (Opsiyonel ama garantidir)
	  	        HAL_Delay(5);

	  	        // --- MESAJ 2: BATARYA VE MENZİL (0x102) ---
	  	        TxHeader.StdId = 0x102;
	  	        TxData[0] = (uint8_t)batarya_yuzdesi;

	  	        uint16_t menzil_send = (uint16_t)kalan_menzil_km;
	  	        TxData[1] = (uint8_t)(menzil_send & 0xFF);
	  	        TxData[2] = (uint8_t)((menzil_send >> 8) & 0xFF);

	  	        uint16_t trip_send = (uint16_t)(toplam_kwh * 100);
	  	        TxData[3] = (uint8_t)(trip_send & 0xFF);
	  	        TxData[4] = (uint8_t)((trip_send >> 8) & 0xFF);

	  	        while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0);
	  	        if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) == HAL_OK)
	  	        {
	  	            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15); // Turuncu/Mavi LED yanıp söner
	  	        }

	  	        can_zaman = su_anki_zaman;
	  	    }
	      // DİKKAT: Burada ASLA HAL_Delay(500) olmamalı!
	      // Delay koyarsan işlemci o sırada hiçbir şey yapamaz, encoder'ı bile kaçırabilirsin.

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.ScanConvMode = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 6;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 83;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_TIM_DISABLE_OCxPRELOAD(&htim3, TIM_CHANNEL_1);
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 84-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, IN1_Pin|IN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SOL_SINYAL_Pin|SAG_SINYAL_Pin|DORTLU_Pin|FARLAR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BRAKE_BTN_Pin */
  GPIO_InitStruct.Pin = BRAKE_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BRAKE_BTN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : IN1_Pin IN2_Pin */
  GPIO_InitStruct.Pin = IN1_Pin|IN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PE9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PD14 BTN1_Pin BTN2_Pin BTN3_Pin
                           BTN5_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_14|BTN1_Pin|BTN2_Pin|BTN3_Pin
                          |BTN5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN4_Pin */
  GPIO_InitStruct.Pin = BTN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SOL_SINYAL_Pin SAG_SINYAL_Pin DORTLU_Pin FARLAR_Pin */
  GPIO_InitStruct.Pin = SOL_SINYAL_Pin|SAG_SINYAL_Pin|DORTLU_Pin|FARLAR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
        last_capture = now;
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
