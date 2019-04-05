#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <xc.h>
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include "lcd.h"

// Define _TMR1 to enable the timer 1 code
#define _TMR1

// Board clock = 8 MHz
// Setting T1_INTR_RATE to 10000 means the timer will 
// fire every: 8Mhz/2*20/8/1/T1_INTR_RATE = 1ms
// See: Unit 2 - Elements of Real-time Systems / Unit 2 for more information
#define T1_INTR_RATE 1000

// CONSTANTE -------------------------------------------------------------------
const int PERIOD_SEND = 100;
const int LENGTH = 100;
const int CLK = 100;
const int PERIOD_SAMPLING = 5;
//const int MEAN_SIZE = 10000;   // grosseur du tableau de moyenne
const int OFFSET_ADC = 0x200;
const int RED = 0;
const int GREEN = 1;
const int ORANGE = 2;
const int IDDLE = 10000;
// ------------------------------------------------------------------------------





// VARIABLE GLOBAL -------------------------------------------------------------
char G_error = 0;
int G_flag_evo = 0;
int G_flag_spasm = 0;

int G_error_buffer[10] = {0};
int G_error_index = 0;

int G_sum_steps = 0;
int G_read_table_index = 0;

int G_intensity_size = 100; // Le nombre de chiffre a avoir avant d'en faire la moyenne
int G_intensity_index = 0;   // Ou on se trouve dans le tableau moyenne
int G_intensity_buffer[100] = {0}; // grosseur de 10000 a la base

int G_Save_flash_evo_index = 0x00000000;
int G_Save_flash_evo_buffer[1] = {0};

int G_Save_flash_spasm_index = 0xFFFFFFFF;
int G_Save_flash_spasm_buffer[1] = {0};

int G_intensity = 0;

int G_read_sum = 0;
int G_read_table[100] = {0};

static volatile int G_flag_1s = 0;
static volatile int G_flag_05ms = 0;

uint8_t accel_buffer[6];
signed short accelX, accelY, accelZ;
signed short accelX_buffer[200];
signed short accelY_buffer[200];
signed short accelZ_buffer[200];
int accel_count = 0;
int spasm_count = 0;
int flag_spasm_avant = 0;

// -----------------------------------------------------------------------------





// FONCTION --------------------------------------------------------------------
/* Entré: Le signal analogique du EMG
 * Sortie: Le signal transformé (maintenant digital) de l'EMG
 * Fonction: Lire le signal de l'emg et le transofmrer en signal digital grace
 *           à l'ADC.
 */
unsigned int Read()
{
    unsigned int digital_emg = 0;
    digital_emg = ADC_AnalogRead(24);
    
    if(digital_emg > 1023)
    {
        digital_emg = 1023;
    }
    else if(digital_emg < 0)
    {
        digital_emg = 0;
    }
    return digital_emg;
}

/* Entré: Le signal digital de l'emg. De 0 a 1024, avec offset de 512
 * Sortie: Le signal digital redresser pour n'avoir que des positifs
 * Fonction: Redresser le signal digital pour ne pas avoir de négatif. On
 *           regarde l'intensité donc le négatif ou positif ne compte pas.
 */
int Rectifier (int digital_emg)
{
    int emg_rect = abs(digital_emg - OFFSET_ADC);
    return emg_rect;
}

int Intensity (int emg_rect)
{
    G_read_sum -= G_read_table[G_read_table_index];
    G_read_table[G_read_table_index] = emg_rect;
    G_read_sum += G_read_table[G_read_table_index];
    G_read_table_index ++;
    if (G_read_table_index >= LENGTH){
        G_read_table_index = 0;
        // Envoie au python et a Zybo
    }
    if(flag_spasm_avant == 0 && G_flag_spasm == 0){
        if (G_read_sum <= IDDLE){
            Set_Led_Color(ORANGE);}
        else if (G_read_sum > IDDLE){
            Set_Led_Color(GREEN);}
        else{
            Set_Led_Color(RED);}
    }
    else{
        Set_Led_Color(RED);
    }
}

void Display_Value_LCD (int value, int state)
{
}

void Send_Value_Spasm (int emg_rect)
{
    
}

int Receive_Spasm (int comm_zybo)
{
    int flag_spasm = 0;
    
    return flag_spasm;
}

/* 0 = Red
 * 1 = Green
 * 2 = Orange
 */
void Set_Led_Color(int color)
{
    union col
    {
        struct rgb
        {
            char b;
            char g;
            char r;
            char spare;
        } rgb_val;
        unsigned int color_int;
    }union_color;

    switch(color)
    {
        // Red
        case 0:
            union_color.rgb_val.r = 0xFF;
            union_color.rgb_val.g = 0x00;
            union_color.rgb_val.b = 0x00;
            break;
        // Green
        case 1:
            union_color.rgb_val.r = 0x00;
            union_color.rgb_val.g = 0xFF;
            union_color.rgb_val.b = 0x00;
            break;
        // Orange
        case 2:
            union_color.rgb_val.r = 0xFF;
            union_color.rgb_val.g = 0x15;
            union_color.rgb_val.b = 0x00;                
            break;
        default:
            union_color.rgb_val.r = 0xFF;
            union_color.rgb_val.g = 0x00;
            union_color.rgb_val.b = 0x00;
            break;            
    }
    RGBLED_SetValueGrouped(union_color.color_int);
}

