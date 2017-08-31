/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/server/ags_service_provider.h>
#include <ags/server/ags_registry.h>

#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ags/i18n.h>

void ags_server_class_init(AgsServerClass *server);
void ags_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_server_init(AgsServer *server);
void ags_server_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_server_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_server_add_to_registry(AgsConnectable *connectable);
void ags_server_remove_from_registry(AgsConnectable *connectable);
void ags_server_connect(AgsConnectable *connectable);
void ags_server_disconnect(AgsConnectable *connectable);
void ags_server_finalize(GObject *gobject);

void ags_server_real_start(AgsServer *server);

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
  PROP_APPLICATION_CONTEXT,
};

enum{
  START,
  LAST_SIGNAL,
};

static gpointer ags_server_parent_class = NULL;
static guint server_signals[LAST_SIGNAL];

static GList *ags_server_list = NULL;

GType
ags_server_get_type()
{
  static GType ags_type_server = 0;

  if(!ags_type_server){
    static const GTypeInfo ags_server = {
      sizeof (AgsServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_server = g_type_register_static(G_TYPE_OBJECT,
					     "AgsServer",
					     &ags_server,
					     0);

    g_type_add_interface_static(ags_type_server,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_server);
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

  gobject->finalize = ags_server_finalize;

  /* properties */
  /**
   * AgsServer:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n("application context object"),
				   i18n("The application context object"),
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /* AgsServer */
  server->start = ags_server_real_start;

  /* signals */
  /**
   * AgsServer::start:
   * @server: the #AgsServer
   *
   * The ::start signal is emitted as the server starts.
   *
   * Since: 1.0.0
   */
  server_signals[START] =
    g_signal_new("start",
		 G_TYPE_FROM_CLASS(server),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsServerClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_server_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = ags_server_add_to_registry;
  connectable->remove_from_registry = ags_server_remove_from_registry;
  connectable->connect = ags_server_connect;
  connectable->disconnect = ags_server_disconnect;
}

void
ags_server_init(AgsServer *server)
{
  server->flags = 0;

  server->mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(server->mutexattr);
  pthread_mutexattr_settype(server->mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(server->mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif
  
  server->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(server->mutex,
		     server->mutexattr);

  /*  */
  server->server_info = ags_server_info_alloc("localhost");

#ifdef AGS_WITH_XMLRPC_C
  server->abyss_server = (TServer *) malloc(sizeof(TServer));
  server->socket = NULL;
#else
  server->abyss_server = NULL;
  server->socket = NULL;
#endif

  
  server->address = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
  memset(server->address, 0, sizeof(struct sockaddr_in));
  
  server->address->sin_port = 8080;
  server->address->sin_family = AF_INET;

  inet_aton("127.0.0.1", &(server->address->sin_addr.s_addr));

  server->auth_module = AGS_SERVER_DEFAULT_AUTH_MODULE;
  
  server->controller = NULL;
  
  server->application_context = NULL;
  server->application_mutex = NULL;
}

void
ags_server_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsServer *server;

  server = AGS_SERVER(gobject);

  //TODO:JK: implement set functionality
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(server->application_context == (GObject *) application_context){
	return;
      }

      if(server->application_context != NULL){
	g_object_unref(G_OBJECT(server->application_context));
      }

      if(application_context != NULL){
	g_object_ref(G_OBJECT(application_context));

	server->application_mutex = application_context->mutex;
      }else{
	server->application_mutex = NULL;
      }

      server->application_context = application_context;
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

  server = AGS_SERVER(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, server->application_context);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_server_add_to_registry(AgsConnectable *connectable)
{
  AgsServer *server;
  //  AgsRegistry *registry;

  server = AGS_SERVER(connectable);
  

  /* children */
  //  ags_connectable_add_to_registry(AGS_CONNECTABLE(server->registry));
  //  ags_connectable_add_to_registry(AGS_CONNECTABLE(server->remote_task));
}

void
ags_server_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_server_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_server_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_server_finalize(GObject *gobject)
{
  AgsServer *server;

  server = AGS_SERVER(gobject);

  /* mutex */
  pthread_mutex_destroy(server->mutex);
  free(server->mutex);

  pthread_mutexattr_destroy(server->mutexattr);
  free(server->mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_server_parent_class)->finalize(gobject);
}

/**
 * ags_server_info_alloc:
 * @server_name: the server name
 * 
 * Allocate server info.
 * 
 * Returns: the allocated #AgsServerInfo-struct
 * 
 * Since: 1.0.0
 */
AgsServerInfo*
ags_server_info_alloc(gchar *server_name)
{
  AgsServerInfo *server_info;

  server_info = (AgsServerInfo *) malloc(sizeof(AgsServerInfo));

  server_info->uuid = ags_id_generator_create_uuid();
  server_info->server_name = server_name;

  return(server_info);
}

void
ags_server_real_start(AgsServer *server)
{
  AgsRegistry *registry;
  const char *error;

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(server->application_context));
  
  ags_connectable_add_to_registry(AGS_CONNECTABLE(server->application_context));

  //  xmlrpc_registry_set_shutdown(registry,
  //			       &requestShutdown, &terminationRequested);
  server->socket_fd = socket(AF_INET, SOCK_RDM, PF_INET);
  bind(server->socket_fd, server->address, sizeof(struct sockaddr_in));

#ifdef AGS_WITH_XMLRPC_C
  SocketUnixCreateFd(server->socket_fd, &(server->socket));

  ServerCreateSocket2(server->abyss_server, server->socket, &error);
  xmlrpc_server_abyss_set_handlers2(server->abyss_server, "/RPC2", registry->registry);
  ServerInit(server->abyss_server);
  //  setupSignalHandlers();

  while((AGS_SERVER_RUNNING & (server->flags)) != 0){
    printf("Waiting for next RPC...\n");
    ServerRunOnce(server->abyss_server);
    /* This waits for the next connection, accepts it, reads the
       HTTP POST request, executes the indicated RPC, and closes
       the connection.
    */
  } 
#endif /* AGS_WITH_XMLRPC_C */
}

void
ags_server_start(AgsServer *server)
{
  g_return_if_fail(AGS_IS_SERVER(server));

  g_object_ref((GObject *) server);
  g_signal_emit(G_OBJECT(server),
		server_signals[START], 0);
  g_object_unref((GObject *) server);
}

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

/**
 * ags_server_new:
 * @application_context: the #AgsApplicationContext
 *
 * Instantiate #AgsServer.
 * 
 * Returns: a new #AgsServer
 * 
 * Since: 0.7.0
 */
AgsServer*
ags_server_new(GObject *application_context)
{
  AgsServer *server;

  server = (AgsServer *) g_object_new(AGS_TYPE_SERVER,
				      "application-context", application_context,
				      NULL);

  return(server);
}
