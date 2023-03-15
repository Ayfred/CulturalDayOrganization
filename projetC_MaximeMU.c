////////////////////////////////////////////////////////////////
// Auteur: Maxime MU                                          //
// Date: 14/03/2023                                           //
// Desc : Les variables modifiables sont dans "Initialisation //
// des variables modifiables".                                //
// Pour tester les optimisations, il y a deux options :       //
// - Décommenter la méthode qui permet de trier en ordre      //
// croissant la matrice des familles.                         //
// - Décommenter la méthode qui permet de trier de manière    //
// aléatoire la matrice des familles.                         //                                    
////////////////////////////////////////////////////////////////

// Importation des bibliothèques
#include <stdio.h>  //Bibliothèque standard d'entrée/sortie
#include <stdlib.h> //Bibliothèque standard
#include <string.h> //Bibliothèque de manipulation de chaînes de caractères
#include <time.h> //Bibliothèque de gestion du temps

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
int max_capacity = 300; //Capacité maximale de population autorisée par jour
char *files[FILENUM] = {"pb10.csv", "pb20.csv", "pb30.csv", "pb40.csv", "pb50.csv"}; //Tableau des fichiers

//Optimisation du résultat
int interation = 0; //Nombre d'itérations
//int interation = 550; //Nombre d'itérations
int minimumTotalCost[] = {}; //Tableau des coûts totaux minimums


//Fonction principale
int main(void){
    srand(time(NULL)); // Générer une nouvelle séquence de nombres aléatoires à chaque exécution

    //Boucle de lecture des fichiers
    for(int i = 0; i < FILENUM; i++)
    {
        for(int k = 0; k <= interation; k++)
        {
            //Lecture des données
            readData(files[i], families);

            //Tri des données par ordre croissant
            //sortDataAscending(families);

            //Tri des données par ordre aléatoire
            //shuffle(families);

            /*// affichage du tableau trié
            for (int i = 0; i < numberOfRows; i++) {
                for (int j = 0; j < numberOfColumns; j++) {
                    printf("%d ", families[i][j]);
                }
                printf("\n");
            }*/

            //Affection des familles à un jour et calcul du coût total
            totalCost = assignFamilies(numberOfRows, numberOfColumns, families);

            findMinimalTotalCost(minimumTotalCost, totalCost, k);

            //Affichage des résultats
            showResults(i);

            //Réinitialisation des données
            resetData();
        }
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

        // Si aucun jour n'est disponible sur les choix de la famille, alors on l'affecte au jour qui a la capacité la plus petite
        if (affected_day == -1)
        {
            affected_day = 0;
            int min = capacities[0];  // Minimum de la liste des capacités

            for (int j = 0; j < NUM_DAYS; j++) {
                if (capacities[j] < min)
                {   // Si la capacité du jour i est inférieure à la capacité minimale
                    min = capacities[j]; // Mise à jour de la capacité minimale
                    affected_day = j;  // Affectation du jour j
                }
            }
        }

        // Affectation de la famille au jour choisi
        capacities[affected_day] += number;

        // Calcul de l'indemnité à payer pour la famille
        int family_choice = -1;//Valeur par défaut de la préférence de la famille
        for (int j = 0; j < sizeof(choice)/sizeof(choice[0]); j++)
        {
            if (choice[j] == affected_day)//Si le jour choisi par la famille est le même que le jour affecté
            {
                family_choice = j;//Affectation de la préférence j de la famille
                break;// Sortie de la boucle
            }
            else{
                family_choice = 4;//Si la famille ne se voit pas affecter le jour, alors on lui doit l'indemnité la plus élevée
            }
        }
        // Calcul de l'indemnité à payer pour la famille avec la méthode indemnity
        indemnities += indemnity(family_choice, number);
        /*
        printf("indemnite %d family %i choice %d (jour %d)and numberMembre %d\n", indemnities, i+1, family_choice, choice[0], number);
        for(int j = 0; j < NUM_DAYS; j++)
        {
            printf("capacite %d jour %d\n", capacities[j], j);
        }
        */
    }

    //Calcul des pénalités
    for (int day = 0; day < NUM_DAYS; day++) // Pour chaque jour
    {
        if(day != 6)//Si le jour n'est pas le dernier jour
        {
            penalties += penalty(capacities[day], capacities[day+1], day); // Calcul de la pénalité pour chaque jour à l'aide de la méthode penalty
        }
        else//Si le jour est le dernier jour
        {
            penalties += penalty(capacities[day], capacities[day], day); // Calcul de la pénalité pour chaque jour à l'aide de la méthode penalty
        }
    }
    //printf("penalites = %d et indemnites = %d\n", penalties, indemnities);
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
    if(day < 6)//Si le jour est un jour de lundi à samedi
    {
        return ((capacityOfToday-125) / 400) * pow(capacityOfToday,(abs(capacityOfToday-capacityOfTomorrow) / 50)); //Calcul de la pénalité
    }
    else//Si le jour est un dimanche
    {
        return ((capacityOfToday-125) / 400) * pow(capacityOfToday,1); //Calcul de la pénalité
    }
}


