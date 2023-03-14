////////////////////////////////////////////////////////////////
// Auteur: Maxime MU                                          //
// Date: 14/03/2023                                           //
// Description:                                               //
////////////////////////////////////////////////////////////////

// Importation des bibliothèques
#include <stdio.h>  //Bibliothèque standard d'entrée/sortie
#include <stdlib.h> //Bibliothèque standard
#include <string.h> //Bibliothèque de manipulation de chaînes de caractères

//Définition des constantes
#define MAXROWS 1000 //Nombre de lignes maximum
#define MAXCOLS 100  //Nombre de colonnes maximum
#define NUM_DAYS 7   //Nombre de jours
#define FILENUM 5    //Nombre de fichiers

//Calcul des fonctions
int indemnity(int choice, int number); //Fonction d'indemnité
int penalty(int capa_today, int capa_tomorrow, int day);//Fonction de pénalité

//Initialisation des variables
int families[MAXROWS][MAXCOLS]; //Tableau des familles
int numberOfRows = 0;   // Nombre de lignes
int numberOfColumns = 0; // Nombre de colonnes
int totalCost = 0;// Total des coûts (fonction objectif)
int capacities[NUM_DAYS] = { 0, 0, 0, 0, 0, 0, 0 }; // capacités de chaque jour

//Initialisation des variables (modifiable)
int max_capacity = 350; //Capacité maximale de population autorisée par jour
char *files[FILENUM] = {"pb10.csv", "pb20.csv", "pb30.csv", "pb40.csv", "pb50.csv"}; //Tableau des fichiers


//Fonction principale
int main(void){
    //Boucle de lecture des fichiers
    for(int i = 0; i < FILENUM; i++){
        //Lecture des données
        readData(files[i], families);

        //Affection des familles à un jour et calcul du coût total
        totalCost = assignFamilies(numberOfRows, numberOfColumns, families);

        //Affichage des résultats
        showResults(i);

        //Réinitialisation des données
        resetData();
    }
    return 0;//Fin du programme
}

// Méthode de lecture des données
void readData(char *fileName, int families[MAXROWS][MAXCOLS])
{
    FILE* fp = fopen(fileName,"r"); // Ouverture du fichier
    char line[1024]; // Variable tampon
    char* token; // Variable tampon
    char* separator = ";"; // Séparateur

    // Passer la première ligne
    fgets(line, sizeof(line), fp); // Lecture de la première ligne

    int row_index = 0; // Index de la ligne
    int col_index; // Index de la colonne

    if (fp != NULL)// Si le fichier existe 
    {
        while (fgets(line, sizeof(line), fp) != NULL && row_index < MAXROWS)// Tant qu'il y a des lignes et que l'index de la ligne est inférieur au nombre de lignes maximum
        {
            token = strtok(line, separator);// Séparation de la ligne en tokens
            col_index = 0;
            while (token != NULL)// Tant qu'il y a des tokens 
            {
                families[row_index][col_index] = atoi(token);// Conversion du token en entier et affectation à la case correspondante
                token = strtok(NULL, separator);// Séparation du token suivant
                col_index++;
            }
            row_index++;
        }
        fclose(fp); // Fermeture du fichier
    }
    numberOfRows = row_index; // Nombre de lignes 
    numberOfColumns = col_index; // Nombre de colonnes
}


// Méthode d'affection des familles à un jour et calcul du coût total
int assignFamilies(int num_rows, int num_cols, int families[MAXROWS][MAXCOLS])
{
    // Initialisation des variables
    int nb_families = num_rows; // Nombre de familles
    int indemnities = 0; // Total des indemnités payées
    int penalties = 0; // Total des pénalités	

    // Boucle pour affecter chaque famille à un jour
    for (int i = 0; i < nb_families; i++)
    {
        int number = families[i][0]; // Nombre de membres de la famille i
        int choice[5]; // Liste des choix de la famille i

        //Remplissage de la liste des choix correspondant à la famille i
        for (int j = 0; j < 5; j++) 
        {
            choice[j] = families[i][j + 1];
        }

        //Cas où tous les jours sont saturés
        int affected_day = -1;//Valeur par défaut du jour affecté
        int choice_day = -1;//Valeur par défaut du jour choisi

        // Boucle pour trouver le premier jour disponible selon les contraintes de capacité
        for (int j = 0; j < sizeof(choice) / sizeof(choice[0]); j++)  // Boucle sur les choix de la famille i
        {
            choice_day = choice[j]; // Choix du jour j
            if (capacities[choice_day] + number <= max_capacity) // Si la capacité du jour est inférieure à la capacité maximale
            {
                affected_day = choice_day; // Affectation du jour
                break;
            }
        }

        // Si aucun jour n'est disponible, on augmente la capacité de tous les jours par 50 et on réaffecte la famille
        if (affected_day == -1) 
        {
            int min_diff = abs(125 - capacities[0]);
            affected_day = 0;
            for (int j = 1; j < 7; j++) 
            {
                int diff = abs(125 - capacities[j]);
                if (diff < min_diff) 
                {
                    min_diff = diff;
                    affected_day = j;
                }
            }
        }

        // Affectation de la famille au jour choisi
        capacities[affected_day] += number;

        // Calcul de l'indemnité à payer pour la famille
        int family_choice = -1;//Valeur par défaut de la préférence de la famille
        for (int j = 0; j < 10; j++)
        {
            if (choice[j] == affected_day)//Si le jour choisi par la famille est le même que le jour affecté
            {
                family_choice = j;//Affectation de la préférence j de la famille
                break;// Sortie de la boucle
            }
        }
        // Calcul de l'indemnité à payer pour la famille avec la méthode indemnity
        indemnities += indemnity(family_choice, number);
        }

    //Calcul des pénalités
    for (int day = 0; day < NUM_DAYS-1; day++) // Pour chaque jour
    {
        penalties += penalty(capacities[day], capacities[day+1], day); // Calcul de la pénalité pour chaque jour à l'aide de la méthode penalty
    }
    printf("penalites = %d et idemnites = %d\n", penalties, indemnities);
    return indemnities + penalties; // Retourne le coût total
}

