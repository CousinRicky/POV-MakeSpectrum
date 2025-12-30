/* makespectrum_ls4.pov version 1.2-beta.1
 * Persistence of Vision Raytracer scene description file
 * A proposed POV-Ray Object Collection demo
 *
 * Demo of makespectrum.inc's non-RGB SPDs using Lightsys IV.
 * Download Lightsys IV at:
 *   http://www.ignorancia.org/index.php?page=lightsys
 *     or
 *   https://news.povray.org/64cffd99%40news.povray.org
 *
 * Copyright (C) 2023, 2025 Richard Callwood III.  Some rights reserved.
 * This file is licensed under the terms of the GNU-LGPL.
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  Please
 * visit https://www.gnu.org/licenses/lgpl-3.0.html for the text
 * of the GNU Lesser General Public License version 3.
 *
 * Vers.  Date         Comments
 * -----  ----         --------
 *        2014-Oct-18  Created.
 * 1.0    2023-Jul-29  Completed and uploaded to GitHub.
 * 1.1    2025-Oct-04  The license is upgraded to LGPL 3.
 * 1.2    2025-Dec-29  No change.
 */
#version max (3.5, min (3.8, version));

#include "consts.inc"
#include "screen.inc"
#include "math.inc"
#include "makespectrum.inc"
#include "CIE.inc" // from Lightsys IV
CIE_GamutMapping (2)

#ifndef (Rad) #declare Rad = on; #end // radiosity off/on
#ifndef (Photons) #declare Photons = 0.002; #end // photons spacing
#ifndef (ID) #declare ID = yes; #end // caption off/on
#ifndef (Debug) #declare Debug = no; #end

//=============================== ENVIRONMENT ==================================

global_settings
{ assumed_gamma 1
  max_trace_level 100
  #if (Rad)
    #declare Pretrace = 1 / image_width;
    radiosity
    { count 100
      error_bound 0.5
      pretrace_start Pretrace * 64
      pretrace_end Pretrace * 2
      recursion_limit 2
    }
  #end
  #if (Photons)
    photons { spacing Photons autostop 0 }
  #end
}
#default { finish { diffuse 1 ambient (Rad? 0: 0.22) } }

light_source
{ <-1, 1, -1> * 1000, rgb 1
  parallel point_at 0
}

sky_sphere { pigment { gradient y color_map { [0 rgb 0.5] [1 rgb 0.1] } } }

plane
{ y, 0
  pigment { rgb 0.59 }
}

Set_Camera (<0, 20, -20>, <0, 0, 0>, 25)

//================================= THE DEMO ===================================

#declare Test_object = sphere { y, 1 scale 0.5 }

#declare Hue = 0;
#while (Hue < 360)
 // Use MakeSpectrum to convert the color specs into spectra,
 // and a Lightsys IV macro to convert the spectra into RGB colors:
  #declare Opaque = ReflectiveSpectrum // from Lightsys IV
  ( MakeSpectrum (<Hue, 1, 1>, MAKESP_SRGB, 0.05, 0.75)
  );
  #declare Transp = ReflectiveSpectrum // from Lightsys IV
  ( MakeSpectrum (<Hue, 1, 1>, MAKESP_SRGB, 0.05, 0.95)
  );
  #if (Debug & (VMax (Opaque) > 1 | VMax (Transp) > 1))
    #debug concat
    ( str (Hue, 3, 0), ": opaque <", vstr (3, Opaque, ", ", 0, 4),
      ">, transparent <", vstr (3, Transp, ", ", 0, 4), ">\n"
    )
  #end
  object
  { Test_object
    pigment { rgb Opaque }
    translate 4 * x
    rotate -Hue * y
  }
  object
  { Test_object hollow
    pigment { rgbf 1 }
    finish
    { reflection { 0 1 fresnel on } conserve_energy
      roughness 0.001 specular 6.26
    }
    interior
    { ior Crown_Glass_Ior
      fade_distance 0.5
      fade_power 1001
      fade_color rgb Transp
      #if (!Photons) caustics 1 #end
    }
    photons { target collect off reflection on refraction on }
    translate 5.25 * x
    rotate -Hue * y
  }
  #declare Hue = Hue + 15;
#end

//================================= CAPTION ====================================

#macro Annotate (S, UV)
  Screen_Object
  ( text
    { ttf "cyrvetic" S 0.001, 0
      scale 0.05
      pigment { rgb 0 }
    },
    UV, <0.015, 0.015>, yes, 1
  )
#end

#if (ID) Annotate ("Conversion by Lightsys IV", <0, 1>) #end

#if (Debug) Annotate (str (version, 0, 2), <0.5, 0.5>) #end

// end of makespectrum_ls4.pov
