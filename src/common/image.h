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

// image.h

#pragma once


#include <stddef.h>
#include <string>


// Forward declaration without including headers to clutter the code
struct SDL_Surface;

//! Implementation-specific image data
/** Note that the struct has no destructor and the surface
    will not be freed at destruction. */
struct ImageData
{
    //! SDL surface with image data
    SDL_Surface* surface;

    ImageData() { surface = NULL; }
};

/**
  \class CImage
  \brief Image loaded from file

  Wrapper around SDL_Image library to load images. Also contains
  function for saving images to PNG.
  */
class CImage
{
private:
    //! Blocked!
    CImage(const CImage &other) {}
    //! Blocked!
    void operator=(const CImage &other) {}

public:
    //! Constructs empty image (with NULL data)
    CImage();
    //! Destroys image, calling Free()
    virtual ~CImage();

    //! Frees the allocated image data
    void Free();

    //! Returns whether the image is empty (has NULL data)
    bool IsEmpty();

    //! Returns the image data; if empty - returns NULL
    ImageData* GetData();

    //! Loads an image from the specified file
    bool Load(const std::string &fileName);

    //! Saves the image to the specified file in PNG format
    bool SavePNG(const std::string &fileName);

    //! Returns the last error
    std::string GetError();

private:
    //! Last encountered error
    std::string m_error;
    //! Image data
    ImageData* m_data;
};