//Méthode de calcul des indemnités
int indemnity(int family_choice, int number)
{
    if (family_choice == 0)//Si la famille a choisi la préférence 0
    {
        return 0;//Pas d'indemnité
    }
    else if (family_choice == 1)//Si la famille a choisi la préférence 1
    {
        return 50;//Indemnité de 50
    }
    else if (family_choice == 2)//Si la famille a choisi la préférence 2
    {
        return 50+9*number;//Indemnité de 50 + 9 * nombre de membres de la famille
    }
    else if (family_choice == 3)//Si la famille a choisi la préférence 3
    {
        return 100+9*number;//Indemnité de 100 + 9 * nombre de membres de la famille
    }
    else if (family_choice == 4)//Si la famille a choisi la préférence 4
    {
        return 200+9*number;//Indemnité de 200 + 9 * nombre de membres de la famille
    }
}

//Méthode de calcul des pénalités
int penalty(int capacityOfToday, int capacityOfTomorrow, int day)
{
    if(day < 5)//Si le jour est un jour de lundi à samedi
    {
        return ((capacityOfToday-125) / 400) * capacityOfToday^(abs(capacityOfToday-capacityOfTomorrow) / 50); //Calcul de la pénalité
    }
    else//Si le jour est un dimanche
    {
        return ((capacityOfToday-125) / 400) * capacityOfToday; //Calcul de la pénalité
    }
}

//Méthode d'affichage des résultats
void showResults(int i){
    printf("----- %s-----\n", "Tableau de donnees :");
    printf("%s\n", files[i]);

    printf("\n----- %s-----\n", "Donnees du tableau :"); 
    printf("Nombre de lignes : %i\n", numberOfRows); 
    printf("Nombre de colonnes : %i\n", numberOfColumns);

    int totalOfPeople = 0;
    printf("\n-------- %s-------\n", "Distribution des familles :");
    for(int i = 0; i < NUM_DAYS; i++){//Pour chaque jour
        printf("%s ", "Nombre de personnes jour");
        printf("%i : ", i);

        printf("%i personnes\n", capacities[i]);
        totalOfPeople += capacities[i];
    }
    printf("\n----- %s-----\n", "Verifications :"); 
    printf("Nombre de personnes total : %i\n", totalOfPeople);

    printf("\n----- %s-----\n", "Fonction objectif :"); 
    printf("Total cost : $%i\n", totalCost);

    printf("\n////////////////////////////////////////\n\n"); 
}

//Méthode de réinitialisation des données
void resetData(){
    // Réinitialisation du tableau de données
    for (int i = 0; i < MAXROWS; i++) {
        for (int j = 0; j < MAXCOLS; j++) {
            families[i][j] = 0;
        }
    }

    numberOfRows = 0;   // Nombre de lignes
    numberOfColumns = 0; // Nombre de colonnes
    totalCost = 0;// Total des coûts (fonction objectif)

    for (int i = 0; i < NUM_DAYS; i++)
    {
        capacities[i] = 0; // Capacité de chaque jour définie à 0
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Question 2
On affecte les familles par ordre décroissant des pourcentages.
On affecte les familles au jour avec le moins de participants afin de mieux gérer le flux de participants 
et d'éviter de surcharger les autres jours où il y a déjà beaucoup de participants.En évitant de surcharger 
certains jours, cela permet de mieux répartir les coûts de l'organisation de l'événement sur l'ensemble des jours, 
ce qui peut aider à réduire les coûts globaux.
Par exemple pour l'événement 1, on affecte les familles au jour 4 avec le moins de participants 13.32%
Idem pour l'événement 2, on affecte les familles au jour 6 avec le moins de participants 13.83%
Et pour l'événement 3, on affecte les familles au jour 1 avec le moins de participants 12.74%
*/