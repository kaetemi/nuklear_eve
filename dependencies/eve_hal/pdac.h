//#define PANL_35					1
//#define PANL_70					2
//#define PANL_TYPE 				PANL_70
#include "EVE_IntTypes.h"

#define PANL_DEBUG 0

#if (PANL_TYPE == PANL_70 || PANL_TYPE == PANL_50)
#define PANL_CENTERX			400
#define PANL_CENTERY			220
#if(PANL_TYPE == PANL_50)
#define PANL50_SCALE_TO_PANL70	1
#endif
#elif PANL_TYPE == PANL_35
#define PANL_CENTERX		160
#define PANL_CENTERY		220
#endif

#define FW_VSN			"1.1.10"
#define HW_VSN			"1.0"
#define SERIAL_NO		"12345678ABCD"
#define CUSTOM_STR		"Factory default firmware"

#define PDAC_THRESHOLD_1_DEFAULT		0xff
#define PDAC_THRESHOLD_2_DEFAULT		0xff


#define SOS0				0
#define SOS1				1
#define SOS2				2
#define SOS_TIME_OUT		1800000L
#define DLOG_DISPLAY_INFO	0
#define DLOG_ERROR_INFO		1

#define DISPWIDTH			0
#define DISPHEIGHT          1
#define DISPHCYCLE          2
#define DISPHOFFSET         3
#define DISPHSYNC0          4
#define DISPHSYNC1          5
#define DISPVCYCLE          6
#define DISPVOFFSET         7
#define DISPVSYNC0          8
#define DISPVSYNC1          9
#define DISPPCLK            10
#define DISPSWIZZLE         11
#define DISPPCLKPOL         12
#define DISPCSPREAD         13
#define DISPDITHER          14

#define EVE_BOOTUP_ERROR							0xE001
#define EVE_DECODING_ERROR							0xE002
#define PDAC_CONFIG_FILE_PARSING_ERROR				0xE003
#define PDAC_FILE_MISSING							0xE004
#define PDAC_FILE_CORRUPTED							0xE005
#define PDAC_APP_NOT_FOUND							0xE006
#define PDAC_UNKNOWN_ERROR							0xFFFF
#define PDAC_ERROR_MESSAGE_LENGTH					252



#define PDAC_SKIN_CONFIG 		"Config/skin.json"
#define PDAC_DEFAULT_CONFIG		"default.json"
#define PDAC_LOGO_CONFIG		"logo.json"
#define PDAC_ICONS_PATH			"Config/icons"
#define PDAC_LOGO				"logo.png"
#define PDAC_TIME_BG			"Config/data/timebg.png"
#define PDAC_FONTS				"Config/fonts"
#define PDAC_BACKGROUND			"Config/bg"
#define PDAC_CONFIG				"Config"
#define PDAC_ROOT_FOLDER		"Config/root"
#define PDAC_WIDGETS			"Config/widgets"
#define PDAC_WARNING			"warning.png"
#define PDAC_OK					"ok.png"

#define PDAC_LABEL_SIZE					32
#define PDAC_FILE_NAME_SIZE				16
#define PDAC_FILE_PATH_MAX				64
#define PDAC_MENU_ITEMS					5
#define PDAC_UID_LENGTH					64
#define PDAC_IDLE_SCREEN_MESSAGE_SIZE	64
#define PDAC_MAX_SCREENS				3
#define PDAC_MAX_ICONS					64
#define PDAC_FONT_HEIGHT				40
#define PDAC_TILE_LABEL_SIZE			PDAC_LABEL_SIZE
#define GESTURE_FREQUENCY 				40
#define GESTURE_EX_TIME					20 //(1000/GESTURE_FREQUENCY)
//#define PDAC_SCREEN_TIME_OUT			10000l
#define PDAC_UPDATE_DISPLAY				0x80
#define PDAC_TILE_LABLE_DOTS			3
#define PDAC_TILE_LABLE_DOT_LOC			(PDAC_TILE_LABEL_SIZE - PDAC_TILE_LABLE_DOTS - 1)
/**
 * TOUCH TAGS
 */

#define PDAC_IDLE_TAG				1
#define PDAC_BACK_TAG				2
#define PDAC_HOME_TAG				3
#define PDAC_FORWARD_TAG			4
#define PDAC_BACKWARD_TAG			5
#define	PDAC_OK_TAG					6
#define	PDAC_MENU_TAG				101
#define PDAC_MENU_TAG_MAX			108

#define MENU_CONTROL_TILE_BUTTON	1

/**
 * FT81x Bitmap handles
 */
#define FONT_HANDLE					16
#define FONT_HANDLE_1				17
#define FONT_HANDLE_2				18
#define ICON_HANDLE			 		19
#define BG_HANDLE					20
#define GRADIENT_HANDLE				21
#define WEATHER_HANDLE				22
#define LOGO_HANDLE					23
#define TIMEBG_HANDLE				24
#define PDAC_NOTIFICATION_HANDLE	25

#define FONT_HANDLE_3				28	// dont change
#define FONT_HANDLE_4				27	// dont change
#define FONT_HANDLE_5				29	// dont change

