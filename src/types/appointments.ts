export type AppointmentStatus = 'Scheduled' | 'Completed' | 'Cancelled';
export type AppointmentType = 'Consultation' | 'Follow-up' | 'Routine check-up';

export interface Patient {
  id: string;
  name: string;
  age: number;
  phone: string;
  gender: string;
}

export interface Doctor {
  id: string;
  name: string;
  specialty: string;
  department: string;
  room: string;
  fee: number;
  color: string;
}

export interface Appointment {
  id: string;
  patientId: string;
  doctorId: string;
  date: string;
  time: string;
  duration: number;
  type: AppointmentType;
  reason: string;
  notes: string;
  status: AppointmentStatus;
}

export interface AppointmentDraft {
  patientId: string;
  doctorId: string;
  date: string;
  time: string;
  type: AppointmentType;
  reason: string;
  notes: string;
  status: AppointmentStatus;
}
