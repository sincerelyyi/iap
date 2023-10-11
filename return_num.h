#ifndef _RETURN_NUM_
#define _RETURN_NUM_

#define SUCCESS                         0   // 成功更新，并顺利退出
#define NO_SERIAL_PORT                  1   // 立即可以重新使用iap
#define SET_SERIAL_ERROR                2   // 立即可以重新使用iap
#define SEND_ERROR                      3   // 复位后也许不能使用iap，必须用u盘更新一次app;复位前能使用iap
#define SOLDWARE_OUTOFRANGE             4   // 复位后也许不能使用iap，必须用u盘更新一次app;复位前能使用iap
#define PROGRAM_FLASH_ERROR             5   // 复位后也许不能使用iap，必须用u盘更新一次app;复位前能使用iap
#define ERASE_SECTOR_ERROR              6   // 复位后也许不能使用iap，必须用u盘更新一次app;复位前能使用iap
#define REPLY_UNKNOW                    7   // 复位后也许不能使用iap，必须用u盘更新一次app;复位前能使用iap
#define REPLY_FORMATE_ERROR             8   // 复位后也许不能使用iap，必须用u盘更新一次app;复位前能使用iap
#define TOO_LESS_ARG                    9   // 立即可以重新使用iap
#define UNVALID_BIN                     10  // 立即可以重新使用iap
#define OPEN_FILE_ERROR                 11  // 立即可以重新使用iap
#define IAP_NO_ACTION                   12  // 复位后还可以使用iap
#define APP_NO_ACTION                   13  // 复位后还可以使用iap
#endif
