#ifndef _EF_PRINT_MENUS_CFG_H_
#define _EF_PRINT_MENUS_CFG_H_


#define SCREEN_UART_BAUDRATE      9600UL

/* Frame Header */
#define SCREEN_FRAME_HEADER_H     0x5A
#define SCREEN_FRAME_HEADER_L     0xA5
/* Frame Used Command */
#define READ_DATA_COMMAND         0x83
#define WRITE_DATA_COMMAND        0x82
#define READ_REGISTER_COMMAND     0x81
#define SEND_PHOTO_COMMAND        0x80
#define GET_RTC_LENGTH            0x0A


/* System Button Address, its Value represent the Button Address */
enum ButtonAddresses
{
    ESCAPE_STATE               = 0,

    START_OPERATION_BUTTON     = 0xA000,
    STOP_OPERATION_BUTTON      = 0xA002,

    SELECT_ANALYSIS_BUTTON     = 0xA004,
    CLEAR_ANALYSIS_BUTTON      = 0xA005,

    ANALYSIS_PATCH_BUTTON      = 0xA10c,
    ANALYSIS_SYSTEM_BUTTON     = 0xA10d,

    ANALYSIS_STATRT_BUTTON     = 0xA200,
    ANALYSIS_STOP_BUTTON       = 0xA201,

    CALIBRATION_CHOOSE_BUTTON  = 0x0004,

    CONFIRM_EDIT_BUTTON        = 0xA00f,
    CONFIRM_SHOW_BUTTON        = 0x00AA,
    SAVE_TOLERANACE_BUTTON     = 0xA006,
    SELECT_EDIT_BUTTON         = 0x0007,
    SELECT_SHOW_BUTTON         = 0x0008,
    BACK_FROM_EDIT_BUTTON      = 0x00A6,
    BACK_FROM_SHOW_BUTTON      = 0x00A7,
    BACK_FROM_ANALYSIS_BUTTON  = 0x00A1,
    SHOW_LIST_HIGH_BUTTON      = 0xA008,
    SHOW_LIST_LOW_BUTTON       = 0xA007,

    EDIT_LIST_LOW_BUTTON       = 0xA108,
    EDIT_LIST_HIGH_BUTTON      = 0xA107,

    PRAM_CAT_HIGH_BUTTON       = 0xA105,
    PRAM_CAT_LOW_BUTTON        = 0xA106,
    PRAMETER_MAX_TOL_BUTTON    = 0xA009,
    PRAMETER_CAT_WEIGHT_BUTTON = 0xA00a,
    PRAM_BARCODE_LIMIT_BUTTON  = 0xA00b,
    PRAMETER_SCALE_LIMIT_BUTTON= 0xA00c,
    SAVE_SCALE_LIMIT_BUTTON    = 0xA00e,
    BACK_PARAMETERS_BUTTON     = 0xA100,
    SAVE_BARCODE_LIMIT_BUTTON  = 0xA101,
    SAVE_MAX_TOL_BUTTON        = 0xA103,
    SAVE_CAT_WEIGHT_BUTTON     = 0xA104,

    CONFIRM_PASSWORD_BUTTON    = 0xA109,
    CHANGE_PASSWORD_BUTTON     = 0xA10a,
    SAVE_PASSWORD_BUTTON       = 0xA10b,

    GET_RTC_VALUES             = 0x2007,
};



/* (_V) means Data Value Address , its Value represent the Value Address*/
enum DataDisplayAddresses
{
    /* to Send these Values not receiving */
    OPERATION_BARCODE_WEIGHT_V = 0xB130,
    OPERATION_SCALE_WEIGHT_V   = 0xB134,
    OPERATION_DIFFERENCE_V     = 0xB138,

    /* to Send these Values not receiving */
    CORRECT_PACKETS_NUM_V      = 0xB100,
    UNDERLOAD_PACKETS_NUM_V    = 0xB108,
    OVERLOAD_PACKETS_NUM_V     = 0xB104,

    ANALYSIS_HOURS_V           = 0xB400,
    ANALYSIS_MINUTES_V         = 0xB404,
    /* to send Zero and [Receive] this Value as Response */
    ENTER_SHOW_CAT_NUM_V       = 0xB11C,
    /* to Send these Values not receiving */
    SHOW_CATEGORY_NUM_V        = 0xB10C,
    SHOW_CATEGORY_WEIGHT_V     = 0xB110,
    SHOW_POSITIVE_TOLERANCE_V  = 0xB114,
    SHOW_NEGATIVE_TOLERANCE_V  = 0xB118,

