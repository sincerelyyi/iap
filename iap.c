/** **************************************************************************************
 * @brief stm32f iap 电脑端
 * @note
 *****************************************************************************************/
//#define LINUX
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "./setup.h"
#include "./command.h"
#include "./return_num.h"
#ifdef LINUX
#include <termios.h>
#else
#include "./serialport.h"
#endif

#define TRY_MAX  3
uint8_t receive_buff[1000] = {0};
uint16_t receive_p = 0;
uint8_t Try_times = 0;

#ifdef LINUX
#define read_serial(a, b, c) read(a,b,c)
#define close_serial(a) close(a)
int serial_port;
#else
#define read_serial(a, b, c) readFromSerialPort(a, b, c)
#define close_serial(a) closeSerialPort(a)
HANDLE serial_port;
#endif
int open_serial(const char *port,uint32_t baud)
{
#ifdef LINUX
    serial_port = open(port, O_RDWR|O_NOCTTY); // 打开串口设备文件
    if (serial_port < 0) {
        perror("无法打开串口");
        return NO_SERIAL_PORT;
    }

    struct termios tty;
    // 获取串口属性
    //  if (tcgetattr(serial_port, &tty) != 0) {
    //      perror("获取串口属性失败");
    //      close_serial(serial_port);
    //      return NO_SERIAL_PORT;
    // }


    //c_iflag
    tty.c_iflag = 0;
    tty.c_iflag &= ~(IGNBRK);       // 忽略BREAK键输入
    tty.c_iflag &= ~(BRKINT);       // 如果设置了IGNBRK，BREAK键输入将被忽略
    tty.c_iflag |= (IGNPAR);        // 忽略奇偶校验错误
    tty.c_iflag &= ~(PARMRK);       // 标识奇偶校验错误
    tty.c_iflag &= ~(INPCK);        // 允许输入奇偶校验
    tty.c_iflag &= ~(ISTRIP);       // 去除字符的第8个比特
    tty.c_iflag &= ~(INLCR);        // 将输入的NL（换行）转换成CR（回车）
    tty.c_iflag &= ~(IGNCR);        // 忽略输入的回车
    tty.c_iflag &= ~(ICRNL);        // 将输入的回车转化成换行（如果IGNCR未设置的情况下）
    tty.c_iflag &= ~(IUCLC);        // 将输入的大写字符转换成小写字符（非POSIX）
    tty.c_iflag &= ~(IXON);         // 允许输出时对XON/XOFF流进行控制
    tty.c_iflag &= ~(IXANY);        // 输入任何字符将重启停止的输出
    tty.c_iflag &= ~(IXOFF);        // 允许输入时对XON/XOFF流进行控制
    tty.c_iflag &= ~(IMAXBEL);      // 当输入队列满的时候开始响铃
//c_oflag
    tty.c_oflag = 0;
    tty.c_oflag &= ~(OPOST);        // 处理后输出
    tty.c_oflag &= ~(OLCUC);        // 将输出的小写字符转换成大写字符（非POSIX）```
    tty.c_oflag &= ~(ONLCR);        // 将输出的NL（换行）转换成CR（回车）及NL（换行）
    tty.c_oflag &= ~(OCRNL);        // 将输出的CR（回车）转换成NL（换行）
    tty.c_oflag &= ~(ONOCR);        // 第一行不输出回车符
    tty.c_oflag &= ~(ONLRET);       // 不输出回车符
    tty.c_oflag &= ~(OFILL);        // 发送填充字符以延迟终端输出
    tty.c_oflag &= ~(OFDEL);        // 以ASCII码的DEL作为填充字符，如果未设置该参数，填充字符为NUL
    tty.c_oflag &= ~(NLDLY);        // 换行输出延时，可以取NL0（不延迟）或NL1（延迟0.1s）
    tty.c_oflag &= ~(CRDLY);        // 回车延迟，取值范围为：CR0、CR1、CR2和 CR3
    tty.c_oflag &= ~(TABDLY);       // 水平制表符输出延迟，取值范围为：TAB0、TAB1、TAB2和TAB3
    tty.c_oflag &= ~(BSDLY);        // 空格输出延迟，可以取BS0或BS1
    tty.c_oflag &= ~(VTDLY);        // 垂直制表符输出延迟，可以取VT0或VT1
    tty.c_oflag &= ~(FFDLY);        // 换页延迟，可以取FF0或FF1
//c_cflag
    tty.c_cflag = 0;
//tty.c_oflag &= ~(CBAUD);          // 波特率（4+1位）（非POSIX）
//tty.c_oflag &= ~(CBAUDEX);        // 附加波特率（1位）（非POSIX）
    // 设置波特率为9600
    cfsetospeed(&tty,baud );
    cfsetispeed(&tty,baud);
    tty.c_cflag &= ~(CSIZE);        // 字符长度，取值范围为CS5、CS6、CS7或CS8
    tty.c_cflag |= CS8;             // 8位字符
    tty.c_cflag &= ~(CSTOPB);       // 设置两个停止位
    tty.c_cflag |= (CREAD);         // 使用接收器
    tty.c_cflag &= ~(PARENB);       // 使用奇偶校验
    tty.c_cflag &= ~(PARODD);       // 对输入使用奇偶校验，对输出使用偶校验
    tty.c_cflag &= ~(HUPCL);        // 关闭设备时挂起
    tty.c_cflag |= (CLOCAL);        // 忽略调制解调器线路状态
    tty.c_cflag &= ~(CRTSCTS);      // 使用RTS/CTS流控制
//c_lflag
    tty.c_lflag &= ~(ISIG);         // 当输入INTR、QUIT、SUSP或DSUSP时，产生相应的信号
    tty.c_lflag &= ~(ICANON);       // 使用标准输入模式
    tty.c_lflag &= ~(XCASE);        // 在ICANON和XCASE同时设置的情况下，终端只使用大写
    tty.c_lflag &= ~(ECHO);         // 显示输入字符
    tty.c_lflag &= ~(ECHOE);        // 如果ICANON同时设置，ERASE将删除输入的字符
    tty.c_lflag &= ~(ECHOK);        // 如果ICANON同时设置，KILL将删除当前行
    tty.c_lflag &= ~(ECHONL);       // 如果ICANON同时设置，即使ECHO没有设置依然显示换行符
    tty.c_lflag &= ~(ECHOPRT);      // 如果ECHO和ICANON同时设置，将删除打印出的字符（非POSIX）
    tty.c_lflag &= ~(TOSTOP);       // 向后台输出发送SIGTTOU信号

    //c_cc
//VINTR Interrupt字符
//VQUIT Quit字符
//VERASE    Erase字符
//VKILL Kill字符
//VEOF  End-of-file字符
//VMIN  非规范模式读取时的最小字符数
//VEOL  附加的End-of-file字符
//VTIME 非规范模式读取时的超时时间
//VSTOP Stop字符


    // 设置超时时间
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        perror("设置串口属性失败");
        return SET_SERIAL_ERROR;
    }
    tcflush(serial_port, TCIOFLUSH);
