/**
 * pnm.c
 * 
 * Ce fichier contient les définitions de types et 
 * les fonctions de manipulation d'images PNM.
 * 
 * @author: Russe Cyril s170220
 * @date: 20-02-2020
 * @projet: INFO0030 Projet 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "pnm.h"


/**
 * Définition du type opaque PNM
 *
 */
struct PNM_t {
   char *nombre_magique;
   unsigned int valeur_max;
   unsigned int **image;
};

/**
 * Définition de l'énumération Type_PNM
 * 
 */
enum Type_PNM_t{PBM, PGM, PPM};


int load_pnm(/*PNM **image,*/ char* filename) {
   int type_image;


   if (filename==NULL)
      return -2;
   //if (image==NULL)
   //   return -3;

   FILE* fichier = fopen(filename, "r");
   if (fichier==NULL)
      return -3;

   type_image = verification_type_image(fichier);
   if (type_image==-1 || verification_extension_fichier(type_image, filename)==-1)
      return -2;

   

   return 0;
}


Type_PNM verification_type_image(FILE*  fichier){

   unsigned int numero_ligne = 0;
   char type_image[100];
   int nbr_fscanf = 0;

   do{
      if (type_image[0]=='#')
         nbr_fscanf = fscanf(fichier, "%*[^\n]");
      
      nbr_fscanf= fscanf(fichier, "%s[^\n]", type_image);
         
      if (type_image[0]!='#'){
         numero_ligne++;
         if (type_image[0]!='P')
            return -1;
         if (type_image[1]=='1')
            return PBM;
         else if (type_image[1]=='2')
            return PGM;
         else if (type_image[1]=='3')
            return PPM;
         else 
            return -1;
      }

   }while(numero_ligne==0 && nbr_fscanf>0);
   return -1;
}

int verification_extension_fichier(Type_PNM type_image, char *filename){
   int taille_nom=0;

   while(filename[taille_nom]!='\0')
      taille_nom++;

   switch (type_image){
      case PBM: 
         if (filename[taille_nom-4]=='.' && filename[taille_nom-3]=='p' && filename[taille_nom-2]=='b' && filename[taille_nom-1]=='m')
            return 0;
         break;
      case PGM:
         if (filename[taille_nom-4]=='.' && filename[taille_nom-3]=='p' && filename[taille_nom-2]=='g' && filename[taille_nom-1]=='m')
            return 0;
         break;
      case PPM:
         if (filename[taille_nom-4]=='.' && filename[taille_nom-3]=='p' && filename[taille_nom-2]=='p' && filename[taille_nom-1]=='m')
            return 0;
         break;
      default: return -1;
   }
   return -1;
}


char *Type_PNM_vers_chaine(Type_PNM image){
   switch (image){
      case PBM:   return "PBM";
      case PGM:   return "PGM";
      case PPM:   return "PPM";
      default:    return "inconnu";
   }
}

int write_pnm(PNM *image, char* filename) {

   /* Insérez le code ici */

   return 0;
}

