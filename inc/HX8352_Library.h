/*************************************************************************************
*
Description : HX8352 Grafics Library :: API
Version :  1.0.2
Date    :  20.08.2011
Author  :  Left Radio                          
Comments:  
*
**************************************************************************************/

#ifndef __HX_LIBRARY_H
#define __HX_LIBRARY_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x.h"

/* Exported typedef -----------------------------------------------------------*/
/** Структурный тип данных параметров кнопок **/
typedef struct 
{
	const uint16_t Left;	             // Левая граница кнопки
	const uint16_t Lower;                // Нижняя граница кнопки
	const uint16_t Width;		         // Ширина
	const uint16_t Height;		         // Высота 
	uint16_t Color;			     		 // Цвет
	uint16_t Active_Color;	             // Цвет фона когда кнопка активна
	uint16_t Active_FontColor;		     // Цвет шрифта когда кнопка активна
	char*    Text;  			         // Текст
	void (*btnEvent_func)(void);		 // указатель на функцию обработчик	события
} btnINFO;


/** Bitmap information for Scheme_pict **/
typedef struct   
{ 
	uint16_t Width;                 // Picture Width
	uint16_t Height;                // Picture Height
	uint8_t RLE;                    // RLE Copressed flag, if yes value = 1
	uint32_t massiveSize;           // Last index of bitmap massive
	const uint8_t *ptrBitmap;       // Bitmap point array
}BITMAP_INFO_8;

typedef struct   
{ 
	uint16_t Width;                 // Picture Width
	uint16_t Height;                // Picture Height
	uint8_t RLE;                    // RLE Copressed flag, if yes value = 1
	uint32_t massiveSize;           // Last index of bitmap massive
	const uint16_t *ptrBitmap;      // Bitmap point array
}BITMAP_INFO_16;


/** Структура доступа к массиву индексов символов **/
typedef struct FontTable {
	uint16_t	width;           // Ширина символа
	uint16_t	start;           // Стартовый индекс на первый байт символа в массиве данных символов
} FONT_CHAR_INFO;

/** Структура для доступа к параметрам используемого шрифта **/
typedef struct
{
	uint8_t Height;		             // Высота символов
	uint8_t HeightBytes;				     // Высота символов в байтах
	uint8_t FirstChar;	              // Индекс первого символа
	uint8_t LastChar;		              // Индекс последнего символа
	uint8_t FontSpace;                 // Пробел между символами
	const FONT_CHAR_INFO *FontTable;	  // Таблица индексов символов
	const uint8_t *FontBitmaps;        // Указатель на массив с данными о символах
} FONT_INFO;

/** Структурный тип данных, сетка **/
typedef struct 
{
	uint16_t CenterX;						//
	uint16_t CenterY;						// 
	uint16_t Width;							//
	uint16_t Height;						//  
	uint16_t Color;						// 
	FunctionalState  EnableCentralLines;	//
} GridType;


typedef enum {IN_OBJECT = 0, OUT_OBJECT = !IN_OBJECT} ClipMode;
typedef enum {CLEAR = 0, DRAW = 1, ReDRAW = 2, CLEAR_ALL = 3} DrawState;
typedef enum {Horizontal = 0, Vertical = !Horizontal} LCD_RotationState;


/* Exported define -----------------------------------------------------------*/
/* Exported constant ---------------------------------------------------------*/
/** Установленные шрифты в проекте **/
extern const FONT_INFO timesNewRoman12ptFontInfo;
extern const FONT_INFO lucidaConsole10ptFontInfo;
extern const FONT_INFO arialUnicodeMS_16ptFontInfo;
extern const FONT_INFO  lucidaConsole_9pt_Bold_FontInfo;

/** Изображения в проекте **/
//extern const BITMAP_INFO_16 logoInfo;
//extern const BITMAP_INFO_8  trigRiseICOInfo;


/* Exported variables --------------------------------------------------------*/
/** указатель, тип данных btnINFO **/
extern btnINFO *btn;
extern btnINFO *saved_btn;


/* Exported function --------------------------------------------------------*/

/** -------------------------------  Назначение выводов МК для управления LCD  ------------------------------- **/

/** Установка порта под шинну данных **/
extern void set_LCD_DATAPort_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pins);
extern void LCD_Set_DataWidth_ColorBitDepth(uint8_t DataWidth, uint8_t NumberDepthBits);

