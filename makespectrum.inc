/* makespectrum.inc version 1.0
 * Persistence of Vision Raytracer include file
 * POV-Ray Object Collection
 *
 * Generates reflective spectral curves for spectral rendering without recourse
 * to a spectrophotometer. The user submits a color specification, and a spline
 * is returned.  The user can then convert the spline to POV-Ray's native RGBFT
 * format using existing tools, such as Lightsys IV or SpectralRender.
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
 *       2014-Apr-10  Work is started.
 * 1.0   2023-Jul-29  Debugged and uploaded.
 *
 * Implementation note: due to POV-Ray namespace scope leakage, local variables
 * and function parameters (other than x, y, and z) *must* be prefixed.
 */
#ifndef (MakeSpectrum_Inc_Temp)
#declare MakeSpectrum_Inc_Temp = version;
#version max (3.5, min (3.8, version)); // Bracket the POV version.

#declare MAKESPECTRUM_VERSION = 1.0;
#ifdef (View_POV_Include_Stack)
  #debug "#include \"makespectrum.inc\" //version 1.0\n"
  #debug concat ("  #version ", str (version, 0, 2), ";\n")
#end

// Code for non-linear transfer function:
#declare MAKESP_SRGB = -999;

#ifndef (MakeSp_Diffuse) #declare MakeSp_Diffuse = 0.6; #end
#ifndef (MakeSp_Gamma) #declare MakeSp_Gamma = MAKESP_SRGB; #end

#include "colors.inc"
#include "math.inc"

//================================= INTERNAL ===================================
// Internal features are not intended for direct use by the end user.

#ifndef (makesp__Debug) #declare makesp__Debug = off; #end
#ifndef (makesp__Diffuse_warned) #declare makesp__Diffuse_warned = no; #end
// This flag is used, rather than MakeSp_Diffuse or MakeSp_Gamma, so
// that the domain checks will know how warnings should be worded:
#declare MAKESP__DEFAULT = -1000;

//------------------- Hue calibration ----------------------

#declare MAKESP__RED = -1.720;
#declare MAKESP__BLUE = 237.012;
#declare MAKESP__VIOLET = 259.783;
#declare MAKESP__MAGENTA = 292.531;

#declare makesp__Hue_to_single_peak = spline
{ natural_spline
  MAKESP__RED, 650,
   -1.249, 647,
   -0.729, 644,
   -0.154, 641,
    0.476, 638,
    1.159, 635,
    1.895, 632,
    2.689, 629,
    3.543, 626,
    4.460, 623,
    5.442, 620,
    6.493, 617,
    7.614, 614,
    8.808, 611,
   10.075, 608,
   11.419, 605,
   12.839, 602,
   14.338, 599,
   15.914, 596,
   17.567, 593,
   19.295, 590,
   21.095, 587,
   22.964, 584,
   24.895, 581,
   26.884, 578,
   28.922, 575,
   31.001, 572,
   33.110, 569,
   35.239, 566,
   37.521, 563,
   40.558, 560,
   41.799, 559,
   43.181, 558,
   44.722, 557,
   46.444, 556,
   48.376, 555,
   50.549, 554,
   53.005, 553,
   55.792, 552,
   58.973, 551,
   62.517, 550,
   66.154, 549,
   69.858, 548,
   73.624, 547,
   77.444, 546,
   81.311, 545,
   85.207, 544,
   89.126, 543,
   93.071, 542,
   97.043, 541,
  101.039, 540,
  105.050, 539,
  109.067, 538,
  113.076, 537,
  117.062, 536,
  120.989, 535,
  124.520, 534,
  127.588, 533,
  130.261, 532,
  132.594, 531,
  134.629, 530,
  136.405, 529,
  137.953, 528,
  139.301, 527,
  140.473, 526,
  141.489, 525,
  142.452, 524,
  143.454, 523,
  144.495, 522,
  145.574, 521,
  146.685, 520,
  147.824, 519,
  151.319, 516,
  154.883, 513,
  158.743, 510,
  162.994, 507,
  167.727, 504,
  173.045, 501,
  179.071, 498,
  185.418, 495,
  191.273, 492,
  196.669, 489,
  201.639, 486,
  206.216, 483,
  210.426, 480,
  214.292, 477,
  217.838, 474,
  221.083, 471,
  224.048, 468,
  226.753, 465,
  229.217, 462,
  231.457, 459,
  233.492, 456,
  235.337, 453,
  MAKESP__BLUE, 450
}

#declare makesp__Hue_to_violet = spline
{ natural_spline
  MAKESP__BLUE, 450,
  237.497, 448,
  237.957, 446,
  238.426, 444,
  238.923, 442,
  239.472, 440,
  240.105, 438,
  240.856, 436,
  242.908, 432,
  246.180, 428,
  251.470, 424,
  MAKESP__VIOLET, 420
}

#declare makesp__Hue_to_purple = spline
{ natural_spline
  MAKESP__VIOLET, 600,
  264.066, 595,
  268.891, 590,
  274.217, 585,
  279.983, 580,
  286.115, 575,
  MAKESP__MAGENTA, 570
}

