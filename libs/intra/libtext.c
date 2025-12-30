#include "libtext.h"

float UnderLinedText(int x, int y, intraFont *font, const char *text, float size, g2dColor colorText, g2dColor colorLine, bool linear)
{	
	intraFontSetStyle(font, 1, WHITE, 0, 0.0f,INTRAFONT_ALIGN_LEFT);
	font->color = colorText;
	font->size = size;

	float width = intraFontMeasureText(font, text);
	
    intraFontActivate(font, linear);
	intraFontPrint(font, x, y, text);

	g2dBeginRects(G2D_VOID);
		g2dSetColor(colorLine);
		g2dSetCoordXY(x, y+ceil(font->texYSize*0.2)*font->size);
		g2dSetScaleWH(width, ceil(size));
		g2dAdd();	
	g2dEnd();
	
	return 1;
}

/*float InversedText(int x, int y, intraFont *font, const char *text, float fontSize, float angle, g2dColor color, g2dColor shadowColor)
{
    int size = strlen(text);
    if (size == 0)
		return 0;

    char *textInverse = (char *)malloc((size + 1) * sizeof(char)); 
    if (textInverse == NULL)
		return 0;

    int j = 0;

	int i, k;
    for (i = size - 1; i >= 0; ) {
        int start = i;
        while (i >= 0 && (text[i] & 0xc0) == 0x80) i--;
        
        for (k = i; k <= start; k++) {
            textInverse[j++] = text[k];
        }
        i--;
    }
    
    textInverse[j] = '\0';

    intraFontSetStyle(font, fontSize, color, shadowColor, angle, 0);
    float result = intraFontPrint(font, x, y, textInverse);

    free(textInverse); 
    return result;
}*/

char* InversedText(const char *text) 
{
    int size = strlen(text);
    if (size == 0)
        return NULL; // Возвращаем NULL для пустой строки

    char *textInverse = (char *)malloc((size + 1) * sizeof(char));
    if (textInverse == NULL)
        return NULL; // Возвращаем NULL при ошибке выделения памяти

    int j = 0;

    int i, k;
    for (i = size - 1; i >= 0; ) {
        int start = i;
        while (i >= 0 && (text[i] & 0xc0) == 0x80) i--;

        for (k = i; k <= start; k++) {
            textInverse[j++] = text[k];
        }
        i--;
    }

    textInverse[j] = '\0';
    return textInverse; // Возвращаем перевёрнутую строку
}

float AlphaDegrade(int x, int y, intraFont *font, const char *text, g2dColor colorText, g2dColor colorMark, float size, float angle, int center, dMode side)
{
    /*colorMark = G2D_RGBA(G2D_GET_R(colorMark), G2D_GET_G(colorMark), G2D_GET_B(colorMark), 180);
    int i, y_Mark = y, x_Mark = x;
	
    for (i = 0; i < ALPHA_LONG_MARK; i++)    
	{
        switch(side)
        {
            case GRAD_UP: 
				y_Mark--; 
				break;
            case GRAD_DOWN: 
				y_Mark++; 
				break;
            case GRAD_LEFT: 
				x_Mark--; 
				break;
            case GRAD_RIGHT: 
				x_Mark++; 
				break;
            case GRAD_UP_LEFT: 
				x_Mark--; 
				y_Mark--; 
				break;
            case GRAD_UP_RIGHT: 
				x_Mark++; 
				y_Mark--;
				break;
            case GRAD_DOWN_LEFT:
				x_Mark--; 
				y_Mark++; 
				break;
            case GRAD_DOWN_RIGHT: 
				x_Mark++; 
				y_Mark++; 
				break;
            default: 
				break;
				
        }
		
		intraFontSetStyle(font, size, G2D_RGBA(G2D_GET_R(colorMark), G2D_GET_G(colorMark), G2D_GET_B(colorMark), G2D_GET_A(colorMark) - 2*i*ALPHA_LONG_MARK), 0, angle, center);
        intraFontPrint(font, x_Mark, y_Mark, text);
    }
	
	intraFontSetStyle(font, size, colorText, 0, angle, center);
    return intraFontPrint(font, x, y,text);*/
	return 1;
}