/** Установка выводов МК под управляющие сигналы LCD **/
extern void set_RES_LCD(GPIO_TypeDef* GPIOx, uint16_t Pin);
extern void set_RD_LCD(GPIO_TypeDef* GPIOx, uint16_t Pin);
extern void set_RS_LCD(GPIO_TypeDef* GPIOx, uint16_t Pin);
extern void set_WR_LCD(GPIO_TypeDef* GPIOx, uint16_t Pin);
extern void set_CS_LCD(GPIO_TypeDef* GPIOx, uint16_t Pin);
extern void set_HC573_LE_LCD(GPIO_TypeDef* GPIOx, uint16_t Pin);





/** --------------------------------  Low level команды и запись в HX8352  -------------------------------- **/

/* Установить активный уровень на выводе Chip Select LCD */
extern void Set_LCD_CS(void);

/* Сбросить активный уровень на выводе Chip Select LCD */
extern void Reset_LCD_CS(void);


/** -----------------------------------  Mid level команды HX8352  ----------------------------------- **/

/* Нарисовать точку */
extern void (*LCD_PutPixel)(uint16_t data_lcd);

/* Установить курсор */
extern void LCD_SetCursor(uint16_t x, uint16_t y);



/** -----------------------------------  Hight level команды HX8352  ----------------------------------- **/

/** Инициализация HX8352 **/
extern void LCD_Init(void);

/** Полностью залить экран одним цветом **/
extern void LCD_FillScreen(uint16_t color);

/** Очистка или закраска области экрана одним цветом **/
extern void LCD_ClearArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

/* Установить область вывода */
extern void LCD_SetArea(uint16_t X0pos, uint16_t Y0pos, uint16_t X1pos, uint16_t Y1pos);

/* Установить область вывода на весь экран */
extern void LCD_SetFullScreen(void);

/** Сменить ориентацию LCD, портрет/альбом **/
extern void LCD_Change_Direction(uint8_t);




/** --------------------------------  Установка и считывание цветов  -------------------------------- **/

/** Установить цвет текста **/
extern void LCD_SetTextColor(uint16_t color);

/** Получить цвета текста **/
extern uint16_t LCD_GetTextColor(void); 

/** Установить цвет графики **/
extern void LCD_SetGraphicsColor(uint16_t color);

/** Получить цвет графики **/
extern uint16_t LCD_GetGraphicsColor(void);
  
/** Установить цвет фона **/
extern void LCD_SetBackColor(uint16_t color);

/** Получить цвет фона **/
extern uint16_t LCD_GetBackColor(void); 



/** --------------------------------  Работа с выводом bitmaps  -------------------------------- **/


/*********************************************************************
* Function: Вывод изображения формата 1bit цвет/8bit данные
*
* Input: 
*       X0pos, Y0pos - координаты нижнего левого угла выводимого изображения
*       *IN_bitmap - указатель на структуру BITMAP_INFO изображения которое выводим
*       RLE - флаг сжатия, 0 - нет сжатия
*
********************************************************************/
extern void LCD_Draw_1_8_IMG(uint16_t X0pos, uint16_t Y0pos, const BITMAP_INFO_8 *IN_bitmap, uint8_t RLE);

/*********************************************************************
* Function: Вывод изображения формата 1bit цвет/16bit данные
*
* Input: 
*       X0pos, Y0pos - координаты нижнего левого угла выводимого изображения
*       *IN_bitmap - указатель на структуру BITMAP_INFO изображения которое выводим
*       RLE - флаг сжатия, 0 - нет сжатия
*
********************************************************************/
extern void LCD_Draw_1_16_IMG(uint16_t X0pos, uint16_t Y0pos, const BITMAP_INFO_16 *IN_bitmap, uint8_t RLE);

/*********************************************************************
* Function: Вывод изображения формата 8bit цвет/8bit данные
*
* Input: 
*       X0pos, Y0pos - координаты нижнего левого угла выводимого изображения
*       *IN_bitmap - указатель на структуру BITMAP_INFO изображения которое выводим
*       RLE - флаг сжатия, 0 - нет сжатия
*
********************************************************************/
extern void LCD_Draw_8_IMG(uint16_t X0pos, uint16_t Y0pos, const BITMAP_INFO_8 *IN_bitmap, uint8_t RLE);

/*********************************************************************
* Function: Вывод изображения формата 16bit цвет/16bit данные
*
* Input: 
*       X0pos, Y0pos - координаты нижнего левого угла выводимого изображения
*       *IN_bitmap - указатель на структуру BITMAP_INFO изображения которое выводим
*       RLE - флаг сжатия, 0 - нет сжатия
*
********************************************************************/
extern void LCD_Draw_16_IMG(uint16_t X0pos, uint16_t Y0pos, const BITMAP_INFO_16 *IN_bitmap, uint8_t RLE);







