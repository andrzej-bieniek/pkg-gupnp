/*
 * Example UPnP device/service, implementing the BinaryLight device and
 * SwitchPower services to emulate a light switch.
 *
 * The user interface is as minimal as possible so that the GUPnP concepts and
 * best practises are more apparent.  For a better implementation of
 * BinaryLight, see gupnp-tools.
 *
 * This example code is in the public domain.
 */

#include <libgupnp/gupnp.h>
#include <stdlib.h>
#include <gmodule.h>

static gboolean status;

G_MODULE_EXPORT void set_target_cb (GUPnPService *service,
                                    GUPnPServiceAction *action,
                                    gpointer user_data);
G_MODULE_EXPORT void get_target_cb (GUPnPService *service,
                                    GUPnPServiceAction *action,
                                    gpointer user_data);
G_MODULE_EXPORT void get_status_cb (GUPnPService *service,
                                    GUPnPServiceAction *action,
                                    gpointer user_data);
G_MODULE_EXPORT void query_target_cb (GUPnPService *service, char *variable,
                                      GValue *value, gpointer user_data);
G_MODULE_EXPORT void query_status_cb (GUPnPService *service, char *variable,
                                      GValue *value, gpointer user_data);

/*
 * Action handlers
 */

/* SetTarget */
G_MODULE_EXPORT void
set_target_cb (GUPnPService *service, GUPnPServiceAction *action, gpointer user_data)
{
  gboolean target;

  /* Get the new target value */
  gupnp_service_action_get (action,
                            "NewTargetValue", G_TYPE_BOOLEAN, &target,
                            NULL);

  /* If the new target doesn't match the current status, change the status and
     emit a notification that the status has changed. */
  if (target != status) {
    status = target;
    gupnp_service_notify (service,
                          "Status", G_TYPE_BOOLEAN, status,
                          NULL);

    g_print ("The light is now %s.\n", status ? "on" : "off");
  }

  /* Return success to the client */
  gupnp_service_action_return (action);
}

/* GetTarget */
G_MODULE_EXPORT void
get_target_cb (GUPnPService *service, GUPnPServiceAction *action, gpointer user_data)
{
  gupnp_service_action_set (action,
                            "RetTargetValue", G_TYPE_BOOLEAN, status,
                            NULL);
  gupnp_service_action_return (action);
}

/* GetStatus */
G_MODULE_EXPORT void
get_status_cb (GUPnPService *service, GUPnPServiceAction *action, gpointer user_data)
{
  gupnp_service_action_set (action,
                            "ResultStatus", G_TYPE_BOOLEAN, status,
                            NULL);
  gupnp_service_action_return (action);
}

/*
 * State Variable query handlers
 */

/* Target */
G_MODULE_EXPORT void
query_target_cb (GUPnPService *service, char *variable, GValue *value, gpointer user_data)
{
  g_value_init (value, G_TYPE_BOOLEAN);
  g_value_set_boolean (value, status);
}

/* Status */
G_MODULE_EXPORT void
query_status_cb (GUPnPService *service, char *variable, GValue *value, gpointer user_data)
{
  g_value_init (value, G_TYPE_BOOLEAN);
  g_value_set_boolean (value, status);
}


int
main (int argc, char **argv)
{
  GMainLoop *main_loop;
  GError *error = NULL;
  GUPnPContext *context;
  GUPnPRootDevice *dev;
  GUPnPServiceInfo *service;
  
  g_thread_init (NULL);
  g_type_init ();

  /* By default the light is off */
  status = FALSE;
  g_print ("The light is now %s.\n", status ? "on" : "off");

  /* Create the UPnP context */
  context = gupnp_context_new (NULL, NULL, 0, &error);
  if (error) {
    g_printerr ("Error creating the GUPnP context: %s\n",
		error->message);
    g_error_free (error);

    return EXIT_FAILURE;
  }

  /* Create root device */
  dev = gupnp_root_device_new (context, "BinaryLight1.xml", ".");
  gupnp_root_device_set_available (dev, TRUE);

  /* Get the switch service from the root device */
  service = gupnp_device_info_get_service
    (GUPNP_DEVICE_INFO (dev), "urn:schemas-upnp-org:service:SwitchPower:1");
  if (!service) {
    g_printerr ("Cannot get SwitchPower1 service\n");

    return EXIT_FAILURE;
  }
  
  /* Autoconnect the action and state variable handlers.  This connects
     query_target_cb and query_status_cb to the Target and Status state
     variables query callbacks, and set_target_cb, get_target_cb and
     get_status_cb to SetTarget, GetTarget and GetStatus actions
     respectively. */
  gupnp_service_signals_autoconnect (GUPNP_SERVICE (service), NULL, &error);
  if (error) {
    g_printerr ("Failed to autoconnect signals: %s\n", error->message);
    g_error_free (error);

    return EXIT_FAILURE;
  }
  
  /* Run the main loop */
  main_loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (main_loop);

  /* Cleanup */
  g_main_loop_unref (main_loop);
  g_object_unref (service);
  g_object_unref (dev);
  g_object_unref (context);
  
  return EXIT_SUCCESS;
}
