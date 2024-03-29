/**
 * pnm.c
 * 
 * Ce fichier contient les définitions de types et 
 * les fonctions de manipulation d'images PNM.
 * 
 * @author: Russe Cyril s170220
 * @date: 01-03-2020
 * @projet: INFO0030 Projet 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "pnm.h"

/**
 * Définition de la struct Dimension_pixel
 * 
 */
struct Dimension_pixel_t{
   int nbr_ligne;
   int nbr_colonne;
};

/**
 * Définition du type opaque PNM
 *
 */
struct PNM_t {
   Type_PNM format;
   Dimension_pixel dimension;
   unsigned int valeur_max;
   unsigned short ***valeurs_pixel;
};


int load_pnm(PNM **image, char* filename) {
   Type_PNM type_image, extension_fichier;
   Dimension_pixel dimension;
   unsigned int valeur_max;


   if (filename==NULL){
      printf("Veuillez entrer un nom de fichier.\n");
      return -2;
   }

   FILE* fichier = fopen(filename, "r");
   if (fichier==NULL){
      printf("Impossible d'ouvrir le fichier %s.\n", filename);
      return -2;
   }

   //Vérifications format
   if (verification_type_image(&type_image, fichier)==-1){
      printf("L'en tête de l'image est malformé.\n");
      return -3;
   }
   if (verification_extension_fichier(type_image, filename, &extension_fichier)==-1){
      printf("L'extension de %s ne correspond pas au type de l'en tête. L'extension est du type %s et non %s.\n", filename, Type_PNM_vers_chaine(extension_fichier), Type_PNM_vers_chaine(type_image));
      return -2;
   }
   //enregistrement dimensions
   if(enregistrement_dimension_image(&dimension, fichier)==-1){
      printf("En tête de fichier mal formée. Impossible de lire les dimensions.\n");
      return -3;
   }

   //enregistrement valeur max
   if(type_image==PGM || type_image==PPM){
      if(enregistrement_valeur_max(&valeur_max, fichier)==-1){
         printf("En tête de fichier mal formée. Impossible de lire la valeur max.\n");
         return -3;
      }
   }


   *image = constructeur_PNM(dimension, type_image, valeur_max, fichier);
   if (*image==NULL){
      printf("Allocation de mémoire impossible.\n");
      return -1;
   }

   return 0;
}

PNM *constructeur_PNM(Dimension_pixel dimensions, Type_PNM format, unsigned int valeur_max, FILE *fichier){
   int i, j, nbr_valeur_ppm=0;
   char stockage_valeur_fichier[100];

   //malloc la matrice et chaque élément de celle-ci
   PNM *image = malloc(sizeof(PNM));
   if (image==NULL)
      return NULL;

   image->valeurs_pixel = malloc(dimensions.nbr_ligne * sizeof(unsigned short**));
   if(image->valeurs_pixel==NULL){
      free(image);
      return NULL;
   }

   for(i=0; i<dimensions.nbr_ligne; i++){
      image->valeurs_pixel[i] = malloc(dimensions.nbr_colonne * sizeof(unsigned short*));
      if (image->valeurs_pixel[i]==NULL){
         for(i--; i>=0; i--){//free reste du tableau avant la ligne n'ayant pu être malloc
            for(j=0; j<dimensions.nbr_colonne; j++)
               free(image->valeurs_pixel[i][j]);
            free(image->valeurs_pixel[i]);
         }
         free(image->valeurs_pixel);
         free(image);
         return NULL;
      }
      else{
         for(j=0; j<dimensions.nbr_colonne; j++){
            if(image->format==PPM)
               image->valeurs_pixel[i][j] = malloc(3 * sizeof(unsigned short));
            else
               image->valeurs_pixel[i][j] = malloc(sizeof(unsigned short));

            if(image->valeurs_pixel[i][j]==NULL){
               for(j--; j>=0; j--)//free éléments de la colonne avant l'élément n'ayant pu être malloc
                  free(image->valeurs_pixel[i][j]);
               free(image->valeurs_pixel[i]);

               for(i--;i>=0; i--){//free reste du tableau avant la colonne dans laquelle un élément n'a pu être malloc
                  for(j=0; j<dimensions.nbr_colonne; j++)
                     free(image->valeurs_pixel[i][j]);
                  free(image->valeurs_pixel[i]);
               }
            free(image->valeurs_pixel);
            free(image);
            return NULL;
            }
         }
      }

   }

   //initialisation des valeurs de l'image

   image->dimension.nbr_ligne = dimensions.nbr_ligne;
   image->dimension.nbr_colonne = dimensions.nbr_colonne;
   image->format = format;
   if(image->format == PBM)
      image->valeur_max = 1;
   else
      image->valeur_max = valeur_max;

   if(image->format==PBM || image->format==PGM){
      for(i=0; i<dimensions.nbr_ligne; i++){
         for (j=0; j<dimensions.nbr_colonne;){
            fscanf(fichier, "%s", stockage_valeur_fichier);
            if(stockage_valeur_fichier[0]!='#'){
               image->valeurs_pixel[i][j][0] = atoi(stockage_valeur_fichier);
               if(atoi(stockage_valeur_fichier) > (int)image->valeur_max){
                  libere_PNM(&image);
                  return NULL;
               }
               j++;
            }
            else
               fscanf(fichier, "%*[^\n]");
         }
      }
   }
   else{
      for(i=0; i<dimensions.nbr_ligne; i++){
         for (j=0; j<dimensions.nbr_colonne;){
            fscanf(fichier, "%s", stockage_valeur_fichier);
            if(stockage_valeur_fichier[0]!='#'){
               image->valeurs_pixel[i][j][nbr_valeur_ppm] = atoi(stockage_valeur_fichier);
               if(atoi(stockage_valeur_fichier) > (int)image->valeur_max){
                  libere_PNM(&image);
                  return NULL;
               }
               if(nbr_valeur_ppm==2)
                  j++;
               nbr_valeur_ppm++;
               nbr_valeur_ppm%=3;
            }
            else
               fscanf(fichier, "%*[^\n]");
         }
      }
   }
   fclose(fichier);

   return image;
}

