/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

gpointer ags_functional_osc_xmlrpc_server_test_add_thread(gpointer data);

int ags_functional_osc_xmlrpc_server_test_init_suite();
int ags_functional_osc_xmlrpc_server_test_clean_suite();

void ags_functional_osc_xmlrpc_server_test_action_controller();
void ags_functional_osc_xmlrpc_server_test_config_controller();
void ags_functional_osc_xmlrpc_server_test_info_controller();
void ags_functional_osc_xmlrpc_server_test_meter_controller();
void ags_functional_osc_xmlrpc_server_test_node_controller();
void ags_functional_osc_xmlrpc_server_test_renew_controller();
void ags_functional_osc_xmlrpc_server_test_status_controller();

void ags_functional_osc_xmlrpc_server_test_authenticate_authenticate_callback(SoupSession *session,
									      SoupMessage *msg,
									      SoupAuth *auth,
									      gboolean retrying,
									      gpointer user_data);
void ags_functional_osc_xmlrpc_server_test_websocket_callback(GObject *source_object,
							      GAsyncResult *res,
							      gpointer user_data);


#define AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"					\
  "simple-file=false\n"						\
  "disable-feature=experimental\n"				\
  "segmentation=4/4\n"						\
  "\n"								\
  "[server]\n"							\
  "realm=ags-test-realm\n"					\
  "auto-start=true\n"						\
  "any-address=false\n"						\
  "enable-ip4=true\n"						\
  "ip4-address=127.0.0.1\n"					\
  "enable-ip6=false\n"						\
  "ip6-address=::1\n"						\
  "server-port=8080\n"						\
  "\n"								\
  "[thread]\n"							\
  "model=super-threaded\n"					\
  "super-threaded-scope=channel\n"				\
  "lock-global=ags-thread\n"					\
  "lock-parent=ags-recycling-thread\n"				\
  "thread-pool-max-unused-threads=8\n"				\
  "max-precision=125\n"						\
  "\n"								\
  "[soundcard-0]\n"						\
  "backend=alsa\n"						\
  "device=default\n"						\
  "samplerate=44100\n"						\
  "buffer-size=1024\n"						\
  "pcm-channels=2\n"						\
  "dsp-channels=2\n"						\
  "format=16\n"							\
  "\n"								\
  "[sequencer-0]\n"						\
  "backend=alsa\n"						\
  "device=default\n"						\
  "\n"								\
  "[recall]\n"							\
  "auto-sense=true\n"						\
  "\n"

#define AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_APPLY_CONFIG_ARGUMENT "[generic]\n" \
  "autosave-thread=false\n"						\
  "simple-file=false\n"							\
  "disable-feature=experimental\n"					\
  "segmentation=4/4\n"							\
  "\n"									\
  "[thread]\n"								\
  "model=super-threaded\n"						\
  "super-threaded-scope=channel\n"					\
  "lock-global=ags-thread\n"						\
  "lock-parent=ags-recycling-thread\n"					\
  "thread-pool-max-unused-threads=8\n"					\
  "max-precision=125\n"							\
  "\n"									\
  "[soundcard-0]\n"							\
  "backend=alsa\n"							\
  "device=default\n"							\
  "samplerate=44100\n"							\
  "buffer-size=256\n"							\
  "pcm-channels=2\n"							\
  "dsp-channels=2\n"							\
  "format=16\n"								\
  "\n"									\
  "[recall]\n"								\
  "auto-sense=true\n"							\
  "\n"

#define AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_METER_PACKET_COUNT (16 * 30)

#define AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_XML_AUTHENTICATION_FILENAME SRCDIR "/" "ags_functional_osc_xmlrpc_server_test_authentication.xml"
#define AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_XML_PASSWORD_STORE_FILENAME SRCDIR "/" "ags_functional_osc_xmlrpc_server_test_password_store.xml"
#define AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_XML_COOKIE_FILENAME SRCDIR "/" "ags_functional_osc_xmlrpc_server_test_cookie"

#define AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_AUTHENTICATE_LOGIN "ags-test-login"
#define AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_AUTHENTICATE_PASSWORD "ags-test-password"

GThread *add_thread = NULL;

AgsApplicationContext *application_context;

AgsAudio *drum;

AgsOscXmlrpcServer *osc_xmlrpc_server;

SoupSession *soup_session;
SoupCookieJar *jar;

GObject *default_soundcard;

struct TestDataMeter{
  gchar *login;
  gchar *security_token;
  gchar *resource_id;

  volatile gint *meter_packet_count;
}meter_data;

