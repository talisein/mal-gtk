#include "malgtk_cellrenderer_score.h"

struct _MalgtkCellRendererScorePrivate
{
    gint           score;
    GtkListStore *liststore;
};
typedef struct _MalgtkCellRendererScorePrivate MalgtkCellRendererScorePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (MalgtkCellRendererScore, malgtk_cell_renderer_score, GTK_TYPE_CELL_RENDERER_COMBO)


/* Some boring function declarations: GObject type system stuff */
static void     malgtk_cell_renderer_score_init            (MalgtkCellRendererScore      *cellscore);

static void     malgtk_cell_renderer_score_class_init      (MalgtkCellRendererScoreClass *klass);

static void     malgtk_cell_renderer_score_get_property    (GObject                      *object,
                                                            guint                         param_id,
                                                            GValue                       *value,
                                                            GParamSpec                   *pspec);

static void     malgtk_cell_renderer_score_set_property    (GObject                      *object,
                                                            guint                         param_id,
                                                            const GValue                 *value,
                                                            GParamSpec                   *pspec);

static void     malgtk_cell_renderer_score_finalize        (GObject                      *gobject);

static void     malgtk_cell_renderer_score_dispose         (GObject                      *gobject);

static void     malgtk_cell_renderer_score_notify_score_cb (GObject                      *obj,
                                                            GParamSpec                   *pspec,
                                                            gpointer                      user_data);

enum
{
  PROP_SCORE = 1,
};

enum {
    COLUMN_IDX,
    COLUMN_TXT,
    N_COLUMNS
};

static const char *malgtk_cell_renderer_score_array[] = {
    "\u2012", "1", "2", "3", "4",
    "5", "6", "7", "8", "9", "10"
};

#define SCORE_ARRAY_SIZE (G_N_ELEMENTS(malgtk_cell_renderer_score_array))

static void
malgtk_cell_renderer_score_init (MalgtkCellRendererScore *cellscore)
{
    MalgtkCellRendererScorePrivate *priv = malgtk_cell_renderer_score_get_instance_private(cellscore);
    priv->score = 0;
    priv->liststore = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, G_TYPE_STRING);
    GtkListStore *store = priv->liststore;

    for (unsigned int i = 0; i < SCORE_ARRAY_SIZE; ++i) {
        gtk_list_store_insert_with_values(store, NULL, -1,
                                          COLUMN_IDX, i,
                                          COLUMN_TXT, malgtk_cell_renderer_score_array[i],
                                          -1);
    }

    g_object_set(cellscore,
                 "has-entry", FALSE,
                 "model", store,
                 "text-column", COLUMN_TXT,
                 "editable", TRUE,
                 NULL);
    
    gtk_cell_renderer_set_alignment( GTK_CELL_RENDERER(cellscore), 0.5, 0.5 );
    g_signal_connect(cellscore, "notify::score",
                     G_CALLBACK(malgtk_cell_renderer_score_notify_score_cb), store);
}

static void
malgtk_cell_renderer_score_notify_score_cb(GObject *obj, GParamSpec *pspec, gpointer user_data)
{
    MalgtkCellRendererScore *cellscore = MALGTK_CELL_RENDERER_SCORE(obj);
    gint score;

    g_object_get(cellscore, "score", &score, NULL);

    if (((guint)score) < SCORE_ARRAY_SIZE) {
        g_object_set(cellscore, "text", malgtk_cell_renderer_score_array[(guint)score], NULL);
    }
}

gint
malgtk_cell_renderer_score_get_score_from_string(const gchar *str)
{
    if (g_ascii_isdigit(*str)) {
        guint64 idx = g_ascii_strtoull(str, NULL, 10);
        if (idx < SCORE_ARRAY_SIZE)
            return (gint)idx;
        else
            return 10;
    } else {
        return 0;
    }
}

static void
malgtk_cell_renderer_score_class_init (MalgtkCellRendererScoreClass *klass)
{
  GObjectClass         *object_class = G_OBJECT_CLASS(klass);
  object_class->finalize = malgtk_cell_renderer_score_finalize;
  object_class->dispose = malgtk_cell_renderer_score_dispose;

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
malgtk_cell_renderer_score_dispose (GObject *object)
{
    MalgtkCellRendererScore *cellscore = MALGTK_CELL_RENDERER_SCORE(object);
    MalgtkCellRendererScorePrivate *priv = malgtk_cell_renderer_score_get_instance_private(cellscore);
    g_clear_object(&priv->liststore);

    G_OBJECT_CLASS (malgtk_cell_renderer_score_parent_class)->dispose(object);
}

static void
malgtk_cell_renderer_score_finalize (GObject *object)
{
    G_OBJECT_CLASS (malgtk_cell_renderer_score_parent_class)->finalize(object);
}

static void
malgtk_cell_renderer_score_get_property (GObject    *object,
                                            guint       param_id,
                                            GValue     *value,
                                            GParamSpec *psec)
{
  MalgtkCellRendererScore  *cellscore = MALGTK_CELL_RENDERER_SCORE(object);
  MalgtkCellRendererScorePrivate *priv = malgtk_cell_renderer_score_get_instance_private(cellscore);

  switch (param_id)
  {
    case PROP_SCORE:
      g_value_set_int(value, priv->score);
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
  MalgtkCellRendererScorePrivate *priv = malgtk_cell_renderer_score_get_instance_private(cellscore);

  switch (param_id)
  {
    case PROP_SCORE:
      priv->score = g_value_get_int(value);
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
    MalgtkCellRendererScorePrivate *priv = malgtk_cell_renderer_score_get_instance_private(cellscore);
    return priv->score;
}
