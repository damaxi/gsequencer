#include "ags_parameter.h"

#include <stdlib.h>
#include <gobject/gvaluecollector.h>

static GParamSpecPool *pspec_pool = NULL;

void
ags_parameter_grow(GType type,
		   guint n_params, GParameter *src,
		   guint *new_n_params, GParameter **dst,
		   const gchar *first_property_name, ...)
{
  GParameter *new_parameter;
  GParamSpec *pspec;
  gchar *property_name;
  GValue value = {0,};
  guint i, count;
  va_list ap;
  gchar *error = NULL;

  if(first_property_name == NULL){
    *new_n_params = 0;
    *dst = NULL;
    return;
  }

  if(pspec_pool == NULL){
    pspec_pool = g_param_spec_pool_new(TRUE);
  }

  /* count the new properties */
  va_start(ap, first_property_name);

  for(i = 0; ; ){
    va_arg(ap, void);
    i++;

    property_name = va_arg(ap, gchar *);
    
    if(property_name == NULL){
      break;
    }
  }
    
  va_end(ap);
  count = i;

  /* create new parameter array */
  new_parameter = (GParameter *) malloc((i + n_params) * sizeof(GParameter));

  /* add the existing parameters */
  for(i = 0; i < n_params; i++){
    new_parameter[i].name = src[i].name;
    g_value_init(&(new_parameter[i].value), G_TYPE_NONE);
    g_value_copy(&(src[i].value),
		 &(new_parameter[i].value));
  }

  /* add the new parameters */
  count += n_params;
  va_start(ap, first_property_name);
  property_name = first_property_name;

  for(i = n_params; i < count; i++){
    pspec = g_param_spec_pool_lookup(pspec_pool,
				     property_name,
				     type,
				     TRUE);
    if (pspec == NULL){
      g_warning ("%s: object class `%s' has no property named `%s'",
		 G_STRFUNC,
		 g_type_name(type),
		 property_name);
      break;
    }
 
    new_parameter[i].name = property_name;

    va_arg(ap, void);
    G_VALUE_COLLECT_INIT(&(new_parameter[i].value), pspec->value_type,
    			 ap, 0, &error);
    

    property_name = va_arg(ap, gchar *);
  }
    
  va_end(ap);

  *new_n_params = count;
  *dst = new_parameter;
}
