//放在已有的文件里面，不然要加.h

#define WIFI_SSID_TEST          "BUPT-portal"
#define WIFI_PASSWORD_TEST      "" //注意 这个上传之前要消掉


uint8 wifi_spi_test_buffer[] = "this is wifi spi test buffer";
uint8 wifi_spi_get_data_buffer[256];
uint16 data_length;
void SPI(){
    printf("this is wifi spi test buffer\r\n");   // 模块固件版本

    // 当前使用的是 高速WIFI SPI模块的 station 模式 可以用于连接路由器 WIFI
    //
    // 发送数据方式可以使用透传发送(默认) 也可以使用命令发送 可以在 wifi_spi_init 函数内修改 <当模块作为服务器时 只能使用命令方式发送数据>
    while(wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST, WIFI_SPI_STATION))
    {
        system_delay_ms(100);                                                   // 初始化失败 等待 100ms
    }

    printf("\r\n module version:%s \r\n",wifi_spi_information.version);   // 模块固件版本
    printf("\r\n module mac    :%s \r\n",wifi_spi_information.mac);       // 模块 MAC 信息
    printf("\r\n module ip     :%s \r\n",wifi_spi_information.local_ip);  // 模块 IP 地址

    // zf_device_wifi_spi.h 文件内的宏定义可以更改模块连接(建立) WIFI 之后，是否自动连接 TCP 服务器、创建 UDP 连接、创建 TCP 服务器等操作
    if(2 != WIFI_SPI_AUTO_CONNECT)                                             // 如果没有开启自动连接 就需要手动连接目标 IP
    {
        if(wifi_spi_connect_udp_client(                                        // 向指定目标 IP 的端口建立 UDP 连接
            WIFI_SPI_TARGET_IP,                                                // 这里使用与自动连接时一样的目标 IP 实际使用时也可以直接填写目标 IP 字符串
            WIFI_SPI_TARGET_PORT,                                              // 这里使用与自动连接时一样的目标端口 实际使用时也可以直接填写目标端口字符串
            WIFI_SPI_LOCAL_PORT,                                               // 这里使用与自动连接时一样的本地端口 也就是自己通信使用的端口号  实际使用时也可以直接填写其他的本地端口字符串
            WIFI_SPI_SERIANET))                                                // 采用透传模式 当然你可以改成命令传输模式 推荐使用透传模式这样才能获得最大速度
        {
            // 如果一直建立失败 考虑一下是不是没有接硬件复位
            printf("\r\n Connect UDP Client error, try again.");
            system_delay_ms(100);                                               // 建立连接失败 等待 100ms
        }
//写在这里：他真的考我理解啊
        else{
            printf("\r\nConnected to UDP Server.\r\n");
                        
            // 定义并发送三个变量
            int var1 = 100, var2 = 200, var3 = 300;
            char varBuffer[128];
            snprintf(varBuffer, sizeof(varBuffer), "%d,%d,%d", var1, var2, var3);
            data_length = wifi_spi_send_buffer((uint8 *)varBuffer, strlen(varBuffer));
            if(data_length > 0)
            {
                printf("\r\nVariables send success.");
            }
            else
            {
                printf("\r\nVariables send failed.");
            }
        }
    }

    // printf("\r\n module port   :%s",wifi_spi_information.local_port);// 模块 PORT 信息

    // // 发送测试数据至服务器
    // data_length = wifi_spi_send_buffer(wifi_spi_test_buffer, sizeof(wifi_spi_test_buffer));
    // if(!data_length)
    // {
    //     printf("\r\n send success.");
    // }
    // else
    // {
    //     printf("\r\n %d bytes data send failed.", data_length);
    // }

    // while(1)
    // {
    //     data_length = wifi_spi_read_buffer(wifi_spi_get_data_buffer, sizeof(wifi_spi_get_data_buffer));
    //     if(data_length)                                                         // 如果接收到数据 则进行数据类型判断
    //     {
    //         printf("\r\n Get data: <%s>.", wifi_spi_get_data_buffer);
    //         if(!wifi_spi_send_buffer(wifi_spi_get_data_buffer, data_length))
    //         {
    //             printf("\r\n send success.");
    //             memset(wifi_spi_get_data_buffer, 0, data_length);          // 数据发送完成 清空数据
    //         }
    //         else
    //         {
    //             printf("\r\n %d bytes data send failed.", data_length);
    //         }
    //     }
    //     system_delay_ms(100);
    // }
}