gpointer
ags_functional_osc_xmlrpc_server_test_add_thread(gpointer data)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");

  putenv("LADSPA_PATH=\"\"");
  putenv("DSSI_PATH=\"\"");
  putenv("LV2_PATH=\"\"");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    exit(CU_get_error());
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFunctionalOscXmlrpcServerTest", ags_functional_osc_xmlrpc_server_test_init_suite, ags_functional_osc_xmlrpc_server_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    exit(CU_get_error());
  }

  /* add the tests to the suite */
  if(
#if 0
    (CU_add_test(pSuite, "test of AgsOscXmlrpcServer providing action controller", ags_functional_osc_xmlrpc_server_test_action_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer providing config controller", ags_functional_osc_xmlrpc_server_test_config_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer providing info controller", ags_functional_osc_xmlrpc_server_test_info_controller) == NULL) ||
#endif
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer providing meter controller", ags_functional_osc_xmlrpc_server_test_meter_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer providing node controller", ags_functional_osc_xmlrpc_server_test_node_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer providing renew controller", ags_functional_osc_xmlrpc_server_test_renew_controller) == NULL) ||
     (CU_add_test(pSuite, "test of AgsOscXmlrpcServer providing status controller", ags_functional_osc_xmlrpc_server_test_status_controller) == NULL)){
    CU_cleanup_registry();
      
    exit(CU_get_error());
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  exit(CU_get_error());
}

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_osc_xmlrpc_server_test_init_suite()
{
  AgsAuthenticationManager *authentication_manager;
  AgsPasswordStoreManager *password_store_manager;
  AgsXmlAuthentication *xml_authentication;
  AgsXmlPasswordStore *xml_password_store;

  AgsConfig *config;

  GList *server;
  GList *start_audio;
  
  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_CONFIG,
			    strlen(AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_CONFIG));

  authentication_manager = ags_authentication_manager_get_instance();
  password_store_manager = ags_password_store_manager_get_instance();

  xml_authentication = ags_xml_authentication_new();
  ags_xml_authentication_open_filename(xml_authentication,
				       AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_XML_AUTHENTICATION_FILENAME);
  ags_authentication_manager_add_authentication(authentication_manager,
						xml_authentication);
  
  xml_password_store = ags_xml_password_store_new();
  ags_xml_password_store_open_filename(xml_password_store,
				       AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_XML_PASSWORD_STORE_FILENAME);
  ags_password_store_manager_add_password_store(password_store_manager,
						xml_password_store);
  
  application_context = ags_audio_application_context_new();
  g_object_ref(application_context);
  
  ags_application_context_prepare(application_context);
  ags_application_context_setup(application_context);

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  /* drum */
  drum = ags_audio_new(default_soundcard);
  g_object_ref(drum);

  g_object_set(drum,
	       "audio-name", "test-drum",
	       NULL);
  
  ags_audio_set_flags(drum,
		      (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		       AGS_AUDIO_INPUT_HAS_RECYCLING |
		       AGS_AUDIO_SYNC |
		       AGS_AUDIO_ASYNC));

  ags_audio_set_ability_flags(drum, (AGS_SOUND_ABILITY_PLAYBACK |
				     AGS_SOUND_ABILITY_SEQUENCER |
				     AGS_SOUND_ABILITY_NOTATION));

  ags_audio_set_audio_channels(drum,
			       2, 0);

  ags_audio_set_pads(drum,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(drum,
		     AGS_TYPE_INPUT,
		     8, 0);

  start_audio = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
			       g_list_prepend(start_audio,
					      drum));

  /* ags-volume */
  ags_recall_factory_create(drum,
			    NULL, NULL,
			    "ags-volume",
			    0, 2, 
			    0, 8,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-mute */
  ags_recall_factory_create(drum,
			    NULL, NULL,
			    "ags-mute",
			    0, 2,
			    0, 8,
			    (AGS_RECALL_FACTORY_INPUT,
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-peak */
  ags_recall_factory_create(drum,
			    NULL, NULL,
			    "ags-peak",
			    0, 2,
			    0, 8,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  ags_connectable_connect(AGS_CONNECTABLE(drum));
  
  /* OSC XMLRPC server */
  signal(SIGPIPE, SIG_IGN);

  server = ags_service_provider_get_server(AGS_SERVICE_PROVIDER(application_context));
  
  osc_xmlrpc_server = ags_osc_xmlrpc_server_new();
  g_object_set(osc_xmlrpc_server,
	       "xmlrpc-server", server->data,
	       NULL);

  while(!ags_server_test_flags(server->data, AGS_SERVER_RUNNING)){
    sleep(1);
  }
  
  ags_osc_xmlrpc_server_add_default_controller(osc_xmlrpc_server);
  
  ags_osc_server_start(osc_xmlrpc_server);

  sleep(5);
  
  /* soup session */
  soup_session = soup_session_new_with_options(SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_AUTH_BASIC,
					       SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_AUTH_DIGEST,
					       SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_LOGGER,
					       NULL);

  jar = soup_cookie_jar_text_new(AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_XML_COOKIE_FILENAME,
				 FALSE);     
  soup_session_add_feature(soup_session, jar);  

  g_object_set(G_OBJECT(soup_session),
	       SOUP_SESSION_SSL_STRICT, FALSE,
	       NULL);
  
  g_signal_connect(soup_session, "authenticate",
		   G_CALLBACK(ags_functional_osc_xmlrpc_server_test_authenticate_authenticate_callback), NULL);

  sleep(5);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_functional_osc_xmlrpc_server_test_clean_suite()
{
  ags_osc_server_stop(osc_xmlrpc_server);

  return(0);
}

void
ags_functional_osc_xmlrpc_server_test_authenticate_authenticate_callback(SoupSession *session,
									 SoupMessage *msg,
									 SoupAuth *auth,
									 gboolean retrying,
									 gpointer user_data)
{
  g_message("authenticate: ****");
  
  soup_auth_authenticate(auth,
			 AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_AUTHENTICATE_LOGIN,
			 AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_AUTHENTICATE_PASSWORD);
}

void
ags_functional_osc_xmlrpc_server_test_websocket_callback(GObject *source_object,
							 GAsyncResult *res,
							 gpointer user_data)
{
  SoupWebsocketConnection *websocket_connection;

  GIOStream *io_stream;
  GInputStream *output_stream;
  GInputStream *input_stream;

  xmlDoc *doc;
  xmlNode *root_node;  
  xmlNode *login_node;
  xmlNode *security_token_node;
  xmlNode *redirect_node;

  struct timespec start_time;
  struct timespec timeout_delay;
  struct timespec idle_delay;  

  xmlChar *buffer;
  guchar *data;
  volatile gint *meter_packet_count;

  int buffer_length;
  gsize num_read;
  GError *error;
  
  g_message("websocket ...");
  
  error = NULL;
  websocket_connection = soup_session_websocket_connect_finish(SOUP_SESSION(source_object),
							       res,
							       &error);

  if(error != NULL){
    g_critical("%s", error->message);
    
    g_error_free(error);
  }

  sleep(5);
  
  io_stream = soup_websocket_connection_get_io_stream(websocket_connection);
  
  output_stream = g_io_stream_get_output_stream(io_stream);
  
  /* websocket */
  doc = xmlNewDoc("1.0");
	    
  root_node = xmlNewNode(NULL, "ags-osc-over-xmlrpc");
  xmlDocSetRootElement(doc, root_node);

  login_node = xmlNewNode(NULL,
			  BAD_CAST "ags-srv-login");

  xmlNodeSetContent(login_node,
		    meter_data.login);
  
  xmlAddChild(root_node,
	      login_node);

  security_token_node = xmlNewNode(NULL,
				   BAD_CAST "ags-srv-security-token");
  
  xmlNodeSetContent(security_token_node,
		    meter_data.security_token);

  xmlAddChild(root_node,
	      security_token_node);

  redirect_node = xmlNewNode(NULL,
			  BAD_CAST "ags-srv-redirect");

  xmlNewProp(redirect_node,
	     "resource-id",
	     meter_data.resource_id);
  
  xmlAddChild(root_node,
	      redirect_node);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);

  /* write out */
  error = NULL;
  g_output_stream_write(output_stream,
			buffer,
			buffer_length,
			NULL,
			&error);
  
  if(error != NULL){
    g_critical("%s", error->message);
    
    g_error_free(error);
  }

  /* response */
  input_stream = g_io_stream_get_input_stream(io_stream);

#ifdef __APPLE__
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
      
  start_time.tv_sec = mts.tv_sec;
  start_time.tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_MONOTONIC, &start_time);
#endif

  timeout_delay.tv_sec = 180;
  timeout_delay.tv_nsec = 0;
  
  idle_delay.tv_sec = 0;
  idle_delay.tv_nsec = NSEC_PER_SEC / 30;

  meter_packet_count = meter_data.meter_packet_count;

  data = g_malloc(65535 * sizeof(guchar));
  
  while(!ags_osc_client_timeout_expired(&start_time,
					&timeout_delay)){
    error = NULL;
    num_read = g_input_stream_read(input_stream,
				   data,
				   65535,
				   NULL,
				   &error);

    if(num_read > 0 &&
       num_read < 65535){
      data[num_read] = '\0';

      g_message(">>\n%s\n<<", data);

      g_atomic_int_inc(&(meter_packet_count[0]));
    }

    nanosleep(&idle_delay,
	      NULL);
  }
}

void
ags_functional_osc_xmlrpc_server_test_action_controller()
{
  SoupMessage *msg;
  SoupMessageHeaders *response_headers;
  SoupMessageBody *response_body;
  
  SoupMessageHeadersIter iter;
  GSList *cookie;

  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *osc_packet_node_list;
  xmlNode *osc_packet_node;

  gchar *data;
  guchar *packet;
  xmlChar *buffer;

  int buffer_length;
  guint status;
    
  static const guchar *start_soundcard_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSoundcard\x00\x00start\x00\x00\x00";
  static const guchar *start_sequencer_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSequencer\x00\x00start\x00\x00\x00";
  static const guchar *start_audio_message =     "/action\x00,ss\x00/AgsSoundProvider/AgsAudio[0]\x00\x00\x00start\x00\x00\x00";
  static const guchar *stop_soundcard_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSoundcard\x00\x00stop\x00\x00\x00\x00";
  static const guchar *stop_sequencer_message = "/action\x00,ss\x00/AgsSoundProvider/AgsSequencer\x00\x00stop\x00\x00\x00\x00";
  static const guchar *stop_audio_message =     "/action\x00,ss\x00/AgsSoundProvider/AgsAudio[0]\x00\x00\x00stop\x00\x00\x00\x00";

  static const guint start_soundcard_message_size = 52;
  static const guint start_sequencer_message_size = 52;
  static const guint start_audio_message_size = 52;
  static const guint stop_soundcard_message_size = 52;
  static const guint stop_sequencer_message_size = 52;
  static const guint stop_audio_message_size = 52;

  /* start soundcard */
  doc = xmlNewDoc("1.0");
	    
  root_node = xmlNewNode(NULL,
			 BAD_CAST "ags-osc-over-xmlrpc");
  xmlDocSetRootElement(doc,
		       root_node);

  osc_packet_node_list = xmlNewNode(NULL,
				    BAD_CAST "ags-osc-packet-list");
  
  xmlAddChild(root_node,
	      osc_packet_node_list);

  osc_packet_node = xmlNewNode(NULL,
				BAD_CAST "ags-osc-packet");

  xmlAddChild(osc_packet_node_list,
	      osc_packet_node);

  /* OSC message */
  packet = (guchar *) g_malloc((4 + start_soundcard_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				start_soundcard_message_size);
  memcpy(packet + 4, start_soundcard_message, (start_soundcard_message_size) * sizeof(guchar));

  data = g_base64_encode(packet,
			 4 + start_soundcard_message_size);

  xmlNodeSetContent(osc_packet_node,
		    data);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);

  /* send message */
  msg = soup_form_request_new("POST",
			      "http://127.0.0.1:8080/ags-xmlrpc/ags-osc-over-xmlrpc",
			      NULL);
  soup_message_set_request(msg,
			   "text/xml; charset=UTF-8",
			   SOUP_MEMORY_COPY,
			   buffer,
			   buffer_length);

  status = soup_session_send_message(soup_session,
				     msg);

  g_object_get(msg,
	       "response-headers", &response_headers,
	       "response-body", &response_body,
	       NULL);

  g_message("status %d", status);

  CU_ASSERT(status == 200);
  
  xmlFree(buffer);
  
  sleep(5);

  /* start sequencer */
  doc = xmlNewDoc("1.0");
	    
  root_node = xmlNewNode(NULL,
			 BAD_CAST "ags-osc-over-xmlrpc");
  xmlDocSetRootElement(doc, root_node);

  osc_packet_node_list = xmlNewNode(NULL,
				    BAD_CAST "ags-osc-packet-list");
  
  xmlAddChild(root_node,
	      osc_packet_node_list);

  osc_packet_node = xmlNewNode(NULL,
				BAD_CAST "ags-osc-packet");

  xmlAddChild(osc_packet_node_list,
	      osc_packet_node);

  /* OSC message */
  packet = (guchar *) malloc((4 + start_sequencer_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				start_sequencer_message_size);
  memcpy(packet + 4, start_sequencer_message, (start_sequencer_message_size) * sizeof(guchar));

  data = g_base64_encode(packet,
			 4 + start_sequencer_message_size);

  xmlNodeSetContent(osc_packet_node,
		    data);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);

  /* send message */
  msg = soup_form_request_new("POST",
			      "http://127.0.0.1:8080/ags-xmlrpc/ags-osc-over-xmlrpc",
			      NULL);
  soup_message_set_request(msg,
			   "text/xml; charset=UTF-8",
			   SOUP_MEMORY_COPY,
			   buffer,
			   buffer_length);
  
  status = soup_session_send_message(soup_session,
				     msg);

  g_object_get(msg,
	       "response-headers", &response_headers,
	       "response-body", &response_body,
	       NULL);

  g_message("status %d", status);

  xmlFree(buffer);

  CU_ASSERT(status == 200);
  
  sleep(5);

  /* start audio */
  doc = xmlNewDoc("1.0");
	    
  root_node = xmlNewNode(NULL, "ags-osc-over-xmlrpc");
  xmlDocSetRootElement(doc, root_node);

  osc_packet_node_list = xmlNewNode(NULL,
				    BAD_CAST "ags-osc-packet-list");
  
  xmlAddChild(root_node,
	      osc_packet_node_list);

  osc_packet_node = xmlNewNode(NULL,
				BAD_CAST "ags-osc-packet");

  xmlAddChild(osc_packet_node_list,
	      osc_packet_node);

  /* OSC message */
  packet = (guchar *) malloc((4 + start_audio_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				start_audio_message_size);
  memcpy(packet + 4, start_audio_message, (start_audio_message_size) * sizeof(guchar));

  data = g_base64_encode(packet,
			 4 + start_audio_message_size);

  xmlNodeSetContent(osc_packet_node,
		    data);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);

  /* send message */
  msg = soup_form_request_new("POST",
			      "http://127.0.0.1:8080/ags-xmlrpc/ags-osc-over-xmlrpc",
			      NULL);
  soup_message_set_request(msg,
			   "text/xml; charset=UTF-8",
			   SOUP_MEMORY_COPY,
			   buffer,
			   buffer_length);
  
  xmlFree(buffer);

  status = soup_session_send_message(soup_session,
				     msg);

  g_object_get(msg,
	       "response-headers", &response_headers,
	       "response-body", &response_body,
	       NULL);

  g_message("status %d", status);

  CU_ASSERT(status == 200);
  
  sleep(5);
  
  /* stop soundcard */
  doc = xmlNewDoc("1.0");
	    
  root_node = xmlNewNode(NULL, "ags-osc-over-xmlrpc");
  xmlDocSetRootElement(doc, root_node);

  osc_packet_node_list = xmlNewNode(NULL,
				    BAD_CAST "ags-osc-packet-list");
  
  xmlAddChild(root_node,
	      osc_packet_node_list);

  osc_packet_node = xmlNewNode(NULL,
				BAD_CAST "ags-osc-packet");

  xmlAddChild(osc_packet_node_list,
	      osc_packet_node);

  /* OSC message */
  packet = (guchar *) g_malloc((4 + stop_soundcard_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				stop_soundcard_message_size);
  memcpy(packet + 4, stop_soundcard_message, (stop_soundcard_message_size) * sizeof(guchar));

  data = g_base64_encode(packet,
			 4 + stop_soundcard_message_size);

  xmlNodeSetContent(osc_packet_node,
		    data);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);

  /* send message */
  msg = soup_form_request_new("POST",
			      "http://127.0.0.1:8080/ags-xmlrpc/ags-osc-over-xmlrpc",
			      NULL);
  soup_message_set_request(msg,
			   "text/xml; charset=UTF-8",
			   SOUP_MEMORY_COPY,
			   buffer,
			   buffer_length);
  
  xmlFree(buffer);

  status = soup_session_send_message(soup_session,
				     msg);

  g_object_get(msg,
	       "response-headers", &response_headers,
	       "response-body", &response_body,
	       NULL);

  g_message("status %d", status);

  CU_ASSERT(status == 200);
  
  sleep(5);

  /* stop sequencer */
  doc = xmlNewDoc("1.0");
	    
  root_node = xmlNewNode(NULL, "ags-osc-over-xmlrpc");
  xmlDocSetRootElement(doc, root_node);

  osc_packet_node_list = xmlNewNode(NULL,
				    BAD_CAST "ags-osc-packet-list");
  
  xmlAddChild(root_node,
	      osc_packet_node_list);

  osc_packet_node = xmlNewNode(NULL,
				BAD_CAST "ags-osc-packet");

  xmlAddChild(osc_packet_node_list,
	      osc_packet_node);

  /* OSC message */
  packet = (guchar *) malloc((4 + stop_sequencer_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				stop_sequencer_message_size);
  memcpy(packet + 4, stop_sequencer_message, (stop_sequencer_message_size) * sizeof(guchar));

  data = g_base64_encode(packet,
			 4 + stop_sequencer_message_size);

  xmlNodeSetContent(osc_packet_node,
		    data);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);

  /* send message */
  msg = soup_form_request_new("POST",
			      "http://127.0.0.1:8080/ags-xmlrpc/ags-osc-over-xmlrpc",
			      NULL);
  soup_message_set_request(msg,
			   "text/xml; charset=UTF-8",
			   SOUP_MEMORY_COPY,
			   buffer,
			   buffer_length);
  
  xmlFree(buffer);

  status = soup_session_send_message(soup_session,
				     msg);

  g_object_get(msg,
	       "response-headers", &response_headers,
	       "response-body", &response_body,
	       NULL);

  g_message("status %d", status);

  CU_ASSERT(status == 200);
  
  sleep(5);

  /* stop audio */
  doc = xmlNewDoc("1.0");
	    
  root_node = xmlNewNode(NULL, "ags-osc-over-xmlrpc");
  xmlDocSetRootElement(doc, root_node);

  osc_packet_node_list = xmlNewNode(NULL,
				    BAD_CAST "ags-osc-packet-list");
  
  xmlAddChild(root_node,
	      osc_packet_node_list);

  osc_packet_node = xmlNewNode(NULL,
				BAD_CAST "ags-osc-packet");

  xmlAddChild(osc_packet_node_list,
	      osc_packet_node);

  /* OSC message */
  packet = (guchar *) malloc((4 + stop_audio_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				stop_audio_message_size);
  memcpy(packet + 4, stop_audio_message, (stop_audio_message_size) * sizeof(guchar));

  data = g_base64_encode(packet,
			 4 + stop_audio_message_size);

  xmlNodeSetContent(osc_packet_node,
		    data);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);

  /* send message */
  msg = soup_form_request_new("POST",
			      "http://127.0.0.1:8080/ags-xmlrpc/ags-osc-over-xmlrpc",
			      NULL);
  soup_message_set_request(msg,
			   "text/xml; charset=UTF-8",
			   SOUP_MEMORY_COPY,
			   buffer,
			   buffer_length);
  
  xmlFree(buffer);

  status = soup_session_send_message(soup_session,
				     msg);

  g_object_get(msg,
	       "response-headers", &response_headers,
	       "response-body", &response_body,
	       NULL);

  g_message("status %d", status);

  CU_ASSERT(status == 200);
  
  sleep(5);
}

void
ags_functional_osc_xmlrpc_server_test_config_controller()
{
  SoupMessage *msg;
  SoupMessageHeaders *response_headers;
  SoupMessageBody *response_body;
  
  SoupMessageHeadersIter iter;
  GSList *cookie;

  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *osc_packet_node_list;
  xmlNode *osc_packet_node;

  guchar *message;
  gchar *data;
  guchar *packet;
  xmlChar *buffer;

  guint config_length;
  guint length;
  int buffer_length;
  guint status;

  static const guchar *config_message = "/config\x00,s\x00\x00";

  static const guint config_message_size = 12;

  /* apply config */
  doc = xmlNewDoc("1.0");
	    
  root_node = xmlNewNode(NULL, "ags-osc-over-xmlrpc");
  xmlDocSetRootElement(doc, root_node);

  osc_packet_node_list = xmlNewNode(NULL,
				    BAD_CAST "ags-osc-packet-list");
  
  xmlAddChild(root_node,
	      osc_packet_node_list);

  osc_packet_node = xmlNewNode(NULL,
				BAD_CAST "ags-osc-packet");

  xmlAddChild(osc_packet_node_list,
	      osc_packet_node);

  /* OSC message */
  config_length = strlen(AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_APPLY_CONFIG_ARGUMENT);

  length = config_message_size + (4 * ceil((double) (config_length + 1) / 4.0));

  message = (guchar *) malloc(length * sizeof(guchar));
  memset(message, 0, length * sizeof(guchar));

  memcpy(message, config_message, config_message_size * sizeof(guchar));
  memcpy(message + config_message_size, AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_APPLY_CONFIG_ARGUMENT, config_length * sizeof(guchar));

  packet = (guchar *) g_malloc((4 + length) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				length);
  memcpy(packet + 4, message, (length) * sizeof(guchar));

  data = g_base64_encode(packet,
			 4 + length);

  xmlNodeSetContent(osc_packet_node,
		    data);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);

  /* send message */
  msg = soup_form_request_new("POST",
			      "http://127.0.0.1:8080/ags-xmlrpc/ags-osc-over-xmlrpc",
			      NULL);
  soup_message_set_request(msg,
			   "text/xml; charset=UTF-8",
			   SOUP_MEMORY_COPY,
			   buffer,
			   buffer_length);
  
  xmlFree(buffer);

  status = soup_session_send_message(soup_session,
				     msg);

  g_object_get(msg,
	       "response-headers", &response_headers,
	       "response-body", &response_body,
	       NULL);

  g_message("status %d", status);

  CU_ASSERT(status == 200);
  
  sleep(5);
}

void
ags_functional_osc_xmlrpc_server_test_info_controller()
{
  SoupMessage *msg;
  SoupMessageHeaders *response_headers;
  SoupMessageBody *response_body;
  
  SoupMessageHeadersIter iter;
  GSList *cookie;

  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *osc_packet_node_list;
  xmlNode *osc_packet_node;

  gchar *data;
  guchar *packet;
  xmlChar *buffer;

  int buffer_length;
  guint status;

  static const guchar *info_message = "/info\x00\x00\x00";

  static const guint info_message_size = 8;

  /* info */
  doc = xmlNewDoc("1.0");
	    
  root_node = xmlNewNode(NULL, "ags-osc-over-xmlrpc");
  xmlDocSetRootElement(doc, root_node);

  osc_packet_node_list = xmlNewNode(NULL,
				    BAD_CAST "ags-osc-packet-list");
  
  xmlAddChild(root_node,
	      osc_packet_node_list);

  osc_packet_node = xmlNewNode(NULL,
				BAD_CAST "ags-osc-packet");

  xmlAddChild(osc_packet_node_list,
	      osc_packet_node);

  /* OSC message */
  packet = (guchar *) g_malloc((4 + info_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				info_message_size);
  memcpy(packet + 4, info_message, (info_message_size) * sizeof(guchar));

  data = g_base64_encode(packet,
			 4 + info_message_size);

  xmlNodeSetContent(osc_packet_node,
		    data);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);

  /* send message */
  msg = soup_form_request_new("POST",
			      "http://127.0.0.1:8080/ags-xmlrpc/ags-osc-over-xmlrpc",
			      NULL);
  soup_message_set_request(msg,
			   "text/xml; charset=UTF-8",
			   SOUP_MEMORY_COPY,
			   buffer,
			   buffer_length);
  
  xmlFree(buffer);

  status = soup_session_send_message(soup_session,
				     msg);

  g_object_get(msg,
	       "response-headers", &response_headers,
	       "response-body", &response_body,
	       NULL);

  g_message("status %d", status);

  CU_ASSERT(status == 200);
  
  sleep(5);
}

void
ags_functional_osc_xmlrpc_server_test_meter_controller()
{
  SoupMessage *msg;
  SoupMessageHeaders *response_headers;
  SoupMessageBody *response_body;
  
  SoupMessageHeadersIter iter;
  GSList *cookie;

  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *child;
  xmlNode *osc_packet_node_list;
  xmlNode *osc_packet_node;
  xmlNode *login_node;
  xmlNode *security_token_node;
  xmlNode *redirect_node;
  
  struct timespec start_time;
  struct timespec timeout_delay;
  struct timespec idle_delay;  

  gchar *login;
  gchar *security_token;
  gchar *resource_id;
  gchar *data;
  guchar *packet;
  xmlChar *buffer;
  
  int buffer_length;
  volatile gint meter_packet_count;
  guint i;
  gboolean retval;
  guint status;

  static const guchar *enable_peak_message = "/meter\x00\x00,sT\x00/AgsSoundProvider/AgsAudio[\"test-drum\"]/AgsInput[0-15]/AgsPeakChannel[0]/AgsPort[\"./peak[0]\"]:value\x00";
  static const guchar *disable_peak_message = "/meter\x00\x00,sF\x00/AgsSoundProvider/AgsAudio[\"test-drum\"]/AgsInput[0-15]/AgsPeakChannel[0]/AgsPort[\"./peak[0]\"]:value\x00";

  static const guint enable_peak_message_size = 112;
  static const guint disable_peak_message_size = 112;

  static const char *protocols[] = {
    "Sec-WebSocket-Protocol",
    "ags-osc-over-xmlrpc.gsequencer.org",
    NULL,
  };

  /* meter */
  doc = xmlNewDoc("1.0");
	    
  root_node = xmlNewNode(NULL, "ags-osc-over-xmlrpc");
  xmlDocSetRootElement(doc, root_node);

  osc_packet_node_list = xmlNewNode(NULL,
				    BAD_CAST "ags-osc-packet-list");
  
  xmlAddChild(root_node,
	      osc_packet_node_list);

  osc_packet_node = xmlNewNode(NULL,
				BAD_CAST "ags-osc-packet");

  xmlAddChild(osc_packet_node_list,
	      osc_packet_node);

  /* OSC message */
  packet = (guchar *) g_malloc((4 + enable_peak_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				enable_peak_message_size);
  memcpy(packet + 4, enable_peak_message, (enable_peak_message_size) * sizeof(guchar));

  data = g_base64_encode(packet,
			 4 + enable_peak_message_size);

  xmlNodeSetContent(osc_packet_node,
		    data);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);

  /* send message */
  msg = soup_form_request_new("POST",
			      "http://127.0.0.1:8080/ags-xmlrpc/ags-osc-over-xmlrpc",
			      NULL);
  soup_message_set_request(msg,
			   "text/xml; charset=UTF-8",
			   SOUP_MEMORY_COPY,
			   buffer,
			   buffer_length);
  
  xmlFree(buffer);

  status = soup_session_send_message(soup_session,
				     msg);

  sleep(60);
  
  g_object_get(msg,
	       "response-headers", &response_headers,
	       "response-body", &response_body,
	       NULL);

  g_message("status %d", status);

  CU_ASSERT(status == 200);

  /* parse cookies */
  cookie = soup_cookies_from_response(msg);
  
  login = NULL;
  security_token = NULL;
  
  while(cookie != NULL){
    char *cookie_name;

    cookie_name = soup_cookie_get_name(cookie->data);
    
    if(!g_ascii_strncasecmp(cookie_name,
			    "ags-srv-login",
			    14)){
      login =
	meter_data.login = soup_cookie_get_value(cookie->data);
    }else if(!g_ascii_strncasecmp(cookie_name,
				  "ags-srv-security-token",
				  23)){
      security_token =
	meter_data.security_token = soup_cookie_get_value(cookie->data);
    }

    if(login != NULL &&
       security_token != NULL){
      break;
    }
    
    cookie = cookie->next;
  }

  /* parse response */
  g_message("%s", response_body->data);

  doc = xmlParseDoc(response_body->data);

  CU_ASSERT(doc != NULL);

  root_node = xmlDocGetRootElement(doc);

  CU_ASSERT(root_node != NULL);

  child = root_node->children;

  resource_id = NULL;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-srv-redirect",
		     17)){
	xmlChar *tmp_resource_id;
	
	tmp_resource_id = xmlGetProp(child,
				     "resource-id");

	resource_id =
	  meter_data.resource_id =  g_strdup(tmp_resource_id);

	xmlFree(tmp_resource_id);
	
	break;
      }
    }
    
    child = child->next;
  }

  CU_ASSERT(resource_id != NULL);
  
  /* read packets */