#else
    serial_port = openSerialPort(port,baud,one,off);
    if(serial_port == INVALID_HANDLE_VALUE)
    {
        return NO_SERIAL_PORT;
    }
#endif
    return SUCCESS;
}
/** **************************************************************************************
  * @brief checksum
  * @note
  * @param buff:发送的数据指针，len：数据长度
  * @return uint8_t checksum值
  * @retval
  *****************************************************************************************/
uint8_t checksum(uint8_t *buff,uint16_t len)
{
    uint8_t sum = 0;
    for(uint16_t i = 0; i< len; i++)
    {
        sum += buff[i];
    }
    return  sum;
}

/** **************************************************************************************
  * @brief bin打包
  * @note
  * @param command,*string,isanswert:1,为回复，0为主动发送
  * @return void
  * @retval
  *****************************************************************************************/
int send_bin(uint8_t command,uint8_t * buff,uint16_t len,uint8_t isanswer)
{
    uint16_t length;
    length = 5+len;
    uint8_t answer[length];
    answer[0] = 0x55;
    if(isanswer)
        answer[1] = command | 0x80;
    else
        answer[1] = command;
    answer[2] = len;
    memcpy(answer+3,buff,len);
    answer[sizeof(answer)-2] =0;
    answer[sizeof(answer)-1] = 0xaa;
    answer[sizeof(answer)-2] = 0-checksum(answer,sizeof(answer));     //赋值checksum位
#ifdef LINUX
    if(write(serial_port, answer, sizeof(answer)) !=(ssize_t) sizeof(answer))
    {
        return SEND_ERROR;
    }
#else
    if(writeToSerialPort(serial_port,answer,sizeof(answer)) != sizeof(answer))
    {
        return SEND_ERROR;
    }
#endif
}
/** **************************************************************************************
  * @brief setup iap
  * @note
  * @param
  * @return
  * @retval
  *****************************************************************************************/