/** --------------------------------  Работа с выводом символов и текста  -------------------------------- **/

/*********************************************************************
* Function: Выбор текущего шрифта для вывода
*
* Input: *selectFont указатель на структуру FONT_INFO шрифта который устанавливается для вывода
*
********************************************************************/ 
extern void LCD_SetFont(const FONT_INFO *selectFont);

   
/*********************************************************************
* Function: Вывод символа
*
* Input: Xpos, Ypos - координаты нижнего левой координаты символа
*		 trspr - прозрачность фона символа
*        с - выводимый символ
*
********************************************************************/ 
extern uint8_t LCD_DrawChar(uint16_t x, uint16_t y, uint8_t trspr, char c);


/*********************************************************************
* Function: Вывод строки
*
* Input: Xpos, Ypos - координаты нижнего левой координаты с которой начинается строка
*		 trspr - прозрачность фона строки
*        *str - указатель на строку
*
********************************************************************/  
extern uint16_t LCD_PutStrig(uint16_t XPos, uint16_t YPos, uint8_t trspr, char *str);


/*********************************************************************
* Function: Вывод строки определенным цветом
*
* Input: Xpos, Ypos - координаты нижнего левой координаты с которой начинается строка
*		 trspr - прозрачность фона строки
*        *str - указатель на строку
*		 Color - Цвет в 565
********************************************************************/
extern uint16_t LCD_PutColorStrig(uint16_t XPos, uint16_t YPos, uint8_t trspr, char *str, uint16_t Color);


/******************************************************************************
* Function Name  : LCD_Strig_PointsWidht
* Description    : Вычисление длинны строки в пикселях
* Input          : XPos, YPos, *str
* Return         : XPos - конечная координата
*******************************************************************************/
char* LCD_Strig_PointsWidht(uint16_t *XPos, char *str);


/*********************************************************************
* Function: Вывод текста с автоматическим переносом 
*
* Input: Xpos, Ypos - координаты нижнего левой координаты с которой начинается текст
*		 Xend - координата переноса
*        trspr - прозрачность фона текста        
*        *text - указатель на начало текста
*
********************************************************************/
extern uint16_t LCD_carryText(uint16_t XPos, uint16_t YPos, uint16_t Xend, uint8_t trspr, char *text);





/** --------------------------------  Графические примитивы  -------------------------------- **/

///*********************************************************************
//* Function: Нарисовать точку
//*
//* Input: Xpos, Ypos - координаты точки
//*
//********************************************************************/ 
//extern void LCD_DrawPoint(uint16_t Xpos, uint16_t Ypos); 


/******************************************************************************
* Function Name  : DrawPixel
* Description    : Нарисовать точку по указанным координатам
* Input          : Xpos, Ypos, Color
* Return         : None
*******************************************************************************/
extern void DrawPixel(uint16_t Xpos, uint16_t Ypos, uint16_t Color);


/******************************************************************************
* Function Name  : DrawLine
* Description    : нарисовать линию
* Input          : Arguments:
					    +  (X0, Y0) is start point of line.
					    +  (X1, Y1) is end point of line.
					    +  BaseColor is intensity of line. Pass 0 for black line.					    
* Return         : None
*******************************************************************************/
extern void LCD_DrawLine(uint16_t X0pos, uint16_t Y0pos, uint16_t X1pos, uint16_t Y1pos); 

 
/*********************************************************************
* Function: Нарисовать прямоугольник
*
* Input: Xpos, Ypos - координаты нижнего левого угла 
*		 Height, Width - высота и ширина прямоугольника
*
********************************************************************/ 
extern void LCD_DrawRect(uint16_t X0pos, uint16_t Y0pos, uint16_t X1pos, uint16_t Y1pos);


/*********************************************************************
* Function: Нарисовать закрашеный прямоугольник
*
* Input: Xpos, Ypos - координаты нижнего левого угла 
*		 Height, Width - высота и ширина прямоугольника
*        border - отрисовка с бордюром или без, 0 или 1 соответсвено
*        borderColor - цвет бордюра, если border = 0 то может принимать любое значение       
*
********************************************************************/ 
extern void LCD_DrawFillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width, DrawState border, uint16_t borderColor);

 
/*********************************************************************
* Function: Нарисовать окружность
*
* Input: Xpos, Ypos - координаты центра окружности
*		 Radius -  радиус окружности
*
********************************************************************/  
extern void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);  


