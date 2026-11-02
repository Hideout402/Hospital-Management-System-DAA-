export type AppointmentStatus = 'Scheduled' | 'Completed' | 'Cancelled';
export type AppointmentType = 'Consultation' | 'Follow-up' | 'Routine check-up';

export interface Patient {
  id: number;
  name: string;
  age: number;
  phone: string;
  gender: string;
}

export interface Doctor {
  id: number;
  name: string;
  specialty: string;
  department: string;
  room?: string;
  fee: number;
  color?: string;
}

export interface Appointment {
  id: number;
  patientId: number;
  doctorId: number;
  date: string;
  time: string;
  duration: number;
  type: AppointmentType;
  reason: string;
  notes: string;
  status: AppointmentStatus;
}

export interface AppointmentDraft {
  patientId: number;
  doctorId: number;
  date: string;
  time: string;
  type: AppointmentType;
  reason: string;
  notes: string;
  status: AppointmentStatus;
}

export interface AppointmentOptions {
  patients: Patient[];
  doctors: Doctor[];
}

export type AppointmentSortAlgorithm = 'bubble' | 'merge' | 'quick';
export type AppointmentSortField = 'dateTime' | 'id' | 'patientId' | 'doctorId' | 'status';

export interface AppointmentSortResult {
  algorithm: AppointmentSortAlgorithm;
  by: AppointmentSortField;
  order: 'asc' | 'desc';
  appointments: Appointment[];
}

export interface AppointmentSearchResult {
  found: boolean;
  message: string;
  appointment?: Appointment;
}

export interface AppointmentQueueState {
  appointmentIds: number[];
  front: number | null;
  empty: boolean;
  full?: boolean;
  capacity?: number;
}

export interface AppointmentAction {
  appointmentId: number;
  action: string;
  details: string;
  timestamp: string;
}
