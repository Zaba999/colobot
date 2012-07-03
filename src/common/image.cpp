// * This file is part of the COLOBOT source code
// * Copyright (C) 2012, Polish Portal of Colobot (PPC)
// *
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with this program. If not, see  http://www.gnu.org/licenses/.

// image.cpp

#include "image.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <png.h>


/* <---------------------------------------------------------------> */

/* The following code is from savesurf program by Angelo "Encelo" Theodorou
   Source: http://encelo.netsons.org/old/sdl/
   The code was refactored and modified slightly to fit the needs.
   The copyright information below is kept unchanged. */


/* SaveSurf: an example on how to save a SDLSurface in PNG
   Copyright (C) 2006 Angelo "Encelo" Theodorou

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   NOTE:

   This program is part of "Mars, Land of No Mercy" SDL examples,
   you can find other examples on http://marsnomercy.org
*/

std::string PNG_ERROR = "";

void PNGUserError(png_structp ctx, png_const_charp str)
{
    PNG_ERROR = std::string(str);
}

int PNGColortypeFromSurface(SDL_Surface *surface)
{
    int colortype = PNG_COLOR_MASK_COLOR; /* grayscale not supported */

    if (surface->format->palette)
        colortype |= PNG_COLOR_MASK_PALETTE;
    else if (surface->format->Amask)
        colortype |= PNG_COLOR_MASK_ALPHA;

    return colortype;
}

bool PNGSaveSurface(const char *filename, SDL_Surface *surf)
{
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    int i, colortype;
    png_bytep *row_pointers;

    PNG_ERROR = "";

    /* Opening output file */
    fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        PNG_ERROR = std::string("Could not open file '") + std::string(filename) + std::string("' for saving");
        return false;
    }

    /* Initializing png structures and callbacks */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, PNGUserError, NULL);
    if (png_ptr == NULL)
        return false;

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        PNG_ERROR = "png_create_info_struct() error!";
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return false;
    }

    png_init_io(png_ptr, fp);

    colortype = PNGColortypeFromSurface(surf);
    png_set_IHDR(png_ptr, info_ptr, surf->w, surf->h, 8, colortype, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    /* Writing the image */
    png_write_info(png_ptr, info_ptr);
    png_set_packing(png_ptr);

    row_pointers = (png_bytep*) malloc(sizeof(png_bytep)*surf->h);
    for (i = 0; i < surf->h; i++)
        row_pointers[i] = (png_bytep)(Uint8 *)surf->pixels + i*surf->pitch;
    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, info_ptr);

    /* Cleaning out... */
    free(row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    return true;
}

/* <---------------------------------------------------------------> */


CImage::CImage()
{
    m_data = NULL;
}

CImage::~CImage()
{
    Free();
}

bool CImage::IsEmpty()
{
    return m_data == NULL;
}

void CImage::Free()
{
    if (m_data != NULL)
    {
        if (m_data->surface != NULL)
        {
            SDL_FreeSurface(m_data->surface);
            m_data->surface = NULL;
        }
        delete m_data;
        m_data = NULL;
    }
}

ImageData* CImage::GetData()
{
    return m_data;
}

std::string CImage::GetError()
{
    return m_error;
}

bool CImage::Load(const std::string& fileName)
{
    if (! IsEmpty() )
        Free();

    m_data = new ImageData();

    m_error = "";

    m_data->surface = IMG_Load(fileName.c_str());
    if (m_data->surface == NULL)
    {
        delete m_data;
        m_data = NULL;

        m_error = std::string(IMG_GetError());
        return false;
    }

    return true;
}

bool CImage::SavePNG(const std::string& fileName)
{
    if (IsEmpty())
    {
        m_error = "Empty image!";
        return false;
    }

    m_error = "";

    if (! PNGSaveSurface(fileName.c_str(), m_data->surface) )
    {
        m_error = PNG_ERROR;
        return false;
    }

    return true;
}