#declare makesp__Hue_to_magenta = spline
{ natural_spline
  MAKESP__MAGENTA, 1.0,
  295.168, 0.95,
  298.090, 0.90,
  301.317, 0.85,
  304.612, 0.80,
  307.915, 0.75,
  311.225, 0.70,
  317.864, 0.60,
  324.530, 0.50,
  331.225, 0.40,
  337.946, 0.30,
  344.696, 0.20,
  351.474, 0.10,
  MAKESP__RED + 360, 0.0
}

// Compensate for violet drop-off in gray curve: bring back to neutral gray.
#declare makesp__Neutralize = spline
{ natural_spline
  0.0, 0.0000,
  0.1, 0.0001,
  0.2, 0.0004,
  0.4, 0.0016,
  0.6, 0.0041,
  0.8, 0.0089,
  0.9, 0.0123,
  1.0, 0.0167
}

// Even out the non-linearity introduced by the violet drop-off:
#declare makesp__Relevel = spline
{ natural_spline
  0.0000, 0.0,
  0.2025, 0.2,
  0.4042, 0.4,
  0.6049, 0.6,
  0.7046, 0.7,
  0.8037, 0.8,
  0.9022, 0.9,
  1.0000, 1.0
}

//----------------- Metamer calibration --------------------

// These vectors are generated by MakeSp__Work():
// .u = saturation * value; .v = value
// with value increased to compensate for the violet drop-off

#declare makesp__Metadjust_red = spline
{ natural_spline
  MAKESP__RED, <0.7534, 0.7220>,
   -1, <0.8326, 0.8066>,
    0, <0.9210, 0.9056>
}
#declare makesp__Metadjust_orange = spline
{ natural_spline
    0, <0.9210, 0.9056>,
    1, <1.0072, 0.9884>,
    2, <1.0811, 1.0589>,
    4, <1.1991, 1.1699>,
    7, <1.3209, 1.2811>,
   11, <1.4153, 1.3620>,
   16, <1.4674, 1.3987>,
   21, <1.4768, 1.3948>,
   26, <1.4606, 1.3679>,
   30, <1.4357, 1.3365>,
   32, <1.4203, 1.3187>,
   34, <1.4032, 1.2999>,
   35, <1.3942, 1.2902>,
   37, <1.3734, 1.2687>,
   38, <1.3586, 1.2539>,
   40, <1.3243, 1.2201>,
   42, <1.2861, 1.1829>,
   46, <1.2066, 1.1060>,
   50, <1.1295, 1.0321>,
   54, <1.0580, 0.9638>,
   58, <0.9927, 0.9017>,
   60, <0.9623, 0.8730>
}
#declare makesp__Metadjust_chartreuse = spline
{ natural_spline
   60, <0.9623, 0.8730>,
   63, <0.9656, 0.8787>,
   68, <0.9696, 0.8871>,
   76, <0.9719, 0.8973>,
   84, <0.9691, 0.9036>,
   92, <0.9601, 0.9033>,
  100, <0.9449, 0.8960>,
  110, <0.9176, 0.8780>,
  115, <0.9007, 0.8659>,
  120, <0.8819, 0.8525>
}
#declare makesp__Metadjust_seagreen = spline
{ natural_spline
  120, <0.8819, 0.8525>,
  122, <0.9037, 0.8465>,
  125, <0.9376, 0.8366>,
  128, <0.9727, 0.8254>,
  131, <1.0087, 0.8130>,
  134, <1.0450, 0.7997>,
  137, <1.0804, 0.7864>,
  139, <1.1019, 0.7784>,
  141, <1.1187, 0.7721>,
  142, <1.1237, 0.7699>,
  144, <1.1308, 0.7660>,
  147, <1.1345, 0.7572>,
  150, <1.1290, 0.7416>,
  152, <1.1195, 0.7261>,
  154, <1.1057, 0.7072>,
  156, <1.0901, 0.6882>,
  160, <1.0557, 0.6512>,
  164, <1.0188, 0.6159>,
  168, <0.9811, 0.5828>,
  172, <0.9437, 0.5519>,
  176, <0.9075, 0.5234>,
  178, <0.8898, 0.5099>,
  179, <0.8810, 0.5032>,
  180, <0.8725, 0.4969>
}
#declare makesp__Metadjust_cerulean = spline
{ natural_spline
  180, <0.8725, 0.4969>,
  185, <0.9024, 0.5390>,
  190, <0.9324, 0.5839>,
  195, <0.9623, 0.6316>,
  200, <0.9914, 0.6816>,
  205, <1.0192, 0.7336>,
  210, <1.0448, 0.7869>,
  215, <1.0668, 0.8403>,
  220, <1.0830, 0.8918>,
  225, <1.0902, 0.9379>,
  230, <1.0823, 0.9726>,
  233, <1.0661, 0.9833>,
  235, <1.0483, 0.9837>,
  236, <1.0365, 0.9812>,
  MAKESP__BLUE, <1.0226, 0.9765>
}
#declare makesp__Metadjust_blue = spline
{ natural_spline
  MAKESP__BLUE, <1.0226, 0.9765>,
  238, <1.0025, 0.9668>,
  239, <0.9779, 0.9532>,
  240, <0.9521, 0.9392>
}
#declare makesp__Metadjust_violet = spline
{ natural_spline
  240, <0.9521, 0.9392>,
  241, <0.9425, 0.9267>,
  243, <0.9276, 0.9065>,
  246, <0.9119, 0.8849>,
  250, <0.8976, 0.8643>,
  255, <0.8852, 0.8455>,
  MAKESP__VIOLET, <0.8760, 0.8308>
}
#declare makesp__Metadjust_purple = spline
{ natural_spline
  MAKESP__VIOLET, <0.8760, 0.8308>,
  268, <0.8815, 0.8294>,
  276, <0.8839, 0.8278>,
  283, <0.8832, 0.8260>,
  287, <0.8814, 0.8247>,
  290, <0.8793, 0.8238>,
  MAKESP__MAGENTA, <0.8769, 0.8229>
}
#declare makesp__Metadjust_magenta = spline
{ natural_spline
  MAKESP__MAGENTA, <0.8769, 0.8229>,
  296, <0.8214, 0.7688>,
  298, <0.7926, 0.7408>,
  300, <0.7667, 0.7157>
}
#declare makesp__Metadjust_rose = spline
{ linear_spline
  300, <0.7667, 0.7157>,
  329, <0.7600, 0.7189>,
  360 + MAKESP__RED, <0.7534, 0.7220>
}