#define FONT_ADDRESS				0
#define FONT_ADDRESS_1				110*1024L
#define FONT_ADDRESS_2				170*1024L
#define ICON_ADDRESS				240*1024L

#define BG_ADDRESS					480*1024L
#define WEATHER_ADDRESS				668*1024L
#define GRADIENT_ADDRESS			719*1024L
#define LOGO_ADDRESS				720*1024L
#define TIMEBG_ADDRESS				770*1024L
#define TIMEBG_ADDRESS				770*1024L
#define NOTIFICATION_ADDRESS		790*1024L

#define PDAC_ZONES						0x0001
#define PDAC_SCENES						0x0002
#define PDAC_FAVORITES					0x0003


#define PDAC_CLOCK_ICON						0x0004
#define PDAC_BACKWARD_BUTTON				0x0008
#define PDAC_FORWARD_BUTTON					0x0009
#define PDAC_BACK_BUTTON					0x000A
#define PDAC_HOME_BUTTON					0x000B
#define PDAC_ONLINE_ICON					0x000C

#define  IHOME_DISP_PANL70 					0x04C0
#define  IHOME_DISP_PANL35 					0x04C1
#define  IHOME_DISP_PANL50 					0x04C2

#if PANL_TYPE == PANL_70
#define IHOME_DISPLAY_OBJ			IHOME_DISP_PANL70
#define PANL_MODEL					"PanL70"
#elif PANL_TYPE == PANL_50
#define IHOME_DISPLAY_OBJ			IHOME_DISP_PANL50
#define PANL_MODEL					"PanL50"
#else
#define IHOME_DISPLAY_OBJ			IHOME_DISP_PANL35
#define PANL_MODEL					"PanL35"
#endif
#define ONLINE_STATUS_POINT_SIZE			10

typedef struct{
	char label_font[PDAC_LABEL_SIZE];
	char datetime_font[PDAC_LABEL_SIZE];
	char weather_font[PDAC_LABEL_SIZE];
}PDACFonts_t;

typedef struct{
	int width;
	int height;
}PDACSize_t;

typedef struct{
	char name[PDAC_FILE_NAME_SIZE];
	PDACSize_t size_t;
}PDACWallPaper_t;

typedef struct{
	int landscape_space;
	int vertical_space;
	int columns;
	int rows;
	PDACSize_t size_t;
}PDACTiles_t;

typedef struct{
	union{
		uint32_t rgb;
		uint8_t r;
		uint8_t g;
		uint8_t b;
	}bgcolor;
	union{
		uint32_t rgb;
		uint8_t r;
		uint8_t g;
		uint8_t b;
	}fgcolor;
}PDACColors_t;

typedef struct{
	PDACSize_t size_t;
}PDACICons_t;

typedef struct{
	PDACFonts_t fonts;
	PDACWallPaper_t wallpaper;
	PDACTiles_t tile;
	PDACColors_t colors;
	PDACICons_t icon;
	int timeout;
}PDACSkinContexts_t;

typedef struct{
	PDACSize_t size_t;
	PDACColors_t colors;
}PDACLogoContexts_t;

typedef struct{
	//uint8_t number_of_members;
	int type;
	int id;
	int iconid;
	char uid[PDAC_UID_LENGTH];
	char name[PDAC_LABEL_SIZE];
}PDACAccessoryContexts_t;

typedef struct{
	uint8_t index;
	char item[PDAC_MENU_ITEMS][PDAC_FILE_PATH_MAX];
}PDACMenuContexts_t;



typedef enum{
	pdac_none = 0,
	pdac_idle,
	pdac_menu,
	pdac_app,
	pdac_error,
	pdac_unconfigured,
	pdac_configure,
}PDACScreens_t;

typedef struct{
	uint8_t brightness;
}Dimlamp_t;

typedef struct{
	int brightness;
	int hue;
	int saturation;
}Colorlamp_t;

typedef struct{
	bool open_close;
	bool stop;
	bool function_1;
	bool function_2;
}Curtain_t;


typedef struct{
	int instance_id;
	bool onoff;
	Dimlamp_t dimlamp;
	Colorlamp_t colorlamp;
	Curtain_t curtain;
	bool unreachable;
	bool getack;
}PanLAccessories_t;


typedef struct{
	uint16_t error_code;
	uint8_t message[PDAC_ERROR_MESSAGE_LENGTH];
}PDACFailSafeErrorCode_t;

PanLAccessories_t panl_accessories[128];



typedef PDACSize_t PDACDisplayResolution_t;
typedef uint16_t IconsTable_t[PDAC_MAX_ICONS];
typedef uint32_t PDACTimeOut;
typedef uint32_t PDACGestureTicks;
typedef char PDACIdleScreenMessage[PDAC_IDLE_SCREEN_MESSAGE_SIZE];
typedef uint8_t PDACMaxPages_t;
PDACDisplayResolution_t PanLDisplay;
uint16_t pdac_threshold_1;
uint16_t pdac_threshold_2;



#define SPECIAL_CHARACTER_INDEX		10
#define SPECIAL_CHARACTER_COLON		10
#define SPECIAL_CHARACTER_DEGREE	11
#define SPECIAL_CHARACTER_C			12

//int pdac_displayInfo(PanLDisplayPrp_t *ptr);
int pdac_initialize(void);