/* Entré: Rien
 * Sortie: rien
 * Fonction: init de la fonction d'interrupt
 */
void initialize_01ms_interrupt(void) 
{ 
    // Refer to : https://reference.digilentinc.com/_media/learn/courses/unit-2/unit_2.pdf 
    // for more information
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
    
    OpenTimer1( (T1_ON | T1_SOURCE_INT | T1_PS_1_1), (T1_INTR_RATE - 1) ); 
    mT1SetIntPriority(2);        
    mT1SetIntSubPriority(0);       
    mT1IntEnable(1);        
} 

/* Entré: Rien
 * Sortie: ???
 * Fonction: interrupt chaque 0.1 ms
 */
void __ISR(_TIMER_1_VECTOR, IPL2SOFT) Timer1Handler(void) 
{	
    static int count1s = 0;
    static int count05ms = 0;
    count1s++;
    count05ms++;

    // Count to 10000 0.1ms (1 s)
    if (count1s >= 10000) {
        count1s = 0;
        G_flag_1s = 1;
    }
    if(count05ms >= PERIOD_SAMPLING)
    {
        count05ms = 0;
        G_flag_05ms = 1;
        /*if (btn_evo == 1)
            G_flag_evo = 1;*/
    }
    mT1ClearIntFlag();	// Macro function to clear the interrupt flag
}

// Detection crise et lecture de l'accelerometre-----------------------------------------------------------------------------

void Read_Accel()
{
    ACL_ReadRawValues(accel_buffer);
    accelX = (((signed int) accel_buffer[0])<<24)>>16 | accel_buffer[1]; //VR
    accelY = (((signed int) accel_buffer[2])<<24)>>16 | accel_buffer[3]; //VR
    accelZ = (((signed int) accel_buffer[4])<<24)>>16 | accel_buffer[5]; //VR 
    accelX_buffer[accel_count] = abs(accelX);
    accelY_buffer[accel_count] = abs(accelY);
    accelZ_buffer[accel_count] = abs(accelZ);
    accel_count++;
    if(accel_count >= 200){         //Appel de Detection_Spasm lorsque 200 echantillons sont recoltes 
        accel_count = 0;
        Dectection_Spasm();
    }

}


void Dectection_Spasm()
{
    LED_SetValue(0,0);      //Led 0 off
    LED_SetValue(1,0);      //
    LED_SetValue(2,0);      //
    LED_SetValue(3,0);      //      Aucune
    LED_SetValue(4,0);      //      crise
    LED_SetValue(5,0);      //
    LED_SetValue(6,0);      //
    LED_SetValue(7,0);      //Led 7 off
    
    int sommeX = 0;         //Somme de l'axe X sur 200 echantillons par 100 ms
    int sommeY = 0;
    int sommeZ = 0;
    int moy = 0;            //Moyenne de la somme des 3 axes (Intensite)
    for(accel_count=0; accel_count<200; accel_count++){
        sommeX += accelX_buffer[accel_count];
        sommeY += accelY_buffer[accel_count];
        sommeZ += accelZ_buffer[accel_count];
    }
    moy = (sommeX+sommeY+sommeZ)/200;
    accel_count = 0;
    if(moy > 40000 ){
        G_flag_spasm = 1;  
    }
    flag_spasm_avant = G_flag_spasm;
}


// INITIALISATION --------------------------------------------------------------
void Init()
{
    //SPIFLASH_Init();
    //UART_Init(9600);
    ACL_Init();
    LCD_Init();
    LED_Init();
    ADC_Init();
    RGBLED_Init();
    initialize_01ms_interrupt();
}
// -----------------------------------------------------------------------------




// MAIN ------------------------------------------------------------------------	
int main()
{
    Init();
    char data_affichage[1024];
    unsigned int rectified_adc_read;
    
    while(1){
        if(G_flag_05ms == 1){
            G_flag_05ms = 0;
            rectified_adc_read = Rectifier(Read());
            Intensity(rectified_adc_read);
            Read_Accel();
            if (G_flag_1s == 1){
                G_flag_1s = 0;
                LCD_DisplayClear();
                sprintf(data_affichage, "ADC: %d", rectified_adc_read);
                LCD_WriteStringAtPos(data_affichage, 0, 0);
                sprintf(data_affichage, "Somme: %d", G_read_sum);
                LCD_WriteStringAtPos(data_affichage, 1, 0);
            }
        }
        if(G_flag_spasm == 1){
            G_flag_spasm = 0;
            LED_SetValue(0,1);
            LED_SetValue(1,1);
            LED_SetValue(2,1);
            LED_SetValue(3,1);
            LED_SetValue(4,1);
            LED_SetValue(5,1);
            LED_SetValue(6,1);
            LED_SetValue(7,1);
        }
    }
}
// -----------------------------------------------------------------------------