int setup_iap(void)
{
#define setup_buff ___f407_iap_build_WL_IO_F407_bin
#define setup_len ___f407_iap_build_WL_IO_F407_bin_len
    uint32_t handle_p = 0;
    uint8_t send_buff[137];
    uint32_t ret;
    uint32_t *addr;
    addr = (uint32_t *)send_buff;
    *addr = 0x8060000;
    printf("setupping iap...\n");
    while( setup_len > handle_p)
    {
        ret =  setup_len - handle_p;
        if(ret>128)
        {
            ret = 128;
        }
        memcpy(send_buff +4,setup_buff+handle_p,ret);
        handle_p +=ret;
        if( send_bin(master_senddata,send_buff, ret+4, 0) == SEND_ERROR)
        {
            perror("send data error");
            return SEND_ERROR;
        }
        for(uint8_t j=0; j<ret/32+((ret%32)?1:0); j++)
        {
            printf("\n%.8x: ",*addr+j*32);
            for(uint8_t i = 0; i<((ret-j*32>=32)?32:(ret-j*32)); i++)
            {
                printf("%.2X",send_buff[i+j*32+4]);
            }
        }
        memset(receive_buff, 0, sizeof(receive_buff));
        read_serial(serial_port, receive_buff, 6);
        if(receive_buff[0] == 0x55 && receive_buff[5] == 0xaa)
        {
            switch (receive_buff[3])
            {
            case 0: // 正常写flash结束
                *addr += ret;
                break;
            case 1: // 固件超界
                perror("soldware out of range");
                return SOLDWARE_OUTOFRANGE ;
                break;
            case 2: // 写flash错误
                perror("program flash error");
                return PROGRAM_FLASH_ERROR;
                break;
            case 3: // 擦除扇区错误
                perror("erase sector error");
                return ERASE_SECTOR_ERROR;
                break;
            default:
                perror("reply unkown");
                return ERASE_SECTOR_ERROR;
            }
        }
        else
        {
            perror("reply formate error");
            return REPLY_FORMATE_ERROR;
        }

    }
    return SUCCESS;
}

/** **************************************************************************************
 * @brief is_valid_bin 判断是否有效的bin文件
 * @note
 * @param {*}
 * @return {*}
 * @retval
 *****************************************************************************************/
int is_valid_bin(FILE *bin)
{
    uint32_t heap = 0;
    uint32_t reset = 0;
    fseek(bin, 0, SEEK_SET);
    fread(&heap,4,1,bin);
    fread(&reset,4,1,bin);
    printf("heap=%.8x reset=%.8x\n", heap,reset);
    fseek(bin, 0, SEEK_SET);
    if(heap> 0x20020000 || heap < 0x20000000) // heap 不在内存地址段
    {
        return UNVALID_BIN;
    }
    if(reset > 0x8060000 || reset < 0x8010000) // reset 不在iap地址段
    {
        return UNVALID_BIN;
    }
    return  SUCCESS;
}
/** **************************************************************************************
 * @brief 主函数
 * @note
 * @param {*}
 * @return {*}
 * @retval
 *****************************************************************************************/
