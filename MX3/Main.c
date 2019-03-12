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
const int CLK = 100;
const int PERIOD_SAMPLING = 5;
const int MEAN_SIZE = 10000;   // grosseur du tableau de moyenne
const int OFFSET_ADC = 512;
// ------------------------------------------------------------------------------





// VARIABLE GLOBAL -------------------------------------------------------------
char G_error = 0;
int G_flag_evo = 0;
int G_flag_spasm = 0;

int G_error_buffer[255] = 0;
int G_error_index = 0;

int G_intensity_size = 100; // Le nombre de chiffre a avoir avant d'en faire la moyenne
int G_intensity_index = 0;   // Ou on se trouve dans le tableau moyenne
int G_intensity_buffer[MEAN_SIZE] = 0; // grosseur de 10000 a la base

int G_Save_flash_evo_index = 0x00000000;
int G_Save_flash_evo_buffer[1] = 0;

int G_Save_flash_spasm_index = 0xFFFFFFFF;
int G_Save_flash_spasm_buffer[1] = 0;

static volatile int G_flag_1s = 0;
static volatile int G_flag_05ms = 0;
// -----------------------------------------------------------------------------





// FONCTION --------------------------------------------------------------------

/* Entré: un num�ro d'erreur
 * Sortie: rien
 * Fonction: envoie au LCD les message d'erreur
 */
void Error_Call (int error_number)
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
    
    if (G_error_index >= 255)
    {
        G_error_index = 0;
    }
}

/* Entré: Le signal analogique du EMG
 * Sortie: Le signal transformé (maintenant digital) de l'EMG
 * Fonction: Lire le signal de l'emg et le transofmrer en signal digital grace
 *           à l'ADC.
 */
unsigned int Read (int analog_emg)
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
    int emg_rect = 0;
    
    if (digital_emg > 1023)    // entr� trop grande
    {
        Error_Call(1);
        G_error = 1;
        return 0;
    }
    else if (digital_emg < 0) // entr� trop petite
    {
        Error_Call(2);
        G_error = 1;
        return 0;
    }
    else
    {
        emg_rect = emg_rect - OFFSET_ADC;
        emg_rect = abs(emg_rect);
        return emg_rect;
    }
}

/* Entré: 1- La valeur redresser du emg. 2- Period de temps avant d'envoyer les 
 *        donné du buffer (1 seconde "worth" de données)
 * Sortie: La moyenne de tout les données dans le buffer
 * Fonction: Prendre les données redressé, les placer dans un buffer et envoyer 
 *           le buffer dans une fonction de calcule de moyenne apres X nombre de donnée recu
 */
int Intensity_Value (int emg_rect)
{
    int intensity = 0;
    int add_mean = 0;
    
    if (emg_rect < 0)   // valeur entr� est negative
    {
        Error_Call(2);
        return 0;
    }
    else if (G_intensity_index < G_intensity_size)   // tant qu'on a pas atteint le nombre de chiffre voulue, on place dans le buffer
    {
        G_intensity_buffer[G_intensity_index] = emg_rect;
        G_intensity_index ++;
        
        return 0;
    }
    else if (G_intensity_index >= G_intensity_size) // Quand on atteint le nombre de chiffre voulu, on fait la moyenne
    {
        while (G_intensity_index >= 0)
        {
            add_mean += G_intensity_buffer[G_intensity_index];
            G_intensity_index --;
        }
        
        G_intensity_index = 0;
        
        intensity = add_mean / G_intensity_size;
        
        if (G_flag_evo == 1)
        {
            G_Save_flash_evo_buffer[0] = intensity;
            Save_Evo(G_Save_flash_evo_buffer);
        }
    
        if (G_flag_spasm == 1)
        {
            G_Save_flash_spasm_buffer[0] = intensity;
            Save_Spasm(G_Save_flash_evo_buffer);
        }

        Display_Intensity_LCD(intensity, CLK);
        
        return intensity;
    }
}

