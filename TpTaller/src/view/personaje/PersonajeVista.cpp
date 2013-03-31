/* 
 * File:   PersonajeVista.cpp
 * Author: damian
 * 
 * Created on 23 de marzo de 2013, 12:25
 */

#include <view/PersonajeVista.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include <cmath>

#define FRAMES_PER_SECOND	5

//#define SCALE				0.2


//como es animado necesito la cantidad de clips por lado.
int NUMERODECLIPS=4;

SDL_Rect clipsDerecha[ 4 ];
SDL_Rect clipsIzquierda[ 4 ];
SDL_Rect clipsArriba[ 4 ];
SDL_Rect clipsAbajo[ 4 ];
SDL_Rect clipsAbajoIzq[ 4 ];
SDL_Rect clipsAbajoDer[ 4 ];
SDL_Rect clipsArribaDer[ 4 ];
SDL_Rect clipsArribaIzq[ 4 ];

SDL_Surface *load_image( char* urlImagen )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    loadedImage = IMG_Load( urlImagen );

    if( loadedImage != NULL )
    {
    	//loadedImage = rotozoomSurfaceXY(loadedImage, 0, SCALE, SCALE, 0);

        //Create an optimized surface
        optimizedImage = SDL_DisplayFormatAlpha( loadedImage );

        SDL_FreeSurface( loadedImage );

        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
        }
    }

    return optimizedImage;
}

void PersonajeVista::Draw( float x, float y, SDL_Surface* source, SDL_Surface* screen, SDL_Rect* clip )
{
    SDL_Rect offset;

    //offset.x = (int)x + cameraX - (clip->w/SCALE)/2;
    offset.x= (int)x +cameraX -clip->w;
    offset.y = (int)y + cameraY- clip->h;
    offset.w = clip->w;
    offset.h = clip->h;
    SDL_BlitSurface( source, clip, screen, &offset );
}

void PersonajeVista::UpdateCameraPos(int x, int y){
	cameraX = x;
	cameraY = y;
}

PersonajeVista::PersonajeVista(Personaje* unPersonaje, char* urlImagen)
{
	cameraX = cameraY = 0;
	marco = 0;
	estado = 0;
	/* TODO: Faltaria lanzar una excepcion en caso de que no recieba alguno de los parametros
     if(!unPersonaje)
    {
        throw new FaltaParametroException("PersonajeVista");
    }*/
    
    miPersonaje = unPersonaje;

   // try
    //{
    	//this->fondo = fondo;
        personajeImagen = this->CargarImagen(urlImagen);
        this->EstablecerLosClips(NUMERODECLIPS);
   /* }catch(ERROR e)
    {
        //TODO:cargo una imagen alternativa.
    }*/
}