//-------------------- RGB Parameters ----------------------

#declare MAKESP__BMIN = 0.0221;
#declare MAKESP__GMIN = 0.0340;
#declare MAKESP__RMIN = 0.0167;
#declare MAKESP__BLONG = 477.8;
#declare MAKESP__GSHORT = 502.9;
#declare MAKESP__GLONG = 575.9;
#declare MAKESP__RSHORT = 609.4;

//----------------------------------------------------------
// Checks that a parameter is with range 0...1.
// s_Caller - the name of the calling macro
// Value    - the value to be range checked
// s_Id     - the name of the value to be ranged checked
#macro MakeSp__Check_0_1 (s_Caller, Value, s_Id)
  #if (Value < 0 | Value > 1)
    #warning concat
    ( s_Caller, "(): ", s_Id, " ", str (Value, 0, 3),
      " should be between 0 and 1."
    )
  #end
#end

//----------------------------------------------------------
// Returns the color argument as a 3-D vector.
#macro MakeSp__Check_color (s_Caller, Color, s_yComponent, s_zComponent)
  #local makesp_C = <0,0,0> + Color;
  #if (makesp_C.y < 0 | makesp_C.y > 1)
    #warning concat
    ( s_Caller, " (<", vstr (3, makesp_C, ", ", 0, 3), ">): ", s_yComponent,
      " should be between 0 and 1."
    )
  #end
  #if (makesp_C.z < 0 | makesp_C.z > 1)
    #warning concat
    ( s_Caller, " (<", vstr (3, makesp_C, ", ", 0, 3), ">): ", s_zComponent,
      " should be between 0 and 1."
    )
  #end
  makesp_C
#end

//----------------------------------------------------------
// Returns the diffuse value.
#macro MakeSp__Check_diffuse (s_Caller, Diffuse)
( #if (Diffuse != MAKESP__DEFAULT)
    #if (Diffuse <= 0 | Diffuse > 1)
      #warning concat
      ( s_Caller, "(): ", str (Diffuse, 0, 6),
        " was given for Diffuse; it should be a positive number not exceeding 1."
      )
    #end
    Diffuse
  #else
    #if (MakeSp_Diffuse > 0 & MakeSp_Diffuse <= 1)
      #declare makesp__Diffuse_warned = no; // Reset warning status.
    #else #if (!makesp__Diffuse_warned)
      #warning concat
      ( s_Caller, "(): ", "MakeSp_Diffuse was set to ",
        str (MakeSp_Diffuse, 0, 6),
        "; it should be a positive number not exceeding 1."
      )
      #declare makesp__Diffuse_warned = yes; // Warn only once.
    #end
    #end
    MakeSp_Diffuse
  #end
)
#end

//----------------------------------------------------------
// Returns the color argument as a 3-D vector.
#macro MakeSp__Check_RGB (s_Caller, Color)
  #local makesp_C = <0,0,0> + Color;
  #if (VMin (makesp_C) < 0 | VMax (makesp_C) > 1)
    #warning concat
    ( s_Caller, " (<", vstr (3, makesp_C, ", ", 0, 3),
      ">): a color channel is out of range."
    )
  #end
  makesp_C
#end

