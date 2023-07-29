/* makespectrum_spectral.pov version 1.0
 * Persistence of Vision Raytracer scene description file
 * POV-Ray Object Collection demo
 *
 * Demo of makespectrum.inc's non-RGB SPDs using SpectralRender.
 * Download SpectralRender at:
 *   https://www.lilysoft.org/CGI/SR/Spectral%20Render.htm
 * Download modified SpectralRender files for gamut mapping at:
 *   https://github.com/CousinRicky/POV-SpectralRender-mods
 * Download Lightsys IV at:
 *   http://www.ignorancia.org/index.php/technical/lightsys/
 *
 * Copyright (C) 2023 Richard Callwood III.  Some rights reserved.
 * This file is licensed under the terms of the CC-LGPL
 * a.k.a. the GNU Lesser General Public License version 2.1.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  Please
 * visit https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html for
 * the text of the GNU Lesser General Public License version 2.1.
 *
 * Vers  Date         Comments
 * ----  ----         --------
 *       2014-Oct-18  Created.
 * 1.0   2023-Jul-29  Completed and uploaded.
 */
// Preview:
//   +W640 +H480 +A Declare=Preview=1 Declare=Photons=0 Declare=Rad=0
// Pass 1:
//   +W640 +H480 +A0.1 +AM2 +R3 +FE +KI1 +KF36 +KFI38 +KFF73
// Pass 2:
//   +W640 +H480 -A
// IMPORTANT:  Before running pass 2, make sure ALL of the #declare FName
// lines in SpectralComposer.pov are commented out.
#version max (3.7, min (3.8, version));

#ifndef (Preview) #declare Preview = no; #end

#if (clock_on | Preview)

  #include "screen.inc"
  #include "CIE.inc" // from Lightsys IV
  #include "spectral.inc" // from SpectralRender
  #include "makespectrum.inc"

  #ifndef (Rad) #declare Rad = on; #end // radiosity off/on
  #ifndef (Photons) #declare Photons = 0.005; #end // photons spacing
  #ifndef (ID) #declare ID = yes; #end // caption off/on

 //=============================== ENVIRONMENT =================================

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
  #declare Lighting = SpectralEmission (E_D65);
  // Compensate for a reduction in brightness caused by the normalization
  // in spectral_lights.inc (see IntegrateLight in the Object Collection
  // for better control):
  #if (SpectralMode) #declare Lighting = Lighting * 1.1153; #end

  #default { finish { diffuse 1 ambient Lighting * 0.22 } }

  light_source
  { <-1, 1, -1> * 1000, Lighting
    parallel point_at 0
  }

  sky_sphere
  { pigment { gradient y color_map { [0 Lighting * 0.5] [1 Lighting * 0.1] } }
  }

  plane
  { y, 0
    pigment { C_Spectral (D_CC_B4) }
  }

  Set_Camera (<0, 20, -20>, <0, 0, 0>, 25)

 //================================ THE DEMO ===================================

  #declare Test_object = sphere { y, 1 scale 0.5 }

  // Use MakeSpectrum to convert a color spec into a spectrum, and
  // SpectralRender to convert the spectrum into a monochrome value.
  #macro Get_color (Color, Min, Max)
    #local Curve = MakeSpectrum (Color, MAKESP_SRGB, Min, Max)
    #if (SpectralMode) // SpectralRender mode check
      #local P = MakeSp_SpectralRender (Curve, 1)
    #else
      #local P = ReflectiveSpectrum (Curve); // Fall back to Lightsys IV.
    #end
    C_Spectral (P) // from SpectralRender
  #end

  #for (Hue, 15, 360, 15)
    object
    { Test_object
      pigment { Get_color (<Hue, 1, 1>, 0.05, 0.75) }
      translate 4 * x
      rotate -Hue * y
    }
    object
    { Test_object hollow
      pigment { rgbf 1 }
      finish
      { reflection { 0 1 fresnel on } conserve_energy
        roughness 0.001 specular albedo 0.05
      }
      interior
      { IOR_Spectral (IOR_Glass_BK7) // from SpectralRender
        fade_distance 0.5
        fade_power 1001
        fade_color Get_color (<Hue, 1, 1>, 0.05, 0.95)
        #if (!Photons) caustics 1 #end
      }
      photons { target collect off reflection on refraction on }
      translate 5.25 * x
      rotate -Hue * y
    }
  #end

 //================================= CAPTION ===================================

  #if (ID)
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
    #if (file_exists ("SpectralComposer-gm2.inc"))
      Annotate ("Conversion by SpectralRender (gamut mapped)", <0, 1>)
    #else
      Annotate ("Conversion by SpectralRender", <0, 1>)
    #end
    #if (!SpectralMode) Annotate ("preview", <1, 1>) #end
  #end

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#else

  #declare FName = "makespectrum_spectral"
  #if (file_exists ("SpectralComposer-gm2.inc"))
    #debug "Using modified SpectralComposer for gamut mapping.\n"
    // from https://github.com/CousinRicky/POV-SpectralRender-mods
    #include "SpectralComposer-gm2.inc"
  #else
    // from SpectralRender
    // IMPORTANT:
    // Make sure ALL of the #declare FName lines in SpectralComposer.pov
    // are commented out, or the above #declare FName will be overridden!
    #include "SpectralComposer.pov"
  #end

#end

// end of makespectrum_spectral.pov
