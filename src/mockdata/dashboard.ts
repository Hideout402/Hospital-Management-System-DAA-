export const dashboardAppointments = [
  {
    id: 'APT-1001',
    patient: 'Ayesha Khan',
    doctor: 'Dr. Ahmed Ali',
    department: 'Cardiology',
    time: '09:00 AM',
    status: 'Confirmed',
  },
  {
    id: 'APT-1002',
    patient: 'Hamza Malik',
    doctor: 'Dr. Sara Noor',
    department: 'Neurology',
    time: '09:30 AM',
    status: 'Waiting',
  },
];

export const dashboardEmergencies = [
  {
    id: 1,
    title: 'Emergency Room',
    description: '2 patients awaiting immediate assessment',
    severity: 'Critical',
  },
  {
    id: 2,
    title: 'Ambulance Arrival',
    description: 'Incoming ambulance — reception notified',
    severity: 'High',
  },
];
export const dashboardStats = {
  totalPatients: 1248,
  availableDoctors: 32,
  todaysAppointments: 86,
  emergencyCases: 5,
};

export const dailySummary = {
  appointmentsCompleted: 24,
  patientsCheckedIn: 42,
  patientsWaiting: 8,
};