//----------------------------------------------------------
// Returns a gamma-decoded color as a 3-D vector; input must be RGB.
// Gamma is domain-checked.
#macro MakeSp__Decode_gamma (s_Caller, Color, Gamma)
  #local makesp_C = <0,0,0> + Color;
  #local makesp_G = (Gamma = MAKESP__DEFAULT? MakeSp_Gamma: Gamma);
  // Temporarily make all RGB channels non-negative:
  #local makesp_v_Sgn =
    <sgn (makesp_C.red), sgn (makesp_C.green), sgn (makesp_C.blue)>;
  #local makesp_nnC = makesp_C * makesp_v_Sgn; // non-negative
  #switch (makesp_G)
    #case (MAKESP_SRGB)
      #local makesp_Cadj =
      < MakeSp__fn_sRGB (makesp_nnC.red),
        MakeSp__fn_sRGB (makesp_nnC.green),
        MakeSp__fn_sRGB (makesp_nnC.blue)
      >;
      #break
    #else #if (makesp_G > 0)
      #local makesp_Cadj =
      < pow (makesp_nnC.red, makesp_G),
        pow (makesp_nnC.green, makesp_G),
        pow (makesp_nnC.blue, makesp_G)
      >;
    #else
      #error concat
      ( s_Caller, "(): ",
        #if (Gamma = MAKESP__DEFAULT)
          "MakeSp_Gamma was set to ", str (MakeSp_Gamma, 0, 6),
          ".  MakeSp_Gamma must be positive or MAKESP_SRGB."
        #else
          "Non-positive Gamma ", str (Gamma, 0, 6),
          " was given.  Gamma must be positive or MAKESP_SRGB."
        #end
      )
    #end
  #end
  #local makesp_Cadj = makesp_Cadj * makesp_v_Sgn; // Restore channel signs.
  #if (makesp__Debug)
    #debug concat
    ( makesp_This, "():\n  uncorrected makesp_C = rgb <",
      vstr (3, makesp_C, ", ", 0, 4), ">\n  ", str (makesp_G, 0, 1),
      "-adjusted makesp_Cadj = <", vstr (3, makesp_Cadj, ", ", 0, 4), ">\n"
    )
  #end
  makesp_Cadj
#end

//----------------------------------------------------------
// A parabolic transition from one level to another,
// to be used inside a spline definition.
#macro MakeSp__Transition (Short, yShort, Long, yLong)
  #local makesp_X = ceil (Short);
  #while (makesp_X <= Long)
    makesp_X,
    (  (yLong - yShort) * MakeSp__fn_Wave ((makesp_X - Short)
     / (Long - Short) - 1)
     + yShort
    )
    #local makesp_X = makesp_X + 1;
  #end
#end

//----------------------------------------------------------
// Silently bring a hue within the spline domains:
//   force the +/- rollover point to our red hue.
#declare MakeSp__fn_Hdomain = function (x)
{ select
  ( x - MAKESP__RED,
    x + 360,
    select (x - 360 - MAKESP__RED, x, x - 360)
  )
}
#declare MakeSp__fn_Hue = function (x)
{ MakeSp__fn_Hdomain (mod (x, 360))
}

//----------------------------------------------------------
// Decodes an sRGB channel value to linear.
#declare MakeSp__fn_sRGB = function (x)
{ select
  ( x - 0.04044824, // Microsoft & HP said 0.03928, but the curves don't cross
                    // there.  (See Wikipedia article on sRGB for explanation.)
    x / 12.92,
    pow ((x + 0.055) / 1.055, 2.4)
  )
}

//----------------------------------------------------------
// Like cosine, but rounder.
// A single complete cycle is scaled to fit domain -1...1 and range 0...1.
// Arguments outside the domain return 0.
#declare MakeSp__fn_Wave = function (x)
{ select
  ( x + 0.5,
    select (x + 1, 0, 2 * pow (x + 1, 2)),
    select
    ( x - 0.5,
      1 - 2 * x*x,
      select (x - 1, 2 * pow (x - 1, 2), 0)
    )
  )
}

//~~~~~~~~~~~~~~~~~~~ Low level SPD construction ~~~~~~~~~~~~~~~~~~~~~

//----------------------------------------------------------
// Constructs an RGB spectral curve by compositing red, green, and blue.
// Range checks gamma, diffuse, & output, but not input color.
#macro MakeSp__Compose (s_Caller, Original_color, c_Color, Gamma, Diffuse)
  #local makesp_D = MakeSp__Check_diffuse (s_Caller, Diffuse);
  #local makesp_C = MakeSp__Decode_gamma (s_Caller, c_Color, Gamma);
  #local makesp_Blevel = makesp_D *
  (  makesp_C.blue * (1 - MAKESP__GMIN - MAKESP__RMIN)
   + makesp_C.green * MAKESP__GMIN + makesp_C.red * MAKESP__RMIN
  );
  #local makesp_Glevel = makesp_D *
  (  makesp_C.green * (1 - MAKESP__BMIN - MAKESP__RMIN)
   + makesp_C.blue * MAKESP__BMIN + makesp_C.red * MAKESP__RMIN
  );
  #local makesp_Rlevel = makesp_D *
  (  makesp_C.red * (1 - MAKESP__BMIN - MAKESP__GMIN)
   + makesp_C.blue * MAKESP__BMIN + makesp_C.green * MAKESP__GMIN
  );
  #if (min (makesp_Blevel, makesp_Glevel, makesp_Rlevel) < 0)
    #warning concat
    ( s_Caller, " (<", vstr (3, <0,0,0> + Original_color, ", ", 0, 3),
      ">) has negative reflection at some wavelengths."
    )
  #end
  #if (max (makesp_Blevel, makesp_Glevel, makesp_Rlevel) > 1)
    #warning concat
    ( s_Caller, " (<", vstr (3, <0,0,0> + Original_color, ", ", 0, 3),
      ">) reflects more light than it receives at some wavelengths."
    )
  #end
  spline
  { floor (MAKESP__BLONG), makesp_Blevel
    MakeSp__Transition
    ( MAKESP__BLONG, makesp_Blevel, MAKESP__GSHORT, makesp_Glevel
    )
    ceil (MAKESP__GSHORT), makesp_Glevel
    floor (MAKESP__GLONG), makesp_Glevel
    MakeSp__Transition
    ( MAKESP__GLONG, makesp_Glevel, MAKESP__RSHORT, makesp_Rlevel
    )
    ceil (MAKESP__RSHORT), makesp_Rlevel
  }
