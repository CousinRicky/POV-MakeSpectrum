/* makespectrum_rgb.pov version 1.1-rc.2
 * Persistence of Vision Raytracer scene description file
 * A proposed POV-Ray Object Collection demo
 *
 * Demo of makespectrum.inc RGB and RGB metamer SPDs using SpectralRender.
 * Download SpectralRender at:
 *   https://www.lilysoft.org/CGI/SR/Spectral%20Render.htm
 * Download modified SpectralRender files for gamut mapping at:
 *   https://github.com/CousinRicky/POV-SpectralRender-mods
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
 *        2014-Apr-29  A prototype scene is created.
 * 1.0    2023-Jul-29  The finalized scene is uploaded to GitHub.
 *        2024-Dec-22  Y-based gamut mapping is recognized.
 * 1.1    2025-Oct-04  The license is upgraded to LGPL 3.
 */
// Preview:
//   +W640 +H480 +A Declare=Preview=1 Declare=Rad=0
// Pass 1:
//   +W640 +H480 +A0.1 +AM1 +R5 +FE +KI1 +KF36 +KFI38 +KFF73
// Pass 2:
//   +W640 +H480 -A
// IMPORTANT:  Before running pass 2, make sure ALL of the #declare FName
// lines in SpectralComposer.pov are commented out.
#version max (3.7, min (3.8, version));

#ifndef (Preview) #declare Preview = no; #end

#if (clock_on | Preview)

  #include "CIE.inc" // from Lightsys IV
  #include "spectral.inc" // from SpectralRender
  #include "makespectrum.inc"

  #ifndef (Rad) #declare Rad = on; #end // radiosity off/on

 //=============================== ENVIRONMENT =================================

  global_settings
  { assumed_gamma 1
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
  }
  #declare Lighting = SpectralEmission (E_D65);
  // Compensate for a reduction in brightness caused by the normalization
  // in spectral_lights.inc (see IntegrateLight in the Object Collection
  // for better control):
  #if (SpectralMode) #declare Lighting = Lighting * 1.1153; #end

  #default { finish { diffuse 1 ambient Lighting * 0.15 emission 0 } }

  camera
  { location <0, 1, -7.6>
    look_at <0, 1, 0>
    angle 22.8
  }

  #declare LIGHT_ALT = 30;
  #declare LIGHT_AZ = 20;

  light_source
  { vrotate (-10 * z, <LIGHT_ALT, LIGHT_AZ, 0>) + y,
    Lighting * 5000
    fade_power 2 fade_distance 0.1
    spotlight point_at y radius 45 falloff 90
  }

  box
  { -1, 1 scale <9, 11, 9>
    hollow
    pigment { C_Spectral (D_CC_B4) }
  }
  plane
  { y, 0
    pigment { checker C_Spectral (D_CC_F4) C_Spectral (D_CC_B4) }
  }

 //================================ THE DEMO ===================================

  #declare MakeSp_Gamma = 1.8;
  #declare MakeSp_Diffuse = 0.75;

  #declare XSPACE = 0.1;
  #declare YSPACE = 0.1;
  #declare Scale = 1 / (cos (radians (LIGHT_ALT)) * (2 * YSPACE + 4) + 1);

  // Make one colored sphere, using SpectralRender to interpret an SPD.
  #macro Demo (Row, Col, Curve)
    #if (SpectralMode) // SpectralRender mode check
      #local P = MakeSp_SpectralRender (Curve, 1)
    #else
      #local P = ReflectiveSpectrum (Curve); // Fall back to Lightsys IV.
    #end
    sphere
    { 0, 1
      pigment { C_Spectral (P) } // from SpectralRender
      translate <(Col - 2.5) * (2 + XSPACE), Row * (2 + YSPACE), 0>
    }
  #end

  union
  { #for (Col, 0, 5)
      Demo (2, Col, MakeSp_HSL (<Col * 30, 1, 0.5>))
    #end
    #for (Col, 0, 5)
      Demo (1, Col, MakeSp_Meta_HSL (<Col * 60 + 30, 1, 0.75>))
    #end
    #for (Col, 0, 5)
      Demo (0, Col, MakeSp_Meta_RGB (rgb (Col + 1) / 6))
    #end
    #for (Col, 0, 5)
      Demo (-1, Col, MakeSp_Meta_HSV (<(5.5 - Col) * 60, 1, 0.5>))
    #end
    #for (Col, 0, 5)
      Demo (-2, Col, MakeSp_HSV (<(11 - Col) * 30, 1, 1>))
    #end
    rotate <LIGHT_ALT, LIGHT_AZ, 0>
    scale Scale
    translate y
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#else

  #declare FName = "makespectrum_rgb"
  #if (file_exists ("SpectralComposer.inc"))
    // from https://github.com/CousinRicky/POV-SpectralRender-mods
    // version RC3-0.22-3 or later; otherwise, use SpectralComposer-gm2.inc
    #declare GamutMap = 4; // luminance-based
    #include "SpectralComposer.inc"
  #else
    // from SpectralRender
    // IMPORTANT:
    // Make sure ALL of the #declare FName lines in SpectralComposer.pov
    // are commented out, or the above #declare FName will be overridden!
    #include "SpectralComposer.pov"
  #end

#end

// end of makespectrum_rgb.pov