/*********************************************************************
* Function: Нарисовать прямоугольную фигуру с закругленными углами
*
* Input: x1, y1 - координаты верхнего левого угла если радиус = 0 или центра окружности для скругления углов 
*		 x2, y2 - координаты нижнего правого угла если радиус = 0 или центра окружности для скругления углов 
*        rad -  радиус окружности
*
* Overview: Функция отрисовки обьекта с скругленными углами. 
*           Для чисто круглого объекта x1 = x2 и y1 = y2. 
*           Для прямоугольного rad = 0.
*
********************************************************************/ 
extern void DrawBeveledRect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t Radius);


/*********************************************************************
* Function: Нарисовать закрашеную прямоугольную фигуру с закругленными углами
*
* Input: x1, y1 - координаты верхнего левого угла если радиус = 0 или центра окружности для скругления углов 
*		 x2, y2 - координаты нижнего правого угла если радиус = 0 или центра окружности для скругления углов 
*        Radius - радиус окружности
*        border - отрисовка с бордюром или без, 0 или 1 соответсвено
*        borderColor - цвет бордюра, если border = 0 то может принимать любое значение 
*
* Overview: Функция отрисовки закрашенного обьекта с скругленными углами. 
*           Для чисто круглого объекта x1 = x2 и y1 = y2. 
*           Для прямоугольного rad = 0.
*
********************************************************************/ 
extern void DrawFillBeveledRect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t Radius, uint8_t border, uint16_t borderColor);


/*********************************************************************
* Function: Нарисовать закрашеную прямоугольную фигуру с закругленными углами
*
* Input: x1, y1 - координаты верхнего левого угла если радиус = 0 или центра окружности для скругления углов 
*		 x2, y2 - координаты нижнего правого угла если радиус = 0 или центра окружности для скругления углов 
*        Radius - радиус окружности
*        border - отрисовка с бордюром или без, 0 или 1 соответсвено
*        borderColor - цвет бордюра, если border = 0 то может принимать любое значение 
*
* Overview: Функция отрисовки закрашенного обьекта с скругленными углами. 
*           Для чисто круглого объекта x1 = x2 и y1 = y2. 
*           Для прямоугольного rad = 0.
*
********************************************************************/ 
extern void DrawFillBeveledRect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t Radius, uint8_t border, uint16_t borderColor);



/*********************************************************************
* Function: Нарисовать треугольник
*
* Input: x1, y1 - координаты вершины А
*		 x2, y2 - координаты вершины В
*        x3, y3 - координаты вершины С
*        
* Overview: Функция отрисовки треугольника           
*
********************************************************************/
extern void LCD_DrawTriangle(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t X3, uint16_t Y3, uint16_t Color);



/*********************************************************************
* Function: Нарисовать закрашенный треугольник
*
* Input: x1, y1 - координаты вершины А
*		 x2, y2 - координаты вершины В
*        x3, y3 - координаты вершины С
*        
* Overview: Функция отрисовки закрашенного треугольника           
*
********************************************************************/
extern void LCD_DrawFillTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t Color);





/******************************************************************************
* Function Name  : LCD_DrawButton
* Description    : Draw Button  / Нарисовать кнопку
* Input          : 	btnINFO *btn		
					active  				- state, ON or OFF		/	состояние, нажата или нет					
* Return         : None
*******************************************************************************/
extern void LCD_DrawButton(btnINFO *btn, uint8_t active);


/******************************************************************************
* Function Name  : LCD_DrawGrid
* Description    : draw custom grid				/	Нарисовать сетку
* Input          : GridType *grid	
* Return         : None
*******************************************************************************/
extern void LCD_DrawGrid(GridType *Grid, DrawState state);


/*********************************************************************
* Function: Нарисовать синус
*
* Input: x1, y1 - координаты нижнего левого углa
*        
* Overview: Draw "SIN"/Нарисовать синус с заданными размерами и периодом
*
********************************************************************/
extern void LCD_DrawSIN(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, float periods, uint16_t Color);


extern ErrorStatus Set_New_ClipObject(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, ClipMode NewMode, uint8_t NumInd);
extern ErrorStatus Clear_ClipObject(uint8_t NumInd);
extern FlagStatus Verify_Clip_Point(uint16_t X0, uint16_t Y0);
extern FlagStatus Verify_Clip_Line(uint16_t *x0, uint16_t *y0, uint16_t *x1, uint16_t *y1);
//extern FlagStatus CS_ClipLine_Out_Window(uint16_t *x0, uint16_t *y0, uint16_t *x1, uint16_t *y1);


#endif /* __HX_LIBRARY_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

