//名字
#ifndef 
#define 

#include ""

// WiFi网络的SSID和密码
#define WIFI_SSID_TEST          "MYSSID"
#define WIFI_PASSWORD_TEST      "PASSWORD"

// 全局变量声明
extern uint8 wifi_spi_test_buffer[];
extern uint8 wifi_spi_get_data_buffer[256];
extern uint16 data_length;

void SPI(void);

// 此处省略了其他的宏定义、枚举类型、结构体定义和函数原型声明...

#endif 
