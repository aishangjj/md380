/****************************************************************
文件功能：和电机控制相关的常量定义
文件版本:
更新日期：

****************************************************************/

#include "SystemDefine.h"
#include "MotorDefine.h"

#ifdef DSP_CLOCK60                                          // 主频60M时的死区设定*/
Uint const gDeadBandTable[14] = 
{
//	12  13  14  15  16  17  18  19  20  21  22  23  24   25
	192,192,192,192,240,240,240,288,288,288,288,288,288, 288 
};

Uint const gDeadCompTable[14] = 
{
//	12  13  14  15  16  17  18  19  20  21  22  23  24   25
	78, 87, 87, 87, 117,117,117,129,129,129,135,135,120, 120 	
};
#endif
#ifdef DSP_CLOCK100                                         // 主频100M时的死区设定*/
Uint const gDeadBandTable[14] = 
{
//	12   13  14   15  16   17   18  19   20  21   22  23   24    25
	320,320,320,320,400,400,400,480,480,480,480,480,480, 480
//    320,320,320,320,400,400,480,480,480,480,480,480,480, 480
};

Uint const gDeadCompTable[14] = 
{
//	12    13    14   15    16    17   18    19    20    21   22    23    24   25
	130, 130, 130, 130, 196, 196, 196, 216, 216, 216, 226, 226, 200,200
//	130, 130, 130, 130, 196, 196, 216, 216, 216, 216, 226, 226, 200,126	
};
#endif

/***********************************************************************
变频器额定电流表:机型小于等于21，小数点2位; 大于21，小数点1位

注意:增加一个机型，首先在相应电流表中添加该机型对应的电流值，同时修改数组长度
然后在电压等级相关信息表中，修改最小机型，最大机型范围。
************************************************************************/
Uint const gInvCurrentTable220S[8] =         // 220V单相
{
	230,   230,   230,   400,                // 0~3    
	700,   960,   1700,  2500                // 4~7
};

Uint const gInvCurrentTable380T[27] =         // 380V,480V
{
    210,   380,   510,   900,  1300,	     //8~12  
    1700,  2500,  3200,  3700, 4500,	     //13~17
    6000,  7500,  9000,	 11000,	             //18~21 以上电流数据包含两个小数点
    
    1520,  1760,  2100,  2530, 3040,         //22~26 以下电流数据包含一个小数点
    3770,  4260,  4650,  5200, 5850,         //27~31
    6500,  7250,  8200                       //32~34
};
Uint const gInvCurrentTable690T[33] =
{
	80,    160,   240,   400,  600,          //8~12
	800,   1200,  1600,  2000, 2400,         //13~17
	3300,  4100,  5000,  6200,               //18~21 以上电流数据包含两个小数点
	
	850,   1020,  1250,  1500, 1750,         //22~26 以下电流数据包含一个小数点
	2150,  2450,  2600,  2990, 3300,         //27~31
	3740,  4100,  4650,  5500, 5900,         //32~36
	6000,  7000,  8000,  9000                //37~40
};
Uint const gInvCurrentTable1140T[19] =
{
	2500,    3000,   3700,   5000,  590,          //19~23
	700,   910,  1170,  1300, 1480,         //24~28
	1570,  1810,  2000,  2260,               //29~32 以上电流数据包含两个小数点
	
	2480,   2810,  3330,  3480, 4000,         //33~37 以下电流数据包含一个小数点
};
Uint const gInvTypeTable380To220T[16] =      //三相220和改制的三相380之间的机型对应关系
{
   
    8,   9,   10,   11,  13,         //7~11       // 中隧非标，机型放大一等
    14,  15,  17,   18,  19,         //12~16
    //20,  21,  22,   23,  24,         //17~21    
    20,  21,  22,   23,  25,         //17~21      // 中隧非标，机型放大一等
    26                               // 22
};
Uint const gInvCurrentTable220T[16] =       // 380T -> 220T
{
    210,   380,   510,   900,  1300,	     // 8、9、10、11、12    -> 7~11
    2500,  3200,  4500,  6000, 7500,         // 14、15、17、18、19  -> 12~16
    9000,  11000,                            // 20、 21             -> 17~18    (0.01A)
    1520,  1760,  2100,  3040                // 22、23、24、26      -> 19~22    (0.1 A)
};