    /* to send Zero and [Receive] this Value  as Response for System request */
    ENTER_EDIT_CAT_NUM_V       = 0xB120,
    /* to Send this Value */
    EDIT_CATEGORY_NUM_V        = 0xB124,
    EDIT_CATEGORY_WEIGHT_V     = 0xB13C,
    /* to send zeros and [Receive] these Values  as Response for System request */
    EDIT_POSITIVE_TOLERANCE_V  = 0xB128,
    EDIT_NEGATIVE_TOLERANCE_V  = 0xB12C,
    /* to Send these Error Values */
    ERROR_CAT_MIN_NUMBER_V     = 0xB300,
    ERROR_CAT_MAX_NUMBER_V     = 0xB304,
    ERROR_SCALE_MIN_WEIGHT_V   = 0xB308,
    ERROR_SCALE_MAX_WEIGHT_V   = 0xB30C,
    ERROR_BARCODE_MIN_WEIGHT_V = 0xB318,
    ERROR_BARCODE_MAX_WEIGHT_V = 0xB31C,
    ERROR_MAX_TOLERANCE_V      = 0xB320,
    ERROR_MAX_CAT_WEIGHT_V     = 0xb324,
    /* Parameters */
    PARAMETER_SCALE_MAX_V      = 0xb33c,
    PARAMETER_BARCODE_MAX_V    = 0xb334,
    PARAMETER_TOL_MAX_V        = 0xb32c,
    PARAMETER_CAT_WEIGHT_V     = 0xb328,
    PARAMETER_CAT_NUM_V        = 0xb330,
    /* Password */
    ENTER_PASSWORD_V           = 0xB340,
    OLD_PASSWORD_V             = 0xB344,
    NEW_PASSWORD_V             = 0xb348,
};



/* System Photos Addresses , its Value represent the Photo ID */
enum PhotosAddresses {
    MAIN_SELECTION_PHOTO       = 6,
    OPERATION_PHOTO            = 15,
    ENTER_EDIT_CAT_NUM_PHOTO   = 21,
    EDIT_POSITIVE_NEG_PHOTO    = 50,
    ENTER_SHOW_CAT_NUM_PHOTO   = 43,
    SHOW_TOLERANCE_PHOTO       = 64,
    DATA_IS_SAVED_PHOTO        = 41,
    START_OPERATION_PHOTO      = 10,
    ACCEPT_OPERATION_PHOTO     = 15,
    OVERLOAD_OPERATION_PHOTO   = 13,
    UNDERLOAD_OPERATION_PHOTO  = 14,
    CALIBRATION_CHOOSE_PHOTO   = 48,

    /*----- analysis Photos -------*/
    ANALYSIS_SYSTEM            = 61,
    ANALYSIS_CHOOSE            = 93,
    ANALYSIS_PATCH             = 92,

    /*----- Errors Photos -------*/
    ERROR_ENTER_CAT_NUM_PHOTO  = 34,
    ERROR_MAX_TOLERANCE_PHOTO  = 40,
    ERROR_SCALE_WEIGHT_PHOTO   = 37,
    ERROR_STABLE_SCALE_PHOTO   = 36,
    ERROR_PROXIMITY_PHOTO      = 33,
    ERROR_BARCODE_WEIGHT_PHOTO = 38,
    ERROR_WATING_BARCODE_PHOTO = 35,
    ERROR_UNSAVED_DATA_PHOTO   = 39,
    ERROR_CAT_WEIGHT_PHOTO     = 52,

    /*------ Parameters ---------*/
    PARAMETERS_CHOOSE_PHOTO    = 46,
    PARAMETER_SCALE_PHOTO      = 44,
    PARAMETER_BARCODE_PHOTO    = 45,
    PARAMETER_TOLERANCE_PHOTO  = 47,
    PARAMETER_CAT_WEIGHT_PHOTO = 51,

    /*------ Password ---------*/
    ENTER_PASSWORD_PHOTO       = 87,
    CHANGE_PASSWORD_PHOTO      = 88,
    CORRECT_PASSWORD_PHOTO     = 90,
    WRONG_PASSWORD_PHOTO       = 91,

};


/* used to toggle between Analysis Print Cat Numbers and Print Weights */
enum AnalysisPrintToggleEnum
{
    ANALYSIS_PRINT_NUMBERS,
    ANALYSIS_PRINT_WEIGHTS
};


#endif
