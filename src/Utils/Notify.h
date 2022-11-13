#ifndef OMPLAYER_NOTIFY_H
#define OMPLAYER_NOTIFY_H

#include <libnotify/notify.h>
#include "Utils.h"


/**********************************************************************************************************************/


/** Exibe notificações no sistema usando o lib-notify */
void notify_send(const char *title, const char *msg) {
    NotifyNotification *n;
    notify_init("Open Multimedia Player");

    n = notify_notification_new(title, msg, Utils::setIcon(Utils::Notify).toStdString().c_str());
    notify_notification_set_timeout(n, 5000);

    if (!notify_notification_show(n, nullptr)) return;

    g_object_unref(G_OBJECT(n));
}

#endif //OMPLAYER_NOTIFY_H
