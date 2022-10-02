#ifndef STATUS_H
#define STATUS_H

void status_init();
void status_clear();
void status_connecting();
void status_provisioning();
void status_provisioned();
void status_resetting();
void status_ready();
void status_waiting();
void status_downloading();
void status_refreshing();
void status_error();

#endif