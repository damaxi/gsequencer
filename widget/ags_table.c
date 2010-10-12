#include "ags_table.h"

void ags_table_class_init(AgsTableClass *table);
void ags_table_init(AgsTable *table);

GType
ags_table_get_type(void)
{
  static GType ags_type_table = 0;

  if(!ags_type_table){
    static const GTypeInfo ags_table_info = {
      sizeof(AgsTableClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_table_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsTable),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_table_init,
    };

    ags_type_table = g_type_register_static(GTK_TYPE_TABLE,
					    "AgsTable\0", &ags_table_info,
					    0);
  }

  return(ags_type_table);
}

void
ags_table_class_init(AgsTableClass *table)
{
}

void
ags_table_init(AgsTable *table)
{
}

AgsTable*
ags_table_new(guint row, guint columns, gboolean homogeneous)
{
  AgsTable *table;

  table = (AgsTable *) g_object_new(AGS_TYPE_TABLE,
				    "n-rows\0", row,
				    "n-columns\0", columns,
				    "homogeneous\0", homogeneous,
				    NULL);
  
  return(table);
}
