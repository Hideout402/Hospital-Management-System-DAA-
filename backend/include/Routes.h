#ifndef ROUTES_H
#define ROUTES_H

#include "libraries/httplib.h"

void registerPatientRoutes(httplib::Server& server);
void registerDoctorRoutes(httplib::Server& server);
void registerDepartmentRoutes(httplib::Server& server);
void registerAppointmentRoutes(httplib::Server& server);
void registerBillingRoutes(httplib::Server& server);
void registerEmergencyRoutes(httplib::Server& server);
void registerNavigationRoutes(httplib::Server& server);

#endif
