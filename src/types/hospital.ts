export type RecordId = number;

export interface Patient {
  id: RecordId;
  name: string;
  age: number;
  phone: string;
  address: string;
}

export interface Doctor {
  id: RecordId;
  name: string;
  department: string;
  phone: string;
  available: boolean;
}

export type AppointmentStatus =
  | 'Scheduled'
  | 'Confirmed'
  | 'Waiting'
  | 'Completed'
  | 'Cancelled';

export interface Appointment {
  id: RecordId;
  patientId: RecordId;
  doctorId: RecordId;
  date: string;
  time: string;
  status: AppointmentStatus;
}

export type EmergencyPriority = 'Low' | 'Medium' | 'High' | 'Critical';

export interface Emergency {
  id: RecordId;
  patientId: RecordId | null;
  description: string;
  priority: EmergencyPriority;
  status: 'Open' | 'InProgress' | 'Resolved';
}

export interface BillingRecord {
  id: RecordId;
  patientId: RecordId;
  amount: number;
  status: 'Pending' | 'Paid' | 'Cancelled';
  date: string;
}