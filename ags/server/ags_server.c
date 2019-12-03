/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/server/ags_server.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_marshal.h>

#include <ags/server/ags_service_provider.h>

#include <ags/server/controller/ags_front_controller.h>

#include <stdlib.h>
#include <string.h>

#ifndef AGS_W32API
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <unistd.h>

#include <ags/i18n.h>

void ags_server_class_init(AgsServerClass *server);
void ags_server_init(AgsServer *server);
void ags_server_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_server_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_server_dispose(GObject *gobject);
void ags_server_finalize(GObject *gobject);

void ags_server_real_start(AgsServer *server);
void ags_server_real_stop(AgsServer *server);

gboolean ags_server_real_listen(AgsServer *server);

void ags_server_xmlrpc_callback(SoupServer *soup_server,
				SoupMessage *msg,
				const char *path,
				GHashTable *query,
				SoupClientContext *client,
				AgsServer *server);

/**
 * SECTION:ags_server
 * @short_description: remote control server
 * @title: AgsServer
 * @section_id:
 * @include: ags/server/ags_server.h
 *
 * The #AgsServer is a XML-RPC server.
 */

enum{
  PROP_0,
  PROP_DOMAIN,
  PROP_SERVER_PORT,
  PROP_IP4,
  PROP_IP6,
  PROP_FRONT_CONTROLLER,
  PROP_CONTROLLER,
};

enum{
  START,
  STOP,
  LISTEN,
  LAST_SIGNAL,
};

static gpointer ags_server_parent_class = NULL;
static guint server_signals[LAST_SIGNAL];

static GList *ags_server_list = NULL;

GType
ags_server_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_server = 0;

    static const GTypeInfo ags_server = {
      sizeof(AgsServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_server_init,
    };
    
    ags_type_server = g_type_register_static(G_TYPE_OBJECT,
					     "AgsServer",
					     &ags_server,
					     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_server);
  }

  return g_define_type_id__volatile;
}

void
ags_server_class_init(AgsServerClass *server)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_server_parent_class = g_type_class_peek_parent(server);

  /* GObjectClass */
  gobject = (GObjectClass *) server;

  gobject->set_property = ags_server_set_property;
  gobject->get_property = ags_server_get_property;

  gobject->dispose = ags_server_dispose;
  gobject->finalize = ags_server_finalize;

  /* properties */
  /**
   * AgsServer:domain:
   *
   * The domain to use.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("domain",
				   i18n_pspec("domain"),
				   i18n_pspec("The domain to use"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DOMAIN,
				  param_spec);
  
  /**
   * AgsServer:server-port:
   *
   * The server port to use.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("server-port",
				 i18n_pspec("server port"),
				 i18n_pspec("The server port to use"),
				 0,
				 G_MAXUINT32,
				 AGS_SERVER_DEFAULT_SERVER_PORT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SERVER_PORT,
				  param_spec);

  /**
   * AgsServer:ip4:
   *
   * The IPv4 address as string of the server.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("ip4",
				   i18n_pspec("ip4"),
				   i18n_pspec("The IPv4 address of the server"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IP4,
				  param_spec);

  /**
   * AgsServer:ip6:
   *
   * The IPv6 address as string of the server.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("ip6",
				   i18n_pspec("ip6"),
				   i18n_pspec("The IPv6 address of the server"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IP6,
				  param_spec);

  /**
   * AgsServer:front-controller:
   *
   * The assigned #AgsFrontController.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("front-controller",
				   i18n_pspec("assigned  front controller"),
				   i18n_pspec("The  front controller it is assigned with"),
				   AGS_TYPE_FRONT_CONTROLLER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FRONT_CONTROLLER,
				  param_spec);
  
  /**
   * AgsServer:controller:
   *
   * The assigned #AgsController providing default settings.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("controller",
				    i18n_pspec("assigned controller"),
				    i18n_pspec("The controller it is assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTROLLER,
				  param_spec);

  /* AgsServer */
  server->start = ags_server_real_start;
  server->stop = ags_server_real_stop;

  server->listen = ags_server_real_listen;

  /* signals */
  /**
   * AgsServer::start:
   * @server: the #AgsServer
   *
   * The ::start signal is emitted as the server starts.
   *
   * Since: 3.0.0
   */
  server_signals[START] =
    g_signal_new("start",
		 G_TYPE_FROM_CLASS(server),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsServerClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsServer::stop:
   * @server: the #AgsServer
   *
   * The ::stop signal is emitted as the server stops.
   *
   * Since: 3.0.0
   */
  server_signals[STOP] =
    g_signal_new("stop",
		 G_TYPE_FROM_CLASS(server),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsServerClass, stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);


  /**
   * AgsServer::listen:
   * @server: the #AgsServer
   *
   * The ::listen signal is emited during listen of server.
   *
   * Returns: %TRUE as a new connection was initiated, otherwise %FALSE
   * 
   * Since: 3.0.0
   */
  server_signals[LISTEN] =
    g_signal_new("listen",
		 G_TYPE_FROM_CLASS(server),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsServerClass, listen),
		 NULL, NULL,
		 ags_cclosure_marshal_BOOLEAN__VOID,
		 G_TYPE_BOOLEAN, 0);
}