// 28035 的AD范围为0-3.3V, 所以此处需要修改
/******************************************************************************
存放与变频器电压等级相关信息，分别为额定电压，欠压点，过压点，PWM发波的基值电压，
最小机型，最大机型,输入缺相保护的起始机型(对应G型机),电压采样系数
电压采样，3.3V对应的电压:
220v为500
380V为1000
480V为1022.7
690V为1806
******************************************************************************/
//                                         Vn    uvDc   ovDc    Udc_b  minType maxType ... CurSampCoff
Uint  const gInvVoltageInfo220S[9] = {220, 2000, 4000,  3111, 0, 7,  99, 4655, 29};  //220V单相,无输入缺相保护
Uint  const gInvVoltageInfo220T[9] = {220, 2000, 4000,  3111, 7, 22, 14, 4655, 29};  //220V三相
Uint  const gInvVoltageInfo380T[9] = {380, 3500, 8100,  5374, 8, 34, 16, 9309, 53};
Uint  const gInvVoltageInfo480T[9] = {480, 3500, 8900,  6500, 8, 34, 16, 10472,62};
Uint  const gInvVoltageInfo690T[9] = {690, 6500, 13500, 9757, 8, 40, 5,  16812,88};  //全系列输入缺相保护
Uint  const gInvVoltageInfo1140T[9] = {1140, 13500, 20000, 16120, 19, 38, 19, 26420,138};
/******************************************************************************
1.2，1.4，1.6，1.8次方V/f曲线
******************************************************************************/
Uint const gExponentVf[4][129] =  // 1.1~~1.9次方曲线，将0~~1均分为128段
{

    {            // 1.2
        0   ,
        12  ,
        28  ,
        45  ,
        64  ,
        84  ,
        104 ,
        125 ,
        147 ,
        169 ,
        192 ,
        215 ,
        239 ,
        263 ,
        288 ,
        313 ,
        338 ,
        363 ,
        389 ,
        415 ,
        442 ,
        468 ,
        495 ,
        522 ,
        549 ,
        577 ,
        605 ,
        633 ,
        661 ,
        690 ,
        718 ,
        747 ,
        776 ,
        805 ,
        835 ,
        864 ,
        894 ,
        924 ,
        954 ,
        984 ,
        1014,
        1045,
        1075,
        1106,
        1137,
        1168,
        1200,
        1231,
        1262,
        1294,
        1326,
        1358,
        1390,
        1422,
        1454,
        1486,
        1519,
        1552,
        1584,
        1617,
        1650,
        1683,
        1716,
        1750,
        1783,
        1816,
        1850,
        1884,
        1917,
        1951,
        1985,
        2019,
        2054,
        2088,
        2122,
        2157,
        2191,
        2226,
        2261,
        2295,
        2330,
        2365,
        2400,
        2436,
        2471,
        2506,
        2542,
        2577,
        2613,
        2648,
        2684,
        2720,
        2756,
        2792,
        2828,
        2864,
        2900,
        2937,
        2973,
        3009,
        3046,
        3082,
        3119,
        3156,
        3193,
        3229,
        3266,
        3303,
        3341,
        3378,
        3415,
        3452,
        3490,
        3527,
        3564,
        3602,
        3640,
        3677,
        3715,
        3753,
        3791,
        3829,
        3867,
        3905,
        3943,
        3981,
        4019,
        4058,
        4096,
    },

    {           // 1.4
        0   ,
        5   ,
        12  ,
        21  ,
        32  ,
        44  ,
        56  ,
        70  ,
        84  ,
        100 ,
        115 ,
        132 ,
        149 ,
        167 ,
        185 ,
        204 ,
        223 ,
        243 ,
        263 ,
        283 ,
        305 ,
        326 ,
        348 ,
        370 ,
        393 ,
        416 ,
        440 ,
        464 ,
        488 ,
        512 ,
        537 ,
        563 ,
        588 ,
        614 ,
        640 ,
        667 ,
        694 ,
        721 ,
        748 ,
        776 ,
        804 ,
        832 ,
        861 ,
        889 ,
        919 ,
        948 ,
        978 ,
        1007,
        1038,
        1068,
        1099,
        1129,
        1161,
        1192,
        1224,
        1255,
        1287,
        1320,
        1352,
        1385,
        1418,
        1451,
        1485,
        1518,
        1552,
        1586,
        1620,
        1655,
        1690,
        1724,
        1760,
        1795,
        1830,
        1866,
        1902,
        1938,
        1974,
        2011,
        2047,
        2084,
        2121,
        2158,
        2196,
        2233,
        2271,
        2309,
        2347,
        2386,
        2424,
        2463,
        2502,
        2541,
        2580,
        2619,
        2659,
        2698,
        2738,
        2778,
        2818,
        2859,
        2899,
        2940,
        2981,
        3022,
        3063,
        3104,
        3146,
        3187,
        3229,
        3271,
        3313,
        3355,
        3398,
        3440,
        3483,
        3526,
        3569,
        3612,
        3655,
        3699,
        3742,
        3786,
        3830,
        3874,
        3918,
        3962,
        4007,
        4051,
        4096,
    },
    
    {               // 1.6
        0   , 
        2   , 
        5   , 
        10  , 
        16  , 
        23  , 
        31  , 
        39  , 
        49  , 
        59  , 
        69  , 
        81  , 
        93  , 
        105 , 
        119 , 
        133 , 
        147 , 
        162 , 
        178 , 
        194 , 
        210 , 
        227 , 
        245 , 
        263 , 
        281 , 
        300 , 
        320 , 
        340 , 
        360 , 
        381 , 
        402 , 
        424 , 
        446 , 
        468 , 
        491 , 
        514 , 
        538 , 
        562 , 
        587 , 
        612 , 
        637 , 
        663 , 
        689 , 
        715 , 
        742 , 
        769 , 
        797 , 
        824 , 
        853 , 
        881 , 
        910 , 
        940 , 
        969 , 
        999 , 
        1030, 
        1060, 
        1091, 
        1123, 
        1154, 
        1186, 
        1219, 
        1251, 
        1284, 
        1318, 
        1351, 
        1385, 
        1419, 
        1454, 
        1489, 
        1524, 
        1559, 
        1595, 
        1631, 
        1668, 
        1704, 
        1741, 
        1779, 
        1816, 
        1854, 
        1892, 
        1931, 
        1970, 
        2009, 
        2048, 
        2088, 
        2128, 
        2168, 
        2208, 
        2249, 
        2290, 
        2331, 
        2373, 
        2415, 
        2457, 
        2499, 
        2542, 
        2585, 
        2628, 
        2672, 
        2715, 
        2759, 
        2804, 
        2848, 
        2893, 
        2938, 
        2984, 
        3029, 
        3075, 
        3121, 
        3167, 
        3214, 
        3261, 
        3308, 
        3355, 
        3403, 
        3451, 
        3499, 
        3547, 
        3596, 
        3645, 
        3694, 
        3744, 
        3793, 
        3843, 
        3893, 
        3943, 
        3994, 
        4045, 
        4096,
    },

    {           // 1.8
        0   ,
        1   ,
        2   ,
        5   ,
        8   ,
        12  ,
        17  ,
        22  ,
        28  ,
        34  ,
        42  ,
        49  ,
        58  ,
        67  ,
        76  ,
        86  ,
        97  ,
        108 ,
        120 ,
        132 ,
        145 ,
        158 ,
        172 ,
        186 ,
        201 ,
        217 ,
        232 ,
        249 ,
        266 ,
        283 ,
        301 ,
        319 ,
        338 ,
        357 ,
        377 ,
        397 ,
        418 ,
        439 ,
        460 ,
        482 ,
        505 ,
        528 ,
        551 ,
        575 ,
        599 ,
        624 ,
        649 ,
        675 ,
        701 ,
        727 ,
        754 ,
        782 ,
        809 ,
        838 ,
        866 ,
        895 ,
        925 ,
        955 ,
        985 ,
        1016,
        1047,
        1079,
        1111,
        1143,
        1176,
        1210,
        1243,
        1277,
        1312,
        1347,
        1382,
        1418,
        1454,
        1491,
        1528,
        1565,
        1603,
        1641,
        1679,
        1718,
        1758,
        1797,
        1838,
        1878,
        1919,
        1960,
        2002,
        2044,
        2087,
        2130,
        2173,
        2216,
        2260,
        2305,
        2350,
        2395,
        2440,
        2486,
        2533,
        2579,
        2627,
        2674,
        2722,
        2770,
        2819,
        2868,
        2917,
        2967,
        3017,
        3067,
        3118,
        3169,
        3221,
        3273,
        3325,
        3378,
        3431,
        3484,
        3538,
        3592,         
        3647,         
        3702,         
        3757,         
        3813,         
        3868,         
        3925,         
        3982,       
        4039,       
        4096,
    }
};    

