#ifndef _SAMPLEAPP_H_
#define _SAMPLEAPP_H_


#define SAMAPP_Lenaface40_SIZE (2769)
#define SAMAPP_Mandrill256_SIZE (14368)
#define SAMAPP_Roboto_BoldCondensed_12_SIZE (19348)

#define SAMAPP_ChineseFont_Metric_SIZE   (148)
#define SAMAPP_ChineseFont_BitmapData_SIZE   (20944)

#define SAMAPP_LENAFACE40_PNG_HEX_DUMP_SIZE (4437)

#define SAMAPP_Tomato_HEX_SIZE (7934)



#if defined(MSVC_PLATFORM) || defined(MSVC_FT800EMU)
/* Compile time switch for enabling sample app api sets - please cross check the same in SampleApp_RawData.cpp file as well */

#define SAMAPP_ENABLE_APIS_SET0
#define SAMAPP_ENABLE_APIS_SET1
#define SAMAPP_ENABLE_APIS_SET2
#define SAMAPP_ENABLE_APIS_SET3
#define SAMAPP_ENABLE_APIS_SET4 
#define SAMAPP_ENABLE_APIS_SET5
#define SAMAPP_ENABLE_APIS_SET6 //Support only multi-touch screen FT801, FT811, FT813 on MSVC and MSVC_FT800EMU
#define SAMAPP_ENABLE_APIS_SET7 //Support only multi-touch screen FT801, FT811, FT813
#define SAMAPP_ENABLE_APIS_SET8 //Suport only FT81x
#define SAMAPP_ENABLE_APIS_SET9 //Support only MSVC and FT81x

#elif defined(ARDUINO_PLATFORM)
/* Compile time switch for enabling sample app api sets - please cross check the same in SampleApp_RawData.cpp file as well */
/* Due to program memory limitation, please choose only one of these test set */

//#define SAMAPP_ENABLE_APIS_SET0_A
#define SAMAPP_ENABLE_APIS_SET0_B
//#define SAMAPP_ENABLE_APIS_SET1_A
//#define SAMAPP_ENABLE_APIS_SET1_B
//#define SAMAPP_ENABLE_APIS_SET2
//#define SAMAPP_ENABLE_APIS_SET3
//#define SAMAPP_ENABLE_APIS_SET4
//#define SAMAPP_ENABLE_APIS_SET7 //support FT801 FT811

	#if defined(SAMAPP_ENABLE_APIS_SET0_A) || defined(SAMAPP_ENABLE_APIS_SET0_B)
		#define SAMAPP_ENABLE_APIS_SET0
	#endif
	#if defined(SAMAPP_ENABLE_APIS_SET1_A) || defined(SAMAPP_ENABLE_APIS_SET1_B)
		#define SAMAPP_ENABLE_APIS_SET1
	#endif


#elif defined( FT900_PLATFORM)
/* Compile time switch for enabling sample app api sets - please cross check the same in SampleApp_RawData.cpp file as well */

#define SAMAPP_ENABLE_APIS_SET0
#define SAMAPP_ENABLE_APIS_SET1
#define SAMAPP_ENABLE_APIS_SET2
#define SAMAPP_ENABLE_APIS_SET3
#define SAMAPP_ENABLE_APIS_SET4
#define SAMAPP_ENABLE_APIS_SET5

#define SAMAPP_ENABLE_APIS_SET7 //Support only multi-touch screen FT801, FT811, FT813
#define SAMAPP_ENABLE_APIS_SET8 //Support only FT81x



#elif defined (FT93X_PLATFORM)
/* There is not enough memory for all Sets enable, please select several of them */
#define SAMAPP_ENABLE_APIS_SET0
#define SAMAPP_ENABLE_APIS_SET1

//#define SAMAPP_ENABLE_APIS_SET2
//#define SAMAPP_ENABLE_APIS_SET3
//#define SAMAPP_ENABLE_APIS_SET4
//#define SAMAPP_ENABLE_APIS_SET5

//#define SAMAPP_ENABLE_APIS_SET7 //Support only multi-touch screen FT801, FT811, FT813
//#define SAMAPP_ENABLE_APIS_SET8 //Support only FT81x

#else
//Should not be here
#endif
//Bouncing Circle macros
#define NO_OF_CIRCLE (5)