#end

//----------------------------------------------------------
// Constructs a spectral curve for a metamer of an RGB color.
#macro MakeSp__Metamer (s_Caller, Original_color, hsv_Color, Gamma, Diffuse)
  #local makesp_D = MakeSp__Check_diffuse (s_Caller, Diffuse);
  #local makesp_Hsv =
    CRGB2HSV (MakeSp__Decode_gamma (s_Caller, CHSV2RGB (hsv_Color), Gamma));
  #local makesp_H = MakeSp__fn_Hue (makesp_Hsv.x);
 // Get metamer adjustment
  #local makesp_Adj =
    #switch (makesp_H)
      #range (MAKESP__RED, 0) makesp__Metadjust_red (makesp_H); #break
      #range (0, 60) makesp__Metadjust_orange (makesp_H); #break
      #range (60, 120) makesp__Metadjust_chartreuse (makesp_H); #break
      #range (120, 180) makesp__Metadjust_seagreen (makesp_H); #break
      #range (180, MAKESP__BLUE) makesp__Metadjust_cerulean (makesp_H); #break
      #range (MAKESP__BLUE, 240) makesp__Metadjust_blue (makesp_H); #break
      #range (240, MAKESP__VIOLET) makesp__Metadjust_violet (makesp_H); #break
      #range (MAKESP__VIOLET, MAKESP__MAGENTA)
        makesp__Metadjust_purple (makesp_H); #break
      #range (MAKESP__MAGENTA, 300) makesp__Metadjust_magenta (makesp_H); #break
      #range (300, 360 + MAKESP__RED) makesp__Metadjust_rose (makesp_H); #break
    #end
 // Create the SPD.  N.B. The 0.9864 compensates for the violet drop-off.
  #local makesp_Term =
    makesp_Adj.u + makesp_Hsv.y - makesp_Adj.v * makesp_Hsv.y;
  #if (0 = makesp_Term)
    #error concat
    ( s_Caller, " (<", vstr (3, <0,0,0> + Original_color, ", ", 0, 3),
      ">): division by zero in metamer calculation; please use a lower saturation."
    )
  #end
  MakeSp__Work
  ( s_Caller, Original_color,
    < makesp_Hsv.x,
      makesp_Hsv.y / makesp_Term,
      makesp_Term * makesp_Hsv.z * makesp_D / (makesp_Adj.u * 0.9864)
    >
  )
#end