void libere_PNM(PNM **image){
   if(*image!=NULL)
   {
      for(int i=0; i<(*image)->dimension.nbr_ligne; i++){
         for(int j=0; j<(*image)->dimension.nbr_colonne; j++)
            free((*image)->valeurs_pixel[i][j]);
         free((*image)->valeurs_pixel[i]);
      }
      free((*image)->valeurs_pixel);
      free(*image);
   }
}

int enregistrement_valeur_max(unsigned int *valeur_max, FILE  *fichier){
   char contenu_fichier[100];
   int nbr_fscanf = 0;
   do{
      if(contenu_fichier[0]!='#'){
         nbr_fscanf = fscanf(fichier, "%s[^\n]", contenu_fichier);
         if (atoi(contenu_fichier)<=255 && atoi(contenu_fichier)>=0){
            *valeur_max = atoi(contenu_fichier);
            return 0;
         }
         else
            return -1;
      }
      else
         fscanf(fichier, "%*[^\n]");
      
   }while(nbr_fscanf>0);

   return -1;
}

int enregistrement_dimension_image(Dimension_pixel *dimension, FILE *fichier){
   unsigned int n = 0;
   char contenu_fichier[100];
   int nbr_fscanf = 0; 
   do{
      if (contenu_fichier[0]=='#')
         nbr_fscanf = fscanf(fichier, "%*[^\n]");
      
      nbr_fscanf = fscanf(fichier, "%s[^\n]", contenu_fichier);
      if (contenu_fichier[0]!='#'){
         n++;
         if(n==1){
            dimension->nbr_ligne = atoi(contenu_fichier);
            if (dimension->nbr_ligne == 0)
               return -1;
         }
         else if(n==2){
            dimension->nbr_colonne = atoi(contenu_fichier);
            if (dimension->nbr_colonne == 0)
               return -1;
            return 0;
         }
      }
   }while(nbr_fscanf>0);
   
   return -1;
}

int verification_type_image(Type_PNM *type, FILE*  fichier){

   unsigned int numero_ligne = 0;
   char contenu_fichier[100];
   int nbr_fscanf = 0;

   do{
      if (contenu_fichier[0]=='#')
         nbr_fscanf = fscanf(fichier, "%*[^\n]");
      
      nbr_fscanf = fscanf(fichier, "%s[^\n]", contenu_fichier);
         
      if (contenu_fichier[0]!='#'){
         numero_ligne++;
         if (contenu_fichier[0]!='P')
            return -1;
         if (contenu_fichier[1]=='1'){
            *type =  PBM;
            return 0;
         }
         else if (contenu_fichier[1]=='2'){
            *type = PGM;
            return 0;
         }
         else if (contenu_fichier[1]=='3'){
            *type = PPM;
            return 0;
         }
         else 
            return -1;
      }

   }while(numero_ligne==0 && nbr_fscanf>0);
   return -1;
}

