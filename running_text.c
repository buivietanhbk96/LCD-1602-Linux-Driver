#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <string.h>
#include "lcd_lib.h"

char *LargeText = "    Viet Nam vo dich!!!    ";

int iLineNumber = 2; /* Line number to show your string (Either 0 or 1) */

int iCursor = 0;

void UpdateLCDDisplay()
{
    int iChar;
    int iLenOfLargeText = strlen(LargeText); /* enght of string. */
    if (iCursor == (iLenOfLargeText - 1))    /* Reset variable for rollover effect. */
    {
        iCursor = 0;
    }
    lcd_goto_xy(iLineNumber, 0);

    if (iCursor < iLenOfLargeText - 16) /* This loop exicuted for normal 16 characters. */
    {
        for (iChar = iCursor; iChar < iCursor + 16; iChar++)
        {
            lcd_put_char(LargeText[iChar]);
        }
    }
    else
    {
        for (iChar = iCursor; iChar < (iLenOfLargeText - 1); iChar++) /* This code takes care of printing charecters of current string. */
        {
            lcd_put_char(LargeText[iChar]);
        }
        for (int iChar = 0; iChar <= 16 - (iLenOfLargeText - iCursor); iChar++) /* Reamining charecter will be printed by this loop. */
        {
            lcd_put_char(LargeText[iChar]);
        }
    }
    iCursor++;
}
int main()
{
    lcd_clear_display();
    lcd_goto_xy(1,0);
    lcd_put_string("    DASAN VN    ");
    while (1)
    {
        UpdateLCDDisplay();
        usleep(80000);
    }
    return 0;
}