//----------------------------------------------------------
// Constructs a spectral curve from non-RGB specifications.
#macro MakeSp__Work (s_Caller, Original_color, hrv_Color)

  #local makesp_Hrv = <0,0,0> + hrv_Color;
  #if (makesp__Debug)
    #local makesp_This = "MakeSp__Work"
    #debug concat
    ( makesp_This, " (\"", s_Caller, "\", <",
      vstr (3, Original_color, ", ", 0, 4), ">, <",
      vstr (3, makesp_Hrv, ", ", 0, 4), ">)\n"
    )
  #end
  #local makesp_Hrv =
    <MakeSp__fn_Hue (makesp_Hrv.x), makesp_Hrv.y, makesp_Hrv.z>;

 //----- Gray parameters -----
  #local makesp_Gray = (1 - makesp_Hrv.y) * makesp_Hrv.z;
  #if (makesp_Gray > 0 & makesp_Gray < 1)
    #local makesp_Leveled = makesp__Relevel (makesp_Gray).y;
  #else
    #local makesp_Leveled = makesp_Gray;
  #end
  #local makesp_VRange = 415 + 20 * makesp_Leveled;
  #local makesp_Dropoff = (1 - pow (1 - makesp_Leveled, 4)) * 0.2;
  #local makesp_Adj = makesp__Neutralize (makesp_Leveled).y;
 //----- Color parameters -----
  #local makesp_Start = 375;
  #local makesp_Color = makesp_Hrv.z - makesp_Gray; // saturated contribution
  #if (makesp_Hrv.x > MAKESP__MAGENTA) // magentas
    #local makesp_Violet = makesp__Hue_to_magenta (makesp_Hrv.x).y;
    #local makesp_Stop = 650;
  #else #if (makesp_Hrv.x > MAKESP__VIOLET) // purples
    #local makesp_Red = makesp__Hue_to_purple (makesp_Hrv.x).y;
    #local makesp_Stop = makesp_Red + 80;
  #else #if (makesp_Hrv.x > MAKESP__BLUE) // violets
    #local makesp_Peak = makesp__Hue_to_violet (makesp_Hrv.x).y;
    #local makesp_RShort = 2 * makesp_Peak - 240;
    #local makesp_RSpan = (makesp_Peak - 360) * 4/3;
    #local makesp_RLevel = sqrt ((450 - makesp_Peak) / 30);
    #local makesp_Stop = makesp_RShort + makesp_RSpan;
  #else // other hues
    #local makesp_Peak = makesp__Hue_to_single_peak (makesp_Hrv.x).y;
    #local makesp_LongFloor =
      (sin (max (-20, min (20, makesp_Peak - 545)) / 40 * pi) + 1) / 2;
    #local makesp_Span =
      #if (makesp_Peak > 515 & makesp_Peak < 545)
        75 + 5 * cos ((makesp_Peak - 515) / 15 * pi);
      #else
        80;
      #end
    #local makesp_Stop =
      makesp_Peak + (makesp_LongFloor = 1? 0: ceil (makesp_Span));
  #end
  #end
  #end

  #if (makesp__Debug)
    #debug concat
    ( "  ", makesp_This, "(): makesp_Gray = ", str (makesp_Gray, 0, 4),
      ", makesp_Color = ", str (makesp_Color, 0, 4), "\n"
    )
  #end

 //----- Create the curve -----
  #local makesp_Optimize = no; // See optimization note below.
  #if (makesp_Optimize) #local makesp_Old = 0; #end
  #local makesp_Hi = false;
  #local makesp_Neg = false;
  spline
  { #local makesp_Wl = makesp_Start;
    #local makesp_Stop = ceil (makesp_Stop);
    #if (makesp__Debug)
      #debug concat
      ( "  ", makesp_This, "(): makesp_Stop = ", str (makesp_Stop, 0, 0)
      )
    #end
    #if (makesp_Gray != 0) #local makesp_Stop = max (makesp_Stop, 700); #end
    #if (makesp__Debug)
      #debug concat (", ", str (makesp_Stop, 0, 0), "\n")
    #end
    #while (makesp_Wl <= makesp_Stop)
     //~~~~~ Gray part ~~~~~
      #local makesp_Gray_part =
        #if (makesp_Wl < makesp_VRange)
          MakeSp__fn_Wave ((makesp_Wl - makesp_VRange) / (makesp_VRange - 375))
          * (makesp_Leveled - makesp_Dropoff) + makesp_Dropoff;
        #else #if (makesp_Wl > 436 & makesp_Wl < 699)
          makesp_Leveled
          - MakeSp__fn_Wave ((makesp_Wl - 567.5) / 131.5) * makesp_Adj;
        #else
          makesp_Leveled;
        #end
        #end
     //~~~~~ Color part ~~~~~
      #if (makesp_Hrv.x > MAKESP__MAGENTA) //------- magentas ---
        #local makesp_Color_part =
          #if (makesp_Wl < 420)
            MakeSp__fn_Wave ((makesp_Wl - 420) / 50) * makesp_Violet;
          #else #if (makesp_Wl < 530)
            MakeSp__fn_Wave ((makesp_Wl - 420) / 110) * makesp_Violet;
          #else
            MakeSp__fn_Wave (min (0, makesp_Wl - 650) / 80);
          #end
          #end
      #else #if (makesp_Hrv.x > MAKESP__VIOLET) //--- purples ---
        #local makesp_Color_part =
          #if (makesp_Wl < 420)
            MakeSp__fn_Wave ((makesp_Wl - 420) / 50);
          #else #if (makesp_Wl < 530)
            MakeSp__fn_Wave ((makesp_Wl - 420) / 110);
          #else #if (makesp_Wl <= makesp_Red)
            0;
          #else
            1 - MakeSp__fn_Wave ((makesp_Wl - makesp_Red) / 80);
          #end
          #end
          #end
      #else #if (makesp_Hrv.x > MAKESP__BLUE) //----- violets ---
        #local makesp_Color_part =
          #if (makesp_Wl < makesp_Peak)
            MakeSp__fn_Wave ((makesp_Wl - makesp_Peak) / (makesp_Peak - 370));
          #else #if (makesp_Wl < 530)
            MakeSp__fn_Wave ((makesp_Wl - makesp_Peak) / (530 - makesp_Peak));
          #else #if (makesp_Wl <= makesp_RShort)
            0;
          #else
            (1 - MakeSp__fn_Wave ((makesp_Wl - makesp_RShort) / makesp_RSpan))
            * makesp_RLevel;
          #end
          #end
          #end
      #else //------------------------------------ other hues ---
        #local makesp_Color_part =
          #if (makesp_Wl < makesp_Peak - makesp_Span)
            0;
          #else #if (makesp_Wl < makesp_Peak)
            MakeSp__fn_Wave ((makesp_Wl - makesp_Peak) / makesp_Span);
          #else #if (makesp_Wl < makesp_Peak + makesp_Span)
              makesp_LongFloor
            + MakeSp__fn_Wave ((makesp_Wl - makesp_Peak) / makesp_Span)
            * (1 - makesp_LongFloor);
          #else
            makesp_LongFloor;
          #end
          #end
          #end
      #end
      #end
      #end
     //~~~~~ Combine gray and color parts ~~~~~
      #if (makesp_Optimize)
        // This (failed) attempt at optimization slows things down:
        #local makesp_New = makesp_Gray_part + makesp_Color_part * makesp_Color;
        #if (makesp_Wl = makesp_Start | makesp_New != makesp_Old)
          makesp_Wl, makesp_New
          #local makesp_Old = makesp_New;
        #end
      #else
        #local Makesp = makesp_Gray_part + makesp_Color_part * makesp_Color;
        #if (Makesp < 0) #local makesp_Neg = true; #end
        #if (Makesp > 1) #local makesp_Hi = true; #end
        makesp_Wl, Makesp
      #end
      #local makesp_Wl = makesp_Wl + 1;
    #end
    #if (makesp_Neg)
      #warning concat
      ( s_Caller, " (<", vstr (3, <0,0,0> + Original_color, ", ", 0, 3),
        ">) has negative reflection at some wavelengths."
      )
    #end
    #if (makesp_Hi)
      #warning concat
      ( s_Caller, " (<", vstr (3, <0,0,0> + Original_color, ", ", 0, 3),
        ">) reflects more light than it receives at some wavelengths."
      )
    #end
  }