#ifdef __APPLE__
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
      
  start_time.tv_sec = mts.tv_sec;
  start_time.tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_MONOTONIC, &start_time);
#endif

  timeout_delay.tv_sec = 180;
  timeout_delay.tv_nsec = 0;
  
  idle_delay.tv_sec = 0;
  idle_delay.tv_nsec = NSEC_PER_SEC / 30;
  
  g_atomic_int_set(&(meter_packet_count),
		   0);
  i = 0;

  /* follow redirect */
  msg = soup_message_new("GET",
			 "http://127.0.0.1:8080/ags-xmlrpc/ags-osc-over-xmlrpc/response");

  meter_data.meter_packet_count = &meter_packet_count;
  soup_session_websocket_connect_async(soup_session,
				       msg,
				       NULL,
				       protocols,
				       NULL,
				       ags_functional_osc_xmlrpc_server_test_websocket_callback,
				       &meter_data);
  
  while(!ags_osc_client_timeout_expired(&start_time,
					&timeout_delay)){
    //empty

    nanosleep(&idle_delay,
	      NULL);
  }
  
  CU_ASSERT(g_atomic_int_get(&(meter_packet_count)) >= AGS_FUNCTIONAL_OSC_XMLRPC_SERVER_TEST_METER_PACKET_COUNT);

  /* disable meter */
  packet = (guchar *) malloc((4 + disable_peak_message_size) * sizeof(guchar));

  ags_osc_buffer_util_put_int32(packet,
				disable_peak_message_size);
  memcpy(packet + 4, disable_peak_message, (disable_peak_message_size) * sizeof(guchar));
  
  data = g_base64_encode(packet,
			 4 + disable_peak_message_size);

  xmlNodeSetContent(osc_packet_node,
		    data);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);

  /* send message */
  msg = soup_form_request_new("POST",
			      "http://127.0.0.1:8080/ags-xmlrpc/ags-osc-over-xmlrpc",
			      NULL);
  soup_message_set_request(msg,
			   "text/xml; charset=UTF-8",
			   SOUP_MEMORY_COPY,
			   buffer,
			   buffer_length);
  
  xmlFree(buffer);

  status = soup_session_send_message(soup_session,
				     msg);

  g_object_get(msg,
	       "response-headers", &response_headers,
	       "response-body", &response_body,
	       NULL);

  g_message("status %d", status);

  CU_ASSERT(status == 200);

  //TODO:JK: implement me
}

