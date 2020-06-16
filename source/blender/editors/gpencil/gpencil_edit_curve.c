/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2008, Blender Foundation
 * This is a new part of Blender
 * Operators for editing Grease Pencil strokes
 */

/** \file
 * \ingroup edgpencil
 */

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MEM_guardedalloc.h"

#include "DNA_gpencil_types.h"
#include "DNA_view3d_types.h"

#include "BKE_context.h"
#include "BKE_gpencil.h"
#include "BKE_gpencil_curve.h"
#include "BKE_gpencil_geom.h"

#include "BLI_listbase.h"
#include "BLI_math.h"

#include "RNA_access.h"
#include "RNA_define.h"

#include "WM_api.h"
#include "WM_types.h"

#include "ED_gpencil.h"

#include "DEG_depsgraph.h"

#include "gpencil_intern.h"

/* -------------------------------------------------------------------- */
/** \name Test Operator for curve editing
 * \{ */

static int gp_write_stroke_curve_data_exec(bContext *C, wmOperator *op)
{
  Object *ob = CTX_data_active_object(C);
  bGPdata *gpd = ob->data;

  // int num_points = RNA_int_get(op->ptr, "num_points");

  if (ELEM(NULL, gpd)) {
    return OPERATOR_CANCELLED;
  }

  bGPDlayer *gpl = BKE_gpencil_layer_active_get(gpd);
  bGPDframe *gpf = gpl->actframe;
  if (ELEM(NULL, gpf)) {
    return OPERATOR_CANCELLED;
  }

  LISTBASE_FOREACH (bGPDstroke *, gps, &gpf->strokes) {
    if (gps->flag & GP_STROKE_SELECT) {
      if (gps->editcurve != NULL) {
        BKE_gpencil_free_stroke_editcurve(gps);
      }
      BKE_gpencil_stroke_editcurve_update(gps, gpd->curve_edit_threshold);
      if (gps->editcurve != NULL) {
        gps->editcurve->resolution = gpd->editcurve_resolution;
      }
    }
  }

  /* notifiers */
  DEG_id_tag_update(&gpd->id, ID_RECALC_TRANSFORM | ID_RECALC_GEOMETRY);
  WM_event_add_notifier(C, NC_GPENCIL | ND_DATA | NA_EDITED, NULL);

  return OPERATOR_FINISHED;
}

void GPENCIL_OT_write_sample_stroke_curve_data(wmOperatorType *ot)
{
  // PropertyRNA *prop;

  /* identifiers */
  ot->name = "Write sample stroke curve data";
  ot->idname = "GPENCIL_OT_write_stroke_curve_data";
  ot->description =
      "Test operator to write sample curve data to the selected grease pencil strokes";

  /* api callbacks */
  ot->exec = gp_write_stroke_curve_data_exec;
  ot->poll = gp_active_layer_poll;

  /* flags */
  ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

  /* properties */
  // prop = RNA_def_int(
  //     ot->srna, "num_points", 2, 0, 100, "Curve points", "Number of test curve points", 0, 100);
}

static int gp_stroke_enter_editcurve_mode(bContext *C, wmOperator *op)
{
  Object *ob = CTX_data_active_object(C);
  bGPdata *gpd = ob->data;

  float error_threshold = RNA_float_get(op->ptr, "error_threshold");
  gpd->curve_edit_threshold = error_threshold;

  if (ELEM(NULL, gpd)) {
    return OPERATOR_CANCELLED;
  }

  const bool is_multiedit = (bool)GPENCIL_MULTIEDIT_SESSIONS_ON(gpd);

  LISTBASE_FOREACH (bGPDlayer *, gpl, &gpd->layers) {
    LISTBASE_FOREACH (bGPDframe *, gpf, &gpl->frames) {
      if (gpf == gpl->actframe) {
        LISTBASE_FOREACH (bGPDstroke *, gps, &gpf->strokes) {
          /* only allow selected and non-converted strokes to be transformed */
          if (gps->flag & GP_STROKE_SELECT && gps->editcurve == NULL) {
            BKE_gpencil_stroke_editcurve_update(gps, gpd->curve_edit_threshold);
            if (gps->editcurve != NULL) {
              gps->editcurve->resolution = gpd->editcurve_resolution;
              gps->editcurve->flag |= GP_CURVE_RECALC_GEOMETRY;
            }
            BKE_gpencil_stroke_geometry_update(gps);
          }
        }
      }
    }
  }

  /* notifiers */
  DEG_id_tag_update(&gpd->id, ID_RECALC_TRANSFORM | ID_RECALC_GEOMETRY);
  WM_event_add_notifier(C, NC_GPENCIL | ND_DATA | NA_EDITED, NULL);

  return OPERATOR_FINISHED;
}

void GPENCIL_OT_stroke_enter_editcurve_mode(wmOperatorType *ot)
{
  PropertyRNA *prop;

  /* identifiers */
  ot->name = "Enter curve edit mode";
  ot->idname = "GPENCIL_OT_stroke_enter_editcurve_mode";
  ot->description = "Called to transform a stroke into a curve";

  /* api callbacks */
  ot->exec = gp_stroke_enter_editcurve_mode;
  ot->poll = gp_active_layer_poll;

  /* flags */
  ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

  /* properties */
  prop = RNA_def_float(ot->srna,
                       "error_threshold",
                       0.1f,
                       FLT_MIN,
                       100.0f,
                       "Error Threshold",
                       "Threshold on the maximum deviation from the actual stroke",
                       FLT_MIN,
                       10.f);
  RNA_def_property_ui_range(prop, FLT_MIN, 10.0f, 0.1f, 5);
}

/** \} */
