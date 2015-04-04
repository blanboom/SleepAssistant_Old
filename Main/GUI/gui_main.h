/*
 * gui_main.h
 *
 *  Created on: 2015年3月17日
 *      Author: maxz
 */

#ifndef GUI_GUI_MAIN_H_
#define GUI_GUI_MAIN_H_

void GUI_Main_StartScreen(void);
void GUI_Main_NoteScreen(void);
void GUI_Main_MainScreen(void);
void GUI_Main_F1_NoteScreen(void);
void GUI_Main_F1_MainScreen(void);
void GUI_Main_F2_MainScreen(void);
void GUI_Main_F3_MainScreen(void);
void GUI_Main_F4_MainScreen(void);
void GUI_Main_F5_MainScreen(void);
void GUI_Main_F6_MainScreen(void);
void GUI_Main_DrawGraph_Prepare(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t background);
void GUI_Main_DrawGraph(uint8_t data, uint16_t color);

#endif /* GUI_GUI_MAIN_H_ */
