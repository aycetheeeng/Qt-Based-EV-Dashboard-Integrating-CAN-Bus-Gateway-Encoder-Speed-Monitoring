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
#include <stdio.h>
#include <stdlib.h>  // Bunu eklemezsen rand() çalışmaz
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

CAN_HandleTypeDef hcan1;

TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
CAN_TxHeaderTypeDef TxHeader;
uint8_t  TxData[8] = {0}; // Test verisi
uint32_t TxMailbox = 0U;
volatile uint32_t adc_val = 0;   // Debug için global yaptık
volatile int16_t gaz_degeri = 0;
volatile uint32_t motor_puls_sayaci = 0;
int      ppr = 11; // Motorunun PPR değerini buraya yaz
int      reduksiyon = 10; // Dişli oranını buraya yaz
float    tekerlek_cevre = 1.88; // 60cm çap için metre cinsinden
float    gercek_fiziksel_hiz = 0; // Motorun gerçekteki hızı (78 km/h olan)
float    kalibre_edilen_hiz = 0;   // Ekranda görünecek olan (220 km/h olan)
uint32_t adc_degeri_akim;        // ADC'den gelen ham sayı için
float okunan_voltaj;             // PA1'deki 0-3.3V arası değer için
float gercek_sensor_voltaji;     // 2 ile çarpılmış hali için
float amper;                     // Sonuç olan Amper değeri için
float anlik_amper;   // Bu Live Watch'ta zıplamasını izleyeceğin ham olan (YENİ!)

// Sabit çarpanımız (78.04'ü 220'ye tamamlayan sihirli sayı)
const float kalibrasyon_katsayisi = 2.819f;

int tam_sayi_kalibre_hiz = 0; // Ekrana basılacak temiz tam sayı


/* --- YENİ EKLENECEK GÜÇ VE ENERJİ DEĞİŞKENLERİ --- */

float pil_voltaji = 12.0f;       // Aracın ana batarya voltajı (Simülasyon için)
float anlik_watt = 0.0f;         // P = V * I (Anlık Güç)
float anlik_kw = 0.0f;           // Dashboard'daki küçük kW yazısı için
float sanal_kw = 0.0f;           // Dashboard'daki büyük 15 kW gibi rakamlar için (Gerekirse)

float toplam_harcanan_wh = 0.0f; // Zamanla biriken enerji (Watt-saat)
float toplam_kwh = 0.0f;         // Yeşil rakam (18.5 kWh) için toplam enerji

float verimlilik = 0.0f;         // kWh / 100km göstergesi için

/* --- ZAMANLAMA --- */
uint32_t son_zaman = 0;          // 1000ms sayacı için
uint32_t su_anki_zaman = 0;      // HAL_GetTick() değerini tutmak için

float toplam_batarya_kapasitesi = 75.0f; // kWh (Togg/Tesla referansımız)
float kalan_batarya_kwh = 0.0f;          // O an pilde kalan enerji
float batarya_yuzdesi = 0.0f;           // % (0-100 arası)
float kalan_menzil_km = 0.0f;           // Kaç km daha gideriz

// Dashboard'da Görülecek 3 Ana Veri:
float anlik_tuketim_100km = 0.0f;  // 3. ANLIK TÜKETİM (100 km'de kaç kWh yakarım?)
float batarya_yuzdesi_baslangic = 0.0f;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN1_Init(void);
static void MX_TIM3_Init(void);
static void MX_ADC2_Init(void);
/* USER CODE BEGIN PFP */
void Hiz_Hesapla(void);
void Guc_Hesapla(void);
void Enerji_Hesapla(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == GPIO_PIN_9) { // PE9'dan sinyal gelince
        motor_puls_sayaci++;
    }
}
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
  /* USER CODE BEGIN 2 */
  // --- 1. Önce Yönü Belirle (PE7 ve PE8) ---
  // 1. CAN Hattını Başlat (Ekrana veri gitmesi için şart)
  HAL_CAN_Start(&hcan1);
  (void)HAL_CAN_Start(&hcan1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

  /* Mesaj Ayarları: ID: 0x103 (Dashboard bu ID'yi bekliyor) */
  TxHeader.StdId = 0x103U;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.DLC = 8U; /* 8 byte veri yolluyoruz */
  TxHeader.TransmitGlobalTime = DISABLE;
  uint32_t son_zaman = 0;


  srand(HAL_GetTick());
  batarya_yuzdesi_baslangic = (float)(rand() % 41 + 55); // %55-%95 arası başla
  batarya_yuzdesi = batarya_yuzdesi_baslangic;
  kalan_batarya_kwh = (batarya_yuzdesi * toplam_batarya_kapasitesi) / 100.0f;
  toplam_kwh = 0; // Her resetlendiğinde harcananı sıfırla

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
	  	      if (gaz_degeri < 5)
	  	      {
	  	          HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
	  	          HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
	  	          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
	  	      }
	  	      else
	  	      {
	  	          HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
	  	          HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
	  	          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, gaz_degeri);
	  	      }

	  	      // 2. CAN Mesajı Gönderme (Her 500ms'de bir)
	  	      static uint32_t can_zaman = 0;
	  	      if (su_anki_zaman - can_zaman >= 500)
	  	      {
	  	          // TxData[0] Hız olsun, TxData[1] Amper olsun (Dashboard'da görmek için)
	  	          TxData[0] = (uint8_t)tam_sayi_kalibre_hiz;
	  	          TxData[1] = (uint8_t)(amper + 100); // Negatif gelirse diye 100 ekleyip gönderebilirsin (Dashboard'da 100 çıkarırsın)

	  	          if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) == HAL_OK)
	  	          {
	  	              HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, IN1_Pin|IN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

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
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

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
