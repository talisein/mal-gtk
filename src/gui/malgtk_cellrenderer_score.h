#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MALGTK_TYPE_CELL_RENDERER_SCORE             (malgtk_cell_renderer_score_get_type ())
G_DECLARE_DERIVABLE_TYPE(MalgtkCellRendererScore, malgtk_cell_renderer_score, MALGTK, CELL_RENDERER_SCORE, GtkCellRendererCombo)

struct _MalgtkCellRendererScoreClass
{
    GtkCellRendererComboClass  parent_class;
};

GtkCellRenderer     *malgtk_cell_renderer_score_new (void);

gint malgtk_cell_renderer_score_get_score(const MalgtkCellRendererScore* cellscore);

gint malgtk_cell_renderer_score_get_score_from_string(const gchar *str);

G_END_DECLS
