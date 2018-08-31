/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/audio/ags_pattern.h>

#include <ags/libags.h>

#include <ags/audio/ags_port.h>

#include <stdarg.h>
#include <math.h>
#include <string.h>

void ags_pattern_class_init(AgsPatternClass *pattern_class);
void ags_pattern_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pattern_tactable_interface_init(AgsTactableInterface *tactable);
void ags_pattern_init(AgsPattern *pattern);
void ags_pattern_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_pattern_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
void ags_pattern_dispose(GObject *gobject);
void ags_pattern_finalize(GObject *gobject);

AgsUUID* ags_pattern_get_uuid(AgsConnectable *connectable);
gboolean ags_pattern_has_resource(AgsConnectable *connectable);
gboolean ags_pattern_is_ready(AgsConnectable *connectable);
void ags_pattern_add_to_registry(AgsConnectable *connectable);
void ags_pattern_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_pattern_list_resource(AgsConnectable *connectable);
xmlNode* ags_pattern_xml_compose(AgsConnectable *connectable);
void ags_pattern_xml_parse(AgsConnectable *connectable,
			  xmlNode *node);
gboolean ags_pattern_is_connected(AgsConnectable *connectable);
void ags_pattern_connect(AgsConnectable *connectable);
void ags_pattern_disconnect(AgsConnectable *connectable);

void ags_pattern_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm);

/**
 * SECTION:ags_pattern
 * @short_description: Pattern representing tones
 * @title: AgsPattern
 * @section_id:
 * @include: ags/audio/ags_pattern.h
 *
 * #AgsPattern represents an audio pattern of tones.
 */

enum{
  PROP_0,
  PROP_PORT,
  PROP_FIRST_INDEX,
  PROP_SECOND_INDEX,
  PROP_OFFSET,
  PROP_CURRENT_BIT,
  PROP_TIMESTAMP,
};

static gpointer ags_pattern_parent_class = NULL;

static pthread_mutex_t ags_pattern_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_pattern_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pattern;

    static const GTypeInfo ags_pattern_info = {
      sizeof (AgsPatternClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pattern_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPattern),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pattern_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pattern_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_tactable_interface_info = {
      (GInterfaceInitFunc) ags_pattern_tactable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pattern = g_type_register_static(G_TYPE_OBJECT,
					      "AgsPattern",
					      &ags_pattern_info,
					      0);

    g_type_add_interface_static(ags_type_pattern,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_pattern,
				AGS_TYPE_TACTABLE,
				&ags_tactable_interface_info);
  }

  return g_define_type_id__volatile;
}

void
ags_pattern_class_init(AgsPatternClass *pattern)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_pattern_parent_class = g_type_class_peek_parent(pattern);

  gobject = (GObjectClass *) pattern;

  gobject->set_property = ags_pattern_set_property;
  gobject->get_property = ags_pattern_get_property;

  gobject->dispose = ags_pattern_dispose;
  gobject->finalize = ags_pattern_finalize;

  /* properties */
  /**
   * AgsPattern:port:
   *
   * The pattern's port.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("port",
				   "port of pattern",
				   "The port of pattern",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsPattern:first-index:
   *
   * Selected bank 0.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("first-index",
				 "the first index",
				 "The first index to select pattern",
				 0, 256,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIRST_INDEX,
				  param_spec);

  /**
   * AgsPattern:second-index:
   *
   * Selected bank 1.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("second-index",
				 "the second index",
				 "The second index to select pattern",
				 0, 256,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SECOND_INDEX,
				  param_spec);

  /**
   * AgsPattern:offset:
   *
   * Position of pattern.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("offset",
				 "the offset",
				 "The offset within the pattern",
				 0, 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OFFSET,
				  param_spec);

  /**
   * AgsPattern:current-bit:
   *
   * Offset of current position.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_boolean("current-bit",
				    "current bit for offset",
				    "The current bit for offset",
				    FALSE,
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_CURRENT_BIT,
				  param_spec);

  /**
   * AgsPattern:timestamp:
   *
   * The pattern's timestamp.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("timestamp",
				   "timestamp of pattern",
				   "The timestamp of pattern",
				   AGS_TYPE_TIMESTAMP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);
}

void
ags_pattern_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_pattern_get_uuid;
  connectable->has_resource = ags_pattern_has_resource;

  connectable->is_ready = ags_pattern_is_ready;
  connectable->add_to_registry = ags_pattern_add_to_registry;
  connectable->remove_from_registry = ags_pattern_remove_from_registry;

  connectable->list_resource = ags_pattern_list_resource;
  connectable->xml_compose = ags_pattern_xml_compose;
  connectable->xml_parse = ags_pattern_xml_parse;

  connectable->is_connected = ags_pattern_is_connected;  
  connectable->connect = ags_pattern_connect;
  connectable->disconnect = ags_pattern_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_pattern_tactable_interface_init(AgsTactableInterface *tactable)
{
  tactable->change_bpm = ags_pattern_change_bpm;
}

void
ags_pattern_init(AgsPattern *pattern)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* base initialization */
  pattern->flags = 0;

  /* add pattern mutex */
  pattern->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  pattern->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* timestamp */
  pattern->timestamp = NULL;

  /* dimension and pattern */
  pattern->dim[0] = 0;
  pattern->dim[1] = 0;
  pattern->dim[2] = 0;

  pattern->pattern = NULL;

  /* port */
  pattern->port = NULL;

  /* indices */
  pattern->i = 0;
  pattern->j = 0;
  pattern->bit = 0;
}