int main(int argc,char *argv[])
{
    uint8_t tmp_data[128+4];
    uint32_t *addr = 0;
    size_t ret;
    int return_num;
    printf("\n         fangli iap program\n\n");
    if(argc < 3)
    {
        printf("usage:\n");
#ifdef LINUX
        printf("      iap <xx.bin> </dev/ttyUSBx> [-f]\n");
#else
        printf("      iap.exe <xx.bin> <comx> [-f]\n");
#endif
        printf("      -f: force install iap to mcu\n");
        return TOO_LESS_ARG;
    }
    FILE *bin = fopen(argv[1], "rb");
    if (!bin)
    {
        perror("open file fail");
        return OPEN_FILE_ERROR;
    }
    if(is_valid_bin(bin))
    {
        perror("unvalid bin file");
        return UNVALID_BIN;
    }
    printf("open file success.\n");
    open_serial(argv[2], B115200);
    send_bin(master_isiap,NULL,0,0);
    read_serial(serial_port, receive_buff, 6);
    printf("isiap:");
    for(uint8_t i= 0; i<6; i++)
    {
        printf("%.2x ",receive_buff[i]);
    }
    printf("\n");
    if(receive_buff[0] == 0x55 && receive_buff[5] == 0xaa)
    {
        switch (receive_buff[3])
        {
        case 0:                     // 在app段，但没有检测到iap
            // 安装iap段到mcu
            Try_times = 0;
            do
            {
                return_num = setup_iap();
                Try_times ++;
                if(Try_times > TRY_MAX)
                {
                    perror("setup iap error");
                    close_serial(serial_port);
                    return return_num;
                }
            }
            while(return_num);
            printf("\nsetup iap success\n");
            printf("\njump to iap\n");
            send_bin(master_jumptoiap, NULL, 0, 0);
            close_serial(serial_port);
            serial_port = 0;
            sleep(1);
            break;
        case 1:                     // 在app段，检测到iap
            if(argc >=4 && strcmp(argv[3],"-f")==0) // 强制安装iap段
            {
                Try_times = 0;
                do
                {
                    return_num = setup_iap();
                    Try_times ++;
                    if(Try_times > TRY_MAX)
                    {
                        perror("setup iap error");
                        close_serial(serial_port);
                        return return_num;
                    }
                }
                while(return_num);
                printf("\nsetup iap success\n");
            }
            printf("\njump to iap\n");
            send_bin(master_jumptoiap, NULL, 0, 0);
            close_serial(serial_port);
            serial_port = 0;
            sleep(1);
            break;
        case 2:                     // 在iap段
            break;
        default:
            ;
        }
    }
    else
    {
        perror("app no action");
        return APP_NO_ACTION;
    }
    //与iap段通信
    for(uint8_t i=0; i<5; i++)
    {
        if(open_serial(argv[2], B115200) == 0)
        {
            break;
        }
    }
    send_bin(master_isiap,NULL,0,0);
    memset(receive_buff, 0, sizeof(receive_buff));
    read_serial(serial_port, receive_buff, 6);
    printf("isiap:");
    for(uint8_t i= 0; i<6; i++)
    {
        printf("%.2x ",receive_buff[i]);
    }
    printf("\n");
    if(receive_buff[0] != 0x55 || receive_buff[5] != 0xaa)
    {
        perror("iap no action");
        fclose(bin);
        close_serial(serial_port);
        return IAP_NO_ACTION;
    }
    // program flash
    printf("\nprogramming flash\n");
    addr = (uint32_t *)tmp_data;
    Try_times = 0;
program_again:
    *addr = 0x8010000;
    fseek(bin, 0, SEEK_SET);
    while( (ret = fread(tmp_data + 4, 1, 128, bin)))
    {
        if( send_bin(master_senddata, tmp_data, ret+4, 0) == -1)
        {
            perror("send data error");
            fclose(bin);
            close_serial(serial_port);
            return SEND_ERROR;
        }
        for(uint8_t j=0; j<ret/32+((ret%32)?1:0); j++)
        {
            printf("\n%.8x: ",*addr+j*32);
            for(uint8_t i = 0; i<((ret-j*32>=32)?32:(ret-j*32)); i++)
            {
                printf("%.2X",tmp_data[i+j*32+4]);
            }
        }
        receive_p = 0;
        memset(receive_buff, 0, sizeof(receive_buff));
        read_serial(serial_port, receive_buff + receive_p, 6);
        if(receive_buff[0] == 0x55 && receive_buff[5] == 0xaa)
        {
            switch (receive_buff[3])
            {
            case 0: // 正常写flash结束
                *addr += ret;
                break;
            case 1: // 固件超界
                perror("soldware out of range");
                fclose(bin);
                close_serial(serial_port);
                return SOLDWARE_OUTOFRANGE;
                break;
            case 2: // 写flash错误
                perror("program flash error");
                fclose(bin);
                close_serial(serial_port);
                return PROGRAM_FLASH_ERROR;
                break;
            case 3: // 擦除扇区错误
                perror("erase sector error");
                fclose(bin);
                close_serial(serial_port);
                return ERASE_SECTOR_ERROR;
                break;
            default:
                perror("reply unknow");
                fclose(bin);
                close_serial(serial_port);
                return REPLY_UNKNOW;
            }
        }
        else
        {

            Try_times ++;
            if(Try_times < TRY_MAX)
            {
                goto program_again;
            }
            perror("reply format error");
            for(uint8_t i=0; i< 6; i++)
            {
                printf("%.2x ",receive_buff[i]);
            }
            fclose(bin);
            close_serial(serial_port);
            return REPLY_FORMATE_ERROR;
        }

    }
    send_bin(master_jumptoapp, NULL, 0, 0);
    printf("\nprogram flash completed.\n");
    printf("jump back to app..\n");
    fclose(bin);
    close_serial(serial_port);
    sleep(1);
    //与iap段通信
    for(uint8_t i=0; i<5; i++)
    {
        if(open_serial(argv[2], B115200) == 0)
        {
            break;
        }
    }
    send_bin(master_isiap,NULL,0,0);
    //usleep(100000); // sleep 100 ms
    memset(receive_buff, 0, sizeof(receive_buff));
    read_serial(serial_port, receive_buff, 6);
    printf("app reply:");
    for(uint8_t i= 0; i<6; i++)
    {
        printf("%.2x ",receive_buff[i]);
    }
    if(receive_buff[0] != 0x55 && receive_buff[5] != 0xaa)
    {
        perror("app no action");
        fclose(bin);
        close_serial(serial_port);
        return APP_NO_ACTION;
    }
    printf("\nnow is in app.\nplease connect to app com\n");
}