float ShadowedText(int x, int y, intraFont *font, const char *text, float size, float angle, float shadowAngle, double distance, g2dColor colorText, g2dColor colorShadow, bool linear)
{
    float radAngle = shadowAngle*M_PI/180;

	float cosX = vfpu_cosf(radAngle);
    float sinY = vfpu_sinf(radAngle);

	int shadowOffsetX = distance * cosX;
	int shadowOffsetY = distance * sinY;

	intraFontSetStyle(font, size, colorShadow, 0, angle, INTRAFONT_ALIGN_LEFT);
	
    intraFontActivate(font, linear);
    intraFontPrint(font, x+shadowOffsetX, y+shadowOffsetY, text);
	
	intraFontSetStyle(font, size, colorText, 0, angle, INTRAFONT_ALIGN_LEFT);
    
    return intraFontPrint(font, x, y, text);
}


float ContouredText(int x, int y, intraFont *font, const char *text, float size, float angle, g2dColor colorText, g2dColor colorContour, unsigned int options, bool linear)
{
    int i, j;

	intraFontSetStyle(font, 1, WHITE, 0, 0.0f, options);
	intraFontSetStyle(font, size, colorContour, 0, angle, options);
	
    intraFontActivate(font, linear);

    for (i = -1; i <= 1; i++)
    {
        for (j = -1; j <= 1; j++)
			intraFontPrint(font, x-i, y-j, text);
	}
	
	intraFontSetStyle(font, size, colorText, 0, angle, options);
    return intraFontPrint(font, x, y, text);
}

float BackgroundColorText(int x, int y, intraFont *font, const char *text, float size, g2dColor colorText, g2dColor colorBackground, unsigned int options, bool linear)
{
	intraFontSetStyle(font, size, colorText, 0, 0.0f, options);
    
    int lineCount = 1;
    int maxLength = 0;
    int currentLength = 0;
    const char *longestSubstring = text;
    const char *ptr;
    
    for (ptr = text; ; ptr++) {  // Бесконечный цикл, выход через break
    if (*ptr == '\n' || *ptr == '\0') {
        // Проверяем, не является ли текущая строка самой длинной
        if (currentLength > maxLength) {
            maxLength = currentLength;
            longestSubstring = ptr - currentLength;
        }
        // Увеличиваем lineCount только при '\n' (но изначально уже 1)
        if (*ptr == '\n') {
            lineCount++;
        }
        currentLength = 0;  // Сброс длины для новой строки
        // Выход после обработки конца строки
        if (*ptr == '\0') {
            break;
        }
    } else {
        currentLength++;
    }
}

    if (currentLength > maxLength) {
        maxLength = currentLength;
        longestSubstring = text + (ptr - text - currentLength);
    }

    double width = intraFontMeasureText(font, longestSubstring);

    if (options == INTRAFONT_ALIGN_CENTER)
        lineCount--;

    g2dBeginRects(NULL);
        g2dSetColor(colorBackground);
		if (options == INTRAFONT_ALIGN_CENTER)
            g2dSetCoordXY(x - width/2 - 3*font->size, y-2*font->size);
        else
            g2dSetCoordXY(x-4*font->size,y-2*font->size);
		g2dSetScaleWH(width+8*font->size, intraFontTextHeight(font)+8*font->size + (intraFontTextHeight(font)*(lineCount-1)));
		g2dAdd();
	g2dEnd();
	
    intraFontActivate(font, linear);
	return intraFontPrint(font, x, y+intraFontTextHeight(font), text);
}