void
ags_pattern_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsPattern *pattern;

  pattern = AGS_PATTERN(gobject);

  switch(prop_id){
  case PROP_FIRST_INDEX:
    {
      guint i;

      i = g_value_get_uint(value);

      pattern->i = i;
    }
    break;
  case PROP_SECOND_INDEX:
    {
      guint j;

      j = g_value_get_uint(value);

      pattern->j = j;
    }
    break;
  case PROP_OFFSET:
    {
      guint bit;

      bit = g_value_get_uint(value);

      pattern->bit = bit;
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      if(timestamp == (AgsTimestamp *) pattern->timestamp){
	return;
      }

      if(pattern->timestamp != NULL){
	g_object_unref(G_OBJECT(pattern->timestamp));
      }

      if(timestamp != NULL){
	g_object_ref(G_OBJECT(timestamp));
      }

      pattern->timestamp = (GObject *) timestamp;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pattern_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsPattern *pattern;

  pattern = AGS_PATTERN(gobject);

  switch(prop_id){
  case PROP_PORT:
    g_value_set_object(value, pattern->port);
    break;
  case PROP_FIRST_INDEX:
    {
      g_value_set_uint(value, pattern->i);
    }
    break;
  case PROP_SECOND_INDEX:
    {
      g_value_set_uint(value, pattern->j);
    }
    break;
  case PROP_OFFSET:
    {
      g_value_set_uint(value, pattern->bit);
    }
    break;
  case PROP_CURRENT_BIT:
    {
      g_value_set_boolean(value, ags_pattern_get_bit(pattern,
						     pattern->i,
						     pattern->j,
						     pattern->bit));
    }
    break;
  case PROP_TIMESTAMP:
    g_value_set_object(value, pattern->timestamp);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pattern_dispose(GObject *gobject)
{
  AgsPattern *pattern;

  pattern = AGS_PATTERN(gobject);

  /* timestamp */
  if(pattern->timestamp != NULL){
    g_object_run_dispose(G_OBJECT(pattern->timestamp));
    
    g_object_unref(G_OBJECT(pattern->timestamp));
  }

  /* port */
  if(pattern->port != NULL){
    g_object_unref(G_OBJECT(pattern->port));

    pattern->port = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_pattern_parent_class)->dispose(gobject);
}

void
ags_pattern_finalize(GObject *gobject)
{
  AgsPattern *pattern;
  
  guint i, j;

  pattern = AGS_PATTERN(gobject);

  /* timestamp */
  if(pattern->timestamp != NULL){
    g_object_unref(G_OBJECT(pattern->timestamp));
  }

  /* pattern */
  if(pattern->pattern != NULL){
    for(i = 0; i < pattern->dim[0]; i++){
      if(pattern->pattern[i] != NULL){
	for(j = 0; j < pattern->dim[1]; j++){
	  if(pattern->pattern[i][j] != NULL){
	    free(pattern->pattern[i][j]);
	  }
	}

	free(pattern->pattern[i]);
      }
    }

    free(pattern->pattern);
  }
 
  /* port */
  if(pattern->port != NULL){
    g_object_unref(G_OBJECT(pattern->port));
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_pattern_parent_class)->finalize(gobject);
}

AgsUUID*
ags_pattern_get_uuid(AgsConnectable *connectable)
{
  AgsPattern *pattern;
  
  AgsUUID *ptr;

  pthread_mutex_t *pattern_mutex;

  pattern = AGS_PATTERN(connectable);

  /* get pattern mutex */
  pthread_mutex_lock(ags_pattern_get_class_mutex());
  
  pattern_mutex = pattern->obj_mutex;
  
  pthread_mutex_unlock(ags_pattern_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(pattern_mutex);

  ptr = pattern->uuid;

  pthread_mutex_unlock(pattern_mutex);
  
  return(ptr);
}

gboolean
ags_pattern_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_pattern_is_ready(AgsConnectable *connectable)
{
  AgsPattern *pattern;
  
  gboolean is_ready;

  pthread_mutex_t *pattern_mutex;

  pattern = AGS_PATTERN(connectable);

  /* get pattern mutex */
  pthread_mutex_lock(ags_pattern_get_class_mutex());
  
  pattern_mutex = pattern->obj_mutex;
  
  pthread_mutex_unlock(ags_pattern_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(pattern_mutex);

  is_ready = (((AGS_PATTERN_ADDED_TO_REGISTRY & (pattern->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(pattern_mutex);
  
  return(is_ready);
}

void
ags_pattern_add_to_registry(AgsConnectable *connectable)
{
  AgsPattern *pattern;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GList *list;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  pattern = AGS_PATTERN(connectable);

  ags_pattern_set_flags(pattern, AGS_PATTERN_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(&(entry->entry),
		       (gpointer) pattern);
    ags_registry_add_entry(registry,
			   entry);
  }

  //TODO:JK: implement me
}

void
ags_pattern_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_pattern_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_pattern_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_pattern_xml_parse(AgsConnectable *connectable,
		     xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_pattern_is_connected(AgsConnectable *connectable)
{
  AgsPattern *pattern;
  
  gboolean is_connected;

  pthread_mutex_t *pattern_mutex;

  pattern = AGS_PATTERN(connectable);

  /* get pattern mutex */
  pthread_mutex_lock(ags_pattern_get_class_mutex());
  
  pattern_mutex = pattern->obj_mutex;
  
  pthread_mutex_unlock(ags_pattern_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(pattern_mutex);

  is_connected = (((AGS_PATTERN_CONNECTED & (pattern->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(pattern_mutex);
  
  return(is_connected);
}

void
ags_pattern_connect(AgsConnectable *connectable)
{
  AgsPattern *pattern;

  GList *list_start, *list;

  pthread_mutex_t *pattern_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  pattern = AGS_PATTERN(connectable);

  ags_pattern_set_flags(pattern, AGS_PATTERN_CONNECTED);  
}

void
ags_pattern_disconnect(AgsConnectable *connectable)
{
  AgsPattern *pattern;

  GList *list_start, *list;

  pthread_mutex_t *pattern_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  pattern = AGS_PATTERN(connectable);

  ags_pattern_unset_flags(pattern, AGS_PATTERN_CONNECTED);    
}

/**
 * ags_pattern_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_pattern_get_class_mutex()
{
  return(&ags_pattern_class_mutex);
}

/**
 * ags_pattern_test_flags:
 * @pattern: the #AgsPattern
 * @flags: the flags
 *
 * Test @flags to be set on @pattern.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_pattern_test_flags(AgsPattern *pattern, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return(FALSE);
  }

  /* get pattern mutex */
  pthread_mutex_lock(ags_pattern_get_class_mutex());
  
  pattern_mutex = pattern->obj_mutex;
  
  pthread_mutex_unlock(ags_pattern_get_class_mutex());

  /* test */
  pthread_mutex_lock(pattern_mutex);

  retval = (flags & (pattern->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(pattern_mutex);

  return(retval);
}

/**
 * ags_pattern_set_flags:
 * @pattern: the #AgsPattern
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.0.0
 */
void
ags_pattern_set_flags(AgsPattern *pattern, guint flags)
{
  pthread_mutex_t *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return;
  }

  /* get pattern mutex */
  pthread_mutex_lock(ags_pattern_get_class_mutex());
  
  pattern_mutex = pattern->obj_mutex;
  
  pthread_mutex_unlock(ags_pattern_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(pattern_mutex);

  pattern->flags |= flags;

  pthread_mutex_unlock(pattern_mutex);
}

/**
 * ags_pattern_unset_flags:
 * @pattern: the #AgsPattern
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.0.0
 */
void
ags_pattern_unset_flags(AgsPattern *pattern, guint flags)
{
  pthread_mutex_t *pattern_mutex;

  if(!AGS_IS_PATTERN(pattern)){
    return;
  }

  /* get pattern mutex */
  pthread_mutex_lock(ags_pattern_get_class_mutex());
  
  pattern_mutex = pattern->obj_mutex;
  
  pthread_mutex_unlock(ags_pattern_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(pattern_mutex);

  pattern->flags &= (~flags);

  pthread_mutex_unlock(pattern_mutex);
}

void
ags_pattern_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm)
{
  //TODO:JK: implement me
}

/**
 * ags_pattern_find_near_timestamp:
 * @pattern: a #GList containing #AgsPattern
 * @timestamp: the matching #AgsTimestamp
 *
 * Retrieve appropriate pattern for timestamp.
 *
 * Returns: Next match.
 *
 * Since: 1.0.0
 */
GList*
ags_pattern_find_near_timestamp(GList *pattern, AgsTimestamp *timestamp)
{
  AgsTimestamp *current_timestamp;

  if(timestamp == NULL){
    return(NULL);
  }

  while(pattern != NULL){
    current_timestamp = (AgsTimestamp *) AGS_PATTERN(pattern->data)->timestamp;

    if((AGS_TIMESTAMP_UNIX & (AGS_TIMESTAMP(timestamp)->flags)) != 0){
      if((AGS_TIMESTAMP_UNIX & (current_timestamp->flags)) != 0){
	if(current_timestamp->timer.unix_time.time_val >= AGS_TIMESTAMP(timestamp)->timer.unix_time.time_val &&
	   current_timestamp->timer.unix_time.time_val < AGS_TIMESTAMP(timestamp)->timer.unix_time.time_val + AGS_PATTERN_DEFAULT_DURATION){
	  return(pattern);
	}
      }
    }

    pattern = pattern->next;
  }

  return(NULL);
}

/**
 * ags_pattern_set_dim:
 * @pattern: an #AgsPattern
 * @dim0: bank 0 size
 * @dim1: bank 1 size
 * @length: amount of beats
 *
 * Reallocates the pattern's dimensions.
 *
 * Since: 1.0.0
 */
void 
ags_pattern_set_dim(AgsPattern *pattern, guint dim0, guint dim1, guint length)
{
  guint ***index0, **index1, *bitmap;
  guint i, j, k, j_set, k_set;
  guint bitmap_size;

  if(dim0 == 0 && pattern->pattern == NULL){
    return;
  }
  
  // shrink
  if(pattern->dim[0] > dim0){
    for(i = dim0; i < pattern->dim[0]; i++){
      for(j = 0; j < pattern->dim[1]; j++){
	free(pattern->pattern[i][j]);
      }

      free(pattern->pattern[i]);
    }

    if(dim0 == 0){
      free(pattern->pattern);
      
      pattern->pattern = NULL;
      pattern->dim[0] = 0;

      return;
    }else{
      pattern->pattern = (guint ***) realloc(pattern->pattern,
					     (int) dim0 * sizeof(guint **));

      pattern->dim[0] = dim0;
    }
  }

  if(pattern->dim[1] > dim1){
    if(dim1 == 0){
      for(i = 0; i < pattern->dim[0]; i++){
	for(j = dim1; j < pattern->dim[1]; j++){
	  free(pattern->pattern[i][j]);
	}

	pattern->pattern[i] = NULL;
      }

      pattern->dim[1] = 0;
      
      return;
    }else{
      for(i = 0; i < pattern->dim[0]; i++){
	for(j = dim1; j < pattern->dim[1]; j++){
	  free(pattern->pattern[i][j]);
	}
      }

      for(i = 0; pattern->dim[0]; i++){
	pattern->pattern[i] = (guint **) realloc(pattern->pattern[i],
						 dim1 * sizeof(guint *));
      }

      pattern->dim[1] = dim1;
    }
  }

  if(pattern->dim[2] > length){
    if(length == 0){
      for(i = 0; i < pattern->dim[0]; i++){
	for(j = 0; j < pattern->dim[1]; j++){
      	  free(pattern->pattern[i][j]);
	  
	  pattern->pattern[i][j] = NULL;
	}
      }

      pattern->dim[2] = 0;
    }else{
      for(i = 0; i < pattern->dim[0]; i++){
	for(j = 0; j < pattern->dim[1]; j++){
	  pattern->pattern[i][j] = (guint *) realloc(pattern->pattern[i][j],
						     (int) ceil((double) length / (double) (sizeof(guint) * 8)) * sizeof(guint));
	}
      }

      pattern->dim[2] = length;
    }
  }

  // grow
  bitmap_size = (int) ceil((double) pattern->dim[2] / (double) (sizeof(guint) * 8)) * sizeof(guint);
    
  if(pattern->dim[0] < dim0){
    if(pattern->pattern == NULL){
      pattern->pattern = (guint ***) malloc(dim0 * sizeof(guint **));
    }else{
      pattern->pattern = (guint ***) realloc(pattern->pattern,
					     dim0 * sizeof(guint **));
    }

    for(i = pattern->dim[0]; i < dim0; i++){
      pattern->pattern[i] = (guint **) malloc(pattern->dim[1] * sizeof(guint *));

      for(j = 0; j < pattern->dim[1]; j++){
	if(bitmap_size == 0){
	  pattern->pattern[i][j] = NULL;
	}else{
	  pattern->pattern[i][j] = (guint *) malloc(bitmap_size);
	  memset(pattern->pattern[i][j], 0, bitmap_size);
	}
      }
    }

    pattern->dim[0] = dim0;
  }

  if(pattern->dim[1] < dim1){  
    for(i = 0; i < pattern->dim[0]; i++){
      if(pattern->pattern[i] == NULL){
	pattern->pattern[i] = (guint **) malloc(dim1 * sizeof(guint *));
      }else{
	pattern->pattern[i] = (guint **) realloc(pattern->pattern[i],
						 dim1 * sizeof(guint *));
      }

      for(j = pattern->dim[1]; j < dim1; j++){
	if(bitmap_size == 0){
	  pattern->pattern[i][j] = NULL;
	}else{
	  pattern->pattern[i][j] = (guint *) malloc(bitmap_size);
	  memset(pattern->pattern[i][j], 0, bitmap_size);
	}
      }
    }

    pattern->dim[1] = dim1;
  }

  if(pattern->dim[2] < length){
    guint new_bitmap_size;
    
    new_bitmap_size = (int) ceil((double) length / (double) (sizeof(guint) * 8)) * sizeof(guint);
    
    for(i = 0; i < pattern->dim[0]; i++){
      for(j = 0; j < pattern->dim[1]; j++){
	if(pattern->pattern[i][j] == NULL){
	  pattern->pattern[i][j] = (guint *) malloc(new_bitmap_size);
	  memset(pattern->pattern[i][j], 0, new_bitmap_size);
	}else{
	  pattern->pattern[i][j] =(guint *) realloc(pattern->pattern[i][j],
						    new_bitmap_size);
	  memset(pattern->pattern[i][j] + bitmap_size, 0, new_bitmap_size - bitmap_size);
	}
      }
    }

    pattern->dim[2] = length;
  }
}

gboolean
ags_pattern_is_empty(AgsPattern *pattern, guint i, guint j)
{
  guint bitmap_length;
  guint n;

  bitmap_length = (guint) ceil((double) pattern->dim[2] / (double) (sizeof(guint) * 8));

  for(n = 0; n < bitmap_length; n++){
    if(pattern->pattern[i][j][n] != 0){
      return(FALSE);
    }
  }

  return(TRUE);
}

/**
 * ags_pattern_get_bit:
 * @pattern: an #AgsPattern
 * @i: bank index 0
 * @j: bank index 1
 * @bit: the tic to check
 *
 * Check for tic to be played.
 *
 * Returns: %TRUE if tone is enabled.
 *
 * Since: 1.0.0
 */
gboolean
ags_pattern_get_bit(AgsPattern *pattern, guint i, guint j, guint bit)
{
  guint k, value;

  k = (guint) floor((double) bit / (double) (sizeof(guint) * 8));
  value = 1 << (bit % (sizeof(guint) * 8));

  //((1 << (bit % (sizeof(guint) *8))) & (pattern->pattern[i][j][(guint) floor((double) bit / (double) (sizeof(guint) * 8))])) != 0
  if((value & (pattern->pattern[i][j][k])) != 0)
    return(TRUE);
  else
    return(FALSE);
}

/**
 * ags_pattern_toggle_bit:
 * @pattern: an #AgsPattern
 * @i: bank index 0
 * @j: bank index 1
 * @bit: the tic to toggle
 *
 * Toggle tone.
 *
 * Since: 1.0.0
 */
void
ags_pattern_toggle_bit(AgsPattern *pattern, guint i, guint j, guint bit)
{
  guint k, value;

  k = (guint) floor((double) bit / (double) (sizeof(guint) * 8));
  value = 1 << (bit % (sizeof(guint) * 8));


  if(value & (pattern->pattern[i][j][k]))
    pattern->pattern[i][j][k] &= (~value);
  else
    pattern->pattern[i][j][k] |= value;
}

/**
 * ags_pattern_new:
 *
 * Creates an #AgsPattern
 *
 * Returns: a new #AgsPattern
 *
 * Since: 1.0.0
 */
AgsPattern*
ags_pattern_new()
{
  AgsPattern *pattern;

  pattern = (AgsPattern *) g_object_new(AGS_TYPE_PATTERN, NULL);

  return(pattern);
}
