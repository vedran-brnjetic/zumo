/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#define C( x )        (  0 + x*12 ) 
#define C_SHARP( x )  (  1 + x*12 ) 
#define D_FLAT( x )   (  1 + x*12 ) 
#define D( x )        (  2 + x*12 ) 
#define D_SHARP( x )  (  3 + x*12 ) 
#define E_FLAT( x )   (  3 + x*12 ) 
#define E( x )        (  4 + x*12 ) 
#define F( x )        (  5 + x*12 ) 
#define F_SHARP( x )  (  6 + x*12 ) 
#define G_FLAT( x )   (  6 + x*12 ) 
#define G( x )        (  7 + x*12 ) 
#define G_SHARP( x )  (  8 + x*12 ) 
#define A_FLAT( x )   (  8 + x*12 ) 
#define A( x )        (  9 + x*12 ) 
#define A_SHARP( x )  ( 10 + x*12 ) 
#define B_FLAT( x )   ( 10 + x*12 ) 
#define B( x )        ( 11 + x*12 ) 
 
// 255 (silences buzzer for the note duration)
#define SILENT_NOTE   0xFF
 
// e.g. frequency = 445 | DIV_BY_10
// gives a frequency of 44.5 Hz
#define DIV_BY_10    (1 << 15)

/* [] END OF FILE */