//Méthode d'optimisation

// fonction de comparaison pour le tri croissant
int compare(const void* a, const void* b) {
    const int* x = *(const int**)a;//On récupère la première colonne de la ligne a
    const int* y = *(const int**)b;//On récupère la première colonne de la ligne b
    return y[0] - x[0];//On compare les deux valeurs
}

// Méthode de tri croissant
void sortDataAscending(int families[MAXROWS][MAXCOLS]) {
    int temp[MAXCOLS]; // tableau temporaire pour échanger les lignes

    // tri croissant en utilisant la première colonne comme clé
    for (int i = 0; i < numberOfRows - 1; i++)
    {
        for (int j = 0; j < numberOfRows - i - 1; j++)
        {
            if (families[j][0] > families[j+1][0]) // si la première colonne de la ligne j est plus grande que la première colonne de la ligne j+1
            {
                // échanger les lignes
                for (int k = 0; k < numberOfColumns; k++)
                {
                    temp[k] = families[j][k]; // sauvegarder la ligne j dans un tableau temporaire
                    families[j][k] = families[j+1][k]; // remplacer la ligne j par la ligne j+1
                    families[j+1][k] = temp[k]; // remplacer la ligne j+1 par la ligne temporaire
                }
            }
        }
    }
}

// Méthode de tri aléatoire
void shuffle(int tableau[MAXROWS][MAXCOLS]) 
{
    for (int i = numberOfRows - 1; i > 0; i--) // pour chaque ligne
    {
        int j = rand() % (i + 1); // j est un nombre aléatoire entre 0 et i
        // échanger les lignes i et j
        int temp[MAXCOLS];
        for (int k = 0; k < numberOfColumns; k++) {
            temp[k] = tableau[i][k];
            tableau[i][k] = tableau[j][k];
            tableau[j][k] = temp[k];
        }
    }
}

// Méthode de recherche de la meilleure solution
void findMinimalTotalCost(int minimalTotalCost[], int totalCost, int k)
{
    minimalTotalCost[k] = totalCost; // Ajouter le coût total à la fin du tableau

    if(k == interation) //Si le nombre d'itération est atteint
    {
        int min = minimalTotalCost[0]; //Initialiser le coût total minimal
        for(int i = 0; i < interation; i++)
        {
            if(minimalTotalCost[i] < min)//Si le coût total minimal est plus petit que le coût total minimal
            {
                min = minimalTotalCost[i];//Trouver le coût total minimal
            }
        }
        printf("Le cout total minimal est : %d\n", min);
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
On affecte les familles par ordre croissant des pourcentages.
On affecte les familles au jour avec le moins de participants afin de mieux gérer le flux de participants
et d'éviter de surcharger les autres jours où il y a déjà beaucoup de participants.En évitant de surcharger
certains jours, cela permet de mieux répartir les coûts de l'organisation de l'événement sur l'ensemble des jours,
ce qui peut aider à réduire les coûts globaux.
Par exemple pour l'événement 1, on affecte les familles au jour 4 avec le moins de participants 13.32%
Idem pour l'événement 2, on affecte les familles au jour 6 avec le moins de participants 13.83%
Et pour l'événement 3, on affecte les familles au jour 1 avec le moins de participants 12.74%

Une amélioration est d'affecter en premier les familles avec le nombre de membres le moins élevé afin de réduire au
maximum les indemnités à payer pour les familles.
*/