/* 1- Setup ADC et autre
 * 2- while(1)
 * 3- read adc
 * 4- redressage signal
 * 5- moyenne de l'intensité
 * 6- si bouton appuyer, sauvegarde de l'evolution en memoire flash
 * 7- display de l'intensité sur le LCD
 * 8- Envoie des valeurs a la zybo pour decider si cest une crise
 * 9- reception de la décision de a zybo
 * 10- envoie des information sur le UART. Soit juste des valeurs, soit juste 
 *     une alerte de crise 
 */

/* Entré: L'intensité moyenne calculé sur 1 seconde
 * Sortie: Rien
 * Fonction: Sauvegarder dans la flash les valeurs que l'utilisateur veut 
 *           sauvegarder
 */
/*int Intensity_Test (int emg_rect)
{

    
    if (G_sum_steps == 0)
    {
        G_read_table_index = 0;
        read_table[G_read_table_index] = emg_rect;
        read_sum += read_table[G_read_table_index];
        G_sum_steps = 1;
    }
    else if (G_sum_steps == 1)
    {
        G_read_table_index ++;
        read_table[G_read_table_index] = emg_rect;
        read_sum += read_table[G_read_table_index];
        if (G_read_table_index == LENGTH)
        {
            G_sum_steps = 2;
            // Envoie au python et a Zybo
            Display_Value_LCD(read_sum, 0);
        }
    }
    else if (G_sum_steps == 2)
    {
        G_read_table_index = 0;
        read_sum -= read_table[G_read_table_index];
        read_table[G_read_table_index] = emg_rect;
        read_sum += read_table[G_read_table_index];
        G_sum_steps = 3;
    }
    else if (G_sum_steps == 3)
    {
        G_read_table_index ++;
        read_sum -= read_table[G_read_table_index];
        read_table[G_read_table_index] = emg_rect;
        read_sum += read_table[G_read_table_index];
        if (G_read_table_index == LENGTH)
        {
            G_sum_steps = 2;
            // Envoie au python et a Zybo
            Display_Value_LCD(read_sum, 0);
        }
    }
}*/

/* Entré: 1- La valeur redresser du emg. 2- Period de temps avant d'envoyer les 
 *        donné du buffer (1 seconde "worth" de données)
 * Sortie: La moyenne de tout les données dans le buffer
 * Fonction: Prendre les données redressé, les placer dans un buffer et envoyer 
 *           le buffer dans une fonction de calcule de moyenne apres X nombre de donnée recu
 */
/*int Intensity_Value (int emg_rect)
{
    int add_mean = 0;
    if (G_intensity_index < G_intensity_size)   // tant qu'on a pas atteint le nombre de chiffre voulue, on place dans le buffer
    {
        G_intensity_buffer[G_intensity_index] = emg_rect;
        G_intensity_index ++;
        
        return G_intensity;
    }
    else if (G_intensity_index >= G_intensity_size) // Quand on atteint le nombre de chiffre voulu, on fait la moyenne
    {
        while (G_intensity_index >= 0)
        {
            add_mean += G_intensity_buffer[G_intensity_index];
            G_intensity_index --;
        }
        
        G_intensity_index = 0;
        add_mean -= 0xA0000000;
        G_intensity = add_mean / G_intensity_size;
        
        if (G_flag_evo == 1)
        {
            G_Save_flash_evo_buffer[0] = G_intensity;
            Save_Evo(G_Save_flash_evo_buffer);
        }
    
        if (G_flag_spasm == 1)
        {
            G_Save_flash_spasm_buffer[0] = G_intensity;
            Save_Spasm(G_Save_flash_evo_buffer);
        }
        
        return G_intensity;
    }
}*/

/* Entré: un num�ro d'erreur
 * Sortie: rien
 * Fonction: envoie au LCD les message d'erreur
 */
/*void Error_Call (int error_number)
{
    G_error_buffer[G_error_index] = error_number;
    if (G_error_buffer[G_error_index] == 1)
    {
        //entr� plus grand que pr�vu
    }
    else if (G_error_buffer[G_error_index] == 2)
    {
        //entr� plus petite que pr�vu
    }
    
    G_error_index ++;
    
    if (G_error_index >= 10)
    {
        G_error_index = 0;
    }
}*/

/*void Save_Evo (int* intensity)
{
    //SPIFLASH_ProgramPage(G_Save_flash_evo_index, intensity, 1);
    //G_Save_flash_evo_index += 0x00000001;
}*/

/* Entré: L'intensité moyenne calculé sur 1 seconde
 * Sortie: Rien
 * Fonction: Sauvegarder dans la flash les valeurs de la crise de l'utilisateur
 */
/*void Save_Spasm (int* intensity)
{
    //SPIFLASH_ProgramPage(G_Save_flash_spasm_index, intensity, 1);
    //G_Save_flash_spasm_index -= 0x00000001;
}*/