void
ags_server_init(AgsServer *server)
{
  server->flags = 0;

  g_rec_mutex_init(&(server->obj_mutex));

  /* uuid */
  server->uuid = ags_uuid_alloc();
  ags_uuid_generate(server->uuid);

  /*  */
  server->server_info = ags_server_info_alloc("localhost", ags_uuid_to_string(server->uuid));

  server->ip4 = g_strdup(AGS_SERVER_DEFAULT_INET4_ADDRESS);
  server->ip6 = g_strdup(AGS_SERVER_DEFAULT_INET6_ADDRESS);
  
  server->domain = g_strdup(AGS_SERVER_DEFAULT_DOMAIN);
  server->server_port = AGS_SERVER_DEFAULT_SERVER_PORT;

  server->ip4_fd = -1;
  server->ip6_fd = -1;

  server->ip4_socket = NULL;
  server->ip6_socket = NULL;

  server->ip4_address = NULL;
  server->ip6_address = NULL;
  
  server->soup_server = NULL;
  server->auth_module = NULL;
  
  server->front_controller = NULL;

  server->controller = NULL;
}

void
ags_server_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsServer *server;

  GRecMutex *server_mutex;

  server = AGS_SERVER(gobject);

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);
  
  switch(prop_id){
  case PROP_DOMAIN:
    {
      gchar *domain;

      domain = g_value_get_string(value);

      g_rec_mutex_lock(server_mutex);
      
      if(server->domain == domain){
	g_rec_mutex_unlock(server_mutex);
	
	return;
      }

      g_free(server->domain);

      server->domain = g_strdup(domain);

      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      guint server_port;

      server_port = g_value_get_uint(value);

      g_rec_mutex_lock(server_mutex);

      server->server_port = server_port;
      
      g_rec_mutex_unlock(server_mutex);      
    }
    break;
  case PROP_IP4:
    {
      gchar *ip4;

      ip4 = g_value_get_string(value);

      g_rec_mutex_lock(server_mutex);
      
      if(server->ip4 == ip4){
	g_rec_mutex_unlock(server_mutex);
	
	return;
      }

      g_free(server->ip4);

      server->ip4 = g_strdup(ip4);

      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_IP6:
    {
      gchar *ip6;

      ip6 = g_value_get_string(value);

      g_rec_mutex_lock(server_mutex);
      
      if(server->ip6 == ip6){
	g_rec_mutex_unlock(server_mutex);
	
	return;
      }

      g_free(server->ip6);

      server->ip6 = g_strdup(ip6);

      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_FRONT_CONTROLLER:
    {
      GObject *front_controller;

      front_controller = g_value_get_object(value);

      g_rec_mutex_lock(server_mutex);

      if(server->front_controller == front_controller){
	g_rec_mutex_unlock(server_mutex);
	
	return;
      }

      if(server->front_controller != NULL){
	g_object_unref(server->front_controller);
      }
      
      if(front_controller != NULL){
	g_object_ref(front_controller);
      }
      
      server->front_controller = front_controller;

      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_CONTROLLER:
    {
      GObject *controller;

      controller = g_value_get_pointer(value);

      g_rec_mutex_lock(server_mutex);

      if(g_list_find(server->controller, controller) != NULL){
	g_rec_mutex_unlock(server_mutex);
	
	return;
      }

      g_rec_mutex_unlock(server_mutex);

      ags_server_add_controller(server,
				    controller);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_server_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsServer *server;

  GRecMutex *server_mutex;

  server = AGS_SERVER(gobject);

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);
  
  switch(prop_id){
  case PROP_DOMAIN:
    {
      g_rec_mutex_lock(server_mutex);

      g_value_set_string(value,
			 server->domain);
      
      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      g_rec_mutex_lock(server_mutex);
      
      g_value_set_uint(value,
		       server->server_port);

      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_IP4:
    {
      g_rec_mutex_lock(server_mutex);
      
      g_value_set_string(value,
			 server->ip4);
      
      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_IP6:
    {
      g_rec_mutex_lock(server_mutex);
      
      g_value_set_string(value,
			 server->ip6);

      g_rec_mutex_unlock(server_mutex);
    }
    break;    
  case PROP_FRONT_CONTROLLER:
    {
      g_rec_mutex_lock(server_mutex);

      g_value_set_object(value, server->front_controller);

      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_CONTROLLER:
    {
      g_rec_mutex_lock(server_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(server->controller,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(server_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_server_dispose(GObject *gobject)
{
  AgsServer *server;

  server = AGS_SERVER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_server_parent_class)->dispose(gobject);
}

void
ags_server_finalize(GObject *gobject)
{
  AgsServer *server;

  server = AGS_SERVER(gobject);

  g_free(server->domain);
  
  /* call parent */
  G_OBJECT_CLASS(ags_server_parent_class)->finalize(gobject);
}

/**
 * ags_server_test_flags:
 * @server: the #AgsServer
 * @flags: the flags
 *
 * Test @flags to be set on @server.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_server_test_flags(AgsServer *server, guint flags)
{
  gboolean retval;  
  
  GRecMutex *server_mutex;

  if(!AGS_IS_SERVER(server)){
    return(FALSE);
  }

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);

  /* test */
  g_rec_mutex_lock(server_mutex);

  retval = (flags & (server->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(server_mutex);

  return(retval);
}

/**
 * ags_server_set_flags:
 * @server: the #AgsServer
 * @flags: see #AgsServerFlags-enum
 *
 * Enable a feature of @server.
 *
 * Since: 3.0.0
 */
void
ags_server_set_flags(AgsServer *server, guint flags)
{
  GRecMutex *server_mutex;

  if(!AGS_IS_SERVER(server)){
    return;
  }

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(server_mutex);

  server->flags |= flags;
  
  g_rec_mutex_unlock(server_mutex);
}
    
/**
 * ags_server_unset_flags:
 * @server: the #AgsServer
 * @flags: see #AgsServerFlags-enum
 *
 * Disable a feature of @server.
 *
 * Since: 3.0.0
 */
void
ags_server_unset_flags(AgsServer *server, guint flags)
{  
  GRecMutex *server_mutex;

  if(!AGS_IS_SERVER(server)){
    return;
  }

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(server_mutex);

  server->flags &= (~flags);
  
  g_rec_mutex_unlock(server_mutex);
}

/**
 * ags_server_info_alloc:
 * @server_name: the server name
 * @uuid: the uuid
 * 
 * Allocate server info.
 * 
 * Returns: the allocated #AgsServerInfo-struct
 * 
 * Since: 3.0.0
 */
AgsServerInfo*
ags_server_info_alloc(gchar *server_name, gchar *uuid)
{
  AgsServerInfo *server_info;

  server_info = (AgsServerInfo *) malloc(sizeof(AgsServerInfo));

  server_info->uuid = uuid;
  server_info->server_name = server_name;

  return(server_info);
}

/**
 * ags_server_add_controller:
 * @server: the #AgsServer
 * @controller: the #AgsController
 *
 * Add @controller to @server.
 * 
 * Since: 3.0.0
 */
void
ags_server_add_controller(AgsServer *server,
			  GObject *controller)
{
  if(!AGS_IS_SERVER(server) ||
     !AGS_IS_CONTROLLER(controller)){
    return;
  }

  if(g_list_find(server->controller, controller) == NULL){
    g_object_ref(controller);
    server->controller = g_list_prepend(server->controller,
					controller);

    g_object_set(controller,
		 "server", server,
		 NULL);
  }
}

/**
 * ags_server_remove_controller:
 * @server: the #AgsServer
 * @controller: the #AgsController
 *
 * Remove @controller from @server.
 * 
 * Since: 3.0.0
 */
void
ags_server_remove_controller(AgsServer *server,
			     GObject *controller)
{
  if(!AGS_IS_SERVER(server) ||
     !AGS_IS_CONTROLLER(controller)){
    return;
  }

  if(g_list_find(server->controller, controller) != NULL){
    server->controller = g_list_remove(server->controller,
				       controller);

    g_object_set(controller,
		 "server", NULL,
		 NULL);

    g_object_unref(controller);
  }
}

void
ags_server_real_start(AgsServer *server)
{
  gboolean any_address;
  gboolean ip4_success, ip6_success;

  GError *error;

  GRecMutex *server_mutex;

  if(ags_server_test_flags(server, AGS_SERVER_STARTED)){
    return;
  }

  ags_server_set_flags(server, AGS_SERVER_STARTED);

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);

  any_address = ags_server_test_flags(server, AGS_SERVER_ANY_ADDRESS);
  
  ip4_success = FALSE;
  ip6_success = FALSE;
    
  if(ags_server_test_flags(server, AGS_SERVER_INET4)){
    ip4_success = TRUE;

    /* create socket */
    g_rec_mutex_lock(server_mutex);
      
    error = NULL;      
    server->ip4_socket = g_socket_new(G_SOCKET_FAMILY_IPV4,
				      G_SOCKET_TYPE_STREAM,
				      G_SOCKET_PROTOCOL_TCP,
				      &error);
    server->ip4_fd = g_socket_get_fd(server->ip4_socket);
      
    g_socket_set_listen_backlog(server->ip4_socket,
				AGS_SERVER_DEFAULT_BACKLOG);
      
    g_rec_mutex_unlock(server_mutex);

    if(error != NULL){
      g_critical("AgsServer - %s", error->message);

      g_error_free(error);
    }
    
    /* get ip4 */
    if(any_address){
      g_rec_mutex_lock(server_mutex);  

      server->ip4_address = g_inet_socket_address_new(g_inet_address_new_any(G_SOCKET_FAMILY_IPV4),
						      server->server_port);

      g_rec_mutex_unlock(server_mutex);  
    }else{
      g_rec_mutex_lock(server_mutex);  

      server->ip4_address = g_inet_socket_address_new(g_inet_address_new_from_string(server->ip4),
						      server->server_port);

      g_rec_mutex_unlock(server_mutex);
    }
  }

  if(ags_server_test_flags(server, AGS_SERVER_INET6)){    
    ip6_success = TRUE;
  
    /* create socket */
    g_rec_mutex_lock(server_mutex);
      
    error = NULL;      
    server->ip6_socket = g_socket_new(G_SOCKET_FAMILY_IPV6,
					  G_SOCKET_TYPE_STREAM,
					  G_SOCKET_PROTOCOL_TCP,
					  &error);
    server->ip6_fd = g_socket_get_fd(server->ip6_socket);

    g_socket_set_listen_backlog(server->ip6_socket,
				AGS_SERVER_DEFAULT_BACKLOG);

    g_rec_mutex_unlock(server_mutex);

    if(error != NULL){
      g_critical("AgsServer - %s", error->message);

      g_error_free(error);
    }

    /* get ip6 */
    if(any_address){
      g_rec_mutex_lock(server_mutex);

      server->ip6_address = g_inet_socket_address_new(g_inet_address_new_any(G_SOCKET_FAMILY_IPV6),
						      server->server_port);
      
      g_rec_mutex_unlock(server_mutex);
    }else{
      g_rec_mutex_lock(server_mutex);

      server->ip6_address = g_inet_socket_address_new(g_inet_address_new_from_string(server->ip6),
						      server->server_port);

      g_rec_mutex_unlock(server_mutex);
    }
  }
  
  if(ip4_success != TRUE && ip6_success != TRUE){
    g_critical("no protocol family");

    return;
  }

  if(ip4_success){
    error = NULL;
    g_socket_bind(server->ip4_socket,
		  server->ip4_address,
		  TRUE,
		  &error);
    
    if(error != NULL){
      g_critical("AgsServer - %s", error->message);

      g_error_free(error);
    }
  }
  
  if(ip6_success){
    error = NULL;
    g_socket_bind(server->ip6_socket,
		  server->ip6_address,
		  TRUE,
		  &error);

    if(error != NULL){
      g_critical("AgsServer - %s", error->message);

      g_error_free(error);
    }
  }

  ags_server_set_flags(server, AGS_SERVER_RUNNING);

  g_message("starting to listen on XMLRPC");

  /* create listen thread */
  server->soup_server = soup_server_new("interface", soup_address_new(server->domain, server->server_port),
					NULL);

  soup_server_add_handler(server->soup_server,
			  NULL,
			  ags_server_xmlrpc_callback,
			  server,
			  NULL);

  ags_server_listen(server);
}

/**
 * ags_server_start:
 * @server: the #AgsServer
 * 
 * Start the XMLRPC-C abyss server.
 * 
 * Since: 3.0.0
 */
void
ags_server_start(AgsServer *server)
{
  g_return_if_fail(AGS_IS_SERVER(server));

  g_object_ref((GObject *) server);
  g_signal_emit(G_OBJECT(server),
		server_signals[START], 0);
  g_object_unref((GObject *) server);
}

void
ags_server_real_stop(AgsServer *server)
{
  GError *error;
  
  GRecMutex *server_mutex;

  if(!ags_server_test_flags(server, AGS_SERVER_RUNNING)){
    return;
  }
  
  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);

  /* stop */
  ags_server_set_flags(server, AGS_SERVER_TERMINATING);
  ags_server_unset_flags(server, AGS_SERVER_RUNNING);

  /* close fd */
  g_rec_mutex_lock(server_mutex);

  soup_server_disconnect(server->soup_server);
  
  if(server->ip4_fd != -1){
    error = NULL;
    g_socket_close(server->ip4_socket,
		   &error);
    g_object_unref(server->ip4_socket);

    server->ip4_socket = NULL;
    server->ip4_fd = -1;
  }

  if(server->ip6_fd != -1){
    error = NULL;
    g_socket_close(server->ip6_socket,
		   &error);
    g_object_unref(server->ip6_socket);

    server->ip6_socket = NULL;
    server->ip6_fd = -1;
  }

  g_rec_mutex_unlock(server_mutex);

  ags_server_unset_flags(server, (AGS_SERVER_STARTED |
				  AGS_SERVER_TERMINATING));
}

/**
 * ags_server_stop:
 * @server: the #AgsServer
 * 
 * Stop the XMLRPC-C abyss server.
 * 
 * Since: 3.0.0
 */
void
ags_server_stop(AgsServer *server)
{
  g_return_if_fail(AGS_IS_SERVER(server));

  g_object_ref((GObject *) server);
  g_signal_emit(G_OBJECT(server),
		server_signals[STOP], 0);
  g_object_unref((GObject *) server);
}

/**
 * ags_server_lookup:
 * @server_info: the #AgsServerInfo-struct
 *
 * Lookup #AgsServer by @server_info.
 *
 * Returns: the associated #AgsServer if found, else %NULL
 * 
 * Since: 3.0.0
 */
AgsServer*
ags_server_lookup(AgsServerInfo *server_info)
{
  GList *current;

  if(server_info == NULL){
    return(NULL);
  }
  
  current = ags_server_list;

  while(current != NULL){
    if(AGS_SERVER(current->data)->server_info != NULL &&
       !g_ascii_strcasecmp(server_info->uuid,
			   AGS_SERVER(current->data)->server_info->uuid) &&
       !g_strcmp0(server_info->server_name,
		  AGS_SERVER(current->data)->server_info->server_name)){
      return(AGS_SERVER(current->data));
    }

    current = current->next;
  }

  return(NULL);
}

gboolean
ags_server_real_listen(AgsServer *server)
{
  GError *error;

  GRecMutex *server_mutex;
  
  if(!ags_server_test_flags(server, AGS_SERVER_STARTED)){
    return(FALSE);
  }
  
  /* get  server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);

  if(server->ip4_fd != -1){
    g_rec_mutex_lock(server_mutex);

    error = NULL;
    soup_server_listen_socket(server->soup_server,
			      server->ip4_socket,
			      SOUP_SERVER_LISTEN_HTTPS,
			      &error);
    
    g_rec_mutex_unlock(server_mutex);

    if(error != NULL){
      g_critical("AgsServer - %s", error->message);

      g_error_free(error);
    }
  }

  if(server->ip6_fd != -1){
    g_rec_mutex_lock(server_mutex);

    error = NULL;
    soup_server_listen_socket(server->soup_server,
			      server->ip6_socket,
			      SOUP_SERVER_LISTEN_HTTPS,
			      &error);
    
    g_rec_mutex_unlock(server_mutex);

    if(error != NULL){
      g_critical("AgsServer - %s", error->message);

      g_error_free(error);
    }
  }  
  
  return(FALSE);
}

/**
 * ags_server_listen:
 * @server: the #AgsServer
 * 
 * Listen as  server.
 * 
 * Returns: %TRUE as a new connection was initiated, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_server_listen(AgsServer *server)
{
  gboolean created_connection;

  g_return_val_if_fail(AGS_IS_SERVER(server), FALSE);
  
  g_object_ref((GObject *) server);
  g_signal_emit(G_OBJECT(server),
		server_signals[LISTEN], 0,
		&created_connection);
  g_object_unref((GObject *) server);

  return(created_connection);
}

void
ags_server_xmlrpc_callback(SoupServer *soup_server,
			   SoupMessage *msg,
			   const char *path,
			   GHashTable *query,
			   SoupClientContext *client,
			   AgsServer *server)
{
}

/**
 * ags_server_new:
 * @application_context: the #AgsApplicationContext
 *
 * Instantiate #AgsServer.
 * 
 * Returns: a new #AgsServer
 * 
 * Since: 3.0.0
 */
AgsServer*
ags_server_new()
{
  AgsServer *server;

  server = (AgsServer *) g_object_new(AGS_TYPE_SERVER,
				      NULL);

  return(server);
}
