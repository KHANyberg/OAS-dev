#include <gio/gio.h>

static char **s_paths;
static GDBusProxy **proxy;

static void list_connections (GDBusProxy *proxy)
{
    GError *error = NULL;
    GVariant *ret;

    /* Call ListConnections D-Bus method */
    ret = g_dbus_proxy_call_sync(proxy,
                                  "ListConnections",
                                  NULL,
                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                  NULL, &error);
    if (!ret) {
        g_dbus_error_strip_remote_error (error);
        g_print ("ListConnections failed: %s\n", error->message);
        g_error_free (error);
        return;
    }

    g_variant_get(ret, "(^ao)", &s_paths);
    g_variant_unref(ret);
}

static void list_connection_settings (GDBusProxy *proxy)
{
    GError *error = NULL;
    GVariant *ret;
    GVariantIter *iter1;

    /* Call ListConnections D-Bus method */
    ret = g_dbus_proxy_call_sync(proxy,
                                  "GetSettings",
                                  NULL,
                                  G_DBUS_CALL_FLAGS_NONE, -1,
                                  NULL, &error);
    if (!ret) {
        g_dbus_error_strip_remote_error (error);
        g_print ("GetSettings failed: %s\n", error->message);
        g_error_free (error);
        return;
    }

    g_variant_get(ret, "(a{sa{sv}})", &iter1);
    g_variant_unref(ret);

    const char *key1;
    GVariantIter *iter2 = NULL;

    while (g_variant_iter_loop (iter1, "{&sa{sv}}", &key1, &iter2))
    {
      const char *key2;
      GVariant *value = NULL;
      char *printed_value;

      while (g_variant_iter_loop (iter2, "{&sv}", &key2, &value))
      {
        printed_value = g_variant_print (value, FALSE);
        if (strcmp (printed_value, "[]") != 0)
        {
            g_message ("KEY1: %s, KEY2: %s, VALUE: %s", key1, key2, printed_value);
        }
        else
        {
            g_message ("KEY1: %s, KEY2: %s, VALUE: %s", key1, key2, "EMPTY");
        }
        
      }
    }
}

int main (int argc, char *argv[])
{
    int n;
    GDBusProxy *proxy;

    /* Create a D-Bus proxy; NM_DBUS_* defined in nm-dbus-interface.h */
    proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                           G_DBUS_PROXY_FLAGS_NONE,
                                           NULL,
                                           "org.freedesktop.NetworkManager",
                                           "/org/freedesktop/NetworkManager/Settings",
                                           "org.freedesktop.NetworkManager.Settings",
                                           NULL, NULL);
    g_assert(proxy != NULL);
    list_connections(proxy);

    for (n = 0; s_paths[n]; n++) {
        g_print("%s\n", s_paths[n]);
        proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                           G_DBUS_PROXY_FLAGS_NONE,
                                           NULL,
                                           "org.freedesktop.NetworkManager",
                                           s_paths[n],
                                           "org.freedesktop.NetworkManager.Settings.Connection",
                                           NULL, NULL);
        g_assert(proxy != NULL);
        list_connection_settings(proxy);
    }

    g_strfreev(s_paths);

    g_object_unref(proxy);

    return 0;
}
