#ifndef _EF_PRINT_MENUS_CFG_H_
#define _EF_PRINT_MENUS_CFG_H_


#define SCREEN_UART_BAUDRATE      9600UL

#define SCREEN_FRAME_HEADER_H     0x5A
#define SCREEN_FRAME_HEADER_L     0xA5
#define READ_DATA_COMMAND         0x83
#define WRITE_DATA_COMMAND        0x82
#define SEND_PHOTO_COMMAND        0x80


/* System Button Address */
enum ButtonAddresses
{
  ESCAPE_STATE               = 0,
  START_OPERATION_BUTTON     = 0xA000,
  STOP_OPERATION_BUTTON      = 0xA002,
  SELECT_ANALYSIS_BUTTON     = 0xA004,
  CLEAR_ANALYSIS_BUTTON      = 0xA005,
  CONFIRM_EDIT_BUTTON        = 0xA00f,
  CONFIRM_SHOW_BUTTON        = 0x00AA,
  SAVE_TOLERANACE_BUTTON     = 0xA006,
  SELECT_EDIT_BUTTON         = 0x0007,
  SELECT_SHOW_BUTTON         = 0x0008,
  BACK_FROM_EDIT_BUTTON      = 0x00A6,
  BACK_FROM_SHOW_BUTTON      = 0x00A7,
};



/* (_V) means Data Value Address */
enum DataDisplayAddresses
{
  /* to Send these Values not receiving */
  OPERATION_BARCODE_WEIGHT_V = 0xB130,
  OPERATION_SCALE_WEIGHT_V   = 0xB134,
  OPERATION_DIFFERENCE_V     = 0xB138,

  /* to Send these Values not receiving */
  CORRECT_PACKETS_NUM_V      = 0xB100,
  UNDERLOAD_PACKETS_NUM_V    = 0xB104,
  OVERLOAD_PACKETS_NUM_V     = 0xB108,

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
  ERROR_MAX_TOLERANCE_V      = 0xB320
};



/* System Photos Addresses */
enum PhotosAddresses {
  OPERATION_PHOTO            = 9,
  ANALYSIS_PHOTO             = 31,
  ENTER_EDIT_CAT_NUM_PHOTO   = 39,
  EDIT_POSITIVE_NEG_PHOTO    = 40,
  ENTER_SHOW_CAT_NUM_PHOTO   = 38,
  SHOW_TOLERANCE_PHOTO       = 33,
  DATA_IS_SAVED_PHOTO        = 24,
  START_OPERATION_PHOTO      = 6,
  /*----- Errors Photos -------*/
  ERROR_ENTER_CAT_NUM_PHOTO  = 14,
  ERROR_MAX_TOLERANCE_PHOTO  = 27,
  ERROR_SCALE_WEIGHT_PHOTO   = 17,
  ERROR_STABLE_SCALE_PHOTO   = 16,
  ERROR_PROXIMITY_PHOTO      = 13,
  ERROR_BARCODE_WEIGHT_PHOTO = 18,
  ERROR_WATING_BARCODE_PHOTO = 15,
  ERROR_UNSAVED_DATA_PHOTO   = 25,
};





#endif
