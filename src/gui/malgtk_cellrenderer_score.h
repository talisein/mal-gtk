#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MALGTK_TYPE_CELL_RENDERER_SCORE             (malgtk_cell_renderer_score_get_type ())
#define MALGTK_CELL_RENDERER_SCORE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),  MALGTK_TYPE_CELL_RENDERER_SCORE, MalgtkCellRendererScore))
#define MALGTK_CELL_RENDERER_SCORE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  MALGTK_TYPE_CELL_RENDERER_SCORE, MalgtkCellRendererScoreClass))
#define MALGTK_IS_CELL_RENDERER_SCORE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MALGTK_TYPE_CELL_RENDERER_SCORE))
#define MALGTK_IS_CELL_RENDERER_SCORE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  MALGTK_TYPE_CELL_RENDERER_SCORE))
#define MALGTK_CELL_RENDERER_SCORE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  MALGTK_TYPE_CELL_RENDERER_SCORE, MalgtkCellRendererScoreClass))

typedef struct _MalgtkCellRendererScore MalgtkCellRendererScore;
typedef struct _MalgtkCellRendererScoreClass MalgtkCellRendererScoreClass;

/* MalgtkCellRendererScore: Our malgtk cell renderer
 *   structure. Extend according to need */

struct _MalgtkCellRendererScore
{
    GtkCellRendererCombo   parent;

    gint           score;
};


struct _MalgtkCellRendererScoreClass
{
    GtkCellRendererComboClass  parent_class;
};


GType                malgtk_cell_renderer_score_get_type (void) G_GNUC_CONST;

GtkCellRenderer     *malgtk_cell_renderer_score_new (void);

gint malgtk_cell_renderer_score_get_score(const MalgtkCellRendererScore* cellscore);

G_END_DECLS