/* Entré: L'intensité moyenne calculé sur 1 seconde
 * Sortie: Rien
 * Fonction: Sauvegarder dans la flash les valeurs que l'utilisateur veut 
 *           sauvegarder
 */
void Save_Evo (int* intensity)
{
    SPIFLASH_ProgramPage(G_Save_flash_evo_index, intensity, 1);
    G_Save_flash_evo_index += 0x00000001;
}

/* Entré: L'intensité moyenne calculé sur 1 seconde
 * Sortie: Rien
 * Fonction: Sauvegarder dans la flash les valeurs de la crise de l'utilisateur
 */
void Save_Spasm (int* intensity)
{
    SPIFLASH_ProgramPage(G_Save_flash_spasm_index, intensity, 1);
    G_Save_flash_spasm_index -= 0x00000001;
}

/* Entré: 1- Intensité moyenne calculé sur 1 seconde. 2- Le clk pour une 
 *        synchronisation de l'affichage. 3- Le nombre de donnée a afficher 
 *        représentant 1 seconde.
 * Sortie: Rien
 * Fonction: Afficher la moyenne de 1 seconde de données sur le LCD
 */
void Display_Intensity_LCD (int intensity, int clk, int period_send)
{
    LCD_DisplayClear();
    
}

/* Entré: Le signal redresser de l'emg
 * Sortie: Rien
 * Fonction: Envoie des valeurs de moyenne a la carte zybo
 */
void Send_Value_Spasm (int emg_rect)
{
    
}

/* Entré: Reception des données envoyer par la zybo
 * Sortie: La valeur du flag qui dit si une crise a lieu
 * Fonction: Lecture des données envoyer par la carte zybo et si une crise 
 *           d'épilepsie est detecter, on retourne un 1. Sinon, on retourne un 0.
 */
int Receive_Spasm (int comm_zybo)
{
    int flag_spasm = 0;
    
    return flag_spasm;
}

/* Entré: 1- Intensité moyenne sur 1 seconde du signal de l'emg. 2- Les valeurs 
 *        du spasm ? 3- Valeur du flag qui indique si une crise a lieux
 * Sortie: Donnée envoyer à l'ordinateur par UART
 * Fonction: Envoyer, à chaque 2 seconde, les valeurs d'intensité calculé et 
 *           d'afficher le message de crise si une crise a lieux
 */
int Interruption_2sec (int intensity, int spasm, int flag_spasm)
{
    int comm_UART = 0;
    
    return comm_UART;
}

/* Entré: Rien
 * Sortie: Flag si l'utilisateur veut enregistré ses données
 * Fonction: Si le bouton d'enregistrement est peser, les données sont 
 *           enregistré sur 3 seconde
 */
int Interruption_10ms ()
{
    int flag_evo = 0;
    
    return flag_evo;
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
    }
    mT1ClearIntFlag();	// Macro function to clear the interrupt flag
}
// -----------------------------------------------------------------------------





// INITIALISATION --------------------------------------------------------------

void Init()
{
    SPIFLASH_Init();
    UART_Init();
    LCD_Init();
    ADC_Init();
    initialize_01ms_interrupt();
}

// -----------------------------------------------------------------------------




// MAIN ------------------------------------------------------------------------	
int main()
{
    Init();
    
    unsigned int lecture_adc;
    char data_affichage[1024];
    
    while(1)
    {
        // Affichage ecran LCd a chaque seconde
        if(G_flag_1s == 1)
        {
            G_flag_1s = 0;
            LCD_DisplayClear();
            DelayAprox10Us(1000);
            sprintf(data_affichage, "ADC: %d", lecture_adc);
            LCD_WriteStringAtPos(data_affichage, 0, 0);
        }
        // Frequence d'echantillonnage
        if(G_flag_05ms == 1)
        {
            G_flag_05ms = 0;
            lecture_adc = Read(0);
        }
    }
    
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
}
// -----------------------------------------------------------------------------