#end

//----- End of internal features -----

/*----- Implementation note: where validity checks are made
DOMAIN CHECKS
  Gamma       [F] MakeSp__Compose(), MakeSp__Metamer()
  Diffuse     [W] MakeSp__Compose(), MakeSp__Metamer()
  RGB         [W] high level macros
  Hue         [*] MakeSp__Metamer(), MakeSp__Work(), high level macros
  Richness    [W] MakeSpectrum()
  Saturation  [W] high level macros
  Lightness   [W] high level macros
  Value       [W] MakeSpectrum(), high level macros
RANGE CHECKS
  Array bins  [W] MakeSp_Bins()
  Spline      [W] MakeSp__Compose(), MakeSp__Work()
------------
[F] Fatal parse error
[W] Parse warning
[*] Silent correction
--------------------------*/

//============================== PUBLIC MACROS =================================

#macro MakeSp_Bins (Curve, wlShort, wlLong, Step, Level)
  #local makesp_N = (wlLong - wlShort) / Step + 1;
 // domain checks and sanity checks:
  #local makesp_Min = min (wlShort, wlLong, Step);
  #local makesp_This = concat
  ( "MakeSp_Bins (, ", vstr (4, <wlShort, wlLong, Step, Level>, ", ", 0, 3),
    "): "
  )
  #if (Level = 0)
    #warning
      concat (makesp_This, "Level is zero; the color is completely black.")
  #else #if (Level = 0)
    #warning concat
    ( makesp_This,
      "Level is negative; be prepared for some interesting effects."
    )
  #end
  #end
  #if (floor (makesp_N) != makesp_N)
    #warning
      concat (makesp_This, "Step does not divide evenly into interval size.")
    #local makesp_N = floor (makesp_N);
  #end
  #if (makesp_Min <= 0)
    #error
      concat (makesp_This, "wlShort, wlLong, and Step must all be positive.")
  #end
  #if (makesp_N < 1)
    #error concat (makesp_This, "wlLong is shorter than wlShort.")
  #end
 //
  #local makesp_Hi = false;
  #local makesp_Neg = false;
  #local makesp_Bins = array[makesp_N]
  #local makesp_Wx = 0; // array index
  #local makesp_X2 = Curve (wlShort - Step/2).y;
  #while (makesp_Wx < makesp_N)
    #local makesp_Wl = makesp_Wx * Step + wlShort; // wavelength in nm
   // Find the mean reflection over the range wavelength +/- Step/2 nm,
   // using Simpson's rule:
    #local makesp_X0 = makesp_X2;
    #local makesp_X2 = Curve (makesp_Wl + Step/2).y;
    #local makesp_Bins [makesp_Wx] =
      (makesp_X0 + 4 * Curve (makesp_Wl).y + makesp_X2) / 6 * Level;
    #if (makesp_Bins [makesp_Wx] < 0) #local makesp_Neg = true; #end
    #if (makesp_Bins [makesp_Wx] > 1) #local makesp_Hi = true; #end
    #local makesp_Wx = makesp_Wx + 1;
  #end
  #if (makesp_Neg)
    #warning concat (makesp_This, "Negative reflection at some wavelengths.")
  #end
  #if (makesp_Hi)
    #warning concat (makesp_This, "Reflects more light than it receives at some wavelengths.")
  #end
  makesp_Bins
#end

//----------------------------------------------------------

#macro MakeSp_SpectralRender (Curve, Level)
  MakeSp_Bins (Curve, 380, 730, 10, Level)
#end

//----------------------------------------------------------

#macro MakeSpectrum (hrv_Color, Gamma, Min, Max)
  #local makesp_This = "MakeSpectrum"
  #local makesp_Hrv =
    MakeSp__Check_color (makesp_This, hrv_Color, "richness", "value");
  MakeSp__Check_0_1 (makesp_This, Min, "Min")
  MakeSp__Check_0_1 (makesp_This, Max, "Max")
  #if (Max < Min)
    #warning concat
    ( makesp_This, "(): Max ", str (Max, 0, 3),
      " is less than Min ", str (Min, 0, 3), "."
    )
  #end
  #local makesp_Adj = CRGB2HSV
  ( MakeSp__Decode_gamma (makesp_This, CHSV2RGB (makesp_Hrv), Gamma)
   * (Max - Min) + Min
  );
  #if (makesp__Debug)
    #debug concat
    ( makesp_This, "(<", vstr (3, makesp_Hrv, ", ", 0, 4), ">, ",
      vstr (3, <Gamma, Min, Max>, ", ", 0, 4), "): makesp_Adj = <",
      vstr (3, makesp_Adj, ", ", 0, 4), ">\n"
    )
  #end
  MakeSp__Work (makesp_This, hrv_Color, makesp_Adj)
