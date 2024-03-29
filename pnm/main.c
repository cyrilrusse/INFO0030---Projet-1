/**
 * main.c
 * 
 * Ce fichier contient la fonction main() du programme de manipulation
 * de fichiers pnm.
 *
 * @author: Russe Cyril s170220
 * @date: 01-03-2020
 * @projet: INFO0030 Projet 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

#include "pnm.h"


int main(int argc, char *argv[]) {

   /* options :
   *  -f <format> <fichier> -> permet de lancer le chargement de fichier et de la recopier dans le dossier de l'executable pnm
   */
   char *optstring = "f:";
   PNM *image;
   Type_PNM format_PNM, extension_fichier;
   char *filename, *format;
   int lancement_chargement = 0, retour_chargement, retour_ecriture, retour_verif_extension, val;

   while((val=getopt(argc, argv, optstring))!=EOF){
      switch(val){
         case 'f':
            format = optarg;
            filename = argv[argc-1];
            lancement_chargement = 1;
            break;
         default: return 0;
      }
   }
   
   if(!lancement_chargement)
      return 0;

   if(filename==NULL || argc<4){
      printf("Option 'f' requiert un argument 'fichier'.\n");
      return -1;
   }

   if(chaine_vers_Type_PNM(format, &format_PNM)==-1){
      printf("Le format %s passé en argument n'est pas un format PNM.\n", Type_PNM_vers_chaine(format_PNM));
      return -1;
   }

   if((retour_verif_extension = verification_extension_fichier(format_PNM, filename, &extension_fichier))==-1){
      printf("Mauvais format passé en argument. Le fichier %s est du type %s et non %s.\n", filename, Type_PNM_vers_chaine(extension_fichier), Type_PNM_vers_chaine(format_PNM));
      return -1;
   }

   retour_chargement = load_pnm(&image, filename);
   if(retour_chargement == 0)
      printf("Chargement de l'image %s en mémoire avec succès.\n", filename);

   if(retour_chargement == 0){
      char nom_fichier_ecriture[] = "test.pxm";
      ecriture_extension_fichier(nom_fichier_ecriture, image);
      retour_ecriture = write_pnm(image, nom_fichier_ecriture);
      if(retour_ecriture==0)
         printf("Image copiée dans le fichier %s.\n", nom_fichier_ecriture);
      libere_PNM(&image);
   }

   return 0;
}

