#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

#define     MESSAGE_NULL                        0
#define     MESSAGE_NAMESERVER_WHOIS            1
#define     MESSAGE_NAMESERVER_REGISTER         2
#define     MESSAGE_NAMESERVER_RESPONSE         3
#define     MESSAGE_RPS                         4
#define     MESSAGE_RANDOM_BYTE                 5
#define     MESSAGE_TYPE_64_BYTE                6
#define     MESSAGE_CLOCKSERVER_DELAY_BY        7
#define     MESSAGE_CLOCKSERVER_NOTIFY_TICK     8
#define     MESSAGE_CLOCKSERVER_WAKE            9
#define     MESSAGE_CLOCKSERVER_KILL            10
#define     MESSAGE_CLOCKSERVER_GET_TIME        11
#define     MESSAGE_IO_GETC                     12
#define     MESSAGE_IO_PUTC                     13
#define     DRIVER_MESSAGE_RCV_CHAR             14
#define     DRIVER_MESSAGE_TX_CHAR              15
#define     MESSAGE_SIG_KILL                    16
#define     MESSAGE_COURIER                     17
#define     MESSAGE_IO_PUTSTR                   18
#define     DRIVER_MESSAGE_RCV_SENSORS          19
#define     DRIVER_MESSAGE_TX_TRAIN             20
#define     DRIVER_MESSAGE_TX_TRAIN_MULTI_BYTE  21
#define     MESSAGE_TRAIN_SET_SPEED             22
#define     MESSAGE_TRAIN_REVERSE               23
#define     MESSAGE_NOTIFY                      24
#define     MESSAGE_CLOCKSERVER_NOTIFY_LORES    25
#define     MESSAGE_CLOCKSERVER_LONG_DELAY_BY   26
#define     MESSAGE_TRAIN_SOLENOID_OFF          0x20 // 32
#define     MESSAGE_TRAIN_SWITCH_STRAIGHT       0x21 // 33
#define     MESSAGE_TRAIN_SWITCH_CURVED         0x22 // 34
#define     MESSAGE_TRAIN_GO                    0x60 // 96
#define     MESSAGE_TRAIN_STOP                  0x61 // 97
#define     MESSAGE_TRAIN_SENSOR_RESET_OFF      0x80 // 128
#define     MESSAGE_TRAIN_GET_SENSOR_ABCDE      0x85 // 133
#define     MESSAGE_TRAIN_GET_SENSOR            0xC0 // (192, 224) is reserved
#define     MESSAGE_TRAIN_GET_SENSOR_A          0xC1
#define     MESSAGE_TRAIN_GET_SENSOR_B          0xC2
#define     MESSAGE_TRAIN_GET_SENSOR_C          0xC3
#define     MESSAGE_TRAIN_GET_SENSOR_D          0xC4
#define     MESSAGE_TRAIN_GET_SENSOR_E          0xC5
#define     MESSAGE_TRAIN_GET_SENSOR_F          0xC6
#endif