SDL_Surface* PersonajeVista::CargarImagen(char* img)
{
    //Load the sprite sheet
    if(!img)
    {
      //  throw new FaltaParametroException();
    }
    
    SDL_Surface* miPersonajeImagen = load_image( img );

    //If there was a problem in loading the sprite
    if( !miPersonajeImagen)
    {
    	printf("NO SE HA ENCONTRADO LA IMAGEN\n");
        //return false;
        //TODO: cargo una alternativa
    }

    marco = 0;
    estado = PERSONAJE_DERECHA;
    return miPersonajeImagen;
}
void PersonajeVista::Mostrar(SDL_Surface* fondo)
{
	Vector2* movementDirection = this->miPersonaje->GetMovementDirection();
	Vector2* pos = this->miPersonaje->GetCurrentPos();

	//printf("marco inicial %i\n",marco);
    if( movementDirection->GetX() < 0  && floor(movementDirection->GetY()==0))
    {

        estado = PERSONAJE_IZQUIERDA;
        this->marco++;         // Move to the next marco in the animation
    	//printf("mueve izq marco %i\n",marco);
    }
    else if( movementDirection->GetX() < 0  && movementDirection->GetY()>0)
       {


           estado = PERSONAJE_ABAJOIZQ;
           this->marco++;         // Move to the next marco in the animation
       	//printf("mueve izqabj %i\n",marco);
       }
    else if( movementDirection->GetX() > 0 && floor(movementDirection->GetY()==0))
    {

        estado = PERSONAJE_DERECHA;
        this->marco++;
        //printf("mueve derecha %i\n",marco);
    }
    else if( movementDirection->GetX() > 0 && movementDirection->GetY()<0)
       {

           estado = PERSONAJE_ARRIBADER;
           this->marco++;
          // printf("mueve derecharriba\n %i",marco);
       }
    else if( movementDirection->GetY() > 0 && floor(movementDirection->GetX()==0))
    {
    	//printf("mueve abajo %i \n",marco);
        this->estado = PERSONAJE_ABAJO;
        marco++;
    }
    else if( movementDirection->GetY() > 0 && movementDirection->GetX()>0)
        {

            estado = PERSONAJE_ABAJODER;
            marco++;
         //   printf("mueve derechaabajo %i\n",marco);
        }
    else if( movementDirection->GetY() < 0 && floor(movementDirection->GetX()==0))
    {

        estado = PERSONAJE_ARRIBA;
        marco++;
       // printf("mueve arriba %i\n",marco);
    }
    else if( movementDirection->GetY() < 0 && movementDirection->GetX()<0)
        {

            estado = PERSONAJE_ARRIBAIZQ;
            marco++;
         //   printf("mueve izqarriba %i\n",marco);
        }
    else     // If Personaje standing
    {
        marco = 0; // Restart animation
    }

    //Loop the animation
    if( marco >= 4 )
    {
        marco = 0;
    }

    //Show the stick figure
    if( estado == PERSONAJE_DERECHA )
    {
        Draw( pos->GetX(), pos->GetY(), this->personajeImagen, fondo, &clipsDerecha[ marco ] );
    }
    else if( estado == PERSONAJE_IZQUIERDA )
    {
        Draw( pos->GetX(), pos->GetY(), this->personajeImagen, fondo, &clipsIzquierda[ marco ] );
    }else if (estado ==PERSONAJE_ARRIBA)
    {
        Draw(pos->GetX(), pos->GetY(), this->personajeImagen, fondo, &clipsArriba[ marco ] );
    }else if (estado ==PERSONAJE_ABAJO)
    {
        Draw(pos->GetX(), pos->GetY(), this->personajeImagen, fondo, &clipsAbajo[ marco ] );

	}else if (estado ==PERSONAJE_ABAJODER)
    {
        Draw(pos->GetX(), pos->GetY(), this->personajeImagen, fondo, &clipsAbajoDer[ marco ] );

	}else if (estado ==PERSONAJE_ABAJOIZQ)
    {
        Draw(pos->GetX(), pos->GetY(), this->personajeImagen, fondo, &clipsAbajoIzq[ marco ] );
    }else if (estado ==PERSONAJE_ARRIBADER)
    {
        Draw(pos->GetX(), pos->GetY(), this->personajeImagen, fondo, &clipsArribaDer[ marco ] );
    }else if (estado ==PERSONAJE_ARRIBAIZQ)
    {
        Draw(pos->GetX(), pos->GetY(), this->personajeImagen, fondo, &clipsArribaIzq[ marco ] );
    }

   //
}
void PersonajeVista::EstablecerLosClips(int cantidadPorLado)
{
	for(int cantidadDePosiciones=0;cantidadDePosiciones<4;cantidadDePosiciones++)
	{
		clipsDerecha[ cantidadDePosiciones ].x = PERSONAJE_ANCHO*cantidadDePosiciones;
		clipsDerecha[ cantidadDePosiciones ].y = 0;
		clipsDerecha[ cantidadDePosiciones ].w = PERSONAJE_ANCHO;
		clipsDerecha[ cantidadDePosiciones ].h = PERSONAJE_ALTO;

		 clipsIzquierda[ cantidadDePosiciones ].x = PERSONAJE_ANCHO*cantidadDePosiciones;
		 clipsIzquierda[ cantidadDePosiciones ].y = PERSONAJE_ALTO;
		 clipsIzquierda[ cantidadDePosiciones ].w = PERSONAJE_ANCHO;
		 clipsIzquierda[ cantidadDePosiciones ].h = PERSONAJE_ALTO;

		 clipsArriba[ cantidadDePosiciones ].x = PERSONAJE_ANCHO*cantidadDePosiciones;
		 clipsArriba[ cantidadDePosiciones ].y = PERSONAJE_ALTO * 2;
		 clipsArriba[ cantidadDePosiciones ].w = PERSONAJE_ANCHO;
		 clipsArriba[ cantidadDePosiciones ].h = PERSONAJE_ALTO;

		 clipsAbajo[ cantidadDePosiciones ].x = PERSONAJE_ANCHO*cantidadDePosiciones;
		 clipsAbajo[ cantidadDePosiciones ].y = PERSONAJE_ALTO * 3;
		 clipsAbajo[ cantidadDePosiciones ].w = PERSONAJE_ANCHO;
		 clipsAbajo[ cantidadDePosiciones ].h = PERSONAJE_ALTO;

		 clipsArribaIzq[ cantidadDePosiciones ].x = PERSONAJE_ANCHO*cantidadDePosiciones;
		 clipsArribaIzq[ cantidadDePosiciones ].y = PERSONAJE_ALTO * 6;
		 clipsArribaIzq[ cantidadDePosiciones ].w = PERSONAJE_ANCHO;
		 clipsArribaIzq[ cantidadDePosiciones ].h = PERSONAJE_ALTO;

		 clipsArribaDer[ cantidadDePosiciones ].x = PERSONAJE_ANCHO*cantidadDePosiciones;
		 clipsArribaDer[ cantidadDePosiciones ].y = PERSONAJE_ALTO * 7;
		 clipsArribaDer[ cantidadDePosiciones ].w = PERSONAJE_ANCHO;
		 clipsArribaDer[ cantidadDePosiciones ].h = PERSONAJE_ALTO;

		 clipsAbajoIzq[ cantidadDePosiciones ].x = PERSONAJE_ANCHO*cantidadDePosiciones;
		 clipsAbajoIzq[ cantidadDePosiciones ].y = PERSONAJE_ALTO * 5;
		 clipsAbajoIzq[ cantidadDePosiciones ].w = PERSONAJE_ANCHO;
		 clipsAbajoIzq[ cantidadDePosiciones ].h = PERSONAJE_ALTO;

		 clipsAbajoDer[ cantidadDePosiciones ].x = PERSONAJE_ANCHO*cantidadDePosiciones;
		 clipsAbajoDer[ cantidadDePosiciones ].y = PERSONAJE_ALTO * 4;
		 clipsAbajoDer[ cantidadDePosiciones ].w = PERSONAJE_ANCHO;
		 clipsAbajoDer[ cantidadDePosiciones ].h = PERSONAJE_ALTO;

	}
}
/*
void PersonajeVista::EstablecerLosClips(int cantidadPorLado)
{
    //Clip the sprites
    clipsDerecha[ 0 ].x = 0;
    clipsDerecha[ 0 ].y = 0;
    clipsDerecha[ 0 ].w = PERSONAJE_ANCHO*SCALE;
    clipsDerecha[ 0 ].h = PERSONAJE_ALTO*SCALE;

    clipsDerecha[ 1 ].x = PERSONAJE_ANCHO*SCALE;
    clipsDerecha[ 1 ].y = 0;
    clipsDerecha[ 1 ].w = PERSONAJE_ANCHO*SCALE;
    clipsDerecha[ 1 ].h = PERSONAJE_ALTO*SCALE;

    clipsDerecha[ 2 ].x = PERSONAJE_ANCHO * 2*SCALE;
    clipsDerecha[ 2 ].y = 0;
    clipsDerecha[ 2 ].w = PERSONAJE_ANCHO*SCALE;
    clipsDerecha[ 2 ].h = PERSONAJE_ALTO*SCALE;

    clipsDerecha[ 3 ].x = PERSONAJE_ANCHO * 3*SCALE;
    clipsDerecha[ 3 ].y = 0;
    clipsDerecha[ 3 ].w = PERSONAJE_ANCHO*SCALE;
    clipsDerecha[ 3 ].h = PERSONAJE_ALTO*SCALE;

    clipsIzquierda[ 0 ].x = 0;
    clipsIzquierda[ 0 ].y = PERSONAJE_ALTO*SCALE;
    clipsIzquierda[ 0 ].w = PERSONAJE_ANCHO*SCALE;
    clipsIzquierda[ 0 ].h = PERSONAJE_ALTO*SCALE;

    clipsIzquierda[ 1 ].x = PERSONAJE_ANCHO*SCALE;
    clipsIzquierda[ 1 ].y = PERSONAJE_ALTO*SCALE;
    clipsIzquierda[ 1 ].w = PERSONAJE_ANCHO*SCALE;
    clipsIzquierda[ 1 ].h = PERSONAJE_ALTO*SCALE;

    clipsIzquierda[ 2 ].x = PERSONAJE_ANCHO * 2*SCALE;
    clipsIzquierda[ 2 ].y = PERSONAJE_ALTO*SCALE;
    clipsIzquierda[ 2 ].w = PERSONAJE_ANCHO*SCALE;
    clipsIzquierda[ 2 ].h = PERSONAJE_ALTO*SCALE;

    clipsIzquierda[ 3 ].x = PERSONAJE_ANCHO * 3*SCALE;
    clipsIzquierda[ 3 ].y = PERSONAJE_ALTO*SCALE;
    clipsIzquierda[ 3 ].w = PERSONAJE_ANCHO*SCALE;
    clipsIzquierda[ 3 ].h = PERSONAJE_ALTO*SCALE;
    
    clipsArriba[ 0 ].x = 0;
    clipsArriba[ 0 ].y = PERSONAJE_ALTO * 2*SCALE;
    clipsArriba[ 0 ].w = PERSONAJE_ANCHO*SCALE;
    clipsArriba[ 0 ].h = PERSONAJE_ALTO*SCALE;

    clipsArriba[ 1 ].x = PERSONAJE_ANCHO*SCALE;
    clipsArriba[ 1 ].y = PERSONAJE_ALTO * 2*SCALE;
    clipsArriba[ 1 ].w = PERSONAJE_ANCHO*SCALE;
    clipsArriba[ 1 ].h = PERSONAJE_ALTO*SCALE;

    clipsArriba[ 2 ].x = PERSONAJE_ANCHO * 2*SCALE;
    clipsArriba[ 2 ].y = PERSONAJE_ALTO * 2*SCALE;
    clipsArriba[ 2 ].w = PERSONAJE_ANCHO*SCALE;
    clipsArriba[ 2 ].h = PERSONAJE_ALTO*SCALE;

    clipsArriba[ 3 ].x = PERSONAJE_ANCHO * 3*SCALE;
    clipsArriba[ 3 ].y = PERSONAJE_ALTO * 2*SCALE;
    clipsArriba[ 3 ].w = PERSONAJE_ANCHO*SCALE;
    clipsArriba[ 3 ].h = PERSONAJE_ALTO*SCALE;

    
    clipsAbajo[ 0 ].x = 0;
    clipsAbajo[ 0 ].y = PERSONAJE_ALTO * 3*SCALE;
    clipsAbajo[ 0 ].w = PERSONAJE_ANCHO*SCALE;
    clipsAbajo[ 0 ].h = PERSONAJE_ALTO*SCALE;

    clipsAbajo[ 1 ].x = PERSONAJE_ANCHO*SCALE;
    clipsAbajo[ 1 ].y = PERSONAJE_ALTO * 3*SCALE;
    clipsAbajo[ 1 ].w = PERSONAJE_ANCHO*SCALE;
    clipsAbajo[ 1 ].h = PERSONAJE_ALTO*SCALE;

    clipsAbajo[ 2 ].x = PERSONAJE_ANCHO * 2*SCALE;
    clipsAbajo[ 2 ].y = PERSONAJE_ALTO * 3*SCALE;
    clipsAbajo[ 2 ].w = PERSONAJE_ANCHO*SCALE;
    clipsAbajo[ 2 ].h = PERSONAJE_ALTO*SCALE;

    clipsAbajo[ 3 ].x = PERSONAJE_ANCHO * 3*SCALE;
    clipsAbajo[ 3 ].y = PERSONAJE_ALTO * 3*SCALE;
    clipsAbajo[ 3 ].w = PERSONAJE_ANCHO*SCALE;
    clipsAbajo[ 3 ].h = PERSONAJE_ALTO*SCALE;

}*/

PersonajeVista::~PersonajeVista() 
{
    //libera la memoria que pide para La imagen
    SDL_FreeSurface( this->personajeImagen );

    //Quita SDL
    SDL_Quit();
}