//Bouncing Points macros
#define NBLOBS (64)
#define OFFSCREEN   (-16384)
#define APP_BLOBS_COUNT  (0x01)
#define APP_BLOBS_NUMTOUCH  (5)

/* sample app structure definitions */
typedef struct SAMAPP_Bitmap_header
{
	uint8_t Format;
	int16_t Width;
	int16_t Height;
	int16_t Stride;
	int32_t Arrayoffset;
}SAMAPP_Bitmap_header_t;


//bouncing squares
#define NO_OF_RECTS (5)
typedef struct BouncingSquares_t{
	int16_t BRy[5],BRx[5],My[5];
	uint8_t E[5];
	uint8_t RectNo[5];
	int16_t Count;
}BouncingSquares_Context;


//bouncing circles structures
typedef  struct 
{
	uint8_t F[NO_OF_CIRCLE];
}TouchNoAndCircleNo;

typedef struct BouncingCircles_t{
	float Tsq1[NO_OF_CIRCLE];
	float C1X[NO_OF_CIRCLE];
	float C1Y[NO_OF_CIRCLE];
	float TouchX[NO_OF_CIRCLE];
	float TouchY[NO_OF_CIRCLE];
	TouchNoAndCircleNo TN[NO_OF_CIRCLE];
}BouncingCircles_Context;

//bouncing pints structures
typedef struct Blobs 
{
  int16_t x;
  int16_t y;
}Blobs;

typedef struct BlobsInst
{
	Blobs blobs[NBLOBS];
	uint8_t CurrIdx;
}BlobsInst;

//moving points structures
#define NO_OF_POINTS (64)
typedef struct MovingPoints_t
{
	uint8_t Prevtouch;
	int16_t SmallX[6], SmallY;
	uint8_t Flag;
	int32_t val[5];
	int16_t X[(NO_OF_POINTS)*4],Y[(NO_OF_POINTS)*4];
	uint8_t t[((NO_OF_POINTS)*4)];
}MovingPoints_Context;

//main windows 
#define ImW (66)
#define ImH (66)
#define NO_OF_TOUCH (5)

 typedef struct logoim
{
  char8_t name[40];
  uint16_t image_height;
  uint8_t image_format;
  uint8_t filter;
  uint16_t sizex;
  uint16_t sizey;
  uint16_t linestride;
  uint16_t gram_address;
}t_imageprp;


typedef struct Squares 
{
  uint16_t x, y;
}Squares;

#define APPBUFFERSIZE					(65536L)
#define APPBUFFERSIZEMINUSONE		(APPBUFFERSIZE - 1)

extern SAMAPP_Bitmap_header_t  SAMAPP_Bitmap_RawData_Header[];

extern PROGMEM prog_uchar8_t SAMAPP_Bitmap_RawData[];
extern PROGMEM prog_uchar8_t Lenaface40[];
extern PROGMEM prog_uchar8_t Font16[];
extern PROGMEM prog_uchar8_t Mandrill256[];
extern PROGMEM prog_uchar8_t Roboto_BoldCondensed_12[];

extern PROGMEM prog_uchar8_t SAMAPP_Tomato_DXT1_C0_Data_Raw[];

//Width:32  Height:32  Format: RGB565   Stride:64   File Size: 2048bytes
extern PROGMEM prog_uchar8_t SAMAPP_Tomato_DXT1_C1_Data_Raw[];
//Width:128  Height:128  Format: L1   Stride:16   File Size: 2048bytes
extern PROGMEM prog_uchar8_t SAMAPP_Tomato_DXT1_B0_Data_Raw[];
//Width:128  Height:128  Format: L1   Stride:16   File Size: 2048bytes
extern PROGMEM prog_uchar8_t SAMAPP_Tomato_DXT1_B1_Data_Raw[];

extern PROGMEM prog_uchar8_t SAMAPP_LENAFACE40_PNG_HEX_DUMP[];
extern PROGMEM prog_uchar8_t SAMAPP_Tomato_HEX[];


/*The font data contains 22 chinese characters, which is generated by fnt_cvt utility from SimFang true type font file of Windows*/
extern PROGMEM prog_uchar8_t SAMApp_ChineseFont_MetricBlock[];
extern PROGMEM prog_uchar8_t  SAMApp_ChineseFont_FontBmpData[];