float StrikedText(int x, int y, intraFont *font, const char *text, float size, g2dColor colorText, g2dColor colorLine, bool linear)
{
	
	intraFontSetStyle(font, size, colorText, 0, 0.0f, INTRAFONT_ALIGN_LEFT);
    intraFontActivate(font, linear);
	intraFontPrint(font, x, y, text);
	
	int width = intraFontMeasureText(font, text);
	
	g2dBeginRects(G2D_VOID);
		g2dSetColor(colorLine);
		g2dSetCoordMode(G2D_DOWN_LEFT);
		g2dSetCoordXY(x, y-ceil(font->texYSize*0.2)*font->size);
		g2dSetScaleWH(width, size);
		g2dAdd();
	g2dEnd();
	
	return 1;
}

//void GradientText(int x, int y, intraFont *font, const char *text, g2dColor colorBegin, g2dColor colorEnd, float size, float angle)
//{
   /* int i, textSize = strlen(text);
    int posX = x;
    int posY = y;
	
    int Rc =  (G2D_GET_R(colorEnd) - G2D_GET_R(colorBegin))/(textSize-1);
    int Gc =  (G2D_GET_G(colorEnd) - G2D_GET_G(colorBegin))/(textSize-1);
    int Bc =  (G2D_GET_B(colorEnd) - G2D_GET_B(colorBegin))/(textSize-1);
	
    g2dColor ColorJ = 0;
	char help[2] = {0};
	
	intraFontSetStyle(font, size, ColorJ, 0, angle, center);
	
    for(i = 0; i < textSize; i++)
    {
        ColorJ = G2D_RGBA(i * Rc+G2D_GET_R(colorBegin), i * Gc+G2D_GET_G(colorBegin), i * Bc+G2D_GET_B(colorBegin), 255);
		
        help[0] = text[i];
		
        intraFontPrint(font, posX, posY, help);
		
        posX += intraFontMeasureText(font, help);
    }*/
//}

void GradientText(int x, int y, intraFont *font, const char *text, g2dColor colorBegin, g2dColor colorEnd, float size, bool linear)
{
    int textSize = strlen(text);
    if (textSize == 0) return;

    int rDiff = G2D_GET_R(colorEnd) - G2D_GET_R(colorBegin);
    int gDiff = G2D_GET_G(colorEnd) - G2D_GET_G(colorBegin);
    int bDiff = G2D_GET_B(colorEnd) - G2D_GET_B(colorBegin);

    char help[4]; 
    help[3] = '\0';

    intraFontSetStyle(font, size, G2D_RGBA(0, 0, 0, 255), 0, 0, 0);

    intraFontActivate(font, linear);
    
	int originalX = x; // Сохранить начальную координату X
    int i = 0;
    while (i < textSize)
    {
        uint8_t character = text[i];
        
        // Проверка на перенос строки
        if (character == '\n')
        {
            y += intraFontTextHeight(font); // Переход на следующую строку
            x = originalX; // Вернуться на исходную координату X
            i++; // Переход к следующему символу
            continue; // Продолжить со следующей итерацией
        }

        int charLen;
        if ((character & 0x80) == 0)
            charLen = 1;
        else if ((character & 0xE0) == 0xC0)
            charLen = 2;
        else if ((character & 0xF0) == 0xE0)
            charLen = 3;
        else if ((character & 0xF8) == 0xF0)
            charLen = 4;
        else
            break;

        strncpy(help, &text[i], charLen);
        
        int currentCharIndex = i;
        float ratio = (float)currentCharIndex / (textSize - 1);

        g2dColor currentColor = G2D_RGBA(
            G2D_GET_R(colorBegin) + (int)(rDiff * ratio),
            G2D_GET_G(colorBegin) + (int)(gDiff * ratio),
            G2D_GET_B(colorBegin) + (int)(bDiff * ratio),
            255
        );

        intraFontSetStyle(font, size, currentColor, 0, 0, 0);
        intraFontPrint(font, x, y, help);

        x += intraFontMeasureText(font, help);
        i += charLen;
    }
}


//EOF 
