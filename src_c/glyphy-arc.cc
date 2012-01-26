/*
 * Copyright 2012 Google, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Google Author(s): Behdad Esfahbod, Maysum Panju, Wojciech Baranowski
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glyphy.h>

#include "glyphy-geometry.hh"

using namespace GLyphy::Geometry;



/*
 * Circular arcs
 */


/* Build from a conventional arc representation */
void
glyphy_arc_from_conventional (glyphy_point_t  center,
			      double          radius,
			      double          angle0,
			      double          angle1,
			      glyphy_bool_t   negative,
			      glyphy_arc_t   *arc)
{
  *arc = Arc (Circle (center, radius), angle0, angle1, negative);
};

/* Convert to a conventional arc representation */
void
glyphy_arc_to_conventional (glyphy_arc_t    arc,
			    glyphy_point_t *center /* may be NULL */,
			    double         *radius /* may be NULL */,
			    double         *angle0 /* may be NULL */,
			    double         *angle1 /* may be NULL */,
			    glyphy_bool_t  *negative /* may be NULL */)
{
  Arc a (arc);
  if (radius) *radius = a.radius ();
  if (center || angle0 || angle1) {
    Point c = a.center ();
    if (center) *center = c;
    if (angle0) *angle0 = (a.p0 - c).angle ();
    if (angle1) *angle1 = (a.p1 - c).angle ();
    if (negative) *negative = a.d < 0;
  }
}

glyphy_bool_t
glyphy_arc_is_a_line (glyphy_arc_t arc)
{
  return arc.d == 0;
}



/*
 * Approximate single pieces of geometry to/from one arc
 */


void
glyph_arc_from_line (glyphy_point_t  p0,
		     glyphy_point_t  p1,
		     glyphy_arc_t   *arc)
{
  *arc = Arc (p0, p1, 0);
}

void
glyph_arc_from_conic (glyphy_point_t  p0,
		      glyphy_point_t  p1,
		      glyphy_point_t  p2,
		      glyphy_arc_t   *arc,
		      double         *error)
{
  /* Port this to go through the bezier code */
  Point m = (Point (p0).midpoint (p1)).midpoint (Point (p1).midpoint (p2));
  *arc = Arc (p0, p2, m, false);
  *error = 0; /*XXXXXXXXXXXX */
}

void
glyph_arc_from_cubic (glyphy_point_t  p0,
		      glyphy_point_t  p1,
		      glyphy_point_t  p2,
		      glyphy_point_t  p3,
		      glyphy_arc_t   *arc,
		      double         *error)
{
  Bezier b (p0, p1, p2, p3);
  *arc = Arc (p0, p3, b.point (.5), false);
  *error = 0; /* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
}

void
glyph_arc_to_cubic (glyphy_arc_t    arc,
		    glyphy_point_t *p0,
		    glyphy_point_t *p1,
		    glyphy_point_t *p2,
		    glyphy_point_t *p3,
		    double         *error)
{
  Bezier b = Arc (arc).approximate_bezier (error);
  *p0 = arc.p0;
  *p1 = b.p1;
  *p2 = b.p2;
  *p3 = arc.p1;
}