void
ags_functional_osc_xmlrpc_server_test_node_controller()
{
  SoupMessage *msg;
  SoupMessageHeaders *response_headers;
  SoupMessageBody *response_body;
  
  SoupMessageHeadersIter iter;
  GSList *cookie;

  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *osc_packet_node_list;
  xmlNode *osc_packet_node;

  gchar *data;
  guchar *packet;
  xmlChar *buffer;

  int buffer_length;
  guint status;

  static const guchar *volume_message = "/node\x00\x00\x00,s\x00\x00/AgsSoundProvider/AgsAudio[\"test-drum\"]/AgsInput[0-1]/AgsVolumeChannel[0]/AgsPort[\"./volume[0]\"]:value\x00\x00";

  static const guint volume_message_size = 116;

  //TODO:JK: implement me
}

void
ags_functional_osc_xmlrpc_server_test_renew_controller()
{
  SoupMessage *msg;
  SoupMessageHeaders *response_headers;
  SoupMessageBody *response_body;
  
  SoupMessageHeadersIter iter;
  GSList *cookie;

  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *osc_packet_node_list;
  xmlNode *osc_packet_node;

  gchar *data;
  guchar *packet;
  xmlChar *buffer;

  int buffer_length;
  guint status;

  static const guchar *mute_message = "/renew\x00\x00,sf\x00/AgsSoundProvider/AgsAudio[\"test-panel\"]/AgsInput[0-1]/AgsMuteChannel[0]/AgsPort[\"./muted[0]\"]:value\x00\x00\x00\x00\x00\x00";

  static const guint mute_message_size = 120;

  //TODO:JK: implement me
}

void
ags_functional_osc_xmlrpc_server_test_status_controller()
{
  SoupMessage *msg;
  SoupMessageHeaders *response_headers;
  SoupMessageBody *response_body;
  
  SoupMessageHeadersIter iter;
  GSList *cookie;

  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *osc_packet_node_list;
  xmlNode *osc_packet_node;

  gchar *data;
  guchar *packet;
  xmlChar *buffer;

  int buffer_length;
  guint status;

  static const guchar *status_message = "/status\x00,\x00\x00\x00";

  static const guint status_message_size = 12;

  //TODO:JK: implement me
}

int
main(int argc, char **argv)
{
  add_thread = g_thread_new("libags_audio.so - functional OSC XMLRPC server test",
			    ags_functional_osc_xmlrpc_server_test_add_thread,
			    NULL);

  g_main_loop_run(g_main_loop_new(g_main_context_default(),
				  FALSE));
  
  g_thread_join(add_thread);
  
  return(-1);
}
