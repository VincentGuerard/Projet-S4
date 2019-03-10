#include "adc.h"
#include "lcd.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "uart.h"

// CONSTANTE -------------------------------------------------------------------
int PERIOD_SEND = 100;
int CLK = 100;
// ------------------------------------------------------------------------------





// VARIABLE GLOBAL -------------------------------------------------------------
char G_error = 0;
int G_flag_evo = 0;
int G_flag_spasm = 0;
// -----------------------------------------------------------------------------





// FONCTION --------------------------------------------------------------------

/* Entré: Le signal analogique du EMG
 * Sortie: Le signal transformé (maintenant digital) de l'EMG
 * Fonction: Lire le signal de l'emg et le transofmrer en signal digital grace
 *           à l'ADC.
 */
int Read (int analog_emg)
{
    int digital_emg = 0;
    ADC_Init();
    digital_emg = ADC_AnalogRead(24);
    return digital_emg;
}

/* Entré: Le signal digital de l'emg
 * Sortie: Le signal digital redresser pour n'avoir que des positifs
 * Fonction: Redresser le signal digital pour ne pas avoir de négatif. On
 *           regarde l'intensité donc le négatif ou positif ne compte pas.
 */
int Rectifier (int digital_emg)
{
    int emg_rect = 0;
    emg_rect = abs(digital_emg);
    return emg_rect;
}

/* Entré: 1- La valeur redresser du emg. 2- Period de temps avant d'envoyer les 
 *        donné du buffer (1 seconde "worth" de données)
 * Sortie: La moyenne de tout les données dans le buffer
 * Fonction: Prendre les données redressé, les placer dans un buffer et envoyer 
 *           le buffer dans une fonction de calcule de moyenne apres X nombre de donnée recu
 */
int Intensity_Value (int emg_rect, int send_period)
{
    int intensity = 0;
    
    if (G_flag_evo == 1)
    {
        Save_Evo(intensity);
    }
    
    if (G_flag_spasm == 1)
    {
        Save_Spasm(intensity);
    }
    
    Display_Intesity_LCD(intensity, CLK, PERIOD_SEND);
    
    return intensity;
}

/* Entré: L'intensité moyenne calculé sur 1 seconde
 * Sortie: Rien
 * Fonction: Sauvegarder dans la flash les valeurs que l'utilisateur veut 
 *           sauvegarder
 */
void Save_Evo (int intensity)
{
    
}

/* Entré: L'intensité moyenne calculé sur 1 seconde
 * Sortie: Rien
 * Fonction: Sauvegarder dans la flash les valeurs de la crise de l'utilisateur
 */
void Save_Spasm (int intensity)
{
    
}

/* Entré: 1- Intensité moyenne calculé sur 1 seconde. 2- Le clk pour une 
 *        synchronisation de l'affichage. 3- Le nombre de donnée a afficher 
 *        représentant 1 seconde.
 * Sortie: Rien
 * Fonction: Afficher la moyenne de 1 seconde de données sur le LCD
 */
void Display_Intesity_LCD (int intensity, int clk, int period_send)
{
    
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

// -----------------------------------------------------------------------------





// MAIN ------------------------------------------------------------------------	
int main()
{
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