void SAMAPP_fadeout();
void SAMAPP_fadein();
int16_t SAMAPP_qsin(uint16_t a);
int16_t SAMAPP_qcos(uint16_t a);
/* Sample app APIs for graphics primitives */

void	SAMAPP_GPU_Points();
void	SAMAPP_GPU_Lines();
void	SAMAPP_GPU_Rectangles();
void	SAMAPP_GPU_Bitmap();
void	SAMAPP_GPU_Fonts();
void	SAMAPP_GPU_Text8x8();
void	SAMAPP_GPU_TextVGA();
void	SAMAPP_GPU_Bargraph();
void	SAMAPP_GPU_LineStrips();
void	SAMAPP_GPU_EdgeStrips();
void	SAMAPP_GPU_Scissor();
void	SAMAPP_GPU_FtdiString();
void	SAMAPP_GPU_StreetMap();
void	SAMAPP_GPU_AdditiveBlendText();
void	SAMAPP_GPU_MacroUsage();
void	SAMAPP_GPU_AdditiveBlendPoints();

/* Sample app APIs for widgets */
void SAMAPP_CoPro_Widget_Logo();
void SAMAPP_CoPro_Widget_Calibrate();
void SAMAPP_CoPro_AppendCmds();
void SAMAPP_CoPro_Inflate();
void SAMAPP_CoPro_Loadimage();
void SAMAPP_CoPro_Widget_Button();
void SAMAPP_CoPro_Widget_Clock();
void SAMAPP_CoPro_Widget_Guage();
void SAMAPP_CoPro_Widget_Gradient();
void SAMAPP_CoPro_Widget_Keys();
void SAMAPP_CoPro_Widget_Progressbar();
void SAMAPP_CoPro_Widget_Scroll();
void SAMAPP_CoPro_Widget_Slider();
void SAMAPP_CoPro_Widget_Dial();
void SAMAPP_CoPro_Widget_Toggle();
void SAMAPP_CoPro_Widget_Text();
void SAMAPP_CoPro_Widget_Number();
void SAMAPP_CoPro_Widget_Spinner();
void SAMAPP_CoPro_Screensaver();
void SAMAPP_CoPro_Snapshot();
void SAMAPP_CoPro_Sketch();
void SAMAPP_CoPro_Matrix();
void SAMAPP_CoPro_Setfont();
void SAMAPP_CoPro_Track();

void SAMAPP_PowerMode();
void SAMAPP_BootupConfig();

//bouncing squares
void RectangleCalc(BouncingSquares_Context *context,uint8_t Arrayno);
void CheckTouch(BouncingSquares_Context *context,int16_t Tx1,int32_t val1);
void BouncingSquaresCall(int16_t BRx,int16_t BRy,int16_t MovingRy,uint8_t SqNumber);
int16_t MovingRect(int16_t BRy,int16_t MovingRy,uint8_t EndPtReach);
void BouncingSquares();

//bouncing circles
void ConcentricCircles(float C1,uint16_t R,uint16_t G,uint16_t B);
void TouchPoints(int16_t C1X,int16_t C1Y,uint8_t i);
void PlotXY();
void CheckCircleTouchCood(BouncingCircles_Context *context, int32_t val,uint8_t TouchNum,uint8_t i);
uint16_t CirclePlot(BouncingCircles_Context *context, uint16_t X,uint16_t Y,uint8_t Val);
void StoreTouch(BouncingCircles_Context *context, int32_t Touchval,uint8_t TouchNo);
void BouncingCircles();

//bouncing points
void BlobColor(BlobsInst *pBInst,int32_t TouchXY);
void BouncingPoints();

//moving points
void ColorSelection(int16_t k,int16_t i);
int16_t linear(float p1,float p2,uint16_t t,uint16_t rate);
void PointsCalc(MovingPoints_Context *context, uint8_t TouchNo, int16_t *X, int16_t *Y, uint8_t *t);
void MovingPoints();

//main windows
  void load_inflate_image(uint32_t address, const char *filename);
  void Logo_Intial_setup(struct logoim sptr[],uint8_t num);
#ifdef MSVC_PLATFORM
  void Load_file2gram(uint32_t add,uint8_t sectors, FILE *afile);
#endif
  void CheckTouch_tile(Squares *Sq, int32_t TouchXY,uint8_t TouchNo);
  void MainWindow();

#endif /* _SAMPLEAPP_H_ */

/* Nothing beyond this */