#end

//----------------------------------------------------------

#macro MakeSp_Meta_HSL (hsl_Color)
  #local makesp_This = "MakeSp_Meta_HSL"
  #local makesp_C =
    MakeSp__Check_color (makesp_This, hsl_Color, "saturation", "lightness");
  MakeSp__Metamer
  ( makesp_This, hsl_Color, CRGB2HSV (CHSL2RGB (makesp_C)),
    MAKESP__DEFAULT, MAKESP__DEFAULT
  )
#end

#macro MakeSp_Meta_HSL_Args (hsl_Color, Gamma, Diffuse)
  #local makesp_This = "MakeSp_Meta_HSL_Args"
  #local makesp_C =
    MakeSp__Check_color (makesp_This, hsl_Color, "saturation", "lightness");
  MakeSp__Metamer
  ( makesp_This, hsl_Color, CRGB2HSV (CHSL2RGB (makesp_C)), Gamma, Diffuse
  )
#end

#macro MakeSp_Meta_HSV (hsv_Color)
  #local makesp_This = "MakeSp_Meta_HSV"
  #local makesp_C =
    MakeSp__Check_color (makesp_This, hsv_Color, "saturation", "value");
  MakeSp__Metamer
  ( makesp_This, hsv_Color, makesp_C, MAKESP__DEFAULT, MAKESP__DEFAULT
  )
#end

#macro MakeSp_Meta_HSV_Args (hsv_Color, Gamma, Diffuse)
  #local makesp_This = "MakeSp_Meta_HSV_Args"
  #local makesp_C =
    MakeSp__Check_color (makesp_This, hsv_Color, "saturation", "value");
  MakeSp__Metamer (makesp_This, hsv_Color, makesp_C, Gamma, Diffuse)
#end

#macro MakeSp_Meta_RGB (c_Color)
  #local makesp_This = "MakeSp_Meta_RGB"
  #local makesp_C = MakeSp__Check_RGB (makesp_This, c_Color);
  MakeSp__Metamer
  ( makesp_This, c_Color, CRGB2HSV (makesp_C), MAKESP__DEFAULT, MAKESP__DEFAULT
  )
#end

#macro MakeSp_Meta_RGB_Args (c_Color, Gamma, Diffuse)
  #local makesp_This = "MakeSp_Meta_RGB_Args"
  #local makesp_C = MakeSp__Check_RGB (makesp_This, c_Color);
  MakeSp__Metamer (makesp_This, c_Color, CRGB2HSV (makesp_C), Gamma, Diffuse)
#end

//----------------------------------------------------------

#macro MakeSp_HSL (hsl_Color)
  #local makesp_This = "MakeSp_HSL"
  #local makesp_Hsl =
    MakeSp__Check_color (makesp_This, hsl_Color, "saturation", "lightness");
  MakeSp__Compose
  ( makesp_This, hsl_Color, CHSL2RGB (makesp_Hsl),
    MAKESP__DEFAULT, MAKESP__DEFAULT
  )
#end

#macro MakeSp_HSL_Args (hsl_Color, Gamma, Diffuse)
  #local makesp_This = "MakeSp_HSL_Args"
  #local makesp_Hsl =
    MakeSp__Check_color (makesp_This, hsl_Color, "saturation", "lightness");
  MakeSp__Compose
  ( makesp_This, hsl_Color, CHSL2RGB (makesp_Hsl), Gamma, Diffuse
  )
#end

#macro MakeSp_HSV (hsv_Color)
  #local makesp_This = "MakeSp_HSV"
  #local makesp_Hsv =
    MakeSp__Check_color (makesp_This, hsv_Color, "saturation", "value");
  MakeSp__Compose
  ( makesp_This, hsv_Color, CHSV2RGB (makesp_Hsv),
    MAKESP__DEFAULT, MAKESP__DEFAULT
  )
#end

#macro MakeSp_HSV_Args (hsv_Color, Gamma, Diffuse)
  #local makesp_This = "MakeSp_HSV_Args"
  #local makesp_Hsv =
    MakeSp__Check_color (makesp_This, hsv_Color, "saturation", "value");
  MakeSp__Compose
  ( makesp_This, hsv_Color, CHSV2RGB (makesp_Hsv), Gamma, Diffuse
  )
#end

#macro MakeSp_RGB (c_Color)
  #local makesp_This = "MakeSp_RGB"
  #local makesp_C = MakeSp__Check_RGB (makesp_This, c_Color);
  MakeSp__Compose
  ( makesp_This, c_Color, c_Color, MAKESP__DEFAULT, MAKESP__DEFAULT
  )
#end

#macro MakeSp_RGB_Args (c_Color, Gamma, Diffuse)
  #local makesp_This = "MakeSp_RGB_Args"
  #local makesp_C = MakeSp__Check_RGB (makesp_This, c_Color);
  MakeSp__Compose (makesp_This, c_Color, c_Color, Gamma, Diffuse)
#end

//----------------------------------------------------------

#version MakeSpectrum_Inc_Temp;
#end
// end of makespectrum.inc
