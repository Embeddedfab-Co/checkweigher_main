#ifndef _EF_UI_CFG_H_
#define _EF_UI_CFG_H_


/* this method define the way to know the Barcode was related to which Category for example:
 * from 800  to 1000 gram ===> Cat#2
 * From 1000 to 2000 gram ===> Cat#3
 * and so on
 * */
#define SECOND_METHOD_CALCULATION

#define GET_SCALE_WITHOUT_STABLE         FALSE         /* To get the scale weight if stable only , make it false */

#define EXIT_FROM_CALIBRATION            5             /* define Exit State in the Calibration Mode */
#define MAX_CATEGORY_NUMBER              21            /* Max Number of Avaliable Tolerance weigth Categories */
#define MIN_CATEGORY_NUMBER              1             /* Min Number of Avaliable Tolerance weigth Categories */

#define CATEGORIES_ADD_EEPROM            4             /* tolerance positive and negative , 21 * 4 = 84 place in memory */
#define TOLERANCE_LAST_ADD_EEPROM        340           /* this Micros wasn't used ,but I defined it here to note that this Addresses are used */

#define CORRECT_PACKAGE_ADD_EEPROM       400           /* The place in EEPROM to Save the Correct Package Number */
#define UNDERLOAD_PACKAGE_ADD_EEPROM     408           /* The place in EEPROM to Save the UnderLoad Package Number */
#define OVERLOAD_PACKAGE_ADD_EEPROM      416           /* The place in EEPROM to Save the OverLoad Package Number */

#define CORRECT_WEIGHT_ADD_EEPROM        424           /* The place in EEPROM to Save the Correct Package Number */
#define UNDERLOAD_WEIGHT_ADD_EEPROM      432           /* The place in EEPROM to Save the UnderLoad Package Number */
#define OVERLOAD_WEIGHT_ADD_EEPROM       440           /* The place in EEPROM to Save the OverLoad Package Number */


#define MAX_CATEGORY_WEIGTH              100000UL       /* MAx Weight for Category Values */
#define MIN_CATEGORY_WEIGTH              0

#define MAX_SCALE_TRIALS                 3             /* MAx Number of Get Stable Weigth Trials */
#ifdef SECOND_METHOD_CALCULATION
#define MAX_WEIGTH                       20000         /* MAx Avaliable Weight for BarCode and Scale */
#else
#define MAX_WEIGTH                       21000         /* MAx Avaliable Weight for BarCode and Scale */
#endif
#define MIN_WEIGTH                       15           /* MIN Number of Get Stable Weigth Trials */

#define CAT_1_NUMBER                     1             /* Category Number 1 (800g) */
#define MAX_TOL_VALUE_800g               200           /* Max Available Tolerance for Cat#1 (800g) in grams */
#define MAX_TOL_VALUE                    1000          /* Max Available Tolerance for All Cat. except Cat#1 in grams */
#define MIN_TOL_VALUE                    10             /* Min Available Tolerance for All Cat in grams */


#define COMPARE_VALUE                    8200          /* if using Barcode stub, to compare scale with fixed Value */

/* Number of Countes is Counted in TimerIsr (every 1ms) to reach 3 hours, to disable Operation if Enabled TRAILS_3_MONTHS  */
#define TIMER_EXPIRED_3_HOURS            10800000UL
 /* Number of Countes is Counted in TimerIsr (every 3hours) to reach 3Months ,to disable Operation if Enabled TRAILS_3_MONTHS  */
#define TIMER_90DAY_PER_3HOURS           720

#define TIMER_EXPIRED_3Months_EEPROM_ADD 444        /* EEPROM ADD to save the 3Month Counter in it to disable Operation if Enabled TRAILS_3_MONTHS */

#define FIRST_RELEASE_FLAG_EEPROM_ADD    448        /* EEPROM ADD to save FirstRelease flag to init Some Varablies when First Realease only */
#define FIRST_RELEASE_FLAG_VALUE         19         /* Value in EEPROM to checked if it not saved --> init Some Varablies when First Realease only and save it */

#define BARCODE_MAX_EEPROM               452
#define TOL_MAX_EEPROM                   456
#define SCALE_MAX_EEPROM                 460

#define SCALE_MAX_EEPROM               460
#define SCALE_MIN_EEPROM               464

#define RX_BUTTON_FRAME_LENGTH           6          /* The Data Frame Lenght of Button Frame */
#define GET_VALUE_FRAME_LENGTH           8          /* The Data Frame Lenght of Value  Frame */

#define BUTTON_VALUE_LENGTH              2          /* The Data Lenght of Button Address Value */
#define DISPLAY_DATA_VALUE_LENGTH        4          /* The Data Lenght of Value  Address Value */
#define SCREEN_MIN_FRAME_LENGTH          9          /* The Min Screen Frame Length */

#define TOLERANCE_DEFAULT                50
#define CAT_WEIGHT_DEFAULT               1000

//#define STUB_SCALE
#define BARCODE_STUB
//#define USING_PROXIMITY
#define USING_BARCODE

#define SCALE_STANDARD_WEIGHT            5000

/* used in Edit tolerance when Save button is pressed , help to get the Two Values Correctly */
enum FlagStatusAtEditTolerance{
  PREPARE_TO_EDIT,
  FIRST_EDITING,
  SECOND_EDITING
};


/* Used in Operation Task States Switch Cases */
enum{
    MENU_WITHOUT_VALUES_STATE,
    GET_ALL_DATA_STATE,
    GET_BARCODE_STATE,
    GET_SCALE_STATE,
    CALCULAION_STATE,
    WAITING_NEW_OPERATION
}vOperationTaskEnum;



/* Used to Check Proximity Found the package or not  */
enum{
    NOT_FOUND_PACKAGE,
    FOUND_PACKAGE
}ProximityStatusEnums;

/* Used to Check if Barcode success in get Value or not */
enum{
    BARCODE_FALSE =FALSE,
    BARCODE_OK =TRUE
}BarcodeStatusEnums;

/* Used to Check if Scale success in get Value or not */
enum{
    SCALE_NOT_STABLE,
    SCALE_STABLE
}ScaleStatusEnums;


/* used in tolerance array */
enum{
    CAT_WEIGHT_ELEMENT,
    POSITIVE_TOL_ELEMENT,
    NEGATIVE_TOL_ELEMENT,
    CAT_STATUS_ELEMENT
}Pos_Neg_Tolenum;


/* used as argument to Print Calculation Function , to defien the result of Packet */
enum {
    UNDERLOAD_PACKET,
    ACCEPT_PACKET,
    OVERLOAD_PACKET,
}OperationResultEnum;

typedef struct {
    U8_t  CategoryStatus;
    U32_t CategoryWeight;
    U32_t CategoryTol_Pos;
    U32_t CategoryTol_Neg;
}CategoryDetailsStr;



typedef struct {
    U32_t Barcode_Max;
    U32_t Scale_Max;
    U32_t Tolerance_Max;
    U32_t CatWeight_Max;
}SystemParametersStr;
#endif