int verification_extension_fichier(Type_PNM type_image, char *filename, Type_PNM *extension_fichier){
   if(filename==NULL)
      return -1;
   int taille_nom=0;

   while(filename[taille_nom]!='\0')
      taille_nom++;

   if (filename[taille_nom-4]=='.' && filename[taille_nom-3]=='p' && filename[taille_nom-2]=='b' && filename[taille_nom-1]=='m')
      *extension_fichier = PBM;
   else if (filename[taille_nom-4]=='.' && filename[taille_nom-3]=='p' && filename[taille_nom-2]=='g' && filename[taille_nom-1]=='m')
      *extension_fichier = PGM;
   else if (filename[taille_nom-4]=='.' && filename[taille_nom-3]=='p' && filename[taille_nom-2]=='p' && filename[taille_nom-1]=='m')
      *extension_fichier = PPM;

   if(*extension_fichier == type_image)
      return 0;
   else
      return -1;

}

char *Type_PNM_vers_chaine(Type_PNM image){
   switch (image){
      case 0:   return "PBM";
      case 1:   return "PGM";
      case 2:   return "PPM";
      default:    return "inconnu";
   }
}

int chaine_vers_Type_PNM(char *chaine, Type_PNM *type_image){
   int taille=0;
   while(chaine[taille]!='\0')
      taille++;
   if(taille!=3)
      return -1;
   if(chaine[0]=='P' && chaine[1]=='B' && chaine[2]=='M')
      *type_image = PBM;
   else if(chaine[0]=='P' && chaine[1]=='G' && chaine[2]=='M')
      *type_image = PGM;
   else if(chaine[0]=='P' && chaine[1]=='P' && chaine[2]=='M')
      *type_image = PPM;
   else
      return -1;
   return 0;
}

int write_pnm(PNM *image, char* filename) {
   FILE *fichier;
   Type_PNM extension_fichier;
   if(image==NULL)
      return -2;


   if(verification_extension_fichier(image->format, filename, &extension_fichier)==-1){
      printf("L'extension du fichier dans lequel copier l'image ne correspond pas au format de celle-ci.\n");
      return -1;
   }
   if(verification_char_interdit_filename(filename)==-1){
      printf("Impossible de copier l'image. Le nom contient des caractères interdits.\n");
      return -1;
   }
   fichier = fopen(filename, "w");
   if (fichier==NULL){
      printf("Impossible d'ouvrir le fichier afin d'y copier l'image.\n");
      return -2;
   }
   if(ecriture_en_tete_PNM(image, fichier)==-1){
      printf("Impossible d'écrire l'en tête.\n");
      fclose(fichier);
      return -2;
   }
   if(ecriture_image(image, fichier)==-1){
      printf("Un problème est survenu lors de l'écriture de l'image.\n");
      fclose(fichier);
      return -2;
   }
   
   fclose(fichier);
   
   return 0;
}

int ecriture_image(PNM *image, FILE *fichier){
    for(int i=0; i<image->dimension.nbr_ligne; i++){
      for(int j=0; j<image->dimension.nbr_colonne; j++){
         if(image->format==PPM){
            for(int x=0; x<3; x++)
               fprintf(fichier, "%hu ", image->valeurs_pixel[i][j][x]);
         }
         else
            fprintf(fichier, "%hu ", image->valeurs_pixel[i][j][0]);
      }
      fprintf(fichier, "\n");
   }
   return 0;
}

int verification_char_interdit_filename(char *filename){
   char caractere=1;
   int i=0;
   while(caractere!='\0'){
      caractere=filename[i];
      if(caractere=='/' || caractere=='\\' || caractere==':' || caractere=='*' || caractere=='?' || caractere=='"' || caractere=='<' || caractere=='>' || caractere=='|')
         return -1;
      i++;
   }
   return 0;
}

int ecriture_en_tete_PNM(PNM *image, FILE *fichier){
   switch (image->format)
   {
   case PBM:
      fprintf(fichier, "P1\n");
      break;
   case PGM:
      fprintf(fichier, "P2\n");
      break;
   case PPM:
      fprintf(fichier, "P3\n");
      break;
   default:
      return -1;
   }

   fprintf(fichier, "%d %d\n", image->dimension.nbr_ligne, image->dimension.nbr_colonne);

   if(image->format!=PBM)
      fprintf(fichier, "%d\n", image->valeur_max);

   return 0;

}

void ecriture_extension_fichier(char *filename, PNM *image){
   int taille=0;
   while(filename[taille]!='\0')
      taille++;

   switch (image->format){
      case PBM:
         filename[taille-2]='b';
         break;
      case PGM:
         filename[taille-2]='g';
         break;
      case PPM:
         filename[taille-2]='p';
         break;
      default:
         break;
   }
}

