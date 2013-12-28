#include "malgtk_cellrenderer_score.h"

/* Some boring function declarations: GObject type system stuff */

static void     malgtk_cell_renderer_score_init       (MalgtkCellRendererScore      *cellscore);

static void     malgtk_cell_renderer_score_class_init (MalgtkCellRendererScoreClass *klass);

static void     malgtk_cell_renderer_score_get_property  (GObject                    *object,
                                                             guint                       param_id,
                                                             GValue                     *value,
                                                             GParamSpec                 *pspec);

static void     malgtk_cell_renderer_score_set_property  (GObject                    *object,
                                                             guint                       param_id,
                                                             const GValue               *value,
                                                             GParamSpec                 *pspec);

static void     malgtk_cell_renderer_score_finalize (GObject *gobject);

enum
{
  PROP_SCORE = 1,
};

G_DEFINE_TYPE (MalgtkCellRendererScore, malgtk_cell_renderer_score, GTK_TYPE_CELL_RENDERER_COMBO)

static void
malgtk_cell_renderer_score_init (MalgtkCellRendererScore *cellrendererscore)
{
}


static void
malgtk_cell_renderer_score_class_init (MalgtkCellRendererScoreClass *klass)
{
  GObjectClass         *object_class = G_OBJECT_CLASS(klass);
  object_class->finalize = malgtk_cell_renderer_score_finalize;

  /* Hook up functions to set and get our
   *   malgtk cell renderer properties */
  object_class->get_property = malgtk_cell_renderer_score_get_property;
  object_class->set_property = malgtk_cell_renderer_score_set_property;

  /* Install our very own properties */
  g_object_class_install_property (object_class,
                                   PROP_SCORE,
                                   g_param_spec_int ("score",
                                                        "Score",
                                                         "The score to display",
                                                         0, 10, 0,
                                                         G_PARAM_READWRITE));
}

static void
malgtk_cell_renderer_score_finalize (GObject *object)
{

    /*MalgtkCellRendererScore *cellrendererscore = MALGTK_CELL_RENDERER_SCORE(object);*/

  G_OBJECT_CLASS (malgtk_cell_renderer_score_parent_class)->finalize(object);
}

static void
malgtk_cell_renderer_score_get_property (GObject    *object,
                                            guint       param_id,
                                            GValue     *value,
                                            GParamSpec *psec)
{
  MalgtkCellRendererScore  *cellscore = MALGTK_CELL_RENDERER_SCORE(object);

  switch (param_id)
  {
    case PROP_SCORE:
      g_value_set_int(value, cellscore->score);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, psec);
      break;
  }
}

static void
malgtk_cell_renderer_score_set_property (GObject      *object,
                                            guint         param_id,
                                            const GValue *value,
                                            GParamSpec   *pspec)
{
  MalgtkCellRendererScore *cellscore = MALGTK_CELL_RENDERER_SCORE (object);

  switch (param_id)
  {
    case PROP_SCORE:
      cellscore->score = g_value_get_int(value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
  }
}

GtkCellRenderer *
malgtk_cell_renderer_score_new (void)
{
  return g_object_new(MALGTK_TYPE_CELL_RENDERER_SCORE, NULL);
}

gint
malgtk_cell_renderer_score_get_score(const MalgtkCellRendererScore* cellscore)
{
    return cellscore->score;
}
